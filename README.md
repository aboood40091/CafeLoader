# CafeLoader
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

## Client
CafeLoader provides a client which can be used to replace game files (Just like Cafiine).  

In order for the Wii U to know which PC on the network it's looking to connect to, you have to type in your PC's IP in a file named ``ip.bin`` and place it in:

```
sd:/cafeloader
```  

``client.py`` is able to generate an ``ip.bin`` for you if you run it with ``--bin`` as an argument, like this:

```
py -3 client.py --bin
```  

Your game's ``content`` folder would placed in the following directory (relative to ``client.py``):

```
vol/TITLE_ID/content
```  

Again, where ``TITLE_ID`` is the title ID of the game you want to patch. (Without ``-``. All letters must be uppercase)  

Example:

```
vol/0005000010101D00/content
```

### Replacing SD Card contents
``client.py`` can also replace files on your SD Card. The root of the SD Card would as follows (relative to ``client.py``):

```
vol/external01
```  

**Don't use this to load the patches!** It will result in a crash. I'm currently working on fixing this.


## Special Thanks:
* [Kinnay](https://github.com/Kinnay): original concept + ``client.py``.