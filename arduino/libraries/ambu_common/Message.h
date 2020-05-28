#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>




class Message {
 public: 
  Message();
  void writeData(uint8_t id,uint32_t timestamp,uint8_t float_len,const float *float_data,
	   uint8_t int_len,const uint32_t *data_int);   
			
  void writeString(uint8_t id,uint32_t timestamp,const char *version);
  void decode(uint8_t rxCount,const char *rxBuffer);
  const char *getBuffer() const {return _base64;}
  static constexpr uint8_t DATA  = 0xc1;
  static constexpr uint8_t CONFIG  = 0xc2;
  static constexpr uint8_t DEBUG   = 0xc3;
  static constexpr uint8_t VERSION  = 0xc4;
  static constexpr uint16_t HEADER   = 0xa55a;
  uint8_t size() const {return _len;}
  uint8_t nFloat() const {return _nFloat;}
  uint8_t nInt() const {return _nInt;}
  const float *getFloat() const {return _tempFloat;}
  const uint32_t *getInt() const {return _tempInt;}
 private:
  uint16_t  _fletcher16(const uint8_t *data,uint8_t len);
  template <typename T> inline void _write(T value,uint8_t &index);
  template <typename T> inline void _write(uint8_t len,T *value,uint8_t &index);
  void _writeTrailer(uint8_t &index);
  uint8_t  _tx[64];
  char  _base64[128];
  uint8_t _len;
  uint8_t _nFloat;
  uint8_t _nInt;
  float _tempFloat[16];
  uint32_t _tempInt[16];
  char tempStr[64];

};
#endif
