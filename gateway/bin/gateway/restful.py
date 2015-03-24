import urllib2
import base64

def method_post(url,data,header):
	req = urllib2.Request(url,data,header)
	resp = urllib2.urlopen(req)
	return resp.read()
	
def method_get(url):
	req = urllib2.Request(url)
	resp = urllib2.urlopen(req)
	return resp.read()

def post_image(url,image_bytes):
	# must be open with 'wb' not 'w'
	fw = open('tmp2.jpg','wb')
	fw.write(image_bytes)
	fw.close()

	print 'post_image() called\n'
	encoded_image = base64.b64encode(image_bytes)
		
	#print 'encod_image:',encoded_image
	#raw_params = {'image': encoded_image}
	#params = urllib.urlencode(raw_params)
	request = urllib2.Request(url, encoded_image)
	#print request
	#request.add_header("Content-type", "application/x-www-form-urlencoded; charset=UTF-8")
	page = urllib2.urlopen(request)
	info = page.read()
	print info