import threading
import time
import socket
import struct
import time

from gateway import WrtGateway
from hb_recv_th import HBRecvThread
import restful
import init

class HBThread(threading.Thread):
	"""docstring for HeartBeat"""
	def __init__(self,interval,port):
		super(HBThread, self).__init__()
		self.interval = interval
		self.port = port
		self.hbsock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

	# send heartbeat using HTTP
	def run(self):
		while True:
			print '[HBThread] uploading heartbeat'
			ret1 = restful.method_get(init.url_hb + '/' + WrtGateway.s_hwid)
			content = ret1.split('Content>')[1].split('<')[0]
			#print '[HBThread] content:',content
			if content == '0#1':
				print '[HBThread] command coming.issuing command to device...'
				self.hbsock.sendto('on',('',self.port))

				try:
					ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
					print 'command ret:',ret2.split('Content>')[1].split('<')[0]

				except:
					print '[HBThread] heartbeat exception'
					pass			

			# heartbeat interval
			time.sleep(self.interval)