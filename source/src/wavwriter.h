/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef WAV_WRITER
#define WAV_WRITER

class wavwriter
{
  private:
    // Output file
    const char* outputFilePath;
    std::ofstream *outputFile;
    size_t dataChunkPos;

    // WAV config
    int numberOfSamplesPerSecond;
    int numberOfChannels;
    int numberOfBytesPerSample;
    int sampleWordSize;

    void initWavFile();
    void finalizeWavFile();

  public:
    wavwriter(const char*, int, int, int);
    void init();
    void writeSampleChannelData(int);
    void finish();
};

#endif
