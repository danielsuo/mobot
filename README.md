# mobot
![Mobot](http://i.telegraph.co.uk/multimedia/archive/02307/bolt_farah_009_2307854b.jpg "Mobot")

## TODO
- Understand current reconstruction pipeline
- Implement bundle adjustment for pose graph and replace existing bundle adjustment
- Compare results of two bundle adjustment and time
- How to incrementally build and query bag of visual words?
- Implement bag of visual words in C
- Test new SIFT code on old data
- Add additional constraints during align and bundle adjustment
- Update rootsift

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

### Process

### Main

