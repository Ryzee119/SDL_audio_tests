#include <SDL.h>
#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define SOUND_FILE "D:\\426669_16.wav"
//#define SOUND_FILE "D:\\426669_44.1.wav"
//#define SOUND_FILE "D:\\426669_48.wav"
#define SOUND_FILE "D:\\426669_96.wav"

#define printf(fmt, ...) debugPrint(fmt, __VA_ARGS__)

int main(void)
{
    XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT);
	size_t fb_size = 640 * 480 * sizeof(uint32_t);
	extern uint8_t* _fb;
	_fb = (uint8_t*)MmAllocateContiguousMemoryEx(fb_size,
												0,
												0xFFFFFFFF,
												0x1000,
												PAGE_READWRITE | PAGE_WRITECOMBINE);
	memset(_fb, 0x00, fb_size);
	#define _PCRTC_START 0xFD600800
	*(unsigned int*)(_PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;

    // Initialize SDL
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL could not be initialized!\n"
            "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    // Load .WAV sound
    SDL_AudioSpec wavFileSpec, obtainedSpec;
    Uint32 wavLength;
    Uint8 * wavBuffer;
    if (!SDL_LoadWAV(SOUND_FILE, &wavFileSpec, &wavBuffer, &wavLength)) {
        printf("Sound '%s' could not be loaded!\n"
            "SDL_Error: %s\n", SOUND_FILE, SDL_GetError());
        return 0;
    }
    printf("File Audio Format:\n");
    printf("Loaded %s\n",SOUND_FILE);
    printf("Frequency: %uHz\n",wavFileSpec.freq);
    printf("Format: %04x\n",wavFileSpec.format);
    printf("Channels: %u\n",wavFileSpec.channels);
    printf("Samples: %u\n",wavFileSpec.samples);
    printf("------------------\n\n");

    // Open audio device
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL,
	                                            0, //0 = playback
												&wavFileSpec,
												&obtainedSpec,
												SDL_AUDIO_ALLOW_ANY_CHANGE);
    printf("Required Xbox Audio Spec:\n");
    printf("Frequency: %uHz\n",obtainedSpec.freq);
    printf("Format: %04x\n",obtainedSpec.format);
    printf("Channels: %u\n",obtainedSpec.channels);
    printf("Samples: %u\n\n",obtainedSpec.samples);

    if (!dev) {
        printf("Audio device could not be opened!\n"
            "SDL_Error: %s\n", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        return 0;
    }

    if (SDL_QueueAudio(dev, wavBuffer, wavLength) < 0) {
        printf("Audio could not be queued!\n"
            "SDL_Error: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(dev);
        SDL_FreeWAV(wavBuffer);
        return 0;
    }

    // Play audio
    SDL_PauseAudioDevice(dev, 0);

    // Event loop
    SDL_JoyButtonEvent* jb_button;
    SDL_JoystickOpen(0);
    while (1) {
        SDL_Event e;
        SDL_WaitEvent(&e);
        if (e.type == SDL_JOYBUTTONDOWN) {
            jb_button = (SDL_JoyButtonEvent*)&e;
            if (jb_button->button == 0) {
                if (SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PLAYING) {
                    SDL_PauseAudioDevice(dev, 1);
                    printf("Audio paused\n");
                } else if (SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PAUSED) {
                    SDL_PauseAudioDevice(dev, 0);
                    printf("Audio resumed\n");
                }
            }
        }
    }

    SDL_CloseAudioDevice(dev);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
    return 0;
}