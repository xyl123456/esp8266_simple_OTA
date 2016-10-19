import socket
from array import *
import sys
import binascii



def send(ip, porta, binfile):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (ip, int(port))
    sock.connect(server_address)

    f = open(binfile,"rb")
    try:
        totalBytes = 0
        data = bytearray()
        data.append('+')
        sock.sendall('U')
        byte = f.read(1)    
        while byte != "":
            data.append(byte)
            if len(data) == 513:
                #print binascii.hexlify(data)
                sock.sendall(data)
                totalBytes=totalBytes+512
                data = bytearray()
                data.append('+')
                print sock.recv(10)
                print totalBytes
            byte = f.read(1)        

        if len(data)>1:
            while len(data)<513:
                data.append(255)
            #print binascii.hexlify(data)
            sock.sendall(data)        
            totalBytes=totalBytes+len(data)
            print sock.recv(10)
            print totalBytes
        sock.sendall('F')
        print 'Reboot command sent'
    finally:
        f.close()


def get_current_userbin(ip, port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip, int(port))
        sock.connect(server_address)
        sock.sendall('S')
        print sock.recv(1)
    except:
        print 'ERROR'

if __name__ == "__main__":
    action = sys.argv[1]
    ip = sys.argv[2]
    port = sys.argv[3]
    #print action
    #print ip
    #print port
    if action == 'get_current_userbin':
        get_current_userbin(ip, port)
    if action == 'send':
        binfile = sys.argv[4]
        send(ip, port, binfile)

    
    
