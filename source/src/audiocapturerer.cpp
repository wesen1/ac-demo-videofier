/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

/**
 * The OpenAL soft rendering code is based on https://gist.github.com/skitaoka/13a0ffa3f6434ca03e87
 */

#include "audiocapturerer.h"
#include <iostream>
#include <memory>
#include <math.h>
#include "AL/alext.h"

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
 * audiocapturerer constructor.
 *
 * @param int _numberOfSamplesPerSecond The number of samples to capture per second
 */
audiocapturerer::audiocapturerer(int _numberOfSamplesPerSecond)
{
  numberOfSamplesPerSecond = _numberOfSamplesPerSecond;
  device = NULL;
  context = NULL;
}


// Getters and Setters

/**
 * Returns the ALCdevice that this audiocapturerer uses.
 *
 * @return ALCdevice* The ALCdevice
 */
ALCdevice* audiocapturerer::getDevice()
{
  return device;
}

/**
 * Returns the ALCcontext that this audiocapturerer uses.
 *
 * @return ALCcontext* The ALCcontext
 */
ALCcontext* audiocapturerer::getContext()
{
  return context;
}

/**
 * Returns the number of samples per second that this audiocapturerer uses.
 *
 * @return int The number of samples per second
 */
int audiocapturerer::getNumberOfSamplesPerSecond()
{
  return numberOfSamplesPerSecond;
}

/**
 * Returns the number of channels that this audiocapturerer uses.
 *
 * @return int The number of channels
 */
int audiocapturerer::getNumberOfChannels()
{
  /*
   * The number of channels is always 2 because ALC_STEREO_SOFT (front-left and front-right channel)
   * is selected as channel format in audiocapturerer::initALCcontext()
   */
  return 2;
}

/**
 * Returns the channel data size in bytes that this audiocapturerer uses.
 *
 * @return int The channel data size in byte
 */
int audiocapturerer::getChannelDataSizeInBytes()
{
  /*
   * The channel data size is always 2 because ALC_SHORT_SOFT (16 bit)
   * is selected as type format in audiocapturerer::initALCcontext()
   */
  return 2;
}


// Public Methods

/**
 * Initializes this audiocapturerer.
 */
void audiocapturerer::init()
{
  device = initALCdevice();
  context = initALCcontext();
}

/**
 * Calculates the size in bytes of the samples that will be captured for a given number of milliseconds.
 *
 * @param int _numberOfMilliseconds The number of milliseconds
 *
 * @return int The size in bytes of the samples that will be captured for the given number of milliseconds
 */
int audiocapturerer::calculateS16leSamplesSizeInBytes(int _numberOfMilliseconds)
{
  int numberOfSamples = calculateNumberOfSamples(_numberOfMilliseconds);
  int numberOfChannels = getNumberOfChannels();
  int channelDataSizeInBytes = getChannelDataSizeInBytes();

  return numberOfSamples * numberOfChannels * channelDataSizeInBytes;
}

/**
 * Captures and returns s16le audio samples for a given time in milliseconds.
 *
 * @param int _numberOfMilliseconds The time in milliseconds for which to capture audio samples
 *
 * @return unsigned char[] The list of captured s16le samples
 */
unsigned char* audiocapturerer::captureS16leSamples(int _numberOfMilliseconds)
{
  int numberOfSamples = calculateNumberOfSamples(_numberOfMilliseconds);
  int numberOfChannels = getNumberOfChannels();
  int channelDataSizeInBytes = getChannelDataSizeInBytes();

  ALshort data[numberOfSamples * numberOfChannels];
  ::alcRenderSamplesSOFT(device, data, numberOfSamples);

  int s16leSamplesArraySize = calculateS16leSamplesSizeInBytes(_numberOfMilliseconds);
  unsigned char* s16leSamples = static_cast<unsigned char *>(malloc(s16leSamplesArraySize * sizeof(unsigned char *)));
  int dataIndex = 0;
  int samplesListIndex = 0;
  for (int i = 0; i < numberOfSamples; i++)
  {
    for (int j = 0; j < numberOfChannels; j++)
    {
      unsigned char* s16leSamplesForChannelData = createS16leSamplesFromChannelData(
        (int)data[dataIndex],
        channelDataSizeInBytes
      );

      for (int k = 0; k < channelDataSizeInBytes; k++)
      {
        s16leSamples[samplesListIndex] = s16leSamplesForChannelData[k];
        samplesListIndex++;
      }

      free(s16leSamplesForChannelData);

      dataIndex++;
    }
  }

  return s16leSamples;
}


// Private Methods

/**
 * Initializes and returns a loopback ALCdevice.
 *
 * @return ALCdevice* The created ALCdevice
 */
ALCdevice* audiocapturerer::initALCdevice()
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
ALCcontext* audiocapturerer::initALCcontext()
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

/**
 * Calculates and returns the number of samples that need to be captured for a given number of milliseconds.
 *
 * @param int _numberOfMilliseconds The number of milliseconds
 *
 * @return int The number of samples that need to be captured for the given number of milliseconds
 */
int audiocapturerer::calculateNumberOfSamples(int _numberOfMilliseconds)
{
  return ceil((_numberOfMilliseconds * numberOfSamplesPerSecond) / 1000);
}

/**
 * Creates and returns s16le samples from given raw audio channel data.
 * This is based on https://www.cplusplus.com/forum/beginner/166954/.
 *
 * @param int _channelData The channel data to create s16le samples from
 * @param int _channelDataSizeInBytes The channel data size in bytes
 *
 * @return unsigned char[] The created s16le samples
 */
unsigned char* audiocapturerer::createS16leSamplesFromChannelData(int _channelData, int _channelDataSizeInBytes)
{
  unsigned char* s16leSamples = static_cast<unsigned char *>(malloc(_channelDataSizeInBytes * sizeof(unsigned char *)));

  for (int i = 0; i < _channelDataSizeInBytes; i++)
  {
    s16leSamples[i] = static_cast<unsigned char>(_channelData & 0xff);
    _channelData >>= 8;
  }

  return s16leSamples;
}
