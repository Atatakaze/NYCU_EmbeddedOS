import sys
import socket

if len(sys.argv) != 3:
	print("Usage: python testClient.py [ip] [port]")
	exit()
host = sys.argv[1]
port = int(sys.argv[2])

sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sockfd.connect((host, port))

print('[Client]: Connect to %s:%s' %(host, str(port)))

transmit_buf = 'hello~ testing...' 
print('[Client]: Send ' + transmit_buf)
sockfd.send(transmit_buf.encode())

sockfd.close()