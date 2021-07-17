/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef VIDEO_RECORDER
#define VIDEO_RECORDER

#include "videocapturerer.h"
#include "audiocapturerer.h"
#include "filewriter.h"
#include <mutex>
#include <condition_variable>
#include "cube.h"

class videorecorder
{
  private:
    audiocapturerer* audiocapturerer;
    videocapturerer* videocapturerer;
    filewriter* audioFileWriter;
    filewriter* videoFileWriter;
    std::condition_variable* conditionVariable;
    std::mutex* mutex;

    void writeRemainingCachedData();
    bool writeNextData();

  public:
    videorecorder(class videocapturerer*, class audiocapturerer*, class filewriter*, class filewriter*);
    void init(audiomanager*);
    void recordNextFrame(int);
    void finish();
};

#endif
