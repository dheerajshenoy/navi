
How does navi work ?
--------------------

Info about navi
~~~~~~~~~~~~~~~

Navi is written using the C++ programming language. For the graphical user interface (GUI) it's using the Qt GUI framework. For the lua scripting it is using the sol2 lua C++ interface library.

How scripting works ?
~~~~~~~~~~~~~~~~~~~~~

As mentioned before, we are using the lua programming language to script, extend and customize navi. The navi lua API interface is written in the lua programming language itself. When you specify any of the valid navi lua APIs, the interface talks to the runtime of the running instance of the C++ **Navi** class.

.. note:: If you are curious about the lua C++ navi api interface, if you have installed navi on your system, you could look at the file ``/usr/share/navifm/_lua/navifm.lua``. Navi actually uses the binary form of this lua file which ends with the extension **luac** because it is faster to load and read by navi during it's startup.

The lua code calls the C++ functions directly using the sol2 library in C++.
 
