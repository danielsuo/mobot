import socket
import time
from bridgeclient import BridgeClient

bridge = BridgeClient()

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

sock.bind(("",5555))

while True :
        try :
                msg = sock.recv(1024)
                if msg :
                        print msg
                        bridge.put("FLAG",msg)
        except KeyboardInterrupt :
                break
        except Exception as err:
                print err
                pass
sock.close()
