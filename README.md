# mobot
![Mobot](http://i.telegraph.co.uk/multimedia/archive/02307/bolt_farah_009_2307854b.jpg "Mobot")

## Resources
- Still disorganized, but old code, drawings, BOM, etc [here](https://drive.google.com/folderview?id=0B_10gtxnPV-_fl9iaG9mdFpnZW1NSVJUenZWOW1wWms0Sm9obnpRZGNpUTRvdnVaVUI0aDg&usp=sharing)

## Set up
### Move (a.k.a. Arduino + motors + wheels; stepper motor + platform)
- To install, run the following from the ```mobot``` directory:
  ```
  ./scripts/install
  ```
- Open ```src/move/init/init.ino``` and upload to Arduino
- Open ```src/move/move.ino``` and upload to Arduino
- If on OS X, use the BluTerm app (see link in references) to pair (code 0000)
- Send an integer via BluTerm to test motors

### Capture (a.k.a., RGBD sensors)
- ~Install OpenNI2 and dependencies (see [here](https://github.com/occipital/openni2))~

### Process

### Main

## Todo
### v0.0.10
- FPS crashes app
- Manual white balance
- High resolution
  - Two modes
    - Disk mode
      - High FPS
      - High resolution
    - Wifi mode
      - Low FPS
      - Low resolution
- Design doc for server / client plus refactor

### On deck
- Server: Read and save on different processes?
- iOS: Add back auto-versioning\
- number of devices on wifi
- iPod touch case
- Syncing devices https://en.wikipedia.org/wiki/Clock_synchronization
  - Start recording
  - Get initial timestamp
  - Each frame get data + timestamp
  - Calculate relative timestamp
  - Hold onto frames that come within certain window on rolling basis; drop older ones that fall out of window
  - When have frames from all devices in a single window; save
  - OR; save continually and record which frames match up to which other frames in a separate file
- Make sure robot is stable
  - Driving
  - Rotating
- Robot supplies
  - M4 or 5/32" screw
  - M4 or 5/32" nut
  - 80/20
  - wheels
- Record only one of X frames
- Oculus version (stream via wifi by default)
- Add resolution toggle button
- BUG: Name a queue for processing uploads, so we can properly kill later
- Synchronizing
  - http://stackoverflow.com/questions/1427250/how-to-use-bluetooth-to-connect-two-iphone
  - http://forums.structure.io/t/multiple-structure-sensors-simultaneously/4087/4
- Change 4" structure sensor positioning to not block camera

- Save infrared as well?
- More sensors?
- Accelerometer data? Frame by frame data?

### Future
- Video toolbox
- Depth compression
  - http://ieeexplore.ieee.org/xpl/articleDetails.jsp?reload=true&arnumber=6463455
  - http://thebytekitchen.com/2014/03/24/data-compression-for-the-kinect/
  - http://research.microsoft.com:8082/pubs/153971/depthcode-final.pdf
- Admin
  - Start tagging commits and sync with changelog
  - Use pvcapture github organization
- GUI
    - Add statistics (e.g., FPS, time elapsed, total frames) to logging / user interface
    - Number of frames
  - Depth overlaid on color
    - http://kiwigis.blogspot.com/2011/08/how-to-align-kinects-depth-image-with.html
- RGBD
  - Structure Sensor
    - Turn on infrared LED when SDK allows
    - Mount sensors
      - Orientation: 0, 90, 180, 270 at +5, -10, -25, -40
    - Set default (offline, buffer, stream) and queue to memory or disk as needed
    - Wrap up as [library](http://www.raywenderlich.com/56885/custom-control-for-ios-tutorial-a-reusable-knob) and [framework](http://www.raywenderlich.com/65964/create-a-framework-for-ios)
    - Possible issues
      - Managing power (8 devices to charge!)
        - http://forums.structure.io/t/charging-ipad-while-using-sensor-at-the-same-time/2656/3
      - Bluetooth range if computer is not on board the robot; may need to change to wifi
      - Wifi bandwidth, especially at distance (5Ghz)
- Kangaroo
  - Speed limit
  - Wheel dimensions
  - Turning
  - Global coordinate system
  - Use hardware [interrupts](http://playground.arduino.cc/Main/RotaryEncoders) for polling
  - Investigate [Arduino Makefile](https://github.com/sudar/Arduino-Makefile#usage)
- Performance
  - Improve [SFTP speed](http://www.psc.edu/index.php/hpn-ssh)
  - Reduce PNG size (maybe Apple saves thumbnail?)
- Connect Matlab to shell
  - matlab -nodesktop -nojvm -nosplash -r 'disp(1+1)' -logfile tmp.txt
  - http://www.mathworks.com/matlabcentral/newsreader/view_thread/34050

## References
- OpenNI
  - http://structure.io/openni
  - https://github.com/OpenNI/OpenNI
  - http://openni.ru/
- Reconstruction
  - Data from Shuran [here](https://www.dropbox.com/s/brmqa6i2v3185yc/third_floor_tearoom.zip?dl=0)
- Bluetooth
  - Set up and additional resources [here](https://github.com/rwaldron/johnny-five/wiki/Getting-Started-with-Johnny-Five-and-JY-MCU-Bluetooth-Serial-Port-Module)
  - Setting up for correct voltage [here](http://42bots.com/tutorials/how-to-connect-arduino-uno-to-android-phone-via-bluetooth/)
  - Bluetooth SPP via [node](https://www.npmjs.com/package/bluetooth-serial-port)
  - Bluetooth SPP OS X [app](https://itunes.apple.com/us/app/bluterm/id615234148?mt=12)
- Encoder and feedback
  - Encoders [here](http://forums.trossenrobotics.com/tutorials/introduction-129/introduction-to-encoders-3256/)
  - Interrupts [here](http://forums.trossenrobotics.com/tutorials/how-to-diy-128/an-introduction-to-interrupts-3248/)
