/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef AUDIO_RECORDER
#define AUDIO_RECORDER

#include "wavwriter.h"

class audiorecorder
{
  private:
    int numberOfSamplesPerSecond;
    ALCdevice *device;
    ALCcontext *context;
    wavwriter *wavWriter;

    ALCdevice* initALCdevice();
    ALCcontext* initALCcontext();

  public:
    audiorecorder(const char*, int);
    void init();
    ALCdevice* getDevice();
    ALCcontext* getContext();
    void recordSamples(int);
    void finish();
};

#endif
