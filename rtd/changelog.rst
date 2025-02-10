Navi Changelogs
---------------

The changelogs for Navi along with their version is mentioned in the reverse chronological order below:

v1.3.3 (8 February 2025)
------------------------

+ Added completion support for lua functions
+ Added general command completions
+ Added completion match count indicator
+ Added completion lua API

v1.3.2 (8 February 2025)
------------------------

+ Changed navi instantiation from lua


v1.3.1 (6 February 2025)
------------------------

+ Changed project name from **navi** to **navifm**
+ Tried to add project to the archlinux linux user repository (AUR)
+ Replaced the QCompleter terrible no use completion with custom working completion like neovim, emacs etc. for the inputbar

v1.3.0 (3 February 2025)
------------------------

+ Changed QStatusBar to QWidget for the statusbar.
+ Fixed X11/XCB docking black artifact.
+ Pathbar widget customization Lua API added.
+ Added tag for the release.
+ Fixed custom input dialog.
+ Added new custom question dialog.

v1.2.9 (1 February 2025)
------------------------

+ Added dock widgets to panels
+ Added KDDockWidgets as submodule as a replacement for the Qt dock widget.
+ Default shortcuts when no user config is found.
+ Disable closing file panel dock widget.
+ Fixed KDDockWidgets MainWindow interfering with QStatusBar, FilePathWidget and Inputbar.

v1.2.8 (29 January 2024)
------------------------

+ Enable disable icons text rendering gap fix
+ Added feature to enable/disable file panel grid
+ File panel grid styles
+ Vertical header cursor color support
+ Added vertical header lua customizing options

v1.2.7 (25 January 2025)
------------------------

+ Ability to check for updates
+ Scrolling up and down unusual visual highlight bug fixed.

v1.2.6 (21 January 2025)
------------------------

+ Added context menu support different for file and folder.
+ Added ctrl+click support for multiple selection
+ Scroll up and down like C+u and C+d
+ Fixed thumbnail caching parellization

v1.2.5 (11 January 2025)
------------------------

+ Added cursor. Cursor shows the current row in a different color (if you choose to) than the rest of the items. This is important during visual selection of files since previously the last item in the selection was assumed to be the position of the cursor in the file panel.
+ Optimized data calls from the model and view using separate roles for filename, filepath and symlink names.

v1.2.4 (8 January 2025)
-----------------------

+ Symlink item property

+ **Fixed**: dragging file from navi when file_panel contained more than one column resulted in dragging files equal to the number of columns.
+ **Fixed**: File preview doesn't work after directory change.

v1.2.3 (6 January 2025)
-----------------------

+ Added copy-to, move-to command to copy and move marked files to a location using a file choosing dialog.
+ Fixed enabled disable icon in file_panel bug.
+ Utils table to navi global namespace.
+ Fixed default directory bug.

v1.2.2 (5 January 2025)
-----------------------

+ Added symlink styling support (italic, bold, underline, separator selection, background and foreground)
+ Lua API functions for symlink
+ Pre-compiled headers addition in source code for faster code compilation

v1.2.1 (2 January 2025)
-----------------------

+ Added set lua API functions for ui elements to set options together in one go.
+ Fixed toolbar lua :func:`set_items` causing navi to crash.
+ Read-in all the members defined in the navi table.

v1.2 (30 December 2024)
----------------------

+ Re-wrote the entire lua API to support getters and setters for options.
+ API is written in lua and interfaces to C++ rather than the other way around.
+ removed :func:`toggle` for UI elements

v1.1 (November 2024)
-------------------

+ Added lua api


v1.0 (October 2024)
------------------

+ Added basic file manager features


**Versions before 1.0 isn't recorded.**
