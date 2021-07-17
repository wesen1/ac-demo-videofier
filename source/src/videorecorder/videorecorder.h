/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef VIDEO_RECORDER
#define VIDEO_RECORDER

#include "videocapturerer.h"
#include "audiocapturerer.h"
#include "filewriter.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include "cube.h"

class videorecorder
{
  private:
    audiocapturerer* audiocapturerer;
    videocapturerer* videocapturerer;
    filewriter* audioFileWriter;
    filewriter* videoFileWriter;
    const char* renderedDemoOutputFilePath;
    std::thread* ffmpegThread;
    std::condition_variable* conditionVariable;
    std::mutex* mutex;

    void startFfmpegBackgroundProcess();
    void writeRemainingCachedData();
    bool writeNextData();

  public:
    videorecorder(class videocapturerer*, class audiocapturerer*, class filewriter*, class filewriter*, const char*);
    void init(audiomanager*);
    void recordNextFrame(int);
    void finish();
};

#endif
