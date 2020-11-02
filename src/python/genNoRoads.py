#!/usr/bin/env python3
#
# Creates new roads.net and roads-EU.net files for use with Ortho4XP tiles and Xroads.
# The new net files remove autogen roads (except highways/autobahn and rails) and
# slows down the AI car traffic to a certain level. 
#
# Create the Xroads scenery library using genXroads.py 
#
# melbo @ x-plane.org
# version 1.0
#

import sys
import re
import os
import shutil
import glob

speed = 70             # default: reduce cars speed to 70 %
xpDir = sys.path[0]
xroadsDir = xpDir + '/Custom Scenery/Xroads'
defRoads = xpDir + '/Resources/default scenery/1000 roads'
xRoads = xroadsDir + '/1000_roads'
xLib = xroadsDir + '/library.txt'

#
# functions
#
def genFile(fname):
   global defRoads,xRoads

   try:
      print("setting up new "+fname+"...",end='',flush=True)
      infile = open(defRoads+"/"+fname, "r")
      outfile = open(xRoads+"/"+fname, "w")
      rail = 0
      keep = 0
      for line in infile:
         if  " Junction BYT" in line:
            if  "BYT1000" in line:
               keep = 1
               #line = "# MELBO KEEP " + line
            elif  "BYT1100" in line:
               keep = 1
               #line = "# MELBO KEEP " + line 
            elif  "BYT1200" in line:
               keep = 1
               #line = "# MELBO KEEP " + line 
            else:
               keep = 0
               #line = "# MELBO HIDE " + line 

         elif keep == 0 and "QUAD " in line:
            line = "#" + line

         elif "TRI " in line or (rail == 0 and "SEGMENT_DRAPED" in line):
            line = "#" + line

         elif  "#rail_" in line:
            rail = 1

         elif "CAR_DRAPED" in line or "CAR_GRADED" in line:
            t = line.split()
            t[3] = str(int(int(t[3]) / 100 * speed)) 
            if int(t[3]) < 1:
               t[3] = 1
            line = ' '.join(t)+"\n"

         outfile.write(line)
      infile.close()
      outfile.close()
   except:
      print("Could not create new "+fname+". Exit.")
      sys.exit()

#
# start main here
#
if __name__ == '__main__':
   if not os.path.isdir(xroadsDir):
      try:
         print("creating directory ",xroadsDir)
         os.makedirs(xroadsDir)
      except:
         print("Could not create required directory ",xroadsDir,". Exit.")
         sys.exit()

   if not os.path.isdir(xRoads):
      try:
         print("creating directory ",xRoads)
         os.makedirs(xRoads)
      except:
         print("Could not create required directory ",xRoads,". Exit.")
         sys.exit()

   if not os.path.isdir(defRoads):
      print("default scenery roads.net not found. Exit.")
      sys.exit()

   genFile("roads.net")

   genFile("roads_EU.net")

   print("done.")
   sys.exit()

#
