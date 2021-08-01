/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "Config.h"


// Public Methods

/**
 * Loads the config values.
 */
void Config::load()
{
  // Video settings
  framesPerSecond = 60;
  numberOfSamplesPerSecond = 48000; // 48kHz

  // Data transmission settings
  framesPipeName = "/tmp/frames";
  audioPipeName = "/tmp/audio";
  maximumFramesQueueSize = 60;
  maximumAudioQueueSize = 60;

  // Output file
  renderedDemoOutputFilePath = getConfigValue("OUTPUT_FILE", "./demo.mp4");

  // DemoViewer config
  demoFileName = getConfigValue("DEMO_NAME", "example");
  followClientNumber = stoi(getConfigValue("FOLLOW_CLIENT_NUMBER", "0"));
  startTimestamp = stoi(getConfigValue("START_TIMESTAMP", "0")); // Spawn - 3000
  endTimestamp = stoi(getConfigValue("END_TIMESTAMP", "5000")); // Score + 3000
}

/**
 * Prints the loaded config.
 */
void Config::dump()
{
  printf("Config:\n");
  printf("  Video:\n");
  printf("  - Frames per second: %d\n", framesPerSecond);
  printf("  - Samples per second: %d\n", numberOfSamplesPerSecond);
  printf("  Data transmission:\n");
  printf("  - Frames pipe name: %s\n", framesPipeName.c_str());
  printf("  - Audio pipe name: %s\n", audioPipeName.c_str());
  printf("  - Maximum frame queue size: %d\n", maximumFramesQueueSize);
  printf("  - Maximum audio queue size: %d\n", maximumAudioQueueSize);
  printf("  Output:\n");
  printf("  - Video output file: %s\n", renderedDemoOutputFilePath.c_str());
  printf("  DemoViewer:\n");
  printf("  - Demo file name: %s\n", demoFileName.c_str());
  printf("  - Follow client number: %d\n", followClientNumber);
  printf("  - Start timestamp: %d\n", startTimestamp);
  printf("  - End timestamp: %d\n", endTimestamp);
}


// Private Methods

/**
 * Loads a config value from the corresponding environment variable.
 *
 * @param const std::string _configValueName The name of the config value
 * @param const std::string _defaultValue The default value to return if the environment variable is not set
 *
 * @return std::string The config value
 */
std::string Config::getConfigValue(const std::string _configValueName, const std::string _defaultValue)
{
  char* rawValue = std::getenv(_configValueName.c_str());
  if (rawValue) return std::string(rawValue);
  else return _defaultValue;
}
