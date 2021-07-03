/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

/*
 * This class is based on https://stackoverflow.com/a/25921244.
 */

#include "cube.h"
#include "videorecorder.h"

/**
 * videorecorder constructor.
 *
 * @param char* _videoOutputFilePath The output file to use for the video (without sound)
 * @param char* _audioOutputFilePath The output file that will be used for the audio
 * @param char* _renderedDemoOutputFilePath The output file to write the combined video and audio to
 * @param SDL_Surface* _screen The screen to record
 * @param int _framesPerSecond The frames per config setting to use for the resulting video file
 */
videorecorder::videorecorder(const char* _videoOutputFilePath, const char* _audioOutputFilePath, const char* _renderedDemoOutputFilePath, SDL_Surface *_screen, int _framesPerSecond)
{
  videoOutputFilePath = _videoOutputFilePath;
  audioOutputFilePath = _audioOutputFilePath;
  renderedDemoOutputFilePath = _renderedDemoOutputFilePath;
  screen = _screen;
  framesPerSecond = _framesPerSecond;
}

// Public Methods

/**
 * Initializes the video recorder by starting a ffmpeg process to which recorded frames can be written.
 */
void videorecorder::init()
{
  char videoRecordCommand[150];
  sprintf(videoRecordCommand, "ffmpeg -y -f rawvideo -s %dx%d -pix_fmt rgb24 -r %d -i - -vf vflip -an -b:v 20000k %s", screen->w, screen->h, framesPerSecond, videoOutputFilePath);

  char demoRenderCommand[100];
  sprintf(demoRenderCommand, "ffmpeg -y -i %s -i %s %s", videoOutputFilePath, audioOutputFilePath, renderedDemoOutputFilePath);

  char command[255];
  sprintf(command, "%s && %s", videoRecordCommand, demoRenderCommand);

  ffmpeg = popen(command, "w");
}

/**
 * Records the current frame and adds it to the ffmpeg process.
 */
void videorecorder::recordFrame()
{
  uchar *pixels = new uchar[3 * screen->w * screen->h];
  glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  if (ffmpeg)
  {
    fwrite(pixels, screen->w * screen->h * 3, 1, ffmpeg);
  }

  free(pixels);
}

/**
 * Finishes recording.
 */
void videorecorder::finish()
{
  if (ffmpeg)
  {
    pclose(ffmpeg);
  }
}
