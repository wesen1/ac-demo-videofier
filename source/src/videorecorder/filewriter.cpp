/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "filewriter.h"
#include <fcntl.h>
#include <unistd.h>

/**
 * filewriter constructor.
 *
 * @param char[] _outputFilePath The path to the output file
 * @param int _maximumCacheSize The maximum amount of data to store in the data queue
 */
filewriter::filewriter(const char* _outputFilePath, int _maximumCacheSize)
{
  outputFilePath = _outputFilePath;
  maximumCacheSize = _maximumCacheSize;

  currentWriteThread = NULL;
  currentWriteFinished = false;

  nextData = new std::queue<unsigned char*>;
  nextDataSizes = new std::queue<int>;

  isFinished = false;
}


// Getters and Setters

/**
 * Returns the output file path.
 *
 * @return char[] The output file path
 */
const char* filewriter::getOutputFilePath()
{
  return outputFilePath;
}

/**
 * Returns whether this file writer was finished via filewriter::finish().
 *
 * @return bool True if this file writer is finished, false otherwise
 */
bool filewriter::getIsFinished()
{
  return isFinished;
}


// Public Methods

/**
 * Initializes this filewriter.
 * Opens the output file.
 */
void filewriter::init()
{
  /*
   * The output file opening can not be done in the constructor because if the output file is a pipe
   * the open() call will block until a different process reads from the pipe.
   */
  outputFileDescriptor = open(outputFilePath, O_WRONLY);
}

/**
 * Adds data that should be written to the output file.
 *
 * @param unsigned char[] _data The data to write
 * @param int _dataSize The size of the data array
 */
void filewriter::addDataToWrite(unsigned char* _data, int _dataSize)
{
  nextData->push(_data);
  nextDataSizes->push(_dataSize);
}

/**
 * Returns whether the maximum data queue size is reached.
 *
 * @return bool True if the maximum data queue size is reached, false otherwise
 */
bool filewriter::isMaximumCacheSizeReached()
{
  return (nextData->size() >= maximumCacheSize);
}

/**
 * Returns whether there is data in the data queue.
 *
 * @return bool True if there is data in the data queue, false otherwise
 */
bool filewriter::hasDataToWrite()
{
  return (!nextData->empty());
}

/**
 * Return whether a data write is currently in progress.
 *
 * @return bool True if a data write is currently in progress, false otherwise
 */
bool filewriter::isWriteInProgress()
{
  return (currentWriteThread != NULL && !currentWriteFinished);
}

/**
 * Starts the next data write.
 * Starts a thread that writes the next entry from the data queue to the output file.
 *
 * @tparam std::condition_variable* _conditionVariable The condition variable to notify when the data write is finished
 */
void filewriter::startNextWrite(std::condition_variable* _conditionVariable)
{
  if (currentWriteThread) currentWriteThread->join();

  currentWriteFinished = false;
  currentWriteThread = new std::thread(
    &filewriter::doWriteData,
    this,
    nextData->front(),
    nextDataSizes->front(),
    _conditionVariable
  );

  nextData->pop();
  nextDataSizes->pop();
}

/**
 * Finishes this filewriter.
 * Closes the output file.
 */
void filewriter::finish()
{
  if (isFinished) return;

  isFinished = true;
  close(outputFileDescriptor);
}


// Private Methods

/**
 * Writes data to the output file.
 *
 * @param unsigned char[] _data The data to write
 * @param int _dataSize The size of the data array
 * @param std::condition_variable* _conditionVariable The condition variable to notify when the writing is finished
 */
void filewriter::doWriteData(unsigned char* _data, int _dataSize, std::condition_variable* _conditionVariable)
{
  if (!outputFileDescriptor)
  { // Open the output file here to avoid blocking in the main thread if the output file is a pipe
    init();
  }

  write(outputFileDescriptor, _data, _dataSize);
  free(_data);

  currentWriteFinished = true;
  _conditionVariable->notify_all();
}
