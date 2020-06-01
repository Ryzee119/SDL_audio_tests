# SDL_audio_tests

SDL_audio tests for https://github.com/XboxDev/nxdk

To build, setup nxdk then do this:

```
export NXDK_DIR=/path/to/nxdk
git clone https://github.com/Ryzee119/SDL_audio_tests.git
cd SDL_audio_tests
make
```

Test tone generated with `ffmpeg -f lavfi -i "sine=frequency=500:duration=5:sample_rate=48000" 500hz48000.wav`