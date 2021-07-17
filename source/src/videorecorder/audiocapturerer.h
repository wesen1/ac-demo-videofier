/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef AUDIO_CAPTURERER
#define AUDIO_CAPTURERER

#include "AL/al.h"
#include "AL/alc.h"

class audiocapturerer
{
 private:
  int numberOfSamplesPerSecond;
  ALCdevice *device;
  ALCcontext *context;

  ALCdevice* initALCdevice();
  ALCcontext* initALCcontext();
  int calculateNumberOfSamples(int);
  unsigned char* createS16leSamplesFromChannelData(int, int);

 public:
  audiocapturerer(int);

  ALCdevice* getDevice();
  ALCcontext* getContext();
  int getNumberOfSamplesPerSecond();
  int getNumberOfChannels();
  int getChannelDataSizeInBytes();

  void init();
  int calculateS16leSamplesSizeInBytes(int);
  unsigned char* captureS16leSamples(int);
};

#endif
