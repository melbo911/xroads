
# Xroads

Xroads is a X-Plane 11 library which improves the realistic touch of the Ortho4XP created ortho images by hiding the auto-generated street polygons layed out by default. They are usually generated from street databases such as OSM. However, if you're using a ZL17 (or higher) Ortho image, you might prefer to not see the generated streets but the roads as seen on the downloaded satellite images. Xroads will NOT hide bridges, highways, autobahn and railroad tracks. As a positive side impact of not drawing the road polygons, you might see a slightly increased FPS count.

To allow better control where to enable or disable these autogen roads, a library.txt file gets generated, which contains the coordinates of all available Ortho4XP tiles. These tiles will then be using the "transparent roads". Locations for which we do not have a Ortho tile, the autogen roads are still being shown.

During the creation of the new roads.net files, the "speed" of the AI cars is being reduced to 70%. This makes the cars travel with a more realistic speed. The 70% is hardcoded in the code but can be modified based on own taste.

After installation you simply run the binary again if Ortho tiles have been added or deleted. The program will then just update the existing library.txt accordingly.


New since version 0.12.0:

"xroads" looks for files and shortcuts starting with "zOrtho4XP\_\*" to get the tile coordinates. It is also scanning the "Earth nav data" folder inside sceneries folders (or symlinks) whos name start with "zOrtho". The difference is, that it still works on Ortho4XP tiles as before, but it also works on "Ortho bundles", which cover multiple tiles, like the "US Orthophotos" provided by @Forkboy2. To hide the autogen roads on them, just rename your scenery folder to start with "zOrtho" and run the program. That's it.

Some examples:

    zOrtho4XP_+48+011
    zOrtho_some_cool_ortho
    zOrtho_Forkboy2_California_v5


# Installation

Download the ZIP file and copy the correct binary (xroads.exe for Windows, xroads.app for macOS) to your X-Plane base directory and execute it. The program creates a folder "Xroads" in your "Custom Scenery" folder. Then it copies the "roads.net" and "roads-EU.net" files from the "default scenery" to it and modifies them on the fly. 

It also creates a "library.txt" file inside the "Custom Scenery/Xroads" folder. The library.txt contains the coordinates of the tiles which should use "transparent roads". The coordinates are taken from the "Earth nav data" folders from sceneries whos name start with "zOrtho".

To improve the great X-Europe scenery even more, it creats dummy objects and facades to "hide" the parking lots and solar panels, to also use the satellite image rather than the gray autogen polygons. 

The content of an optional xroads.opt is being appended to the library.txt, which allows manual additions.
The content of an optional xroads.add in the same folder is being inserted after the tile coordinates. This allows to set non-Ortho tiles to use "transparent roads" as well.


# Uninstall

Simply delete the "Xroads" folder in your "Custom Scenery" folder and restart X-Plane. No other file has been changed. 


# scenery_packs.ini

Make sure the Xroads folder stay on the top of your scenery_packs.ini.


# Build

Windows:

    requires "dirent.h"  ( i.e. from https://github.com/tronkko/dirent/blob/master/include/dirent.h )

	cl xroads


macOS or Linux:

	make xroads



