/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

/**
 * The OpenAL soft rendering code is based on https://gist.github.com/skitaoka/13a0ffa3f6434ca03e87
 */

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include <iostream>
#include <memory>
#include "audiorecorder.h"

namespace
{
  LPALCLOOPBACKOPENDEVICESOFT alcLoopbackOpenDeviceSOFT;
  LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
  LPALCRENDERSAMPLESSOFT alcRenderSamplesSOFT;

  template <typename Fn>
  inline void load_soft_extension(char const* const name, Fn* const fn)
  {
    *fn = reinterpret_cast<Fn>(::alcGetProcAddress(NULL, name));
    if (!*fn)
    {
      std::cerr << "could not load a extension" << name << '\n';
      return;
    }
  }

  template <typename T, typename D>
  inline std::unique_ptr<T, D> make_unique(T* p, D d)
  {
    return std::unique_ptr<T, D>(p, d);
  }
}

#define ALC_LOAD_SOFT_EXTENSION(p) load_soft_extension(#p, &p)
#define AL_ERROR_CHECK()                         \
  do                                             \
  {                                              \
    if (ALenum const error = ::alGetError())     \
    {                                            \
      std::cerr << ::alGetString(error) << '\n'; \
    }                                            \
  } while (0)



/**
 * audiorecorder constructor.
 *
 * @param char* _outputFilePath The output file path
 * @param int _numberOfSamplesPerSecond The number of samples to record per second
 */
audiorecorder::audiorecorder(const char* _outputFilePath, int _numberOfSamplesPerSecond)
{
  numberOfSamplesPerSecond = _numberOfSamplesPerSecond;
  device = NULL;
  context = NULL;
  wavWriter = new wavwriter(_outputFilePath, numberOfSamplesPerSecond, 2, 2);
}

// Getters and Setters

/**
 * Returns the ALCdevice that this audiorecorder uses.
 *
 * @return ALCdevice* The ALCdevice that this audiorecorder uses
 */
ALCdevice* audiorecorder::getDevice()
{
  return device;
}

/**
 * Returns the ALCcontext that this audiorecorder uses.
 *
 * @return ALCcontext* The ALCcontext that this audiorecorder uses
 */
ALCcontext* audiorecorder::getContext()
{
  return context;
}

// Public Methods

/**
 * Initializes this audiorecorder.
 */
void audiorecorder::init()
{
  device = initALCdevice();
  context = initALCcontext();
  wavWriter->init();
}

/**
 * Records audio samples for a given time in milliseconds.
 *
 * @param int _milliseconds The time in milliseconds for which to record audio samples
 */
void audiorecorder::recordSamples(int _milliseconds)
{
  int numberOfSamples = (_milliseconds * numberOfSamplesPerSecond) / 1000;
  ALshort data[numberOfSamples * 2];
  ::alcRenderSamplesSOFT(device, data, numberOfSamples);

  for (int i = 0; i < numberOfSamples; i++)
  {
    // front-left channel
    wavWriter->writeSampleChannelData((int)data[2 * i]);

    // front-right channel
    wavWriter->writeSampleChannelData((int)data[2 * i + 1]);
  }
}

/**
 * Finishes the audio recording by finalizing the wav file.
 */
void audiorecorder::finish()
{
  wavWriter->finish();
}


// Private Methods

/**
 * Initializes and returns a loopback ALCdevice.
 *
 * @return ALCdevice* The created ALCdevice
 */
ALCdevice* audiorecorder::initALCdevice()
{
  if (!::alcIsExtensionPresent(NULL, "ALC_SOFT_loopback"))
  {
    std::cerr << "ALC_SOFT_loopback not supported!\n";
    return NULL;
  }

  // load extensions
  ALC_LOAD_SOFT_EXTENSION(alcLoopbackOpenDeviceSOFT);
  ALC_LOAD_SOFT_EXTENSION(alcIsRenderFormatSupportedSOFT);
  ALC_LOAD_SOFT_EXTENSION(alcRenderSamplesSOFT);

  // open device
  ALCdevice *device = ::alcLoopbackOpenDeviceSOFT(NULL);
  if (!device)
  {
    std::cerr << "could not open a device\n";
    return NULL;
  }

  return device;
}

/**
 * Initializes and returns a ALCcontext for the loopback ALCdevice.
 *
 * @return ALCcontext* The created ALCcontext
 */
ALCcontext* audiorecorder::initALCcontext()
{
  ALCint attrs[16] = {
    ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
    ALC_FORMAT_TYPE_SOFT,     ALC_SHORT_SOFT,   // 16bit
    ALC_FREQUENCY,            numberOfSamplesPerSecond,
    0,                                          // end of attrs
  };
  if (::alcIsRenderFormatSupportedSOFT(device, attrs[5], attrs[1], attrs[3]) == ALC_FALSE)
  {
    std::cerr << "render format not supported: format-channels=" << attrs[1]
              << ", format-type=" << attrs[3] << ", frequency=" << attrs[5]
              << "hz\n";
  }

  context = ::alcCreateContext(device, attrs);
  if (!context)
  {
    std::cerr << "could not create a context\n";
  }

  return context;
}
