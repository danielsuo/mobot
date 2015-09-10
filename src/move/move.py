import BaseHTTPServer
from time import sleep

import serial
from Phidgets.Devices.Stepper import Stepper

# connArduino = serial.Serial('/dev/ttyACM0', 9600)
connArduino = serial.Serial('/dev/tty.usbmodemc041', 9600)

connPhidget = Stepper()

class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    command = self.path[1:].split('/')

    if command[0][0] == 'd' or command[0][0] == 't':
      connArduino.write(command[0][0] + command[1] + '\n')
      print 'MOVE: %s %s units' % (command[0][0], command[1])

    elif command[0][0] == 'r':
      print 'MOVE: Rotating %s degrees' % command[1]
      connPhidget.openPhidget()
      connPhidget.waitForAttach(10000)
      connPhidget.setCurrentPosition(0, 0)
      # sleep(1)
      connPhidget.setEngaged(0, True)
      # sleep(1)
      connPhidget.setAcceleration(0, 87543)
      connPhidget.setVelocityLimit(0, 40000)
      connPhidget.setCurrentLimit(0, .7)
      sleep(1)

      target = int(90 * 200 * 16 * int(command[1]) / 360)
      connPhidget.setTargetPosition(0, target)

      while connPhidget.getCurrentPosition(0) != target:
        pass

      connPhidget.setEngaged(0, False)
      connPhidget.closePhidget()

    self.send_response(200)
    

server = BaseHTTPServer.HTTPServer(('0.0.0.0', 8000), HTTPHandler)
server.serve_forever()