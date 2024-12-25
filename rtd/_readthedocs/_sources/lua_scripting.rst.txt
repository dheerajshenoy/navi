.. toctree::
    :maxdepth: 1

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
        navi.hook.add("item_select", function ()
            navi.io.msg("You selected an item...YAY!")
        end)
    end

Now, whenever you select (open or enter a directory) an item, you'll get the message, "You selected an item...YAY!".

Next, you can refer to :ref:`navi-lua-api` for reference to the lua API for navi.

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

.. note:: Use the following helper function if you want the `fd` and `rg` code to run correctly

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
