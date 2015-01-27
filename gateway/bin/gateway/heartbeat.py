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
	def __init__(self,interval,remote_ip,remote_udpport):
		super(HBThread, self).__init__()
		self.interval = interval

		# share udp socket with HBRecvThread
		self.s = HBRecvThread.s;
		self.remote_ip = remote_ip
		self.remote_udpport = remote_udpport

		self.hbsock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

	def heartbeat_package(self):
		return struct.pack('iBBiBi',4,0,0,time.time(),32,int(WrtGateway.s_hwid))

	# send heartbeat using UDP instead of HTTP
	def run(self):
		while True:
			print '[HBThread] uploading heartbeat'
			ret1 = restful.method_get(init.url_hb + '/' + WrtGateway.s_hwid)
			content = ret1.split('Content>')[1].split('<')[0]
			#print '[HBThread] content:',content
			if content == '0#1':
				print '[HBThread] command coming.issuing command...'
				self.hbsock.sendto('On',('',9000))

			try:
				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
			except:
				print 'heartbeat exception'

			print 'command ret:',ret2.split('Content>')[1].split('<')[0]

			time.sleep(self.interval)

			'''
			s = self.heartbeat_package()
			a,b,c,d,e,f = struct.unpack('iBBiBi',s)
			#print a,b,c,d,e,f
			self.s.sendto(self.heartbeat_package(),(self.remote_ip,self.remote_udpport))
			time.sleep(self.interval)
			'''