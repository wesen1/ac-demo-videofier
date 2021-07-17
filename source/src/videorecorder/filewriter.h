/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef FILE_WRITER
#define FILE_WRITER

#include <queue>
#include <thread>
#include <condition_variable>

class filewriter
{
 private:
  const char* outputFilePath;
  int maximumCacheSize;

  int outputFileDescriptor;
  std::queue<unsigned char*>* nextData;
  std::queue<int>* nextDataSizes;
  std::thread* currentWriteThread;
  bool currentWriteFinished;

  void doWriteData(unsigned char*, int, std::condition_variable*);


 public:
  filewriter(const char*, int);

  void init();
  void addDataToWrite(unsigned char*, int);
  bool hasDataToWrite();
  bool isMaximumCacheSizeReached();
  bool isWriteInProgress();
  void startNextWrite(std::condition_variable*);
  void finish();
};

#endif
