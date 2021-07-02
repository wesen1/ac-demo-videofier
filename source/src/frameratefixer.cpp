/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "frameratefixer.h"
#include <cmath>

/**
 * frameratefixer constructor.
 *
 * @param int _framesPerSecond The number of frames per second to fix the frame rate to
 */
frameratefixer::frameratefixer(int _framesPerSecond)
{
  framesPerSecond = _framesPerSecond;
  millisecondsPerFrame = calculateMillisecondsPerFrame(_framesPerSecond);
}

// Public Methods

/**
 * Calculates and returns the total number of milliseconds based on a given number of frames.
 *
 * @param int _frames The number of frames
 *
 * @return int The calculated total number of milliseconds
 */
int frameratefixer::calculateTotalMilliseconds(int _frames)
{
  int millis = (int)(_frames / framesPerSecond) * 1000; // Full seconds

  int currentFrameOfSecond = _frames % framesPerSecond;
  if (currentFrameOfSecond > 0)
  { // Current partial second
    for (int i = 1; i <= currentFrameOfSecond; i++)
    {
      millis += millisecondsPerFrame[i];
    }
  }

  return millis;
}

// Private Methods

/**
 * Calculates and returns the milliseconds to use per frame number per second.
 * Returns a reference to an array in the format [ <frame number> => <milliseconds>, ... ].
 *
 * @param int _framesPerSecond The number of frames that should be rendered per second
 *
 * @return int* A reference to an array containing the milliseconds to use per frame number per second
 */
int* frameratefixer::calculateMillisecondsPerFrame(int _framesPerSecond)
{
  int *millisecondsPerFrame = static_cast<int *>(malloc(_framesPerSecond * sizeof(int *)));

  // Fill the array with the default minimum milliseconds per frame
  int minimumMillisecondsPerFrame = 1000 / _framesPerSecond;
  for (int i = 0; i < _framesPerSecond; i++)
  {
    millisecondsPerFrame[i] = minimumMillisecondsPerFrame;
  }

  /*
   * Distribute the remaining milliseconds evenly accross the frames.
   * The remaining milliseconds are always < _framesPerSecond, so 1 additional millisecond must be added
   * to some frames until all of these remaining milliseconds are used up.
   *
   * The algorithm calculates the highest power of 2 that fits into the number of frames per second
   * (e.g. 5 (-> 32) for 60 frames per second).
   *
   * Then it adds 1 additional second to every 2^x frame (alternating between "right" and "left",
   * e.g. 32, 48, 16, 56, 8, 40, 24, ... for 60 FPS), decreases the power by 1 and repeats the process
   * while ignoring frame numbers that are dividable by one of the previous powers.
   * When it reaches 2^0 it repeats the process for the uneven numbers.
   */
  int highestPowerOfTwo = log(_framesPerSecond) / log(2);

  int remainingAdditionalFrameMillisecondsPerSecond = 1000 % _framesPerSecond;
  int divider = pow(2, highestPowerOfTwo);
  int offset = 0;

  while (remainingAdditionalFrameMillisecondsPerSecond > 0)
  {
    if (divider == 1 && offset == 0)
    {
      divider = pow(2, highestPowerOfTwo);
      offset = 1;
    }

    int highestMultipleOfDivider = (int)(_framesPerSecond / divider) * divider;
    int nextMultipleOfDivider = highestMultipleOfDivider + ((int)(_framesPerSecond % divider > 0) * divider);
    int distanceToZero = offset;
    int distanceToNextMultipleOfDivider = offset * -1;
    int frameNumber;
    for (int i = 1; i <= highestMultipleOfDivider / divider; i++)
    {
      if (i % 2 == 0)
      { // Pick the next multiple of the divider from the "left"
        distanceToZero += divider;
        frameNumber = distanceToZero;
      }
      else
      { // Pick the next multiple of the divider from the "right"
        distanceToNextMultipleOfDivider += divider;
        frameNumber = nextMultipleOfDivider - distanceToNextMultipleOfDivider;
      }

      if ((frameNumber - offset) % (divider * 2) > 0)
      { // Not dividable by next higher power of 2
        millisecondsPerFrame[frameNumber]++;

        remainingAdditionalFrameMillisecondsPerSecond--;
        if (remainingAdditionalFrameMillisecondsPerSecond == 0) break;
      }
    }

    divider /= 2;
  }

  return millisecondsPerFrame;
}
