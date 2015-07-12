#ifndef CommandBuffer_h
#define CommandBuffer_h

#include <inttypes.h>
#include <Print.h>
#include "Command.h"

class CommandBuffer : public Print
{
protected:
	int timedRead() { return read(); }
	int timedPeek() { return peek(); }
	long parseInt(char skipChar); // as above but the given skipChar is ignored
	float parseFloat(char skipChar);  // as above but the given skipChar is ignored
	int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout

private:
   char * _buffer;
   const uint16_t _len;
   bool _buffer_overflow;
   uint16_t _endOffset;

   volatile uint8_t _readOffset;

   int nextEndOffSet();
   bool isListEnd(char c);

public:
  // public methods
  CommandBuffer(char *buffer, const uint16_t len, bool clean = true);
  ~CommandBuffer() {}

  operator const char *() const { return _buffer; }

  const uint16_t current_length() const { return _endOffset; }

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

  long parseInt(); // returns the first valid (long) integer value from the current position.
  float parseFloat();               // float version of parseInt


  int getReadOffset(){ return _readOffset; };

  using Print::write;
};

#endif
