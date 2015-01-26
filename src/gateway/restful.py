import urllib2


def method_post(url,data,header):
	req = urllib2.Request(url,data,header)
	resp = urllib2.urlopen(req)
	return resp.read()
	
def method_get(url):
	req = urllib2.Request(url)
	resp = urllib2.urlopen(req)
	return resp.read()