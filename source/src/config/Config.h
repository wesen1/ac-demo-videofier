/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include <string>

#ifndef CONFIG
#define CONFIG

class Config
{
  private:
    std::string getConfigValue(std::string, std::string);

  public:
    int framesPerSecond;
    int numberOfSamplesPerSecond;

    std::string framesPipeName;
    std::string audioPipeName;
    int maximumFramesQueueSize;
    int maximumAudioQueueSize;

    std::string renderedDemoOutputFilePath;

    std::string demoFileName;
    int followClientNumber;
    int startTimestamp;
    int endTimestamp;

    void load();
    void dump();
};

#endif
