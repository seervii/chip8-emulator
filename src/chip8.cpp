#include "chip8.h"
#include <cstring>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;

default_random_engine randGen;
uniform_int_distribution<uint8_t> randByte (0,255U);


const unsigned int FONT_START_ADDRESS = 0x50;
const unsigned int ROM_START_ADDRESS = 0x200;
uint8_t fontset[80] =
{
  
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  
};
chip8::chip8()
{
   pc = ROM_START_ADDRESS;

   randGen = default_random_engine( chrono::system_clock::now().time_since_epoch().count());

   for (unsigned int i = 0; i < 80; i++)
   {
      memory[FONT_START_ADDRESS+i] = fontset[i];
   }
    table[0x0] = &chip8::Table0;
    table[0x1] = &chip8::OP_1nnn;
    table[0x2] = &chip8::OP_2nnn;
    table[0x3] = &chip8::OP_3xkk;
    table[0x4] = &chip8::OP_4xkk;
    table[0x5] = &chip8::OP_5xy0;
    table[0x6] = &chip8::OP_6xkk;
    table[0x7] = &chip8::OP_7xkk;
    table[0x8] = &chip8::Table8;
    table[0x9] = &chip8::OP_9xy0;
    table[0xA] = &chip8::OP_Annn;
    table[0xB] = &chip8::OP_Bnnn;
    table[0xC] = &chip8::OP_Cxkk;
    table[0xD] = &chip8::OP_Dxyn;
    table[0xE] = &chip8::TableE;
    table[0xF] = &chip8::TableF;

    // Fill table0, table8, tableE with OP_NULL first (safety default)

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &chip8::OP_NULL;
        table8[i] = &chip8::OP_NULL;
        tableE[i] = &chip8::OP_NULL;
    }
    // Set up table0 (opcodes starting with 0)

    table0[0x0] = &chip8::OP_00E0;
    table0[0xE] = &chip8::OP_00EE;

    // Set up table8 (opcodes starting with 8)

    table8[0x0] = &chip8::OP_8xy0;
    table8[0x1] = &chip8::OP_8xy1;
    table8[0x2] = &chip8::OP_8xy2;
    table8[0x3] = &chip8::OP_8xy3;
    table8[0x4] = &chip8::OP_8xy4;
    table8[0x5] = &chip8::OP_8xy5;
    table8[0x6] = &chip8::OP_8xy6;
    table8[0x7] = &chip8::OP_8xy7;
    table8[0xE] = &chip8::OP_8xyE;

    // Set up tableE (opcodes starting with E)

    tableE[0x1] = &chip8::OP_ExA1;
    tableE[0xE] = &chip8::OP_Ex9E;

    // Fill tableF with OP_NULL first (safety default)

     for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &chip8::OP_NULL;
    }

    // Set up tableF (opcodes starting with F)
    tableF[0x07] = &chip8::OP_Fx07;
    tableF[0x0A] = &chip8::OP_Fx0A;
    tableF[0x15] = &chip8::OP_Fx15;
    tableF[0x18] = &chip8::OP_Fx18;
    tableF[0x1E] = &chip8::OP_Fx1E;
    tableF[0x29] = &chip8::OP_Fx29;
    tableF[0x33] = &chip8::OP_Fx33;
    tableF[0x55] = &chip8::OP_Fx55;
    tableF[0x65] = &chip8::OP_Fx65;
}

  void chip8::LoadROM(char const* filename){
    ifstream file(filename, ios::binary | ios::ate);
    if (file.is_open())
    {
        streampos size =  file.tellg();
        char* buffer = new char [size];
        file.seekg(0, ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++)
        {
            memory[ROM_START_ADDRESS+i] = buffer[i];
        }
     delete[] buffer;
    }
  }

void chip8::OP_00E0()
{

  memset(video,0,sizeof(video));
  
}

void chip8::OP_00EE()
{
  
  --sp;
  pc = stack[sp];

}

void chip8::OP_1nnn()
{

  uint16_t nnn = opcode & 0x0FFFu;
  pc = nnn;

}

void chip8::OP_2nnn()
{

  uint16_t nnn = opcode & 0x0FFFu;
  stack[sp] = pc;
  ++sp;
  pc = nnn;
 
}

void chip8::OP_3xkk()
{
  
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;

  if (registers[Vx] == kk)
  {
    pc+=2;
  }

}

void chip8::OP_4xkk()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;

  if (registers[Vx]!= kk)
  {
    pc+=2;
  }
  
}

void chip8::OP_5xy0()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] == registers[Vy])
  {
    pc+=2;
  }
  
}

void chip8::OP_9xy0()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] != registers[Vy])
  {
    pc+=2;
  } 
  
} 

void chip8::OP_6xkk()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;

  registers[Vx] = kk;

}

void chip8::OP_7xkk()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;
  
  registers[Vx] += kk;

}

void chip8::OP_8xy0()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] = registers[Vy];

}

void chip8::OP_Annn()
{

  uint16_t nnn = opcode & 0x0FFFu;

  index = nnn;

}

void chip8::OP_8xy1()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] |= registers[Vy];

}

void chip8::OP_8xy2()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] &= registers[Vy];

}

void chip8::OP_8xy3()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] ^= registers[Vy];

}

void chip8::OP_8xy4()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  uint16_t sum = registers[Vx] + registers[Vy];

  if (sum > 255u)
  {
    registers[0xF] = 1;
  }
  
  else
  {
    registers[0xF] = 0;
  }

  registers[Vx] = sum & 0xFFu;
  
}

void chip8::OP_8xy5()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] > registers[Vy])
  {
   registers[0xF] = 1;          
  }
  else
  {
    registers[0xF] = 0;
  }
  
  registers[Vx] -= registers[Vy]; 
  
}

void chip8::OP_8xy6()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  registers[0xF] = (registers[Vx] & 0x1u);
  registers[Vx] >>= 1;

}

void chip8::OP_8xy7()
{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

if (registers[Vy] > registers[Vx])
{
  registers[0xF] = 1;
}
else
{
  registers[0xF] = 0;
}

 registers[Vx] = registers[Vy] - registers[Vx];
}

void chip8::OP_8xyE()

{

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
  registers[Vx] <<= 1;

}
 void chip8::OP_Bnnn()
 {

  uint16_t nnn = opcode & 0x0FFFu;
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  pc = registers[0] + nnn;

 }

 void chip8::OP_Cxkk()
 {
    
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;

  registers[Vx] = randByte(randGen) & kk;

 }

 void chip8::OP_Ex9E()
 {
   
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (keypad[key])
  {
    pc +=2;
  }
  
 }

 void chip8::OP_ExA1()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (!keypad[key])
  {
     pc +=2;
  }
  
 }

 void chip8::OP_Fx07()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  registers[Vx] = delayTimer;

 }

 void chip8::OP_Fx15()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  delayTimer = registers[Vx];

 }

 void chip8::OP_Fx18()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  soundTimer = registers[Vx];

 }

 void chip8::OP_Fx1E()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  index += registers[Vx];

 }

 void chip8::OP_Fx29()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t digit = registers[Vx];

  index = FONT_START_ADDRESS + (5 * digit);

 }

 void chip8::OP_Fx0A()
 {

  uint8_t Vx = ( opcode & 0x0F00u) >> 8u;
  bool keyPressed = false;
  
  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
     if (keypad[i])
     {
        registers[Vx] = i;
        keyPressed = true;
        break;
     }
  }

  if (!keyPressed)
  {
    pc -= 2;
  }
 }

 void chip8::OP_Fx33()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t value = registers[Vx];

  memory[index + 2] = value % 10;
  value/=10;
  memory[index + 1] = value % 10;
  value/=10;
  memory[index] = value % 19;
 }

 void chip8::OP_Fx55()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i <= Vx; i++)
  {
    memory[index + i] = registers[i];
  }
  
 }

 void chip8::OP_Fx65()
 {

  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i <= Vx; i++)
  {
     registers[i] = memory[index + i];
  }
  
 }
  void chip8::OP_Dxyn()
  {

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++)
    {
       uint8_t spriteByte = memory[index + row];

       for (unsigned col = 0; col < 8; col++)
       {
           uint8_t spritePixel = spriteByte & (0x80u >> col);
           uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH +(xPos + col)];
           if (spritePixel)
           {
               if (*screenPixel == 0xFFFFFFFF)
               {
                 registers[0xF] = 1;
               }
               
               *screenPixel ^= 0xFFFFFFFF;
           }  
       }  
    }  
  }

  void chip8::Table0()
  {
    uint8_t lastNibble = opcode & 0x000Fu;
    ((*this).*(table0[lastNibble]))();
  }

  void chip8::Table8()
  {
    uint8_t lastNibble = opcode & 0x000Fu;
    ((*this).*(table8[lastNibble]))();
  }

  void chip8::TableE()
  {
    uint8_t lastNibble = opcode & 0x000Fu;
    ((*this).*(tableE[lastNibble]))();
  }

  void chip8::TableF()
  {
    uint8_t lastNibble = opcode & 0x000Fu;
    ((*this).*(tableF[lastNibble]))();
  }

  void chip8::OP_NULL()
  {

  }

  void chip8::Cycle()
  {

    opcode = (memory[pc] << 8u) | (memory[pc+1]);
    pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
  }

  void chip8::UpdateTimers()
  {
    if (delayTimer > 0)
    {
       --delayTimer;
    }

    if (soundTimer > 0)
    {
      --soundTimer;
    }
  }

  