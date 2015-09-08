import serial, time

conn = serial.Serial('/dev/ttyACM0', 9600)

while 1:
	print conn.readline()
	conn.write('3')
	time.sleep(2)

