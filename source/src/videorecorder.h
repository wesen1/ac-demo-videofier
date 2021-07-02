/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef VIDEO_RECORDER
#define VIDEO_RECORDER

class videorecorder
{
  private:
    const char* outputFilePath;
    SDL_Surface *screen;
    int framesPerSecond;
    FILE *ffmpeg;

  public:
    videorecorder(const char*, SDL_Surface*, int);
    void init();
    void recordFrame();
    void finish();
};

#endif
