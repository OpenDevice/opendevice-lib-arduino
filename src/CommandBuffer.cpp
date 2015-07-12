#include "CommandBuffer.h"

#define NO_SKIP_CHAR  1  // a magic char not found in a valid ASCII numeric field

CommandBuffer::CommandBuffer(char *buffer, const uint16_t len, bool clean)
:_buffer(buffer),_len(len), _buffer_overflow(false), _endOffset(0)
{
  if(clean)	memset(_buffer, 0, _len);
}

size_t CommandBuffer::write(uint8_t byte) {
  if (_endOffset >= _len) {
    _buffer_overflow = true;
    return 0;
  }
  _buffer[_endOffset] = byte;
  ++_endOffset;
  return 1;
}

void CommandBuffer::flush() {
  memset(_buffer, 0, _len);
  _endOffset = 0;
  _readOffset = 0;
  _buffer_overflow = false;
}

int CommandBuffer::available(void)
{
  return (unsigned int)(_len + _endOffset - _readOffset) % _len;
}

int CommandBuffer::peek(void)
{
  if (_endOffset == _readOffset) {
    return -1;
  } else {
    return _buffer[_readOffset];
  }
}

int CommandBuffer::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_endOffset == _readOffset) {
    return -1;
  } else {
    unsigned char c = _buffer[_readOffset];
    _readOffset = (uint8_t)(_readOffset + 1);
    return c;
  }
}

int CommandBuffer::readLongValues(long values[], int max){
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseInt();
		size++;
	}

	_readOffset+=2;// skip separator '];'
	_endOffset = lastOffset;

	return size;
}

int CommandBuffer::readIntValues(int values[], int max){
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseInt();
		size++;
	}

	_readOffset+=2;// skip separator '];'
	_endOffset = lastOffset;

	return size;
}

int CommandBuffer::readFloatValues(float values[], int max)
{
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseFloat();
		size++;
	}

	_readOffset+=2;// skip separator '];'
	_endOffset = lastOffset;

	return size;
}

String CommandBuffer::readString()
{
	  String ret;
	  int c = read();

	  if(c == Command::SEPARATOR || c == ',') c = read(); // skip first

	  while (c >= 0 && c != Command::SEPARATOR && c != ',')
	  {
	    ret += (char) c;
	    c = read();
	  }
	  return ret;
}

// returns the first valid (long) integer value from the current position.
// initial characters that are not digits (or the minus sign) are skipped
// function is terminated by the first character that is not a digit.
long CommandBuffer::parseInt()
{
  return parseInt(NO_SKIP_CHAR); // terminate on first non-digit character (or timeout)
}

// as above but a given skipChar is ignored
// this allows format characters (typically commas) in values to be ignored
long CommandBuffer::parseInt(char skipChar)
{
  bool isNegative = false;
  long value = 0;
  int c;

  c = peekNextDigit();
  // ignore non numeric leading characters
  if(c < 0)
    return 0; // zero returned if timeout

  do{
    if(c == skipChar)
      ; // ignore this charactor
    else if(c == '-')
      isNegative = true;
    else if(c >= '0' && c <= '9')        // is c a digit?
      value = value * 10 + c - '0';
    read();  // consume the character we got with peek
    c = timedPeek();
  }
  while( (c >= '0' && c <= '9') || c == skipChar );

  if(isNegative)
    value = -value;
  return value;
}


// as parseInt but returns a floating point value
float CommandBuffer::parseFloat()
{
  return parseFloat(NO_SKIP_CHAR);
}

// as above but the given skipChar is ignored
// this allows format characters (typically commas) in values to be ignored
float CommandBuffer::parseFloat(char skipChar){
  bool isNegative = false;
  bool isFraction = false;
  long value = 0;
  char c;
  float fraction = 1.0;

  c = peekNextDigit();
    // ignore non numeric leading characters
  if(c < 0)
    return 0; // zero returned if timeout

  do{
    if(c == skipChar)
      ; // ignore
    else if(c == '-')
      isNegative = true;
    else if (c == '.')
      isFraction = true;
    else if(c >= '0' && c <= '9')  {      // is c a digit?
      value = value * 10 + c - '0';
      if(isFraction)
         fraction *= 0.1;
    }
    read();  // consume the character we got with peek
    c = timedPeek();
  }
  while( (c >= '0' && c <= '9')  || c == '.' || c == skipChar );

  if(isNegative)
    value = -value;
  if(isFraction)
    return value * fraction;
  else
    return value;
}


// returns peek of the next digit in the stream or -1 if timeout
// discards non-numeric characters
int CommandBuffer::peekNextDigit()
{
  int c;
  while (1) {
    c = timedPeek();
    if (c < 0) return c;  // timeout
    if (c == '-') return c;
    if (c >= '0' && c <= '9') return c;
    read();  // discard non-numeric
  }
}


int CommandBuffer::nextEndOffSet(){
	int endOffset = _readOffset;
	for(int end = _readOffset;end < _endOffset;end++){
		if (isListEnd(_buffer[end]) && end != _readOffset) break;
		endOffset++;
	}

	return endOffset;
}

bool CommandBuffer::isListEnd(char c){
	return  c==']' || c==')' || c=='}' || c==Command::SEPARATOR;
}

