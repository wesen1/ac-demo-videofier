/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "videorecorder.h"
#include <sys/stat.h>

/**
 * videorecorder constructor.
 *
 * @param videocapturerer* _videocapturerer The videocapturerer to use
 * @param audiocapturerer* _audiocapturerer The audiocapturerer to use
 * @param filewriter* _videoFileWriter The filewriter to use for video data
 * @param filewriter* _audioFileWriter The filewriter to use for audio data
 * @param char[] _renderedDemoOutputFilePath The file path to write the generated video file to
 */
videorecorder::videorecorder(class videocapturerer* _videocapturerer, class audiocapturerer* _audiocapturerer, class filewriter* _videoFileWriter, class filewriter* _audioFileWriter, const char* _renderedDemoOutputFilePath)
{
  audiocapturerer = _audiocapturerer;
  videocapturerer = _videocapturerer;
  audioFileWriter = _audioFileWriter;
  videoFileWriter = _videoFileWriter;
  renderedDemoOutputFilePath = _renderedDemoOutputFilePath;

  conditionVariable = new std::condition_variable();
  mutex = new std::mutex();
}


// Public Methods

/**
 * Initializes the audio capturing and starts the ffmpeg process that merges video and audio.
 *
 * @param audiomanager* _audiomanager The audiomanager to capture audio from
 */
void videorecorder::init(audiomanager* _audiomanager)
{
  audiocapturerer->init();

  _audiomanager->setDevice(audiocapturerer->getDevice());
  _audiomanager->setContext(audiocapturerer->getContext());

  mkfifo(videoFileWriter->getOutputFilePath(), 0666);
  mkfifo(audioFileWriter->getOutputFilePath(), 0666);
  ffmpegThread = new std::thread(&videorecorder::startFfmpegBackgroundProcess, this);
}

/**
 * Records the next frame video and audio data.
 * Also writes the next audio or video data to the output files.
 *
 * Note:
 * The video recording relies on the slow calculations of the next frame data, if there is not
 * enough time between the frame data writes then the frames in the video will sometimes be in the
 * wrong order.
 *
 * @param int _elapsedTimeInMilliseconds The elapsed time in milliseconds for which to capture audio data
 */
void videorecorder::recordNextFrame(int _elapsedTimeInMilliseconds)
{
  videoFileWriter->addDataToWrite(
    videocapturerer->captureFrame(),
    videocapturerer->calculateFrameSizeInPixels()
  );

  audioFileWriter->addDataToWrite(
    audiocapturerer->captureS16leSamples(_elapsedTimeInMilliseconds),
    audiocapturerer->calculateS16leSamplesSizeInBytes(_elapsedTimeInMilliseconds)
  );

  bool dataWritten;
  do
  {
    dataWritten = writeNextData();
  }
  while (dataWritten && (videoFileWriter->isMaximumCacheSizeReached() || audioFileWriter->isMaximumCacheSizeReached()));
}

/**
 * Cancels the video recording.
 */
void videorecorder::cancel()
{
  ffmpegThread = NULL;
  finish();
}

/**
 * Finishes recording.
 * Writes the remaining cached video and audio data and finishes the file writers.
 */
void videorecorder::finish()
{
  writeRemainingCachedData();
  videoFileWriter->finish();
  audioFileWriter->finish();

  if (ffmpegThread)
  {
    ffmpegThread->join();
  }
}


// Private Methods

/**
 * Starts a ffmpeg process that merges the videorecorder audio and video output into a single output file.
 */
void videorecorder::startFfmpegBackgroundProcess()
{
  char ffmpegCommand[255];
  sprintf(
    ffmpegCommand,
    "ffmpeg -y -f rawvideo -video_size %dx%d -pixel_format rgb24 -framerate %d -i %s -f s16le -sample_rate %d -channels %d -async 0 -i %s -vf vflip -pix_fmt yuv420p %s",
    videocapturerer->getScreen()->w,
    videocapturerer->getScreen()->h,
    videocapturerer->getNumberOfFramesPerSecond(),
    videoFileWriter->getOutputFilePath(),
    audiocapturerer->getNumberOfSamplesPerSecond(),
    audiocapturerer->getNumberOfChannels(),
    audioFileWriter->getOutputFilePath(),
    renderedDemoOutputFilePath
  );

  system(ffmpegCommand);
}

/**
 * Writes the remaining cached video and audio data to the output files.
 */
void videorecorder::writeRemainingCachedData()
{
  clientlogf("Writing remaining cached data ...");
  while (writeNextData())
  {
    clientlogf("Remaining cached data: frames = %d; audio = %d",
               videoFileWriter->remainingDataToWriteCount(),
               audioFileWriter->remainingDataToWriteCount());

    if (!videoFileWriter->isWriteInProgress() && !videoFileWriter->hasDataToWrite())
    {
      videoFileWriter->finish();
    }

    if (!audioFileWriter->isWriteInProgress() && !audioFileWriter->hasDataToWrite())
    {
      audioFileWriter->finish();
    }
  }
}

/**
 * Starts the next audio and video writing if required and waits for either the current
 * audio or video writing to finish.
 *
 * @return bool True if the next data was successfully written, false if no new data could be written
 */
bool videorecorder::writeNextData()
{
  if (videoFileWriter->getIsFinished() && audioFileWriter->getIsFinished())
  {
    return false;
  }

  bool frameWriteInProgress = videoFileWriter->isWriteInProgress();
  if (!frameWriteInProgress && videoFileWriter->hasDataToWrite())
  {
    videoFileWriter->startNextWrite(conditionVariable);
    frameWriteInProgress = true;
  }

  bool audioWriteInProgress = audioFileWriter->isWriteInProgress();
  if (!audioWriteInProgress && audioFileWriter->hasDataToWrite())
  {
    audioFileWriter->startNextWrite(conditionVariable);
    audioWriteInProgress = true;
  }

  if ((frameWriteInProgress || videoFileWriter->getIsFinished()) &&
      (audioWriteInProgress || audioFileWriter->getIsFinished()))
  {
    std::unique_lock<std::mutex> lock(*mutex);
    while ((videoFileWriter->isWriteInProgress() || videoFileWriter->getIsFinished()) &&
           (audioFileWriter->isWriteInProgress() || audioFileWriter->getIsFinished()))
    {
      conditionVariable->wait(lock);
    }

    return true;
  }
  else
  {
    return false;
  }
}
