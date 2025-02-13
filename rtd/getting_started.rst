Getting Started
===============

Take control of your files using navi, because they deserve more than just **open** and **close**.

Launching Navi
++++++++++++++

The navi binary is actually called ``navifm`` [navi name for a package was already taken :( ]. You can execute the execute this command or if you have a run launcher you can find the program ``NaviFM`` with no icon [still haven't gotten to work on the icon yet].

.. _Default_Keybindings:

First time usage
++++++++++++++++

Now that you have navi opened, have a good look at it. You might or might not like the way the default appearance of navi. Remember that you can configure every part (almost) of the UI through lua (covered in the next section).

If you have loaded navi without a config file, navi loads the default settings. Here are the default keybindings:

.. list-table:: Keybindings
   :widths: 20 30 50
   :header-rows: 1

   * - Key
     - Command
     - Description
   * - h
     - up-directory
     - Go to the parent directory
   * - j
     - next-item
     - Go to the next item
   * - k
     - prev-item
     - Go to the previous item
   * - l
     - select-item
     - Select item
   * - f
     - filter
     - Filter item visibility
   * - g,g
     - first-item
     - Go to the first item
   * - z,z
     - middle-item
     - Go to middle item
   * - Shift+g
     - last-item
     - Go to the last item
   * - Shift+r
     - rename-dwim
     - Rename item(s)
   * - Shift+d
     - delete-dwim
     - Delete item(s)
   * - Space
     - toggle-mark-dwim
     - Mark item(s)
   * - F5
     - refresh
     - Refresh current directory
   * - Shift+Space
     - mark-inverse
     - Mark inverse item(s)
   * - Shift+v
     - visual-select
     - Visual selection mode
   * - :
     - execute-extended-command
     - Execute extended command
   * - y,y
     - copy-dwim
     - Copy item(s)
   * - p
     - paste
     - Paste item(s)
   * - Shift+u
     - unmark-local
     - Unmark all item(s)
   * - /
     - search
     - Search (regex)
   * - n
     - search-next
     - Search next
   * - Shift+n
     - search-prev
     - Search previous
   * - Ctrl+m
     - menu-bar
     - Toggle menu bar
   * - Ctrl+p
     - preview-pane
     - Toggle preview pane
   * - Ctrl+l
     - focus-path
     - Focus path bar
   * - Shift+t
     - trash-dwim
     - Trash item(s)
   * - .
     - hidden-files
     - Toggle hidden items
   * - q
     - macro-record
     - Record or Finish recording macro
   * - Shift+c
     - copy-path
     - Copy path(s)
   * - Ctrl+u
     - scroll-up
     - Scroll up a page
   * - Ctrl+d
     - scroll-down
     - Scroll down a page

Configuring Navi
++++++++++++++++

Navi is configured using the `Lua <https://www.lua.org/start.html>`_ scripting language. Do not worry if you do not know this programming language as it's very easy to understand. Once you get used to the syntax and ideology of lua, you can write your own functions and extend navi. Navi checks for the configuration file in the directory *~/.config/navi/* and the file *init.lua*. If no such file exists, navi loads with the defaults settings with the vim style :ref:`Default_Keybindings` and sane defaults.

Sample configuration file
+++++++++++++++++++++++++

Check out the sample :download:`config file <./_static/config.lua>`. Copy this file to the *~/.config/navifm/* directory and relaunch navi to see the new settings.

Directory Structure
+++++++++++++++++++

You can have your *~/.config/navi/* directory structured in the following way:

.. code-block:: text

    ~/.config/navi/
    ├── init.lua
    ├── bookmarks.lua
    └── lua/
        ├── functions.lua
        └── some_other_file.lua

The ``lua`` directory is a special directory which is added to the :ref:`runtime_path` of navi. Any lua files in this directory can be used to load functions, variabled etc. from. This becomes very useful and powerful for organizing and modularizing your navi scripts.

**Example**

Assume we have the directory structure as the following:

.. code-block:: text

    ~/.config/navi/
    ├── init.lua
    ├── bookmarks.lua
    └── lua/
        ├── functions.lua
        └── keymaps.lua

Inside the ``init.lua`` file you can do something like:

.. code-block:: lua

   local functions = require("functions")
   functions.hello() -- assuming that hello function exists in the functions.lua file

   require("keymaps") -- we just include and load keymaps here without using whatever is inside.

Using Navi
++++++++++

.. _runtime_path:

Runtime Path
~~~~~~~~~~~~

The runtime path is a mechanism for managing the locations where Lua scripts are loaded from. It is particularly important in situations where Lua files are stored in various directories, and you need to dynamically determine where to load these scripts from, based on user preferences, installation directories, or custom paths.

The runtime path refers to a collection of directories that Lua uses to search for Lua files during runtime. This search path is defined by the package.path in Lua, which consists of one or more directory paths where Lua scripts (with the .lua extension) are located. Each time navi executes Lua code, it uses the paths specified in the runtime path to locate and load the necessary Lua files.


Naming Conventions
~~~~~~~~~~~~~~~~~~

1.  `Item` refers to a file or a directory.

2.  `Highlighting` refers to the action of clicking a directory item (one or more than one) once. This means that you can use your mouse to select and highlight multiple items in the directory.

3.  `Marking` refers to the action of the `mark` or `toggle-mark` command to "select" one or more currently highlighted directory items. You can see the list of all the files that you have marked in the `Marks List` using `View -> Marks List` or by using the `show-marks` command. When you call any of the commands, the commands will be aware of these markings and do the action accordingly. For example, if we use the `rename` command and if there are marked files, your choice of text editor is opened with the list of all the file names. You can change these and write to the file to rename them accordingly.

Bookmarks
+++++++++

Bookmarks allows you to instantly go to your favourite or frequently visited directories. You can add, edit, remove and load bookmarks from a special bookmarks file `bookmark.lua` which should be stored in config directory where the `config.lua` file exists. Bookmark file has the following structure:

.. code-block:: lua

    BOOKMARKS = {
        bookmarkName1 = {
            path = "bookmarkPath1",
        },

        bookmarkName2 = {
            path = "bookmarkPath2",
            highlight_only = true,
        }
    }

The *highlight_only* key tells Navi not to go into the directory bookmarkPath2, instead just go the directory containing that file and just put the cursor (or highlight) the item pointed by the location.

**NOTE: When navi startsup, it checks for the `bookmark.lua` file and loads it if it exists.**

Go to bookmark
~~~~~~~~~~~~~~

Once bookmarks are loaded, you can use the command `bookmark-go` and provide a bookmark name as argument to it (or it asks for the bookmark name if you do not give the argument) and if the name exists, it takes you to the path associated with that bookmark name.

Edit bookmark
~~~~~~~~~~~~~

Name
----

You can use the command `bookmark-edit-name` to edit the name of the bookmark which exists.

Filepath
--------

You can use the command `bookmark-edit-path` to edit the path pointed to an item associated with a bookmark name.

Add bookmark
~~~~~~~~~~~~

You can add new bookmarks directly to the `bookmark.lua` file BOOKMARKS table. Or, you can add bookmarks interactively from within Navi. You can visit any directory you want to bookmark, and then once you are there, just call `bookmark-add` followed by the name of the bookmark as argument (or it asks for the bookmark name if no argument is provided).

**NOTE: The bookmark names have to be unique, and therefore navi errors out if you provide the same bookmark name that already exists in the bookmarks file**

Save bookmark
~~~~~~~~~~~~~

Once you add bookmarks from within navi, you can save the changes by calling `bookmarks-save` to write the bookmarks to the `bookmark.lua` file.

Interactive Commands
++++++++++++++++++++

These are commands that can be executed using the `inputbar` UI component. By default, pressing `: (colon)` key pops up the inputbar. You can type a name of the command in this bar or a line number to go to in the current directory of the focused pane. Example: `mark` or `42`, where `mark` would mark the current item under focus or highlighted items (more than one items).

Types of Command
~~~~~~~~~~~~~~~~

There are four variants for most of the commands: `Current`, `Local`, `Global` and `DWIM` (Do What I Mean).

Current Scope
-------------

This applies to the currently selected item. Any command that is `Current` scoped will run only on the currently highlighted item.

**This includes mouse selection (also called as Highlighting)**.

Local Scope
-----------

This applies to the marks that are in the current directory which is conveniently called as a **Local Mark**. Any command that is `Local` scoped will run on the local marks.

**This does not include mouse highlighting.**

Global Scope
------------

This applies to the marks that are present in any directory. Unlike the `Local` scope the command that is `Global` scoped will run on the marks that are present in any directory.

**This does not include mouse highlighting**.

DWIM
----

`DWIM wikipedia page <https://en.wikipedia.org/wiki/DWIM>`_

Do What I Mean version of commands. If no local marks are present, run the command on the current item, otherwise run the commands on the marks.

Commands List
~~~~~~~~~~~~~

Below is an exhaustive list of commands that are currently available in Navi.

Navigation
----------

``next-item``

Go to the next item

``prev-item``

Go to the previous item

``select-item``

Select the current item

``middle-item``

Go to the middle item

``up-directory``

Go to the parent directory

``first-item``

Go to the first item

``last-item``

Go to the last item

Echo (Printing) Commands
------------------------

``echo-info``

Echos or prints the information given as argument with the *info-face* font

``echo-warn``

Echos or prints the information given as argument with the *warning-face* font

``echo-error``

Echos or prints the information given as argument with the *error-face* font

Marking Item
------------

``mark``

Marks the current item.

``toggle-mark``

Toggle the mark of the current item.

``toggle-mark-dwim``

DWIM version of `toggle-mark` command

``mark-all``

Marks all the items in the current directory.

``mark-inverse``

Inverses the marks in the current directory.

``mark-dwim``

DWIM version of `toggle-mark`

Unmarking Item
--------------

``unmark``

Unmarks the current item.

``unmark-local``

Unmarks **Local Marks**

``unmark-global``

Unmarks **Global Marks**

Selecting Item
--------------

``visual-select``

Enters into *visual selection* mode. Navigation is followed by selection until the mode is toggled off.

Change Permission
-----------------

``chmod``

Change the permission of the current item using three digit numbers

Example: 777, 666, 000 etc.

``chmod-local``

Change permission for **Local Marks**

``chmod-global``

Change permission for **Global Marks**

``chmod-dwim``

Change permission **DWIM** style

Renaming Files
--------------

**NOTE: Renaming more than `bulk-rename-threshold` (configuration option) which is by default 5 will trigger a **[Bulk Rename](#bulk-rename)** process.

``rename``

Rename the highlighted item.

``rename-local``

Renames items in the **Local Marks**

``rename-global``

Renames items in the **Global Marks**

``rename-dwim``

Renames items in **DWIM** style

Bulk Rename
-----------

This is where a text editor of your choice (set it in the configuration) opens up a temporary "rename file" and enables you to rename all the marked files once you save and close the said "rename file". By default navi uses the `neovim` text editor.

Cutting Files
-------------

``cut``

Cut (prepare for moving) the current item.

``cut-local``

Register cut for **Local Marks**

``cut-global``

Register cut for **Global Marks**

``cut-dwim``

Register cut in **DWIM** style

Copying Files
-------------

``copy``

Copy the current item.

``copy-local``

Register copy for **Local Marks**

``copy-global``

Register copy for **Global Marks**

``copy-dwim``

Register copy in **DWIM** style

Pasting (Moving) Files
----------------------

``paste``

Paste (Move) the current item.

Deleting Files
--------------

**WARNING: Please be careful when using delete commands, this does not trash the items, it directly deletes them. If you want to trash use the *trash* command**

``delete``

Delete the highlighted items(s).

``delete-local``

Deletes items in the **Local Marks**

``delete-global``

Deletes items in the **Global Marks**

``delete-dwim``

Deletes items in **DWIM** style

Trashing Files
--------------

``trash``

Trash the current item.

``trash-local``

Trashes items in **Local Marks**

``trash-global``

Trashes items in **Global Marks**

``trash-dwim``

Trashes items in **DWIM** style

Filtering Items
---------------

``filter``

Set a filter to directory.

Example: `*` displays everything, `*.csv` displays only the csv files, `*.png` displays only the png files

``reset-filter``

Reset the appplied filter.

``hidden-files``

Toggles the hidden files.

**NOTE**: Hidden files are those items whose name start with a period like `.config`, `.gitignore` etc.

``dot-dot``

Toggles the .. file item.

Panes
-----

``bookmark-pane``

Opens the bookmarks list.

TODO: work in progress

``marks-pane``

Opens the marks list.

<img src="./screenshot/marks-pane-demo.gif" height="400px" width="600px" />

``messages-pane``

Opens the messages list.

<img src="./screenshot/messages_pane.png" height="400px" width="500px" />

``preview-pane``

Toggles the preview pane.

The preview pane handles previewing images (good number of formats) and PDF documents (first page) `asynchronously`. This means that the previewing experience will be seamless and without any lag. Navi uses `ImageMagick` library under the hood for previewing images and therefore any image formats supported my ImageMagick is supported by Navi.

``shortcuts-pane``

This displays the list of all the shortcuts.

<img src="./screenshot/shortcuts_pane.png" height="400px" width="600px" />

Misc
----

``fullscreen``

Toggle fullscreen mode for Navi.

``execute-extended-command``

This is the function that pops up the inputbar to enter the interactive commands.

``menu-bar``

Toggles the menu bar.

``focus-path``

Focuses the path widget and sets it in edit mode.

``item-property``

Display the property of the currently focused item.

``header``

Toggle the display of header information.

``cycle``

Toggle the cycle (last item to first item and vice-versa) during navigation.

``refresh``

Force refresh the current directory.

``syntax-highlight``

Toggle the syntax highlight for text preview

**NOTE: By default, Navi watches the directory for changes and loads them, so there is no requirement to refresh the directory. This command is there just in case something does not look right.**

``mouse-scroll``

Toggle mouse scroll support for file panel

``reload-config``

Re-reads the configuration file if it exists and loads the configurations.

``tasks``

Opens the task widget showing all the running tasks like command execution with outputs and file operations like copying, moving and deleting/trashing.

https://github.com/user-attachments/assets/7cc31950-3cd1-4d2b-a244-b6ba6cdcd32a

``cd``

Change directory with the provided argument or ask for input.

``folder-property``

Get the property of the currently open folder

``copy-path``

Copy the path(s) of the currently "highlighted" (not to be confused with marked) files.

You can pass an optional argument which will be the separator that separates the paths of all the selected files.

**NOTE: You can configure the default separator in the config using the `copy_path_separator` variable**

``exit``

Exits Navi

``new-window``

Creates a new instance of navi

Shell Commands
--------------

``shell``

Run a shell command *asynchronously* (non-blocking).

The running commands can be seen in the `Task Widget` using the `tasks` command

Macro
-----

Macros allow you to record actions performed inside Navi and save it, which you can then recall it to perform the action over and over again.

``macro-record``

This records / finishes recording (if already recording) a macro under a _macro key_. Macro key is a string identifier under which the macro has to be recorded. It can be any random string that you can remember to recall it when you want to play the macro.

``macro-play``

This plays a macro under a specific macro key.

``macro-edit``

Opens macro commands recorded under a specific macro key (if it exists) with your editor to edit, which will then be saved and can be recalled.

``macro-list``

Lists all the macros that have been defined.

``macro-save-to-file``

Saves macro to a file which can be used to load for the next boot of Navi.

``macro-delete``

Delete macro under a macro key (if it exists).

Theming
+++++++

Since navi is a Qt GUI library based application, it can be styled using the `qt6ct` package available on linux. The colors mentioned in the configuration file will override the respective control colors of the theme.
