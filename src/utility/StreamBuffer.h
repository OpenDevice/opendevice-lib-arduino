#ifndef StreamBuffer_h
#define StreamBuffer_h

#include <inttypes.h>
#include <Stream.h>
#include "Command.h"

class StreamBuffer : public Stream
{
protected:
	int timedRead() { return read(); }
	int timedPeek() { return peek(); }
private:
   const uint16_t _len;
   bool _buffer_overflow;
   uint16_t _endOffset;

   volatile uint8_t _readOffset;

   int nextEndOffSet();
   bool isListEnd(char c);

public:
   uint8_t * _buffer;

  // public methods
  StreamBuffer(uint8_t *buffer, const uint16_t len, bool clean = true);
  ~StreamBuffer() {}

  operator const uint8_t *() const { return _buffer; }

  const uint16_t current_length() const { return _endOffset; }

  void begin(long speed) {}
  bool listen() { return true; }
  void end() {}
  bool isListening() { return true; }
  bool overflow() { return _buffer_overflow; }

  virtual size_t write(uint8_t byte);
  virtual int peek();
  virtual int read();
  virtual int available();
  virtual void flush();

  String readString();
  int readLongValues(long values[], int max);
  int readIntValues(int values[], int max);
  int readFloatValues(float values[], int max);

  int getReadOffset(){ return _readOffset; };

  using Print::write;
};

#endif
