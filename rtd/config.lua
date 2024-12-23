SETTINGS = {
    terminal = os.getenv("TERMINAL"),
    copy_path_separator = " " ,
    default_directory = "~",
    bookmarks = {
        auto_save = true,
    },

    bulk_rename = {
        file_threshold = 5, -- threshold at which bulk rename should trigger
        editor = "nvim", -- editor used for bulk renaming along with flag %f for file
        terminal = true, -- if the editor uses terminal set this flag to true
    },

    ui = {

        toolbar = {
            shown = true,
            icons_only = true,
        },

        preview_pane = {
            shown = false,
            max_file_size = "10M", -- max file size to preview
            fraction = 0.2,
            syntax_highlight = true,
            dimension = { width = 400, height = 400 },
        },

        menubar = {
            shown = true,
            icons = true,
        },

        statusbar = {
            shown = true,
            visual_line_mode = {
                text = "V",
                background = "#FF5000",
                foreground = "#000000",
                italic = true,
                bold = true,
                padding = "4px",
            },
            macro_mode = {
                text = "M",
                background = "#FF3124",
                foreground = "#000000",
                italic = true,
                bold = true,
                padding = "4px",
            }
        },

        inputbar = {
            background = "#FF5000",
            foreground = "#FFFFFF",
            font = "JetBrainsMono Nerd Font Mono",
        },

        pathbar = {
            shown = true,
            background = nil,
            foreground = nil,
            italic = false,
            bold = false,
            font = nil,
        },

        tasks_pane = {
            output_scrollback_lines = 100, -- TODO: Not yet added
        },

        file_pane = {
            symlink = {
                shown = true,
                foreground = "#FF5000",
                separator = "->",
            },
            highlight = {
                foreground = "#000",
                background = "#f05af4",
            },
            columns = {
                { name = "Name", type = "file_name" },
                -- { name = "PERM", type = "file_permission"},
                { name = "Size", type = "file_size" },
                -- modified_date = "Date",
                -- size = "SIZE"
            },
            headers = false,
            cycle = false,
            mark = {
                foreground = "#FF5000",
                background = nil,
                italic = true,
                bold = nil,
                font = "JetBrainsMono Nerd Font Mono",
                header = {
                    foreground = "#FF5000",
                    background = nil,
                    italic = nil,
                    bold = true,
                }
            }
        }
    }
}

KEYBINDINGS = {
    { key = "h", command = "up-directory", desc = "Go to the parent directory" },
    { key = "j", command = "next-item", desc = "Go to the next item" },
    { key = "k", command = "prev-item", desc = "Go to the previous item" },
    { key = "l", command = "select-item", desc = "Select item" },
    { key = "f", command = "filter", desc = "Filter item visibility" },
    { key = "g,g", command = "first-item", desc = "Go to the first item" },
    { key = "z,z", command = "middle-item", desc = "Go to middle item" },
    { key = "Shift+g", command = "last-item", desc = "Go to the last item" },
    { key = "Shift+r", command = "rename-dwim", desc = "Rename item(s)" },
    { key = "Shift+d", command = "delete-dwim", desc = "Delete item(s)" },
    { key = "Space", command = "toggle-mark-dwim", desc = "Mark item(s)" },
    { key = "F5", command = "refresh", desc = "Refresh current directory" },
    { key = "Shift+Space", command = "mark-inverse", desc = "Mark inverse item(s)" },
    { key = "Shift+v", command = "visual-select", desc = "Visual selection mode" },
    { key = ":", command = "execute-extended-command", desc = "Execute extended command" },
    { key = "y,y", command = "copy-dwim", desc = "Copy item(s)" },
    { key = "p", command = "paste", desc = "Paste item(s)" },
    { key = "Shift+u", command = "unmark-local", desc = "Unmark all item(s)" },
    { key = "/", command = "search", desc = "Search (regex)" },
    { key = "n", command = "search-next", desc = "Search next" },
    { key = "Shift+n", command = "search-prev", desc = "Search previous" },
    { key = "Ctrl+m", command = "menu-bar", desc = "Toggle menu bar" },
    { key = "Ctrl+p", command = "preview-pane", desc = "Toggle preview pane" },
    { key = "Ctrl+l", command = "focus-path", desc = "Focus path bar" },
    { key = "Shift+t", command = "trash-dwim", desc = "Trash item(s)" },
    { key = ".", command = "hidden-files", desc = "Toggle hidden items" },
    { key = "q", command = "macro-record", desc = "Record or Finish recording macro" },
    { key = "Shift+c", command = "copy-path", desc = "Copy path(s)" },
}

local terminal = os.getenv("TERMINAL")

function customModule()
    local custom = navi.ui.statusbar.create_module("custom", {
        text = "HELLO",
        italic = true,
        bold = true,
        background = "#FF5000",
        hidden = true,
        color = "#000000",
    })

    navi.ui.statusbar.set_modules({
        "name",
        "macro",
        "visual_mode",
        "stretch",
        -- "filter",
        "count",
        "custom",
        "size",
        "modified_date",
        "permission",
    });


    -- navi.hook.add("visual_line_mode_on", function ()
    --                                          navi.ui.statusbar.set_module_text("custom", "DD")
    --                                      end)

end

function INIT_NAVI()
    customModule()
end

-- Sets the wallpaper using the file at point
function setWallpaper()
    local file = navi.api.item_name()
    navi.shell.execute("xwallpaper --stretch " .. file)
end

-- Opens a terminal at the given directory
function terminalHere()
    local dir = navi.api.dir_name()
    navi.api.spawn(terminal, { dir })
end

function command(commandString)
    local command = commandString
    local handle = io.popen(command)
    local result = handle:read("*a")
    handle:close()
    return result:gsub('[\n\r]', '')
end

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
            navi.io.msg("No results", navi.io.msgtype.warn)
        end
    end
end

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
            navi.io.msg("No results", navi.io.msgtype.warn)
        end
    end
end

-- Use `zoxide` to go to the directory using the zoxide database
function zoxide()
    local input = navi.io.input("Zoxide CD")
    if input ~= "" then
        local commandString = string.format("zoxide %s", input)
        local result = command(commandString)
        if result ~= "" then
            navi.api.cd(result)
            navi.api.highlight(result)
        else
            navi.io.msg("No results", navi.io.msgtype.warn)
        end
    end
end
