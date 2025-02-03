local M = {}

M._harpoon = {}


M.harpoon_set = function(key)
    local path = navi.api.pwd()
    M._harpoon[key] = path
    navi.io.msg(string.format("Harpoon set to key %s", key), navi.io.msgtype.info)
end

M.harpoon = function(key)
    local dir = M._harpoon[key]
    if dir ~= nil then
        navi.api.cd(dir)
    end
end

M.setup = function ()
    navi.api.create_user_command("harpoon", function (key)
        return M.harpoon(key)
    end)

    navi.api.create_user_command("harpoon-set", function (key)
        return M.harpoon_set(key)
    end)


    navi.keymap.set("Shift+H,a", "harpoon-set a", "Harpoon A")
    navi.keymap.set("Shift+H,b", "harpoon-set b", "Harpoon B")

    navi.keymap.set("Ctrl+H,a", "harpoon a", "Harpoon A")
    navi.keymap.set("Ctrl+H,b", "harpoon b", "Harpoon B")

    -- navi.keymap.set("Shift+h a", "harpoon a", "Harpoon A")
    -- navi.keymap.set("Shift+h b", "harpoon b", "Harpoon B")

end

return M
