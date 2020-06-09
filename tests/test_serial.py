import sys
sys.path.append("python_client")
import comm
import message
import time


c=comm.Comm()
c.connect()

print("Found device id=%s, port=%s" % (c.id,c.port))

run=True
count=0
first=0
last=0
start=0
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
        if(count==0): 
            first=m.millis
            start=time.time()
        count=count+1
        if(count==100):
            last=m.millis
            stop=time.time()
            d=100./((last-first)/1000.)
            print("connects: %d, data packets: %d, rate: %f" % (c.connects,count,100/(stop-start)))
            print("first: %d, last: %d, rate: %f" %(first,last,d))
            count=0
