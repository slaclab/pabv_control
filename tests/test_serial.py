import sys
sys.path.append("python_client")
import comm
import message
import time


c=comm.Comm()
c.connect()

print("Found device id=%s, port=%s" % (c.id,c.port))

start=time.time()
run=True
count=0
first=0
last=0
while (run):
    line=c.readPacket()
    if(line is None): continue
    m=message.Message()
    try:
        m.decode(line)
    except:
        continue
    if(m.status!=m.ERR_OK): continue
    if(m.id == m.DATA and m.nFloat==5 and m.nInt==2):
        if(count==0): first=m.millis
        count=count+1
        if(count==1000):
            last=m.millis
            run=False

stop=time.time()
d=(last-first)/1000
print("connects: %d, data packets: %d, rate: %f" % (c.connects,count,1000/(stop-start)))
print("millis: %d" % d)
