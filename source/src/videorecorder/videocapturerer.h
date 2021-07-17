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

 public:
  videocapturerer(SDL_Surface*);

  int calculateFrameSizeInPixels();
  unsigned char* captureFrame();
};

#endif
