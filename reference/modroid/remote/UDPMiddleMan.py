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
                        if msg == 'U' :                                         
                                bridge.put("D13","1")                           
                        elif msg == 'D' :                                       
                                bridge.put("D13","0")                           
                        print msg                                               
        except KeyboardInterrupt :                                              
                break                                                           
        except Exception as err:                                                
                print err                                                       
                pass  