#! python3
"""
 Implements a simple HTTP/1.0 Server
 https://www.codementor.io/@joaojonesventura/building-a-basic-http-server-from-scratch-in-python-1cedkg0842

"""

import socket
import minimalmodbus


# Define socket host and port
SERVER_HOST = '0.0.0.0'
SERVER_PORT = 8080

# Create socket
led = led = minimalmodbus.Instrument('/dev/ttyS1', 123)
led.handle_local_echo=True

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((SERVER_HOST, SERVER_PORT))
server_socket.listen(1)
print('Listening on port %s ...' % SERVER_PORT)
old_val1 = 50
old_val2 = 50
old_val3 = 50
(old_val1, old_val2, old_val3) = led.read_registers(0, 3)


while True:
    # Wait for client connections
    client_connection, client_address = server_socket.accept()

    # Get the client request
    request = client_connection.recv(1024).decode()
    headers = request.split('\n')
    filename = headers[0].split()[1]
    #print(filename)
    if "?" in filename:
        splNamPar = filename.split("?")
        #print(splNamPar[0])
        splParams = splNamPar[1].split("&")
        params = dict(subString.split("=") for subString in splParams)
        #print(params)
        if params.get('Jas1') != None:
            if params['Jas1'].isnumeric():
                #print(int(params['Jas1']))
                old_val1 = int(params['Jas1'])
        if params.get('Jas2') != None:
            if params['Jas2'].isnumeric():
                #print(int(params['Jas2']))
                old_val2 = int(params['Jas2'])
        if params.get('Jas3') != None:
            if params['Jas3'].isnumeric():
                #print(int(params['Jas3']))
                old_val3 = int(params['Jas3'])
        led.write_registers(0, [old_val1, old_val2, old_val3])
    #else:
        #print(filename)
    # Send HTTP response
    response1 = 'HTTP/1.0 200 OK\n\n'
    cont = "<html><body><form action=\"led.html\" method=\"GET\"> Jas 1: <input type=\"number\" min=\"0\" max=\"100\" size=\"10\" name=\"Jas1\" value=\"{val1}\"> <br>Jas 2: <input type=\"number\" min=\"0\" max=\"100\" size=\"10\" name=\"Jas2\" value=\"{val2}\"> <br>Jas 3: <input type=\"number\" min=\"0\" max=\"100\" size=\"10\" name=\"Jas3\" value=\"{val3}\"> <br><input type=\"submit\" value=\"odeslat\"> </form></html>".format(val1 = old_val1, val2 = old_val2, val3 = old_val3)
    response = response1 + cont
    client_connection.sendall(response.encode())
    client_connection.close()

# Close socket
server_socket.close()
