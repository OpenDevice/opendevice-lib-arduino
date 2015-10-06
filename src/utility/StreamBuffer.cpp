#include "StreamBuffer.h"

StreamBuffer::StreamBuffer(uint8_t *buffer, const uint16_t len, bool clean)
:_buffer(buffer),_len(len), _buffer_overflow(false), _endOffset(0)
{
  if(clean)	memset(_buffer, 0, _len);
  setTimeout(0);
}

size_t StreamBuffer::write(uint8_t byte) {
  if (_endOffset >= _len) {
    _buffer_overflow = true;
    return 0;
  }
  _buffer[_endOffset] = byte;
  ++_endOffset;
  return 1;
}

void StreamBuffer::flush() {
  memset(_buffer, 0, _len);
  _endOffset = 0;
  _readOffset = 0;
  _buffer_overflow = false;
}

int StreamBuffer::available(void)
{
  return (unsigned int)(_len + _endOffset - _readOffset) % _len;
}

int StreamBuffer::peek(void)
{
  if (_endOffset == _readOffset) {
    return -1;
  } else {
    return _buffer[_readOffset];
  }
}

int StreamBuffer::read(void)
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

int StreamBuffer::readLongValues(long values[], int max){
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

int StreamBuffer::readIntValues(int values[], int max){
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

int StreamBuffer::readFloatValues(float values[], int max)
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

String StreamBuffer::readString()
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

int StreamBuffer::nextEndOffSet(){
	int endOffset = _readOffset;
	for(int end = _readOffset;end < _endOffset;end++){
		if (isListEnd(_buffer[end]) && end != _readOffset) break;
		endOffset++;
	}

	return endOffset;
}

bool StreamBuffer::isListEnd(char c){
	return  c==']' || c==')' || c=='}' || c==Command::SEPARATOR;
}
