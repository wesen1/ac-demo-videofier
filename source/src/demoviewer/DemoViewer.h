/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#ifndef DEMO_VIEWER
#define DEMO_VIEWER

class DemoViewer
{
  private:
    const char* demoFileName;
    int startTimestamp;
    int endTimestamp;
    int followClientNumber;
    bool isSetupFinished;

  public:
    DemoViewer(const char*, int, int, int);
    bool getIsSetupFinished();
    void startDemoPlayback();
    void tryFinishSetup();
    bool isFinished();
};

#endif
