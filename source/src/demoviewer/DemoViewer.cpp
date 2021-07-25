/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "DemoViewer.h"
#include "cube.h"

extern void shiftgametime(int, bool);
extern void setfollowplayer(int);


/**
 * DemoViewer constructor.
 *
 * @param const char[] _demoFileName The name of the demo file to view
 * @param int _startTimestamp The start timestamp in the demo when to start viewing
 * @param int _endTimestamp The end timestamp in the demo when to stop viewing
 * @param int _followClientNumber The client number of the player to spectate while viewing the demo
 */
DemoViewer::DemoViewer(const char* _demoFileName, int _startTimestamp, int _endTimestamp, int _followClientNumber)
{
  demoFileName = _demoFileName;
  startTimestamp = _startTimestamp;
  endTimestamp = _endTimestamp;
  followClientNumber = _followClientNumber;
  isSetupFinished = false;
}


// Getters and Setters

/**
 * Returns whether the demo viewing setup is finished.
 *
 * @return bool True if the demo viewing setup is finished, false otherwise
 */
bool DemoViewer::getIsSetupFinished()
{
  return isSetupFinished;
}


// Public Methods

/**
 * Starts the demo playback.
 */
void DemoViewer::startDemoPlayback()
{
  callvote(SA_MAP, demoFileName, "-1", "");
  shiftgametime(startTimestamp, true);
}

/**
 * Tries to finish the demo viewing setup.
 */
void DemoViewer::tryFinishSetup()
{
  if (isSetupFinished || !watchingdemo) return;

  if (gametimecurrent >= startTimestamp)
  {
    isSetupFinished = true;
    setfollowplayer(followClientNumber);
  }
}

/**
 * Returns whether the demo viewing is finished.
 *
 * @return bool True if the demo viewing is finished, false otherwise
 */
bool DemoViewer::isFinished()
{
  return (watchingdemo && gametimecurrent > endTimestamp);
}
