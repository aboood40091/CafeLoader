# CafeLoader (SD branch)
Custom code loader for the Nintendo Wii U.

## Wii U Plugin System
This is a plugin for the [Wii U Plugin System (WUPS)](https://github.com/Maschell/WiiUPluginSystem). To be able to use this plugin, you have to place the resulting `.mod` file in to the following folder:

```
sd:/wiiu/plugins
```  

When the file is placed on the SDCard, you can load it with [Wii U Plugin Loader](https://github.com/Maschell/WiiUPluginLoader).

## How to use
Projects have to be compiled to a special format in order to be loaded by CafeLoader.  
For that reason, you will have to compile your project using [CafeLoader Project Compiler](https://github.com/aboood40091/CafeLoader-Project-Compiler).  

An example can be found [here](https://github.com/aboood40091/NSMBU-haxx).  

After you have compiled your project, you would then need to place the patches (``Addr.bin``, ``Code.bin``, ``Ctors.bin``, ``Data.bin`` and ``Patches.hax``) in the following folder:

```
sd:/cafeloader/TITLE_ID
```  

Where ``TITLE_ID`` is the title ID of the game you want to patch. (Without ``-``. All letters must be uppercase)  

Example:

```
sd:/cafeloader/0005000010101D00
```

## Replacing game files
This branch of CafeLoader allows you to load game files from the SD card (Just like SDCafiine).  

This is where your game's ``content`` folder would be:

```
sd:/cafeloader/vol/TITLE_ID/content
```  

Again, where ``TITLE_ID`` is the title ID of the game you want to patch. (Without ``-``. All letters must be uppercase)  

Example:

```
sd:/cafeloader/vol/0005000010101D00/content
```


## Special Thanks:
* [Kinnay](https://github.com/Kinnay): original concept.