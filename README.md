
# Xroads

Xroads is a X-Plane 11 library which improves the realistic touch of the Ortho4XP created ortho images by hiding the auto-generated street polygons layed out by default. They are usually generated from street databases such as OSM. However, if you're using a ZL17 (or higher) Ortho image, you might prefer to not see the generated streets but the roads as seen on the downloaded satellite images. As a positive side impact of not drawing the road polygons, you might see a slightly increased FPS count.

To allow better control where to enable or disable these autogen roads, a library.txt file gets generated, which contains the coordinates of all available Ortho4XP tiles. These tiles will then be using the "hidden roads". Locations for which we do not have a Ortho tile, the autogen roads are still being shown.

During the creation of the new net files, the "speed" of the AI cars is being reduced to 70%. This makes the cars travel with a more realistic speed. The 70% is hardcoded but can be modified based on own taste.


There are Python scripts and C code incl. Windows and macOS binaries available.


