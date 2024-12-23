Lua scripting
=============

Navi can be extended using lua for adding custom behaviour by creating functions, subscribing to hooks, changing default navi behaviour.

Special Function (INIT_NAVI)
++++++++++++++++

The function `INIT_NAVI` will be called during the startup time of Navi. This function can be used to add hooks and whatnot to your configuration.

.. code-block:: lua

    function INIT_NAVI()
        navi.io.msg("Navi just booted")
    end


Custom functions
++++++++++++++++

You can write custom functions in lua with arguments. Navi passes the current file name and directory name to each of these functions when they are called. You can then proceed to process the file from within lua using Navi.

.. code-block:: lua

    function someFunction(fileName, dirName)
        navi.io.msg("WOW! Navi is currently on the file " .. fileName .. " and inside the " .. dirName .. " directory!")
    end

You can then call this function within Navi by calling the `lua <function_name>` command or by just calling `lua` command and then typing in the function name in the inputbar. Doing this will execute the function. The function in the example above when executed will display:

"WOW! Navi is currently on the file `<fileName>` and inside the `<dirName>` directory!"

Hooks
+++++

Hooks are signals that are emitted when navi does a certain action which you can "subscribe" to to create a custom action associated with that signal. Hooks are emitted for actions like selecting an item, changing directory etc.

You can add hook using the `navi.hook.add` api. It takes two arguments, both of which are required. The first one is a `hook_name` which is a valid hook name (see hook names below). Second argument is a function that will be executed in response to the hook.

Hook functions can be of any number. But keep in mind that, after emitting the hook, Navi executes each of these functions (if there are multiple), so it's best to add functions that are efficient.

Hook Names
~~~~~~~~~~

* Select Item ``item_select``
* Item Changed ``item_changed``
* Up Directory ``directory_up``
* Directory Loaded ``directory_loaded``
* Directory Changed ``directory_changed``
* Drive Mounted ``drive_mounted``
* Drive Unmounted ``drive_unmounted``
* Visual Line mode ``visual_line_mode_on`` & ``visual_line_mode_off``
* Filter mode ``filter_mode_on`` & ``filter_mode_off``

Example:

.. code-block:: lua

    function INIT_NAVI()
        navi.hook.add("item\_select", function ()
            navi.io.msg("You selected an item...YAY!")
        end)
    end

Now, whenever you select (open or enter a directory) an item, you'll get the message, "You selected an item...YAY!".

Navi Lua API
++++++++++++

API or Application Programming Interface allows user to customize the behaviour or add extra functionalities to Navi.

There is a main `navi` table. In this table are *four* more tables: `ui`, `api`, `io` and `shell`.

UI
~~

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

+ .. function:: statusbar

    Toggles the statusbar.

    :rtype: ``void``

menubar
*******

    Menubar table

    :type: ``table``

    + .. function:: toggle

        Toggles the visibility state of the menubar.

        :rtype: ``void``

    + .. function:: toggle(state bool)

        Toggles the menubar with the state *state*.

        :state: Visibility state of the menubar
        :type: ``bool``

        :rtype: ``void``


.. function:: add_menu(menu_item MenuItem)

   :menu_item: The menu item table to add to the menu
   :type: ``MenuItem``
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


Example Usage:
~~~~~~~~~~~~~~

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


API
~~~

+ .. function:: sort_name

    Sorts the item by name

    :rtype: void

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

    :filename: name of the file
    :type: ``str``

    :rtype: ``void``

+ .. function:: new_folder(foldername str, [foldername2 str, ...])

    Create one or more folders in the current working directory.

    :foldername: name of the folder
    :type: ``str``

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

IO
~~

+ .. function:: msg(message str, msgtype MsgType)

    Prints message in the statusbar

    :message: The message string to print out
    :type: ``str``

    :msgtype: The type of the message
    :type: ``MsgType``

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

    :prompt: Prompt text
    :type: ``str``

    :default_text: The default text in the input field
    :type: ``str``

    :selection_text: The text that has to be selected in the input field
    :type: ``str``

    :rtype: ``str``


Shell
~~~~~

+ .. function:: execute(command str)

    Execute a shell command asynchronously.

    :command: command string to execute

    :rtype: ``void``

You can execute any shell commands from within navi.

What can you do with scripting ?
++++++++++++++++++++++++++++++++

Setting wallpaper
~~~~~~~~~~~~~~~~~

.. code-block:: lua

    function setWallpaper()
        navi.shell("xwallpaper --stretch" .. filename)
    end

Open a terminal at the current directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: lua

    function terminalAtPoint()
        local terminal = os.getenv("TERMINAL")
        local dir = navi.api.pwd()
        navi.spawn(terminal, { dir })
    end

Using `fd` to navigate to directory with matching name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Helper function
***************

*NOTE: Use the following helper function if you want the `fd` and `rg` code to run correctly*

.. code-block:: lua

    -- Helper function that returns the STDOUT of a command `commandString`
    function command(commandString)
        local command = commandString
        local handle = io.popen(command)
        local result = handle:read("*a")
        handle:close()
        return result:gsub('[\n\r]', '')
    end

FD and RG for file searching
****************************

.. code-block:: lua

    -- Use `fd` to go to the best file match having the name given as the input
    function fd()
        local input = navi.io.input("Search (FD)")
        if input ~= "" then
            local pwd = navi.api.pwd()
            local commandString = string.format("fd %s %s --type=directory | head -n 1", input, pwd)
            local result = command(commandString)
            if result ~= "" then
                navi.api.cd(result)
            else
                navi.io.msg("No results", navi.io.messagetype.warn)
            end
        end
    end

Use `rg` (ripgrep) command to navigate to a directory with the matching content
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: lua

    -- Use `ripgrep` to go to the file content with the name given as the input
    function rg()
        local input = navi.io.input("Search (RG)")
        if input ~= "" then
            local pwd = navi.api.pwd()
            local commandString = string.format("rg %s %s -l --sort-files | head -n 1", input, pwd)
            local result = command(commandString)
            if result ~= "" then
                navi.api.cd(result)
                navi.api.highlight(result)
            else
                navi.io.msg("No results", navi.io.messagetype.warn)
            end
        end
    end

Use `zoxide` for faster navigation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: lua

    function zoxide()
        local input = navi.io.input("Zoxide CD")
        if input ~= "" then
            local commandString = string.format("zoxide %s", input)
            local result = command(commandString)
            if result ~= "" then
                navi.api.cd(result)
            else
                navi.io.msg("No results", navi.io.messagetype.warn)
            end
        end
    end

*NOTE: You could also use the `navi.shell.execute` api to execute the commands. But because this functionality exists in Lua, we use it.*

Go to a random directory inside your home directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: lua

    function randomDirectory()
        local commandString = "fd . ~ -t d | shuf -n 1"
        local result = command(commandString)
        if result ~= "" then
            navi.api.cd(result)
        end
    end

Highlight a random file inside your home directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: lua

    function randomFile()
        local commandString = "fd . ~ | shuf -n 1"
        local result = command(commandString)
        if result ~= "" then
            navi.api.cd(result)
            navi.api.highlight(result)
        end
    end
