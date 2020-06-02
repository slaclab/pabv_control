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
  // binary data messages 0xc1 - 0xc7
  static constexpr uint8_t DATA  = 0xc1;
  static constexpr uint8_t CONFIG  = 0xc2;
  static constexpr uint8_t PARAM_INTEGER  = 0xc3;
  static constexpr uint8_t PARAM_FLOAT  = 0xc4;
  static constexpr uint8_t PARAM_SET  = 0xc5;
  static constexpr uint8_t CPU_ID  = 0xc6; 
  // string messages 0xc8-0xcf
  static constexpr uint8_t VERSION  = 0xc8;
  static constexpr uint8_t DEBUG   = 0xc9;

  // errors
  static constexpr uint8_t ERR_OK=0x0;
  static constexpr uint8_t ERR_LENGTH=0xe1;
  static constexpr uint8_t ERR_HEADER=0xe2;
  static constexpr uint8_t ERR_CHECKSUM=0xe3;

  uint8_t size() const {return _len;}
  uint8_t nFloat() const {return _nFloat;}
  uint8_t nInt() const {return _nInt;}
  uint8_t status() const {return _status;}
  uint8_t id() const { return _id;}
  void getFloat(float *f) {for (unsigned i=0;i<_nFloat;i++) f[i]=_tempFloat[i]; }
  void getInt(uint32_t *d)  {for (unsigned i=0;i<_nInt;i++) d[i]=_tempInt[i];   }
  const float *getFloat() {return _tempFloat;}
  const uint32_t *getInt() {return _tempInt;}
  const char *getString() const { return _tempStr; }
 private:
  uint16_t  _fletcher16(const uint8_t *data,uint8_t len);
  template <typename T> inline void _write(T value,uint8_t &index);
  template <typename T> inline void _write(uint8_t len,T *value,uint8_t &index);
  template <typename T> inline void _read(T *value,uint8_t &index);
  template <typename T> inline void _read(uint8_t len,T *value,uint8_t &index);
  constexpr bool _is_data(uint8_t id) {return (id&8)==0;};
  constexpr bool _is_string(uint8_t id) {return (id&0x8)!=0;};
  void _writeTrailer(uint8_t &index);
  uint8_t  _tx[64];
  char  _base64[128];
  uint8_t _len;
  uint8_t _nFloat;
  uint8_t _nInt;
  uint8_t _id;
  float _tempFloat[16];
  uint32_t _tempInt[16];
  char _tempStr[64];
  uint8_t _status;

};
#endif
