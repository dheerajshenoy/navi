.. _navi-lua-api:

Navi Lua API
============

API or Application Programming Interface allows user to customize the behaviour or add extra functionalities to Navi.

There is a main `navi` table. In this table are *four* more tables: `ui`, `api`, `io` and `shell`.

ui
++

+ .. function:: marks

    Toggles the marks pane.

    :rtype: ``void``

+ .. function:: shortcuts

    Toggles the shortcuts pane.

    :rtype: ``void``

+ .. function:: messages

    Toggles the messages pane.

    :rtype: ``void``

+ .. function:: preview_panel

    Toggles the preview pane.

    :rtype: ``void``

+ .. function:: pathbar

    Toggles the path bar.

    :rtype: ``void``

statusbar
~~~~~~~~~

    Statusbar table.

    :rtype: ``void``

    + .. function:: toggle

        Toggles the visibility state of the menubar.

        :rtype: ``void``

    + .. function:: toggle(state bool)

        Toggles the menubar with the state *state*.

        :state: Visibility state of the menubar ``bool``

        :rtype: ``void``

    + .. function:: create_module(module_name str, options table)

        Create a statusbar module with the name *module_name* and with the options *options*.
        The module thus created can be used in :func:`set_modules`

        .. data:: options

        This has the following members:
        
            :text: text to be displayed ``str``
            :italic: whether the text has to be italic or not ``bool``
            :bold: whether the text has to be bold or not ``bool``
            :background: background color of the module ``str``
            :hidden: visibility of the module ``bool``
            :color: foreground color of the module ``str``

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

    + .. function:: set_modules(module_names str[])

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

toolbar
~~~~~~~

    Toolbar table.

    :type: ``table``


    + .. function:: toggle

        Toggles the visibility state of the menubar.

        :rtype: ``void``

    + .. function:: toggle(state bool)

        Toggles the menubar with the state *state*.

        :state: Visibility state of the menubar ``bool``

        :rtype: ``void``


    + .. data:: ToolbarItem

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


    + .. function:: create_button(name str, options table)

        :name: unique name to identify the button ``str``
        :options: options for the button ``table``
        :rtype: ``ToolbarItem``

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

    + .. function:: add_button(toolbar_item ToolbarItem)

        :toolbar_item: The toolbar item table to add to the toolbar ``ToolbarItem``
        :returns: ``void``

        Adds the toolbar_item provided as argument to the menubar.

        **Example Usage**

        .. code-block:: lua

            local btn = navi.ui.toolbar.create_button("btn", {
                label = "BTN",
                action = function ()
                    navi.io.msg("HELLO WORLD", navi.io.msgtype.warn)
                end,
                icon = "printer",
            })

            navi.ui.toolbar.add_button(btn)

    + .. function:: set_items(item_names str[])

        List of items to add to the toolbar. Items are nothing but the buttons. Items can be standard item names or custom created buttons.


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


        **Example Usage**

        .. code-block:: lua

            navi.ui.toolbar.set_items({
                "home",
                "previous_dir",
                "next_dir",
                "parent_dir",
                "refresh",
            })


menubar
~~~~~~~

    Menubar table

    :type: ``table``

    + .. function:: toggle

        Toggles the visibility state of the menubar.

        :rtype: ``void``

    + .. function:: toggle(state bool)

        Toggles the menubar with the state *state*.

        :state: Visibility state of the menubar ``bool``

        :rtype: ``void``


    + .. function:: add_menu(menu_item MenuItem)

        :menu_item: The menu item table to add to the menu ``MenuItem``
        :returns: ``void``

        Adds the menuitem provided as argument to the menubar.

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

        **Example Usage**

        .. code-block:: lua

            function INIT_NAVI()
                -- This is a function with special meaning to Navi.
                -- This will be called on startup

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

            end


api
+++

+ .. function:: sort_name

    Sorts the item by name

    :rtype: ``void``

+ .. function:: search

    Search for the text string given as argument.

    :rtype: ``void``

+ .. function:: search_next

    Search for the next item matching the search term. If no search has been performed, asks user for the search term.

    :rtype: ``void``


+ .. function:: search_prev

    Search for the previous item matching the search item. If no search has been performed, asks user for the search term.

    :rtype: ``void``

+ .. function:: new_files(filename str, [filename2 str, ...])

    Create one or more files in the current working directory.

    :filename: name of the file ``str``
    :rtype: ``void``

+ .. function:: new_folder(foldername str, [foldername2 str, ...])

    Create one or more folders in the current working directory.

    :foldername: name of the folder ``str``
    :rtype: ``void``

+ .. function:: trash

    Trashes the currently highlighted file(s) (if no marked files exist).

    :rtype: ``void``

+ .. function:: copy

    Registers the currently highlighted file(s) (if no marked files exist) for copying.

    :rtype: ``void``

+ .. function:: copy_dwim

    Registers the file for copying in a DWIM fashion.

    :rtype: ``void``

+ .. function:: cut_dwim

    Registers the file for cutting in a DWIM fashion.

    :rtype: ``void``

+ .. function:: cut

    Registers the file (s) for copying (if no marked files exist).

    :rtype: ``void``

+ .. function:: has_marks_local

    Check for local marks. Returns ``true`` if there are local marks, else ``false``.

    :rtype: ``bool``

+ .. function:: has_marks_global

    Check for global marks. Returns ``true`` if there are global marks, else ``false``.

    :rtype: ``bool``

+ .. function:: global_marks

    Returns the list of global marks.

    :rtype: ``table``

+ .. function:: local_marks_count

    Returns the count of local marks.

    :rtype: ``int``

+ .. function:: global_marks_count

    Returns the count of global marks

    :rtype: ``int``

+ .. function:: local_marks

    Returns the list of local marks in the current working directory

    :rtype: ``table``

+ .. function:: highlight(item_name str)

    Highlights the item with the name passed as argument if it exists.

    :item_name: ``str``

    :rtype: ``void``

+ .. function:: cd()

    Change the current working directory to the string provided as argument.

    :rtype: ``void``

+ .. function:: pwd()

    Returns the current working directory

    :rtype: ``str``

io
++

+ .. function:: msg(message str, msgtype MsgType)

    Prints message in the statusbar

    :message: The message string to print out ``str``
    :msgtype: The type of the message ``MsgType``

    :returns: ``void``

+ .. data:: MsgType

    Represents message types.

    **Members**

    * ``error``

        Prints the message using the *error face*

    * ``warning``

        Prints the message using the *warning face*

    * ``info``

        Prints the message using the *info face*

+ .. function:: input(prompt str, default_text str, selection_text str)

    Get input from the user and return the value

    :prompt: Prompt text ``str``
    :default_text: The default text in the input field ``str``
    :selection_text: The text that has to be selected in the input field ``str``
    :rtype: ``str``

shell
+++++

+ .. function:: execute(command str)

    Execute a shell command asynchronously.

    :command: command string to execute ``str``
    :rtype: ``void``

You can execute any shell commands from within navi.

