import socket
import pygame

UDP_IP = "10.0.100.82"
# UDP_IP = ""
UDP_PORT = 5555

pygame.init()

sock = socket.socket(socket.AF_INET, #Internet
	socket.SOCK_DGRAM) #UDP

done = False

keydn = {'U':False,'D':False,'L':False,'R':False}
key_msg = {pygame.K_UP:'U',pygame.K_DOWN:'D',pygame.K_LEFT:'L',pygame.K_RIGHT:'R',pygame.K_i:'I',pygame.K_s:'S'}

while not done:
	for event in pygame.event.get() :
		msg = ''
		if event.type == pygame.KEYDOWN :
			msg = key_msg.get(event.key,'')
			if event.key == pygame.K_ESCAPE :
				done = True
				break
		if msg :
			print msg
			sock.sendto(msg, (UDP_IP,UDP_PORT))
