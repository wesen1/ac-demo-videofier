/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

#include "Itemspawn.h"


// Public Methods

/**
 * Extracts the values for this message from a given buffer.
 *
 * @param ucharbuf _buffer The buffer to extract the message values from
 */
void Itemspawn::extractDataFromBuffer(ucharbuf* _buffer)
{
  itemId = getint(*_buffer);
}
