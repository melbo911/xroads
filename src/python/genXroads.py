#!/usr/bin/env python3
#
# Creates or updates the Xroads "Custom Scenery" library to force Ortho4XP tiles to use 
# the "Xroads" road net files. It also removes parking ground polygons from ortho tiles.
#
# Create the net files using genNoRoads.py
#
# melbo @ x-plane.org
# version 0.8
#

import sys
import re
import os
import shutil
import glob
import time

if __name__ == '__main__':

   xpDir=sys.path[0]
   xroadsDir = xpDir + '/Custom Scenery/Xroads'
   defRoads = xpDir + '/Resources/default scenery/1000 roads'
   xRoads = xroadsDir + '/1000_roads'
   xObjects = xroadsDir + '/objects'
   xLib = xroadsDir + '/library.txt'
   xBlankFac = xroadsDir + '/blank.fac'
   xBlankObj = xroadsDir + '/blank.obj'

   #print("xroadsDir = ",xroadsDir)

   if not os.path.isdir(xroadsDir):
      try:
         print("creating directory ",xroadsDir)
         os.makedirs(xroadsDir)
      except:
         print("Could not create required directory ",xroadsDir,". Exit.")
         sys.exit()

   if not os.path.isdir(xRoads):
      if not os.path.isdir(defRoads):
         print("default scenery roads not found. Exit.")
         sys.exit()
      try:
         print("copying default roads...",end='',flush=True)
         destination = shutil.copytree(defRoads, xRoads)  
         print("done.")
      except:
         print("Could not copy default roads. Exit.")
         sys.exit()

   if not os.path.isdir(xObjects):
      try:
         print("creating directory ",xObjects)
         os.makedirs(xObjects)
      except:
         print("Could not create required directory ",xObjects,". Exit.")
         sys.exit()

   if not os.path.isfile(xObjects+"/blank.fac"):
      f = open(xObjects+"/blank.fac", 'w')
      print("A\n800\nFACADE\n", file=f)  # Python 3.x
      f.close()

   if not os.path.isfile(xObjects+"/blank.obj"):
      f = open(xObjects+"/blank.obj", 'w')
      print("A\n800\nOBJ\n", file=f)  # Python 3.x
      f.close()

   # 
   # writing library file
   #
   f = open(xLib, 'w')
   print("creating library.txt...",end='',flush=True)
   print("A\n800\nLIBRARY\n\nREGION_DEFINE Xroads", file=f)  # Python 3.x

   # find Orthos 
   for ortho in glob.glob(xpDir+"/Custom Scenery/zOrtho4XP*"):
      #print("doing "+ortho,flush=True)
      ortho = re.sub('\.lnk',r'',ortho)
      p = re.sub('^.*XP_([\+\-][^\+\-]*)(.*)',r'REGION_RECT \2 \1 \2 \1',ortho)
      print(p, file=f)

   print("\nREGION Xroads\nEXPORT_EXCLUDE lib/g10/roads.net 1000_roads/roads.net\nEXPORT_EXCLUDE lib/g10/roads_EU.net 1000_roads/roads_EU.net\n", file=f)  # Python 3.x

   print("EXPORT_EXCLUDE simheaven/ground/parking_cars.fac    objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/parking_trucks.fac  objects/blank.fac\nEXPORT_EXCLUDE simheaven/ground/solar_panel.obj     objects/blank.obj\n", file=f)  # Python 3.x

   f.close();
   print("done.")

   time.sleep(2)
#
