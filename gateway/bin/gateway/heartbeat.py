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

			if content == 'camera#on':
				print '[HBThread] camera ON command coming. sending command ...'
				self.hbsock.sendto(content,('',self.port))

				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
				print 'command ret:',ret2.split('Content>')[1].split('<')[0]
				
			elif content == 'tv#up':
				print '[HBThread] tv UP command comming. sending command...'
				self.hbsock.sendto(content,('',self.port))

				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
				print 'command ret:',ret2.split('Content>')[1].split('<')[0]

			elif content == 'tv#down':
				print '[HBThread] tv DOWN command coming. sending command...'
				self.hbsock.sendto(content,('',self.port))

				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
				print 'command ret:',ret2.split('Content>')[1].split('<')[0]

			elif content == 'tv#left':
				print '[HBThread] tv LEFT command coming. sending command...'
				self.hbsock.sendto(content,('',self.port))

				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
				print 'command ret:',ret2.split('Content>')[1].split('<')[0]

			elif content == 'tv#right':
				print '[HBThread] tv RIGHT command coming. sending command...'
				self.hbsock.sendto(content,('',self.port))

				ret2 = restful.method_get(init.url_control + '=' + WrtGateway.s_hwid)
				print 'command ret:',ret2.split('Content>')[1].split('<')[0]

			else:
				pass		

			# heartbeat interval
			time.sleep(self.interval)