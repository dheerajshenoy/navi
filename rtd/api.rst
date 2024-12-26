.. _navi-lua-api:

Navi Lua API
============

API or Application Programming Interface allows user to customize the behaviour or add extra functionalities to Navi.

Datatypes
+++++++++

All the datatypes supported by lua are supported in the Navi lua API. Here is the description of the datatypes that you will come across in this api documentation.

+----------+---------------------------------------------------------------------------+
| Datatype | Values                                                                    |
+==========+===========================================================================+
| nil      | Used to differentiate the value from having some data or no(nil) data.    |
+----------+---------------------------------------------------------------------------+
| boolean  | Includes true and false as values. Generally used for condition checking. |
+----------+---------------------------------------------------------------------------+
| number   | real(double precision floating point) numbers.                            |
+----------+---------------------------------------------------------------------------+
| string   | array of characters.                                                      |
+----------+---------------------------------------------------------------------------+
| function | a method that is written in C or Lua.                                     |
+----------+---------------------------------------------------------------------------+
| table    | ordinary arrays, symbol tables, sets, records, graphs, trees, etc.,       |
+----------+---------------------------------------------------------------------------+

.. note:: In addition to the above datatypes, it is useful to know the following:

   ``<datatype>[]`` represents array of that datatype.

   **Example:** ``str[]`` represents table (array) of strings { "string1", "string2", "string3", ... }

Navi Table
++++++++++

**navi** is the main `table <https://www.tutorialspoint.com/lua/lua_tables.htm>`_.

Members of the navi table:

+ ui
+ api
+ keymap
+ io
+ shell
+ hook
+ bookmark

ui
++

.. module:: navi.ui

.. function:: marks

   Toggles the marks pane.

   :return: ``void``

.. function:: shortcuts

   Toggles the shortcuts pane.

   :return: ``void``

.. function:: messages

   Toggles the messages pane.

   :return: ``void``

.. function:: preview_panel

   Toggles the preview pane.

   :return: ``void``

.. function:: pathbar

   Toggles the path bar.

   :return: ``void``

context_menu
~~~~~~~~~~~~

    .. module:: navi.ui.context_menu

.. function:: create(t: table)

   Creates a custom context menu

   TODO

statusbar
~~~~~~~~~

    Statusbar table.

    .. module:: navi.ui.statusbar

.. function:: toggle

       Toggles the visibility state of the menubar.
       :return: ``void``

.. function:: toggle(state: bool)

   Toggles the menubar with the state *state*.

   :param bool state: Visibility state of the menubar.
   :return: ``void``

.. function:: create_module(module_name: str, options: table)

   Create a statusbar module with the name *module_name* and with the options *options*.
   The module thus created can be used in :func:`set_modules`

   :return: ``void``

   .. data::
    Options has the following members:

    :param str text: text to be displayed
    :param bool italic: whether the text has to be italic or not
    :param bool bold: whether the text has to be bold or not
    :param str background: background color of the module
    :param bool hidden: visibility of the module
    :param str color: foreground color of the module


**Example**

.. code-block:: lua

    local custom = navi.ui.statusbar.create_module("custom", {
        text = "HELLO",
        italic = true,
        bold = true,
        background = "#FF5000",
        hidden = true,
        color = "#000000",
    })

.. function:: set_modules(module_names: str[])

    List of module names to add to the statusbar.

    There are standard modules which can be added to the statusbar. Their names are mentioned below.

    .. note:: When defining custom modules be mindful while naming them and do not use the standard module names.

    **Standard module names**

        +---------------+-----------------------------------------------------+
        | Module Name   | Purpose                                             |
        +===============+=====================================================+
        | name          | Current item name                                   |
        +---------------+-----------------------------------------------------+
        | macro         | Macro indicator                                     |
        +---------------+-----------------------------------------------------+
        | visual_mode   | Visual mode indicator                               |
        +---------------+-----------------------------------------------------+
        | stretch       | Add stretch to separate the left and right section  |
        +---------------+-----------------------------------------------------+
        | filter        | Filter mode indicator                               |
        +---------------+-----------------------------------------------------+
        | count         | Total item count in the directory                   |
        +---------------+-----------------------------------------------------+
        | size          | Current highlighted item size                       |
        +---------------+-----------------------------------------------------+
        | modified_date | Modified date of the current item                   |
        +---------------+-----------------------------------------------------+
        | permission    | Permission of the current item                      |
        +---------------+-----------------------------------------------------+

  **Example**

  .. code-block:: lua

        navi.ui.statusbar.set_modules({
            "name",
            "macro",
            "visual_mode",
            "stretch",
            "filter",
            "count",
            "custom", -- custom module name goes here
            "size",
            "modified_date",
            "permission",
        });

.. function:: set_module_text(name: str, text: str)

   :param str name: Name of the module
   :param str text: Text to assign to the module

   :return: ``void``

   **Example**

    .. code-block:: lua

        navi.ui.statusbar.set_module_text("mod1", "HELLO WORLD")

.. function:: set_module_text(name: str, func: function)

   :param str name: Name of the module
   :param function func: Some function which returns a text

   :return: ``void``

   **Example**

    .. code-block:: lua

        navi.ui.statusbar.set_module_text("mod1", function ()
            return "HELLO WORLD"
        end)


toolbar
~~~~~~~

.. module:: navi.ui.toolbar

    Toolbar table.

    :type: ``table``

.. function:: toggle

    Toggles the visibility state of the menubar.

    :return: ``void``

.. function:: toggle(state: bool)

    Toggles the menubar with the state *state*.

    :param bool state: Visibility state of the menubar
    :return: ``void``

.. data:: ToolbarItem

    ToolbarItem has the following structure:

    .. code-block:: lua

        ToolbarItem {
            label = "Hello World",
            -- path to supported image for icon or xdg-standard icons
            icon = "<path-to-image-file>", -- or icon = "<xdg-standard-icons>",
            -- action to execute on user click
            action = function ()
            navi.io.msg("HELLO WORLD", navi.io.msgtype.info)
            end,
            --[[
            position of the button in the toolbar.
            Not zero index based.
            If not mentioned, widget will be inserted at the end.
            --]]
            position = 2,
        }


.. function:: create_button(name: str, options: table)

    :param str name: unique name to identify the button
    :param table options: options for the button
    :return: ``ToolbarItem``

    Creates a button with provided options. This is used to add button to the toolbar.

    **Example**

    .. code-block:: lua

        local btn = navi.ui.toolbar.create_button("btn", {
            label = "BTN",
            action = function ()
                navi.io.msg("HELLO WORLD", navi.io.msgtype.warn)
            end,
            icon = "printer",
        })

.. function:: add_button(toolbar_item: ToolbarItem)

    :param ToolbarItem toolbar_item: The toolbar item table to add to the toolbar
    :return: ``void``

    Adds the toolbar_item provided as argument to the menubar.

    **Example**

    .. code-block:: lua

        local btn = navi.ui.toolbar.create_button("btn", {
            label = "BTN",
            action = function ()
                navi.io.msg("HELLO WORLD", navi.io.msgtype.warn)
            end,
            icon = "printer",
        })

        navi.ui.toolbar.add_button(btn)

.. function:: set_items(item_names: str[])

    List of items to add to the toolbar. Items are nothing but the buttons. Items can be standard item names or custom created buttons.

    :param str[] item_names: table of item names
    :return: ``void``

    .. note:: When defining custom modules be mindful while naming them and do not use the standard module names.

    **Standard module names**

    +---------------+-----------------------------------------------------+
    | Item Name     | Action                                              |
    +===============+=====================================================+
    | home          | Go to Home directory                                |
    +---------------+-----------------------------------------------------+
    | previous_dir  | Go to previous directory                            |
    +---------------+-----------------------------------------------------+
    | next_dir      | Go to next directory                                |
    +---------------+-----------------------------------------------------+
    | parent_dir    | Go to parent directory                              |
    +---------------+-----------------------------------------------------+
    | refresh       | Refresh the current directory                       |
    +---------------+-----------------------------------------------------+


    **Example**

    .. code-block:: lua

        local btn = navi.ui.toolbar.create_button("btn", {
            label = "BTN",
            action = function ()
                navi.io.msg("HELLO WORLD", navi.io.msgtype.warn)
            end,
            icon = "printer",
        })

        navi.ui.toolbar.set_items({
            "home",
            "previous_dir",
            "next_dir",
            "parent_dir",
            btn,
            "refresh",
        })


menubar
~~~~~~~


.. module:: navi.ui.menubar

Menubar table

.. function:: toggle

    Toggles the visibility state of the menubar.

    :rtype: ``void``

.. function:: toggle(state: bool)

    Toggles the menubar with the state *state*.

    :param bool state: Visibility state of the menubar
    :return: ``void``


.. function:: add_menu(menu_item: MenuItem)

    Adds the menuitem provided as argument to the menubar.

    :param MenuItem menu_item: The menu item table to add to the menu.
    :return: ``void``

    MenuItem has the following structure:

    .. code-block:: lua

        MenuItem = {
            label = "menu label",  -- The label of the menu item.
            submenu = {            -- A nested submenu (optional).
            label = "submenu", -- The label of the submenu.
            action = function() -- Action associated with the submenu.
            -- Function implementation here
            end,
            -- Additional submenu items can be added here.
            }
        }

    **Example**

    .. code-block:: lua

        custom_menu = {
            label = "Custom Menu",
            submenu = {
                {
                    label = "Open",
                    action = function()
                        navi.io.msg("HELLO WORLD");
                    end,
                },

                {
                    label = "sub",
                    submenu = {
                        {
                            label = "item 1",
                            action = function ()
                                navi.io.msg("SUB HELLO");
                            end
                        }
                    },
                },

                {
                    label = "Save",
                    action = function() print("Save clicked") end,
                }
            }
        }

        navi.ui.menubar.add_menu(custom_menu)

api
+++

.. module:: navi.api

.. function:: list_runtime_paths

    Lists all runtime paths configured for Navi. It is useful for debugging or for Lua scripts that need to interact with runtime directories (e.g., searching for scripts, configurations, or plugins). In short, these are the paths at which navi looks for user defined lua scripts.

    :return: ``str[]``

.. function:: register_function(name: str, func: sol::function)

   Registers a Lua function with the specified name.

   :param str name: The name of the function to register in the Lua environment.
   :param function func: The Lua function to be registered.
   :return: ``void``

   .. note:: Registering a function means to expose the user defined lua function to be used inside navi by allowing user to call the function using the ``lua`` command followed by the function name.

.. function:: unregister_function(name: str)

   Unregisters a previously registered Lua function.

   :param str name: The name of the function to unregister.
   :return: ``void``

.. function:: list_registered_functions()

   Returns a list of all registered Lua functions.

   :return: A table of function names and the corresponding functions (``table``)

.. function:: sort_name

    Sorts the item by name

    :return: ``void``

.. function:: count

    Returns the number of items in the current directory.

    :return: ``int``

.. function:: search

    Search for the text string given as argument.

    :return: ``void``

.. function:: next_item

   Go to the next item.

   :return: ``void``

.. function:: prev_item

   Go to the previous item.

   :return: ``void``

.. function:: first_item

   Go to the first item in the current directory.

   :return: ``void``

.. function:: last_item

   Go to the last item in the current directory.

   :return: ``void``

.. function:: middle_item

   Go to the middle item in the current directory.

.. function:: select_item

   Select the current item.

   :return: ``void``

.. function:: item_name

   Returns the name of the currently highlighted item.

   :return: ``str``

.. function:: item_property

   Returns the item property of the currently highlighted item.

   :return: ``ItemProperty``

.. function:: parent_dir

    Go to the parent directory.

    :return: ``void``


.. function:: mount_drive(drive_name str)

   Mounts drive with the name `drive_name` to an available mount point.

   :drive_name: The device name of the drive that should be mounted. ``str`` 
   :raises: Emits an error signal if the mounting operation fails.

   .. note:: Under the hood navi uses ``udisks2`` utility for mounting disks.

   **Example:**
   
   .. code-block:: lua

       local name = "/dev/nvme0n1p1/"
       navi.api.mount_drive(name)

.. function:: unmount_drive(drive_name str)

   Unmounts the drive specified with the *drive_name*.

   :drive_name: The filesystem path of the mounted drive to unmount. ``str``
   :raises: Emits an error signal if the unmounting operation fails.

   **Example:**
   
   .. code-block:: lua

       local name = "/dev/nvme0n1p1"
       navi.api.unmount_drive(name)

.. function:: search_next

    Search for the next item matching the search term. If no search has been performed, asks user for the search term.

    :return: ``void``

.. function:: search_prev

    Search for the previous item matching the search item. If no search has been performed, asks user for the search term.

    :return: ``void``

.. function:: is_file(path str)

    Check if the filepath provided points to a file.
    Returns ``true`` if it's a valid and existent file or returns ``false``.

    :return: ``bool``

.. function:: is_dir(path str)

    Check if the filepath provided points to a directory.
    Returns ``true`` if it's a valid and existent directory or returns ``false``.

    :return: ``bool``

.. function:: create_file(file_name str, [file_name2 str, ...])

    Create one or more files in the current working directory.

    :filename: name of the file ``str``
    :return: ``void``

.. function:: create_dir(dir_name str, [dir_name2 str, ...])

    Create one or more directory(s) in the current working directory.

    :foldername: name of the folder ``str[]``
    :return: ``void``

.. function:: trash

    Trashes the currently highlighted file(s) (if no marked files exist).

    :return: ``void``

.. function:: trash_dwim

   DWIM version of :func:`trash`

    :return: ``void``

.. function:: copy

    Registers the currently highlighted file(s) (if no marked files exist) for copying.

    :return: ``void``

.. function:: copy_dwim

    Registers the file for copying in a DWIM fashion.

    :return: ``void``

.. function:: cut_dwim

    Registers the file for cutting in a DWIM fashion.

    :return: ``void``

.. function:: cut

    Registers the file (s) for copying (if no marked files exist).

    :return: ``void``

.. function:: has_marks_local

    Check for local marks. Returns ``true`` if there are local marks, else ``false``.

    :return: ``bool``

.. function:: has_marks_global

    Check for global marks. Returns ``true`` if there are global marks, else ``false``.

    :return: ``bool``

.. function:: global_marks

    Returns the list of global marks.

    :return: ``table``

.. function:: local_marks_count

    Returns the count of local marks.

    :return: ``int``

.. function:: global_marks_count

    Returns the count of global marks

    :return: ``int``

.. function:: local_marks

    Returns the list of local marks in the current working directory

    :return: ``table``

.. function:: highlight(item_name str)

    Highlights the item with the name passed as argument if it exists.

    :item_name: ``str``

    :return: ``void``

.. function:: cd

    Change the current working directory to the string provided as argument.

    :return: ``void``

.. function:: pwd

    Returns the current working directory

    :return: ``str``

.. function:: filter(filter_string: str)

    Filter the directory using the *filter_string*.

    :return: ``void``

    **Example**

    .. code-block:: lua

        navi.api.filter("*.csv") -- this would filter the directory to show only csv files.

.. function:: reset_filter

   Resets the filter applied using :func:`filter`.

   :return: ``void``

   .. note:: This is the same as using the filter function with the ``*`` argument i.e ``navi.api.filter("*")``

.. function:: mark

   Marks the currently highlighted item.

   :return: ``void``

.. function:: mark_inverse

   Marks the current directory items inversly.

   :return: ``void``

   .. note:: Inverse behaviour means that any item which is not highlighted will be highlighted and those which have been highlighted will be unhighlighted.

.. function:: mark_all

   Marks all the items in the current directory.

   :return: ``void``

.. function:: mark_dwim

   DWIM version of :func:`mark`.

   :return: ``void``

   .. note:: DWIM (Do What I Mean) versions of a command is similar to the original command except this version will act depending on whether items have been marked or not. If items have been marked, the commands will act on the marked items else it will act only on the currently selected item.

.. function:: unmark

   Unmarks the currently highlighted item.

   :return: ``void``

.. function:: unmark_dwim

   DWIM version of :func:`unmark`

   :return: ``void``

.. function:: toggle_mark

   Toggles the mark of the currently highlighted item.

   :return: ``void``

.. function:: toggle_mark_dwim

   DWIM version of :func:`toggle_mark`.

   :return: ``void``

.. function:: rename

   Rename the currently highlighted item.

   :return: ``void``

.. function:: rename_dwim

   DWIM version of :func:`rename`.

    :return: ``void``

.. function:: delete

   Deletes the currently highlighted item.

   :return: ``void``

   .. note:: Deleting files will get rid of the files permanently. If you want to send the files to trash, use :func:`trash` instead.

.. function:: delete_dwim

   DWIM version of :func:`delete`.

   :return: ``void``

.. function:: chmod

   Changes permission of the file. Asks the user for permission number change.

   :return: ``void``

.. function:: spawn(command: str, args: str[])

   :param str command: name of the command
   :param str[] args: arguments to be passed to the command
   :return: ``void``

.. function:: has_selection

   Returns ``true`` if there is any selection in the current working directory, else ``false``.

   :return: ``bool``



.. data:: ItemProperty

   :members:
       - name (str): Name of the item
       - size (str): Formatted size of the item 
       - mimeName (str): Mime name of the item


io
++

.. module:: navi.io

.. function:: msg(message str, msgtype MsgType)

    Prints message in the statusbar

    :message: The message string to print out ``str``
    :msgtype: The type of the message ``MsgType``

    :returns: ``void``

.. function:: input(prompt str, default_text str, selection_text str)

    Get input from the user and return the value

    :prompt: Prompt text ``str``
    :default_text: The default text in the input field ``str``
    :selection_text: The text that has to be selected in the input field ``str``
    :return: ``str``

.. data:: MsgType

    Represents message types.

    **Members**

    * ``error``

        Prints the message using the *error face*

    * ``warning``

        Prints the message using the *warning face*

    * ``info``

        Prints the message using the *info face*


keymap
++++++

.. module:: navi.keymap

.. function:: set(t table)

    This function takes a table processes it to set a key mapping.

    The table is expected to include key-value pairs that define a keyboard shortcut (key), the associated command (command), and an optional description (desc).

    :table: A table containing the keymap details. The table should include:

        :key: The keyboard shortcut (e.g., "Ctrl+C") ``str``.
        :command: The command to execute (e.g., "copy") ``str``.
        :desc (optional): A description of the keymap (e.g., "Copy text") ``str``.

    .. note:: You can have shortcuts with repeating keys by separating them with a comma.

        **Example**: g,g ; this is activated when you press g followed by another g.

    **Example**

    .. code-block:: lua

        navi.keymap.set({ key = "g,z", command = "next-item", desc = "say hello" })


.. function:: set(key str, command str, [desc str])

    This function allows directly setting a key mapping by specifying the keyboard shortcut, command, and description as separate string arguments.

    :key: A std::string specifying the keyboard shortcut (e.g., "Ctrl+C") ``str``.
    :command: A std::string specifying the command to execute (e.g., "copy") ``str``.
    :desc: A std::string providing a description of the keymap (e.g., "Copy text") ``str``.

   .. note:: You can have shortcuts with repeating keys by separating them with a comma.

     **Example**: g,g ; this is activated when you press g followed by another g.

   **Example**

   .. code-block:: lua

      navi.keymap.set( "h", "up-directory", "Go to the parent directory" )

shell
+++++

.. module:: navi.shell

.. function:: execute(command str)

    Execute a shell command asynchronously.

    :command: command string to execute ``str``
    :return: ``void``

You can execute any shell commands from within navi.

hook
++++

The Lua API provides functionality to manage hooks, allowing developers to define and trigger custom behaviors dynamically.

.. note:: For information about hooks please refer to :ref:`hooks-section`

.. module:: navi.hook

.. function:: add(hook_name str, func function)

    Adds a Lua function to a specified hook.

    :hook_name: The valid name of the hook to which the function should be added. ``str``
    :func: The Lua function to execute when the hook is triggered. ``function``
    :raises: Emits an error signal if the hook name is invalid or the function cannot be added.
    :returns: ``void``


   **Example:**

   .. code-block:: lua

       navi.hook.add("on_file_open", function()
           print("File opened!")
       end)

       -- Output:
       -- "File opened!" will be printed when the `on_file_open` hook is triggered.

.. function:: trigger(hook_name str)

   Triggers all functions attached to a specified hook.

   :hook_name: The valid name of the hook to trigger. ``str``
   :raises: Emits an error signal if the hook name is invalid or no functions are attached.

   **Example:**

   .. code-block:: lua

       navi.hook.trigger("on_file_open")

       -- Output:
       -- Executes all functions added to the "on_file_open" hook.

.. function:: clear_functions(hook_name str)

   Clears all functions attached to a specified hook.

   :hook_name: The name of the hook to clear. ``str``
   :raises: Emits an error signal if the hook name is invalid or the functions cannot be cleared.

   **Example:**
   
   .. code-block:: lua

       navi.hook.clear_functions("on_file_open")

       -- Output:
       -- Removes all functions from the "on_file_open" hook.

Usage Notes
~~~~~~~~~~~

- **Hooks** allow for customizable and dynamic behaviors to be executed when specific events occur.
- Ensure `hook_name` corresponds to a valid event or identifier used in your application.
- Functions added to hooks are executed in the order they are added when the hook is triggered.
- Add efficient functions to hooks that are executed on every small motions like ``file_open``, ``directory_change`` etc.

Error Handling
~~~~~~~~~~~~~~

- Invalid `hook_name` values will result in error signals. Ensure hooks are registered properly before adding functions or triggering them.
- Functions that fail during execution do not halt the triggering process but should be debugged individually.

Bookmark
++++++++

The `bookmark` module in the Lua API allows you to manage bookmarks in the application. This includes adding, removing, and organizing bookmarks.

.. module:: navi.bookmark

.. function:: add(bookmark_name str, file_path str, highlight bool)

   Add a bookmark to the application.

   :param str bookmark_name: The name of the bookmark.
   :param str file_path: The file path associated with the bookmark.
   :param bool highlight: Whether to visually highlight the bookmark in the application.
   :return: None
   :raises: Any exceptions from the internal bookmark manager.

   **Example**::

       navi.bookmark.add("MyBookmark", "/home/user/documents", true)

.. function:: navi.bookmark.remove(bookmark_name)

   Remove a bookmark from the application.

   :param str bookmark_name: The name of the bookmark to remove.
   :return: None
   :raises: Any exceptions from the internal bookmark manager.

   **Example**::

       navi.bookmark.remove("MyBookmark")
