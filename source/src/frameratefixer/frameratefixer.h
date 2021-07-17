/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef FRAME_RATE_FIXER
#define FRAME_RATE_FIXER

class frameratefixer
{
  private:
    int framesPerSecond;
    int* millisecondsPerFrame;
    int* calculateMillisecondsPerFrame(int);

  public:
    frameratefixer(int);
    int calculateTotalMilliseconds(int);
};

#endif
