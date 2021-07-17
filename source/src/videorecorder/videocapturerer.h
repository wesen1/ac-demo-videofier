/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef VIDEO_CAPTURERER
#define VIDEO_CAPTURERER

#include "platform.h"

class videocapturerer
{
 private:
  SDL_Surface* screen;
  int numberOfFramesPerSecond;

 public:
  videocapturerer(SDL_Surface*, int);

  SDL_Surface* getScreen();
  int getNumberOfFramesPerSecond();

  int calculateFrameSizeInPixels();
  unsigned char* captureFrame();
};

#endif
