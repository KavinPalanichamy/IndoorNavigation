import socket

host = "10.2.1.223"
port = 4568

server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

server.connect((host,port))

print("Binding done \n")


socket.send(("Hello World").encode('ascii'))
