/**
 * @author wesen
 * @copyright 2021 wesen <wesen-ac@web.de>
 */

/**
 * This class is based on https://www.cplusplus.com/forum/beginner/166954/
 */

#include <stddef.h>
#include <fstream>
#include "wavwriter.h"

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}

/**
 * wavwriter constructor.
 *
 * @param char* _outputFilePath The path to the output file
 * @param int _numberOfSamplesPerSecond The number of samples per second
 * @param int _numberOfBytesPerSample The number of bytes per sample
 * @param int _numberOfChannels The number of channels
 */
wavwriter::wavwriter(const char* _outputFilePath, int _numberOfSamplesPerSecond, int _numberOfBytesPerSample, int _numberOfChannels)
{
  outputFilePath = _outputFilePath;
  outputFile = NULL;

  numberOfSamplesPerSecond = _numberOfSamplesPerSecond;
  numberOfChannels = _numberOfChannels;
  numberOfBytesPerSample = _numberOfBytesPerSample;

  int wordByteSize = numberOfChannels * numberOfBytesPerSample;
  sampleWordSize = wordByteSize / 2 + (int)(wordByteSize % 2 > 0);
}

// Public Methods

/**
 * Initializes this wavwriter.
 */
void wavwriter::init()
{
  initWavFile();
}

/**
 * Writes data for a single sample channel to the wav file.
 *
 * @param int _channelData The channel data to write
 */
void wavwriter::writeSampleChannelData(int _channelData)
{
  little_endian_io::write_word( *outputFile, _channelData, sampleWordSize);
}

/**
 * Finalizes the wav file.
 */
void wavwriter::finish()
{
  finalizeWavFile();
}

// Private Methods

/**
 * Writes the wav header to the output wav file.
 */
void wavwriter::initWavFile()
{
  outputFile = new std::ofstream( outputFilePath, std::ios::binary );

  // Write the file headers
  *outputFile << "RIFF----WAVEfmt ";                                 // (chunk size to be filled in later)
  little_endian_io::write_word( *outputFile, 16, 4 );                // no extension data
  little_endian_io::write_word( *outputFile, 1, 2 );                 // PCM - integer samples
  little_endian_io::write_word( *outputFile, numberOfChannels, 2 );  // number of channels
  little_endian_io::write_word( *outputFile, numberOfSamplesPerSecond, 4 );  // samples per second (Hz)
  little_endian_io::write_word( *outputFile, numberOfSamplesPerSecond * numberOfBytesPerSample * numberOfChannels, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  little_endian_io::write_word( *outputFile, numberOfBytesPerSample * numberOfChannels, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  little_endian_io::write_word( *outputFile, numberOfBytesPerSample * 8, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  dataChunkPos = outputFile->tellp();
  *outputFile << "data----";  // (chunk size to be filled in later)
}

/**
 * Adds missing wav header information to the output wav file.
 */
void wavwriter::finalizeWavFile()
{
  // (We'll need the final file size to fix the chunk sizes)
  size_t fileLength = outputFile->tellp();

  // Fix the data chunk header to contain the data size
  outputFile->seekp( dataChunkPos + 4 );
  little_endian_io::write_word( *outputFile, fileLength - dataChunkPos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  outputFile->seekp( 0 + 4 );
  little_endian_io::write_word( *outputFile, fileLength - 8, 4 );
}
