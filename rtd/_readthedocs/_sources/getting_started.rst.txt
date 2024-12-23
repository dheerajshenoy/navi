.. toctree::
    :titlesonly:

Getting Started
===============

Take control of your files because they deserve more than just 'open' and 'close'.

Configuring Navi
================

Navi is configured using the `Lua <https://www.lua.org/start.html>`_ scripting language. Do not worry if you do not know this programming language as it's very easy to understand. Once you get used to the syntax and ideology of lua, you can write your own functions and extend navi. Navi checks for the configuration file in the directory *~/.config/navi/* and the file *config.lua*. If no such file exists, navi loads with the defaults settings with the vim style :ref:`Default_Keybindings` and sane defaults.

Sample configuration file
=========================

Check out the sample :download:`config file <./_static/config.lua>`. Copy this file to the *~/.config/navi/* directory and relaunch navi to see the new settings.

Scripting with Lua
==================

Navi can be scripted to include custom actions by subscribing to hooks and perform custom actions in response to those hooks. Check out :doc:`lua_scripting` to know more about scripting and including custom behavior to navi and extending it.

Using Navi
==========

Naming Conventions
++++++++++++++++++

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

.. _Default_Keybindings:

Default Keybindings
===================

Using navi is easy. You have bunch of commands that do certain things and sometimes they depend on the context (see DWIM commands). The default settings are set up for you to use navi without any configurations needed. The default keybindings are listed below:

* `h` to go to the parent directory
* `j` to go the next item
* `k` to go the previous item
* `l` to select the current item
* `gg` to go to the first item
* `Shift+g` to go the last item
* `Space` to toggle marking for the current item
* `Shift+Space` to toggle inverse marking for the current directory
* `Ctrl+a` to mark all the items in the current directory
* `Shift+r` to rename current item if no marks are present or all the marked items if marks are present
* `Shift+d` to delete DWIM
* `yy` to copy DWIM
* `p` to paste cut/copy marked items to the current directory
* `Shift+u` to unmark all the local marks
* `.` toggle hidden/dot files
* `/` Search for item in the current directory
* `n` Go to the next match for the search
* `Shift+n` Go to the previous match for the search
* `Ctrl+m` toggle menubar
* `Ctrl+p` toggle preview panel
* `Ctrl+l` focus the path widget
* `Shift+v` Toggle visual line selection mode

**NOTE: These keybindings are not loaded if there is a `keybindings` table in the lua configuration file**

Theming
=======

Since navi is a Qt GUI library based application, it can be styled using the `qt6ct` package available on linux. The colors mentioned in the configuration file will override the respective control colors of the theme.
