# JIM-DB
To build JIM-DB you need:
 - python version < 3.0 
 - scons 2.4.
 - 7zip installed and included to path
 - Visual Studio 2015 Community recommendet for the v140 compiler. (It's the only tested yet!)
 
To build JIM-DB call in cmd:
1. scons library
2. scons library -c
3. scons

To use it properly you need a configuration for it. You can generate a configuration by starting JIM-DB with ```-generate configfile```. If you have a configuration start it with the ```-config configuration``` flag.
