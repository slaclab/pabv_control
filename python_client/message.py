import base64
import struct


class Message():
    def __init__(self):
        self.MSGID_DATA=0xc1;
        self.MSGID_CONFIG=0xc2;
        self.MSGID_DEBUG=0xc3;
        self.MSGID_VERSION=0xc8; 
        self.ERR_OK=0x0;
        self.ERR_LENGTH=0xe1;
        self.ERR_HEADER=0xe2;
        self.ERR_CHECKSUM=0xe3;

        self.id=0
        self.checksum=0
        self.data=None
        self.nFloat=0
        self.nInt=0
        self.floatData=None
        self.intData=None
        self.status=self.ERR_OK
        self.string=None

    def decode(self,msg):
        dec=base64.b64decode(msg)
        if(len(dec)<7): 
            self.status=self.ERR_LENGTH
            return
        header=struct.unpack("<BIB",dec[:6])
        l=header[-1]
        self.id=header[0]
        data=None
        if((self.id&0xc0)!=0xc0):
            self.status=self.ERR_HEADER
            return
        if((self.id&8)==0):
            int_len=(l>>4);
            float_len=(l&0xf);
            data=struct.unpack("<"+str(float_len)+"f"+str(int_len)+  "IH",dec[6:])
            checksum_sent=data[-1]        
            checksum=self._fletcher16(dec[:-2])
            if(checksum_sent!=checksum):
                status=self.ERR_CHECKSUM
                return
            self.nFloat=float_len
            self.nInt=int_len
            self.floatData=data[:float_len]
            self.intData=data[float_len:float_len+int_len]
        else:            
            data=struct.unpack("<"+str(l)+"sH",dec[6:])       
            checksum_sent=data[-1]        
            checksum=self._fletcher16(dec[:-2])
            if(checksum_sent!=checksum):
                status=self.ERR_CHECKSUM
                return
            self.string=data[0]
        
    def writeData(self,id,timestamp,float_data,int_data):
        float_len=len(float_data)
        int_len=len(int_data)
        l=float_len+int_len*16
        fmt="<BIB"+str(float_len)+"f"+str(int_len)+"I"
        data=struct.pack(fmt,
                    id,
                    timestamp,
                    l,
                    *float_data,
                    *int_data                    
                )
        checksum=self._fletcher16(data)
        data=data+struct.pack("H",checksum)
        en=base64.b64encode(data)
        en=en+b'---'
        return en
    def writeString(self,id,timestamp,s):
        l=len(s)
        fmt="<BIB"+str(l)+"s"
        data=struct.pack(fmt,
                    id,
                    timestamp,
                    l,
                    s
                )
        checksum=self._fletcher16(data)
        data=data+struct.pack("H",checksum)
        en=base64.b64encode(data)
        en=en+b'---'
        return en

    def _fletcher16(self,data):
        sum1=0
        sum2=0
        for i in range(len(data)):
            sum1=((sum1+data[i]) & 0xffff) % 255
            sum2=((sum2+sum1) & 0xffff) % 255
        result=((sum2 << 8) | sum1) & 0xffff
        return result
