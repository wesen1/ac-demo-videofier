/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "Pong.h"


// Public Methods

/**
 * Extracts the values for this message from a given buffer.
 *
 * @param ucharbuf _buffer The buffer to extract the message values from
 */
void Pong::extractDataFromBuffer(ucharbuf* _buffer)
{
  millis = getint(*_buffer);
}
