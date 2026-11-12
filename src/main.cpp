#include <chrono>
#include <iostream>
#include "chip8.h"
#include "Platform.h"
#include <stdexcept>

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

    chip8 chip8;
    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::steady_clock::now();
    auto lastTimerTime = lastCycleTime;
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;

            try
            {
                chip8.Cycle();
            }
            catch (std::runtime_error const& e)
            {
                std::cerr << "Emulator stopped: " << e.what() << "\n";
                return EXIT_FAILURE;
            }

            platform.Update(chip8.video, videoPitch);
        }

        float timerDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimerTime).count();

        if (timerDt >= 1000.0f / 60.0f)
        {
            chip8.UpdateTimers();
            lastTimerTime = currentTime;
        }
    }

    return 0;
}