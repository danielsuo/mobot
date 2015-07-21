import sys, os, math
from pgmagick import Image, ImageType

data_dir = sys.argv[1] + '/' if sys.argv[1][-1] != '/' else sys.argv[1]
out_dir = sys.argv[2] + '/' if sys.argv[2][-1] != '/' else sys.argv[2]
out_dir = out_dir + 'image_data/'

target = 1 if len(sys.argv) < 4 else int(sys.argv[3])

if not os.path.exists(out_dir):
  os.makedirs(out_dir)

def process_images(subdir):
  curr_dir = data_dir + subdir + '/'

  counter = 1
  files = os.listdir(curr_dir)
  interval = int(len(files) / target)

  for file in files:
    if counter % interval == 0 and counter / interval <= target:
      img = Image(curr_dir + '/' + file)
      img.type(ImageType.GrayscaleType)
      img.write(out_dir + subdir + str(counter / interval) + '.tif')
    counter = counter + 1

process_images('color')
process_images('infrared')