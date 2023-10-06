import socket

host = "10.2.1.223"
port = 4568

server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

server.bind((host,port))

print("Binding done \n")

server.listen(10)


while True:

    client_socket,accept = server.accept()
    print("running")
    print(f"Connection established to {client_socket}")
    message = client_socket.recv(1024).decode('ascii')
    print(f"The message is {message} ")

  


