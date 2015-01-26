import socket

import threading
import json
import fcntl 
import struct 
 

f = open('test.txt','r')
data = f.read()
print data

f.close()
f = open('test.txt','w')

dic = json.loads(data)
print dic

print dic['updated']

dic['hwid'] = 'aa'

data = json.dumps(dic)
f.write(data)
