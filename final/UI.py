import sys
import socket

if len(sys.argv) != 2:
	print("Usage: python UI.py [port]")
	exit()
port = int(sys.argv[1])

sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sockfd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sockfd.bind(('', port))
sockfd.listen(2)

print('[Server]: Start at port %s' %port)
print('[Server]: Wait for connection ...')


connfd, addr = sockfd.accept()
print('[Server]: Connected by ', str(addr))
    
receive_buf = connfd.recv(1024)
print('[Server]: Receive ' + receive_buf.decode())

sockfd.close()
connfd.close()