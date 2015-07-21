# Calibration

- Print out pattern.pdf
- Mount on rigid board
- Take 20-25 images with color / infrared
  - Make sure entire pattern is in image for both cameras
  - Need sunlight for best results, but not so much that the pattern is distorted (e.g., from too much sunlight)
  - Take from a variety of angles
- Calibrate color and infrared separately via calib_gui
- Set CORNERFINDOFF to some value to turn automatic corner finding off
- Calibrate the results together via stereo_gui
- R (rotation matrix) and T (translation vector) are right (infrared) camera pose with respect to left (color) camera pose