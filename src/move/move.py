import BaseHTTPServer
import SocketServer
import socket
from time import sleep
import sys
import math

import serial
from Phidgets.Devices.Stepper import Stepper

serverType = 'tcp'
connUSB = '/dev/ttyACM0'

if len(sys.argv) > 1:
  serverType = sys.argv[1]

if len(sys.argv) > 2:
  connUSB = sys.argv[2]

connArduino = serial.Serial(connUSB, 9600, timeout=0)

connPhidget = Stepper()

NUM_ATTEMPTS_BEFORE_DIE = 10
TIME_BETWEEN_READ = 0.05

def getProgress(progress, target):
  return str(int(math.ceil(100000000.0 * progress / target))) + '\0'

# TODO: Document calculations more clearly and using variables
def getShitDone(server, request):
  command = request[0]
  magnitude = request[1:]

  if command == 'd' or command == 't':
    if command == 'd':

      # Command in mm
      # 1 wheel rotation per 2 * PI * r, r = 123mm / 2
      # 71 + 524 / 3179 gear rotations per wheel rotation
      # 7 pole counts per gear rotation http://www.sdrobots.com/ig3242-52-encoder-interfacing-cpr-calculation/
      # 8 raw counts per pole count
      print 'DRIVE: %s %smm' % (command, magnitude)
      target = int(int(magnitude) / (math.pi * 123) * 8 * 7 * (71 + 524 / 3179))

    if command == 't':
      # Command in degrees
      # Same as above, but converting to mm, which then we convert into raw counts
      print 'TURN: %s %smm' % (command, magnitude)
      # Original: target = int(int(magnitude) * math.pi * 474.533396274 / 360 / (math.pi * 123) * 8 * 7 * (71 + 524 / 3179))
      # simplified
      target = int(int(magnitude) / 360 * 474.533396274 / 123 * 8 * 7 * (71 + 524 / 3179))

    connArduino.write(command + str(target) + '\n')

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

          # Report progress or error to base station
          if (response[0] == 'm' or response[0] == 'e') and serverType == 'tcp':
            server.wfile.write(response[:index])

          response = response[index + 1:]

        # We have successfully received data; reset numAttempts
        numAttempts = 0
      elif numAttempts > NUM_ATTEMPTS_BEFORE_DIE:
        print 'No new data, breaking'
        break;
      else:
        numAttempts = numAttempts + 1

      # Sleep for some time before reading again
      sleep(TIME_BETWEEN_READ)

  # TODO: read until get STOP command
  elif command == 'r':
    print 'MOVE: Rotating %s degrees' % magnitude
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
    # 16 counts per step resolution http://www.phidgets.com/documentation/web/PythonDoc/Phidgets.Devices.Stepper.html
    target = int(90 * 200 * 16 * int(magnitude) / 360)
    connPhidget.setTargetPosition(0, target)

    while connPhidget.getCurrentPosition(0) != target:
      progress = getProgress(connPhidget.getCurrentPosition(0), target)
      print 'PHIDGET: ' + progress

      # Report progress to base station
      if serverType == 'tcp':
        server.wfile.write(progress)
      sleep(TIME_BETWEEN_READ)

    progress = getProgress(connPhidget.getCurrentPosition(0), target)
    print 'PHIDGET: ' + progress
    if serverType == 'tcp':
      server.wfile.write(progress)
    sleep(TIME_BETWEEN_READ)

    connPhidget.setEngaged(0, False)
    connPhidget.closePhidget()

class TCPHandler(SocketServer.StreamRequestHandler):
  def handle(self):
    
    inputs = [self.server.fileno()]
    outputs = []

    while True:
      # self.rfile is a file-like object created by the handler;
      # we can now use e.g. readline() instead of raw recv() calls
      try:
        self.data = self.rfile.readline().strip()
      except:
        print >>sys.stderr, 'Connection closed'
        break

      if len(self.data) > 0:
        print '{} wrote:'.format(self.client_address[0])
        print self.data
        # Likewise, self.wfile is a file-like object used to write back
        # to the client
        # self.wfile.write(self.data.upper())
        getShitDone(self, self.data)
      else:
        break

    print >>sys.stderr, 'Connection closed'

class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    request = self.path[1:].split('/')
    getShitDone(self, ''.join(request))
    self.send_response(200)
    
if serverType == 'http':
  server = BaseHTTPServer.HTTPServer(('0.0.0.0', 8000), HTTPHandler)
  server.serve_forever()
elif serverType == 'tcp':
  server = SocketServer.TCPServer(('0.0.0.0', 8125), TCPHandler)
  server.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
  server.serve_forever()
elif serverType == 'tty':
  while True:
    request = raw_input("Next command please: ")
    getShitDone([], request)

