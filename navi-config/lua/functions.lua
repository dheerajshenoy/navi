local M = {}

-- Sets the wallpaper using the file at point
function M.setWallpaper()
    local file = navi.api.item_name()
    navi.shell.execute("xwallpaper --stretch " .. file)
end

-- Opens a terminal at the given directory
function M.terminalHere()
    local dir = navi.api.dir_name()
    navi.api.spawn(navi.opt.terminal, { dir })
end

function M.command(commandString)
    local command = commandString
    local handle = io.popen(command)
    local result = nil
    if handle ~= nil then
        result = handle:read("*a")
        handle:close()
    end
    return result:gsub('[\n\r]', '')
end

-- Use `fd` to go to the best file match having the name given as the input
function M.fd()
    local input = navi.io.input("Search (FD)")
    if input ~= "" then
        local pwd = navi.api.pwd()
        local commandString = string.format("fd %s %s --type=directory | head -n 1", input, pwd)
        local result = M.command(commandString)
        if result ~= "" then
            navi.api.cd(result)
        else
            navi.io.msg("No results", navi.io.msgtype.warn)
        end
    end
end

-- Use `ripgrep` to go to the file content with the name given as the input
function M.rg()
    local input = navi.io.input("Search (RG)")
    if input ~= "" then
        local pwd = navi.api.pwd()
        local commandString = string.format("rg %s %s -l --sort-files | head -n 1", input, pwd)
        local result = M.command(commandString)
        if result ~= "" then
            navi.api.cd(result)
            navi.api.highlight(result)
        else
            navi.io.msg("No results", navi.io.msgtype.warn)
        end
    end
end

---Use `zoxide` to go to the directory using the zoxide database
function M.zoxide(...)
    local args = table.pack(...)

    if #args == 0 or args == nil then
        args = navi.io.input("Zoxide CD")
        args = navi.utils.split(args, " ")
    end


    local commandString = string.format("zoxide query %s", table.concat(args, " "))
    local result = M.command(commandString)

    -- Handle the result
    if result ~= "" then
        navi.api.cd(result)
        navi.api.highlight(result)
    else
        navi.io.msg("No results", navi.io.msgtype.warn)
    end
    --]]
end

function M.hello()
    navi.io.msg("HELLO WORLD")
end

function M.print_table(table)
    if type(table) == "table" then
        for k, v in pairs(table) do
            print(k, v)
        end
    end
end


return M
