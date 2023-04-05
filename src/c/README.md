
# Xroads

Xroads is a X-Plane library which improves the realistic touch of the Ortho4XP created ortho images by hiding the street polygons drawn on the road network. They are usually generated from street databases such as OSM. However, if you're using a ZL17 (or higher) Ortho image, you might prefer to not see the generated streets but the roads as seen on the downloaded satellite images. Xroads will NOT hide bridges, highways, autobahn and railroad tracks. As a positive side impact of not drawing the road polygons, you might see a slightly increased FPS count.

To allow better control where to enable or disable these autogen roads, a library.txt file gets generated, which contains the coordinates of all available Ortho4XP tiles. These tiles will then be using the "transparent roads". Locations for which we do not have a Ortho tile, the autogen roads are still being shown.

During the creation of the new roads.net files, the "speed" of the AI cars is being reduced to 70%. This makes the cars travel with a more realistic speed. The 70% is the default but the command line option "-v" (stands for Velocity) can be used to select any other percentage between 50 and 100%.

After installation you simply run the binary again if Ortho tiles have been added or deleted. The program will then just update the existing library.txt accordingly.

"xroads" looks for folders and shortcuts starting with "zOrtho..." to get the tile coordinates. It is also scanning the "Earth nav data" folder inside sceneries folders (or symlinks) whos name start with "zPhoto...", "zVstates.." and "z_..." . The difference is, that it still works on Ortho4XP tiles as before, but it also works on "Ortho bundles", which cover multiple tiles, like the "US Orthophotos" provided by @Forkboy2. To hide the roads network on them, just rename your scenery folder to start with "zOrtho", "zPhoto" ,"zVstates" or "z_" and run the program. That's it.

Some examples:

    zOrtho4XP_+48+011
    zOrtho_some_cool_ortho
    zOrtho_Forkboy2_California_v5
    zPhoto_Italy_Milano
    z_ortho_1234
    z_ParisTexas



Xroads "help" and command line options (macos example):

   % ./xroads.app -h
   Xroads - 0.39 - melbo @x-plane.org

     usage: ./xroads.app [-v velocity] [-l] [-h]

       -v  set percentage of default car velocity
       -l  left hand driving support
       -h  this help



# Installation

Download the ZIP file and copy the correct binary (xroads.exe for Windows, xroads.app for macOS) to your X-Plane base directory and execute it. The program creates a folder "Xroads" in your "Custom Scenery" folder. Then it copies the "roads.net" and "roads-EU.net" files from the "default scenery" to it and modifies them on the fly. 

It also creates a "library.txt" file inside the "Custom Scenery/Xroads" folder. The library.txt contains the coordinates of the tiles which should use "transparent roads". The coordinates are taken from the "Earth nav data" folders from sceneries whos name start with "zOrtho", "zPhoto", "zVstates" or "z_".

The content of an optional xroads.pre in the same folder is being inserted BEFORE the region definition.
The content of an optional xroads.add in the same folder is being inserted AFTER the tile coordinates. This allows to set non-Ortho tiles to use "transparent roads" as well.
The content of an optional xroads.opt is being APPENDED to the and of the library.txt, which allows manual additions.


# Uninstall

Simply delete the "Xroads" folder in your "Custom Scenery" folder and restart X-Plane. No other file has been changed. 


# scenery_packs.ini

Make sure the Xroads folder stay on the top of your scenery_packs.ini.


# Build

Windows:

    requires "dirent.h"  ( i.e. from https://github.com/tronkko/dirent/blob/master/include/dirent.h )

	cl xroads.c /D "NODEBUG" /O2 /link Ole32.lib


macOS or Linux:

	make xroads



# Left-Hand-Driving Support

Based on @troopie's idea, Xroads is now able to integrate LHD into the transparent roads too. However there is some manual work to do.

First create a "xroads.add" file inside the X-Plane main folder. Its content will be included in the generated library.txt. You can simply use the one included in the ZIP file and add/remove the tile coordinates you want to use LHD. You will have to put the tiles coordinates under the Xroads_UK or Xroads_LH definition. The difference is which NET file is being used ( roads.net or roads_EU.net). The format is the same as in the existing library.txt. Actually you can just copy/paste tile coordinates from the library.txt to the xroads.add file.

Next step is to run xroads ( exe or app ) with the command line option "-l" (stands for LHD). You can use the included "xroads.bat" file which you can edit with NOTEPAD or any other text-editor. Note: Starting a line with REM turns the whole line into a comment, so commands in that line are not executed. It allows you to prepare the command with parameters but you only need click on the BAT and let it pass the options automatically to the binary.

