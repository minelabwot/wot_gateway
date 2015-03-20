import socket
import threading
import gateway
import urllib

class CameraThread(threading.Thread):
	def __init__(self):
		super(CameraThread,self).__init__()
	
	def init_socket(self,hostip,port=8002):
		self.s = socket.socket()
		self.s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
		#host = socket.gethostname()
		# print 'hostname:',host
		self.s.bind((hostip,port))
		self.s.listen(1)
		print 'listening camera socket on %s:%d' %(hostip,port)
		
	def run(self):
		while True:
			print '[CameraThread] waiting client connection...'
			c,addr = self.s.accept()
			print '[CameraThread] Got conn from:',addr
			while True:
				try:
					data = c.recv(614400)
				except:
					print 'recv exception occur'
					c.close()
					break
				if not data:
					# if client disconnects suddenly, data is ''
					print 'client may be disconnected'
					break
				print 'picdata length:',len(data)
				gateway.WrtGateway.upload_image(6,urllib.urlencode(data))