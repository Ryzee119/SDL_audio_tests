#include <SDL.h>
#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SOUND_FILE "D:\\500hz48000.wav"

#define printf(fmt, ...) debugPrint(fmt, __VA_ARGS__)

int main(void)
{
    // Setup Xbox video output
    XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT);

    // Initialize SDL
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        return 0;
    }


    // Load .WAV sound
    SDL_AudioSpec wavFileSpec, obtainedSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    if (!SDL_LoadWAV(SOUND_FILE, &wavFileSpec, &wavBuffer, &wavLength))
    {
        printf("Sound '%s' could not be loaded!\n"
               "SDL_Error: %s\n",
               SOUND_FILE, SDL_GetError());
        return 0;
    }
    printf("File Audio Format:\n");
    printf("Loaded %s\n", SOUND_FILE);
    printf("Frequency: %uHz\n", wavFileSpec.freq);
    printf("Channels: %u\n", wavFileSpec.channels);
    printf("Samples: %u\n", wavFileSpec.samples);
    printf("------------------\n\n");

    // Open audio device
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL,
                                                0, //0 = playback
                                                &wavFileSpec,
                                                &obtainedSpec,
                                                0);
    printf("Obtained Audio Spec:\n");
    printf("Frequency: %uHz\n", obtainedSpec.freq);
    printf("Channels: %u\n", obtainedSpec.channels);
    printf("Samples: %u\n\n", obtainedSpec.samples);

    if (!dev)
    {
        printf("Audio device could not be opened!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        return 0;
    }

    if (SDL_QueueAudio(dev, wavBuffer, wavLength) < 0)
    {
        printf("Audio could not be queued!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        SDL_CloseAudioDevice(dev);
        SDL_FreeWAV(wavBuffer);
        return 0;
    }

    // Play audio
    SDL_PauseAudioDevice(dev, 0);

    // Event loop
    SDL_JoyButtonEvent *jb_button;
    SDL_JoystickOpen(0);

    int quit = 0;
    while (!quit)
    {

        //Replay the audio file indefinitely
        if (SDL_GetQueuedAudioSize(dev) == 0)
        {
            SDL_QueueAudio(dev, wavBuffer, wavLength);
        }

        static SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_JOYBUTTONDOWN)
            {
                jb_button = (SDL_JoyButtonEvent *)&e;
                if (jb_button->button == 0) //A
                {
                    if (SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PLAYING)
                    {
                        SDL_PauseAudioDevice(dev, 1);
                        printf("Audio paused\n");
                    }
                    else if (SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PAUSED)
                    {
                        SDL_PauseAudioDevice(dev, 0);
                        printf("Audio resumed\n");
                    }
                }
                if (jb_button->button == 1) //B
                {
                    printf("Exiting\n");
                    quit = 1;
                }
            }
        }
    }

    SDL_CloseAudioDevice(dev);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
    return 0;
}
