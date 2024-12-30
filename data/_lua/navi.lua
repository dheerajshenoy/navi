local M = {}

local _navi = Navi.new()

_store_instance(_navi)

if not _navi then
    print("Could not create NAVI instance")
    os.exit(-1)
end

M.keymap = {}
M.io = {}
M.api = {}
M.ui = {}
M.opt = {}
M.hook = {}

setmetatable(M.opt, {
    __index = function(_, key)

        if key == "terminal" then
            return _navi:get_terminal()

        elseif key == "default_directory" then
                return _navi:get_default_dir()

        elseif key == "copy_path_separator" then
            return _navi:get_copy_path_separator()
        end
    end,

    __newindex = function(_, key, value)

        if key == "terminal" then
            _navi:set_terminal(value)

        elseif key == "default_directory" then
                _navi:set_default_dir(value)

        elseif key == "copy_path_separator" then
                _navi:set_copy_path_separator(value)
            end
        end
})

---Set the keymap
---@param key string
---@param command string
---@param desc string
M.keymap.set = function (key, command, desc)
    _navi:set_keymap(key, command, desc)
end


--[[ Inputbar metatable ]]--
M.ui.inputbar = {}

setmetatable(M.ui.inputbar, {

    __index = function (_, key)

        if key == "font" then
            return _navi:get_inputbar_font()

        elseif key == "background" then
            return _navi:get_inputbar_background()

        elseif key == "foreground" then
            return _navi:get_inputbar_foreground()

        elseif key == "font_size" then
            return _navi:get_inputbar_font_size()

        elseif key == "visible" then
            return _navi:get_inputbar_visible()

        end
    end,

    __newindex = function (_, key, value)
        if key == "font" then
            _navi:set_inputbar_font(value)

        elseif key == "background" then
            _navi:set_inputbar_background(value)

        elseif key == "foreground" then
            _navi:set_inputbar_foreground(value)

        elseif key == "font_size" then
            _navi:set_inputbar_font_size(value)

        end
    end
})

M.ui.pathbar = {}

M.ui.pathbar.toggle = function ()
    _navi:pathbar_toggle()
end

---Visibility of pathbar
---@param state boolean
M.ui.pathbar.visible = function (state)
    _navi:pathbar_shown(state)
end

---Focus the pathbar
M.ui.pathbar.focus = function ()
    _navi:focus_pathbar()
end

M.ui.statusbar = {}

---Toggles the statusbar
M.ui.statusbar.toggle = function ()
    _navi:toggle_statusbar()
end

---Set/update text to a statusbar module
---@param name string
---@param value string
M.ui.statusbar.set_module_text = function (name, value)
    _navi:set_statusbar_module_text(name, value)
end

M.ui.statusbar.set_modules = function (modules)
    _navi:set_statusbar_modules(modules)
end

---Creates module with name and options
---@param name string
---@param options table
M.ui.statusbar.create_module = function (name, options)
    return _navi:create_statusbar_module(name, options)
end

M.ui.toolbar = {}

---Toggle the toolbar
M.ui.toolbar.toggle = function ()
    _navi:toggle_toolbar()
end

---Represents a toolbar button with properties
---@class ToolbarItem
---@field label string - text to display
---@field icon string - path to valid image or standard theme icon names
---@field action function
---@field position integer

---Create toolbar button
---@param name string
---@param options table
M.ui.toolbar.create_button = function (name, options)
    _navi:create_toolbar_button(name, options)
end

---Adds button to toolbar
---@param button ToolbarItem
M.ui.toolbar.add_button = function (button)
    _navi.add_toolbar_button(button)
end

---Sets item for toolbar
---@param items string[]
M.ui.toolbar.set_items = function (items)
    _navi.set_toolbar_items(items)
end

---@class MenuItem
---@field label string - text to display
---@field submenu table - table with fields same as MenuItem (optional)
---@field action function - action to execute on clicking the item (optional)

M.ui.menubar = {}

M.ui.menubar.toggle = function ()
    _navi:toggle_menubar()
end

---Add menu to menubar
---@param item MenuItem
M.ui.menubar.add_menu = function (item)
    _navi:add_menubar_menu(item)
end

M.ui.preview_panel = {}

---Toggle preview panel
M.ui.preview_panel.toggle = function ()
    _navi:toggle_preview_panel()
end

---Returns the runtimepaths of Navi
---@return table
M.api.list_runtime_paths = function ()
    return _navi:list_runtime_paths()
end

---Registers a lua function to be used interactively in Navi
---@param name string
---@param func function
M.api.register_function = function (name, func)
    _navi:register_function(name, func)
end

---Unregister a function from Navi interactive functions
---@param name string
M.api.unregister_function = function (name)
    _navi:unregister_function(name)
end

---Lists the user registered functions inside Navi
---@return string[]
M.api.list_registered_functions = function ()
    return _navi:list_registered_functions()
end

---Returns the number of item(s) in the current working directory
---@return integer
M.api.count = function ()
    return _navi:count()
end

---Searches the current working directory for the item with the provided base name
---@param base_name string
M.api.search = function (base_name)
    _navi:search(base_name)
end

---Searches for the next occurence of the base name in the current working directory
M.api.search_next = function ()
    _navi:search_next()
end

---Searches for the previous occurence of the base name in the current working directory
M.api.search_prev = function ()
    _navi:search_prev()
end

---Go to the next item
M.api.next_item = function ()
    _navi:next_item()
end

---Go to the previous item
M.api.prev_item = function ()
    _navi:prev_item()
end

---Go to the first item
M.api.first_item = function()
    _navi:first_item()
end

---Go to the last item
M.api.last_item = function()
    _navi:last_item()
end

---Change the current working directory
---@param path string
M.api.cd = function (path)
    _navi:cd(path)
end

---Get the current working directory
---@return string
M.api.pwd = function ()
    return _navi:pwd()
end

---Copy
M.api.copy = function ()
    return _navi:copy()
end

---Copy (DWIM)
M.api.copy_dwim = function ()
    return _navi:copy_dwim()
end

---cut
M.api.cut = function ()
    return _navi:cut()
end

---Cut (DWIM)
M.api.cut_dwim = function ()
    return _navi:cut_dwim()
end

---Trash current item
M.api.trash = function ()
    return _navi:trash()
end

---Trash (DWIM)
M.api.trash_dwim = function ()
    return _navi:trash_dwim()
end

---Mark current item
M.api.mark = function ()
    return _navi:mark()
end

---Mark (DWIM)
M.api.mark_dwim = function ()
    return _navi:mark_dwim()
end

---Mark all files in the current working directory
M.api.mark_all = function ()
    return _navi:mark_all()
end

---Mark current directory inversly
M.api.mark_inverse = function ()
    return _navi:mark_inverse()
end

---Unmark current item
M.api.unmark = function ()
    return _navi:unmark()
end

---Unmark (DWIM)
M.api.unmark_dwim = function ()
    return _navi:unmark_dwim()
end

---Marks files using regex expression
---@param regex string
M.api.mark_regex = function (regex)
    return _navi:mark_regex(regex)
end

---Returns `t` if there are local marks (marks in the current directory)
---@return boolean
M.api.has_marks_local = function ()
    return _navi:has_marks_local()
end

---Returns `t` if there are global marks (marks anywhere)
---@return boolean
M.api.has_marks_global = function ()
    return _navi:has_marks_global()
end

---Get the list of global marks
---@return table
M.api.global_marks = function ()
    return _navi:global_marks()
end

---Get the list of local marks
---@return table
M.api.local_marks = function ()
    return _navi:local_marks()
end

---Get the count of local marks
---@return integer
M.api.local_marks_count = function ()
    return _navi:local_marks_count()
end

---Get the count of global marks
---@return integer
M.api.global_marks_count = function ()
    return _navi:global_marks_count()
end

---Highlights item with the given basename
---@param item_name string
M.api.highlight = function (item_name)
    return _navi:highlight(item_name)
end

---Filters item in the current directory with the provided filters
---@param filter_string string
M.api.filter = function (filter_string)
    return _navi:filter(filter_string)
end

---Resets filter_string
M.api.reset_filter = function ()
    return _navi:reset_filter()
end

---Toggles mark of current item
M.api.toggle_mark = function ()
    return _navi:toggle_mark()
end

--- Toggles mark (DWIM)
M.api.toggle_mark_dwim = function ()
    return _navi:toggle_mark_dwim()
end

---Renames current item
M.api.rename = function ()
    return _navi:rename()
end

---Renames current item
M.api.rename_dwim = function ()
    return _navi:rename_dwim()
end

---Deletes current item
M.api.delete = function ()
    return _navi:rename_dwim()
end

---Deletes current item (DWIM)
M.api.delete_dwim = function ()
    return _navi:delete_dwim()
end

---Execute a shell command asynchronously
---@param command string
---@param args string[]
M.api.execute = function (command, args)
    return _navi:execute(command, args)
end

---MessageType enum
---@class MessageType
---@field info 0
---@field warning 1
---@field error 2
M.io.msgtype = _navi_msgtype()

---Show a message to user
---@param msg string - message to display
---@param type MsgType
M.io.msg = function (msg, type)
    type = type or M.io.msgtype.info
    _navi:message(msg, type)
end

---Get input from user and return the input value
---@param prompt string - prompt text to display
---@param title string - title of the input window
---@param default_text string - placeholder text
M.io.input = function (prompt, title, default_text)
    return _navi:input(prompt, title, default_text)
end

---Get choice out of a list of items from user and return the choice
---@param prompt string - prompt text to display
---@param title string - title of the input window
---@param items string[] - list of items to give as options
---@param default_choice integer - default choice (1 bases index)
M.io.input_choice = function (prompt, title, items, default_choice)
    if default_choice < 0 then
        if #items > 0 then
            default_choice = 1
        end
    end
    return _navi:input_items(prompt, title, items, default_choice)
end

setmetatable(M.ui, {

    __index = function(_, key)

        if key == "inputbar" then
            print(_navi:get_inputbar_props())
            return _navi:get_inputbar_props()
        end

    end,

    __newindex = function(_, key, value)

        if key == "inputbar" then
            if type(value) == "table" then
                if #value ~= 0 then
                    _navi:set_inputbar_props(value)
                else
                    value = {
                        font = "Rajdhani Semibold",
                        font_size = 14
                    }
                    _navi:set_inputbar_props(value)
                end
            end
        end
    end
})

---Adds a function to the list for hook
---@param name string
---@param func function
M.hook.add = function (name, func)
    _navi:add_hook(name, func)
end

---Removes all the functions registered for hook
M.hook.clear_functions = function (name)
    _navi:clear_functions_for_hook(name)
end

---Triggers hook
---@param name string
M.hook.trigger = function (name)
    _navi:trigger_hook(name)
end

setmetatable(M.ui.menubar, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_menubar_visible()
        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_menubar_visible(value)
        end
    end
})

setmetatable(M.ui.preview_panel, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_preview_panel_visible()
        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_preview_panel_visible(value)
        end
    end
})

setmetatable(M.ui.pathbar, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_pathbar_visible()
        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_pathbar_visible(value)
        end
    end
})


setmetatable(M.ui.statusbar, {

    __index = function (_, key)

        if key == "font" then
            return _navi:get_statusbar_font()

        elseif key == "font_size" then
            return _navi:get_statusbar_font_size()

        elseif key == "background" then
            return _navi:get_statusbar_background()

        elseif key == "visible" then
            return _navi:get_statusbar_visible()

        end
    end,
    __newindex = function (_, key, value)

        if key == "font" then
            _navi:set_statusbar_font(value)

        elseif key == "font_size" then
            _navi:set_statusbar_font_size(value)

        elseif key == "background" then
            _navi:set_statusbar_background(value)

        elseif key == "visible" then
            _navi:set_statusbar_visible(value)

        end

    end
})


return M
