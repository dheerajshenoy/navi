Navi Changelogs
===============

The changelogs for Navi along with their version is mentioned in the reverse chronological order below:

v1.2.8 (29 January 2024)
------------------------

+ Enable disable icons text rendering gap fix
+ Added feature to enable/disable file panel grid
+ File panel grid styles

v1.2.7 (25 January 2024)
------------------------

+ Ability to check for updates
+ Scrolling up and down unusual visual highlight bug fixed.

v1.2.6 (21 January 2024)
------------------------

+ Added context menu support different for file and folder.
+ Added ctrl+click support for multiple selection
+ Scroll up and down like C+u and C+d
+ Fixed thumbnail caching parellization

v1.2.5 (11 January 2024)
-----------------------

+ Added cursor. Cursor shows the current row in a different color (if you choose to) than the rest of the items. This is important during visual selection of files since previously the last item in the selection was assumed to be the position of the cursor in the file panel.
+ Optimized data calls from the model and view using separate roles for filename, filepath and symlink names.

v1.2.4 (8 January 2024)
-----------------------

+ Symlink item property

+ **Fixed**: dragging file from navi when file_panel contained more than one column resulted in dragging files equal to the number of columns.
+ **Fixed**: File preview doesn't work after directory change.

v1.2.3 (6 January 2024)
-----------------------

+ Added copy-to, move-to command to copy and move marked files to a location using a file choosing dialog.
+ Fixed enabled disable icon in file_panel bug.
+ Utils table to navi global namespace.
+ Fixed default directory bug.

v1.2.2 (5 January 2024)
-----------------------

+ Added symlink styling support (italic, bold, underline, separator selection, background and foreground)
+ Lua API functions for symlink
+ Pre-compiled headers addition in source code for faster code compilation

v1.2.1 (2 January 2024)
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
