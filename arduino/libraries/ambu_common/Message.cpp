#include <Arduino.h>
#include <Message.h>
#include <string.h>
#include <Base64.h>

Message::Message() {} 
 

void Message::writeString(uint8_t id,uint32_t timestamp,const char *version) {
  uint8_t index=0;
  uint8_t len=strlen(version);
  uint16_t checksum;
  _write(id,index);
  _write(timestamp,index);				\
  _write(len,index);
  _write(len,version,index);
  checksum=_fletcher16(_tx,index);
  _write(checksum,index);
  uint8_t base64_len=base64_encode(_base64, (char*)_tx, index);
  _writeTrailer(base64_len);
}


void Message::writeData(uint8_t id,uint32_t timestamp,uint8_t float_len,const float *float_data,
		       uint8_t int_len,const uint32_t *data_int)   
{
  uint8_t index=0;
  uint8_t len=0;
  uint16_t checksum;
  _write(id,index);
  _write(timestamp,index);
  if(float_len>0 && float_data!=0)
    len=float_len;
  if(int_len>0 && data_int!=0)
    len|=(int_len<<4);
  _write(len,index);
  _write(float_len,float_data,index);
  _write(int_len,data_int,index);
  checksum=_fletcher16(_tx,index);
  _write(checksum,index);
  uint8_t base64_len=base64_encode(_base64, (char*)_tx, index);
  _writeTrailer(base64_len);
}

void Message::decode(uint8_t rxCount,const char *rxBuffer) {
  uint8_t len=strlen(rxBuffer);
  base64_decode((char *)_tx,rxBuffer,rxCount);
}

void Message::_writeTrailer(uint8_t &index)
{
  for(unsigned i=0;i<3;i++) {
	_base64[index++]='-';
  }
  _base64[index]='\0';
  _len=index;
}

template <typename T> 
void Message::_write(T value,uint8_t &index) {
  T temp=value;
  uint8_t *p=(uint8_t*) &temp;
  for(uint8_t i=0;i<sizeof(T);i++) {
    _tx[index]=p[i];
    index++;
  }
}
template <typename T> 
void Message::_write(uint8_t len,T *value,uint8_t &index) {
  uint8_t *p=(uint8_t*) value;
  for(uint8_t i=0;i<sizeof(T)*len;i++) {
    _tx[index]=p[i];
    index++;
  }
}



  // https://en.wikipedia.org/wiki/Fletcher%27s_checksum#Example_calculation_of_the_Fletcher-16_checksum
uint16_t  Message::_fletcher16(const uint8_t *data,uint8_t len) {    
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  
  for ( uint8_t i = 0; i < len; ++i )
    {
      sum1 = (sum1 + data[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }
  return (sum2 << 8) | sum1;
}

