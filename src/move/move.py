import BaseHTTPServer
from time import sleep
import sys
import math

import serial
from Phidgets.Devices.Stepper import Stepper

connUSB = '/dev/ttyACM0'

if len(sys.argv) > 1:
  connUSB = sys.argv[1]

connArduino = serial.Serial(connUSB, 9600, timeout=0)

connPhidget = Stepper()

NUM_ATTEMPTS_BEFORE_DIE = 10

class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    command = self.path[1:].split('/')

    if command[0][0] == 'd' or command[0][0] == 't':

      if command[0][0] == 'd':

        # Command in mm
        # 1 wheel rotation per 2 * PI * r, r = 125mm / 2
        # 71 + 524 / 3179 gear rotations per wheel rotation
        # 7 pole counts per gear rotation
        # 8 raw counts per pole count
        target = int(int(command[1]) / (math.pi * 125) * 8 * 7 * (71 + 524 / 3179))

      if command[0][0] == 't':
        # Command in degrees
        # Same as above, but converting to mm, which then we convert into raw counts
        target = int(int(command[1]) * math.pi * 493.38 / 360 / (math.pi * 125) * 8 * 7 * (71 + 524 / 3179))

      connArduino.write(command[0][0] + str(target) + '\n')
      print 'MOVE: %s %smm' % (command[0][0], command[1])

      response = ''
      numAttempts = 0
      index = -1

      while True:
        response = response + connArduino.read(4096)
        
        if len(response) > 0:

          # Split off newline. Commands from Arduino are newline delimited
          index = response.find('\n')

          # If we find a full line, process and remove from response
          if index > -1:
            print 'ARDUINO: ' + response[:index]
            response = response[index + 1:]

          # We have successfully received data; reset numAttempts
          numAttempts = 0
        elif numAttempts > NUM_ATTEMPTS_BEFORE_DIE:
          print 'No new data, breaking'
          break;
        else:
          numAttempts = numAttempts + 1

        # Sleep for some time before reading again
        sleep(0.05)

      # TODO: read until get STOP command

    elif command[0][0] == 'r':
      print 'MOVE: Rotating %s degrees' % command[1]
      connPhidget.openPhidget()
      connPhidget.waitForAttach(10000)
      connPhidget.setCurrentPosition(0, 0)
      connPhidget.setEngaged(0, True)
      connPhidget.setAcceleration(0, 87543)
      connPhidget.setVelocityLimit(0, 40000)
      connPhidget.setCurrentLimit(0, .7)
      sleep(1)

      # Command in degrees
      # 90 shaft turns per stage turn
      # 360 degrees per shaft turn
      # 1 step per 1.8 degrees
      # 4 lines per step
      # 4 counts per step
      target = int(90 * 200 * 16 * int(command[1]) / 360)
      connPhidget.setTargetPosition(0, target)

      while connPhidget.getCurrentPosition(0) != target:
        pass

      connPhidget.setEngaged(0, False)
      connPhidget.closePhidget()

    self.send_response(200)
    

server = BaseHTTPServer.HTTPServer(('0.0.0.0', 8000), HTTPHandler)
server.serve_forever()
