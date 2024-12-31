---@class navi

local module = navi.ui.statusbar.create_module("test", {
    text = "DD",
    italic = true,
    bold = true,
    background = "#FFFFFF"
})

navi.ui.statusbar.add_module(module)

function hello ()
    navi.ui.statusbar.set_module_text("test", "FF")
end

navi.api.register_function("hello", hello)
