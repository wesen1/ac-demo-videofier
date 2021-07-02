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
 * @param char* _outputFilePath The output file to use
 * @param SDL_Surface* _screen The screen to record
 * @param int _framesPerSecond The frames per config setting to use for the resulting video file
 */
videorecorder::videorecorder(const char* _outputFilePath, SDL_Surface *_screen, int _framesPerSecond)
{
  outputFilePath = _outputFilePath;
  screen = _screen;
  framesPerSecond = _framesPerSecond;
}

// Public Methods

/**
 * Initializes the video recorder by starting a ffmpeg process to which recorded frames can be written.
 */
void videorecorder::init()
{
  char command[150];
  sprintf(command, "ffmpeg -y -f rawvideo -s %dx%d -pix_fmt rgb24 -r %d -i - -vf vflip -an -b:v 20000k %s", screen->w, screen->h, framesPerSecond, outputFilePath);
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
