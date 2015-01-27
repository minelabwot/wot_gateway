import threading
import socket

class HBRecvThread(threading.Thread):
	"""docstring for HBRecvThread"""

	s = None

	def __init__(self,port=8003):
		super(HBRecvThread, self).__init__()
		self.port = port

	def init_socket(self):
		HBRecvThread.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		HBRecvThread.s.bind(('',self.port))

	# can't receive heartbeat return data ???
	def run(self):
		while True:
			data,addr = HBRecvThread.s.recvfrom(1024)
			print '[HBRecvThread] received heartbeat socket data'
			if not data:
				print 'data empty'
			print 'data:' + data + ',addr:',addr
