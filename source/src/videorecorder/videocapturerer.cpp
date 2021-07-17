/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "videocapturerer.h"

/**
 * videocapturerer constructor.
 *
 * @param SDL_Surface* _screen The screen to record
 * @param int _numberOfFramesPerSecond The number of frames per second
 */
videocapturerer::videocapturerer(SDL_Surface* _screen, int _numberOfFramesPerSecond)
{
  screen = _screen;
  numberOfFramesPerSecond = _numberOfFramesPerSecond;
}


// Getters and Setters

/**
 * Returns the screen that this videocapturerer uses.
 *
 * @return SDL_Surface* The screen
 */
SDL_Surface* videocapturerer::getScreen()
{
  return screen;
}

/**
 * Returns the number of frames per second that this videocapturerer uses.
 *
 * @return int The number of frames per second
 */
int videocapturerer::getNumberOfFramesPerSecond()
{
  return numberOfFramesPerSecond;
}


// Public Methods

/**
 * Calculates and returns the frame size in pixels of a captured frame.
 *
 * @return int The frame size in pixels
 */
int videocapturerer::calculateFrameSizeInPixels()
{
  return 3 * screen->w * screen->h;
}

/**
 * Captures and returns the currently shown frame.
 *
 * @return unsigned char[] The pixels of the currently shown frame
 */
unsigned char* videocapturerer::captureFrame()
{
  int pixelArraySize = calculateFrameSizeInPixels();
  unsigned char* pixels = new unsigned char[pixelArraySize];
  glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  return pixels;
}
