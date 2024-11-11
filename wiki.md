**NOTE: This page is still a work in progress.**

# Table of Contents
<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->

- [Table of Contents](#table-of-contents)
- [Installation](#installation)
- [Using Navi](#using-navi)
    - [Naming Conventions](#naming-conventions)
    - [Bookmarks](#bookmarks)
    - [Interactive Commands](#interactive-commands)
        - [Types of Command](#types-of-command)
            - [Current Scope](#current-scope)
            - [Local Scope](#local-scope)
            - [Global Scope](#global-scope)
            - [DWIM](#dwim)
    - [Commands List](#commands-list)
        - [Navigation](#navigation)
        - [Echo (Printing) Commands](#echo-printing-commands)
        - [Marking Item](#marking-item)
        - [Unmarking Item](#unmarking-item)
        - [Selecting Item](#selecting-item)
        - [Change Permission](#change-permission)
        - [Renaming Files](#renaming-files)
        - [Cutting Files](#cutting-files)
        - [Copying Files](#copying-files)
        - [Pasting (Moving) Files](#pasting-moving-files)
        - [Deleting Files](#deleting-files)
        - [Trashing Files](#trashing-files)
        - [Filtering Items](#filtering-items)
        - [Panes](#panes)
        - [Misc](#misc)
        - [Shell Commands](#shell-commands)
        - [Bookmarks](#bookmarks-1)
        - [Searching Items](#searching-items)
        - [Sorting Items](#sorting-items)
        - [Storage Device](#storage-device)
    - [Linux Only](#linux-only)
    - [Theming](#theming)
    - [<span class="todo TODO">TODO</span> Scripting with Lua [scripting-with-lua]](#span-classtodo-todotodospan-scripting-with-lua-scripting-with-lua)
    - [Configuration with Lua](#configuration-with-lua)
- [Acknowledgement](#acknowledgement)

<!-- markdown-toc end -->




# Installation

Navi can be built from source. The following libraries are dependencies
for navi:

- [Qt6](https://www.qt.io/product/qt6)
- [Lua](https://www.lua.org/start.html)
- [ImageMagick(Magick++)](https://imagemagick.org/script/magick++.php)
- [Poppler (Poppler-Qt6)](https://poppler.freedesktop.org/api/qt6/)
- [Udisks](https://www.freedesktop.org/wiki/Software/udisks/)
- [TreeSitter](https://tree-sitter.github.io/tree-sitter/)

If you're on ArchLinux you can copy paste the following `pacman` code to
install these dependencies:

``` bash
sudo pacman -S qt6-base lua imagemagick poppler poppler-qt6 udisks2 tree-sitter tree-sitter-query tree-sitter-c tree-sitter-markdown tree-sitter-python tree-sitter-lua tree-sitter-bash tree-sitter-javascript
```

If you're on Ubuntu/Debian based distribution, you can paste the
following `apt` code to install the dependencies:

``` bash
sudo apt install qt6-base-dev lua5.4 libmagick++-dev libpoppler-dev libpoppler-cpp-dev libpoppler-qt6-dev udisks2 libtree-sitter-dev
```

**NOTE**: I have not tested Navi on an Ubuntu machine. If there are any
problems, please open an issue.

After installing these dependencies on your system and cloning this
project repository, you can build navi by using the following command
(inside the project’s directory):

``` bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

One liner:
```bash
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc) && sudo make install
```

# Using Navi
## Naming Conventions

1.  `Item` refers to a file or a directory.

2.  `Highlighting` refers to the action of clicking a directory item
(one or more than one) once. This means that you can use your mouse
to select and highlight multiple items in the directory.

3.  `Marking` refers to the action of the `mark` or `toggle-mark`
command to "select" one or more currently highlighted directory
items. You can see the list of all the files that you have marked in
the `Marks List` using `View -> Marks List` or by using the
`show-marks` command. When you call any of the commands, the
commands will be aware of these markings and do the action
accordingly. For example, if we use the `rename` command and if
there are marked files, your choice of text editor is opened with
the list of all the file names. You can change these and write to
the file to rename them accordingly.

## Bookmarks

Bookmarks allows you to instantly go to your favourite or frequently
visited directories. You can add, edit, remove and even load bookmarks
directly from a text file. Bookmarks are read and stored in plain text
files that has the following structure:

```python
​# This is a comment – Any lines that begin with # are ignored and empty lines are ignored as well.

bookmarktitle1 bookmarkdirectorypath1
bookmarktitle2 bookmarkdirectorypath2
```

## Interactive Commands

These are commands that can be executed using the `inputbar` UI component. By default, pressing `: (colon)` key pops up the inputbar. You can type a name of the command in this bar or a line number to go to in the current directory of the focused pane. Example: `mark` or `42`, where `mark` would mark the current item under focus or highlighted items (more than one items).

### Types of Command

There are four variants for most of the commands: `Current`, `Local`, `Global` and `DWIM` (Do What I Mean).

#### Current Scope

This applies to the currently selected item. Any command that is `Current` scoped will run only on the currently highlighted item.

**This includes mouse selection (also called as Highlighting)**.

#### Local Scope

This applies to the marks that are in the current directory which is conveniently called as a **Local Mark**. Any command that is `Local` scoped will run on the local marks.

**This does not include mouse highlighting.**

#### Global Scope

This applies to the marks that are present in any directory. Unlike the `Local` scope the command that is `Global` scoped will run on the marks that are present in any directory.

**This does not include mouse highlighting**.

#### DWIM

[DWIM wikipedia page](https://en.wikipedia.org/wiki/DWIM)

Do What I Mean version of commands. If no local marks are present, run the command on the current item, otherwise run the commands on the marks.

## Commands List

Below is an exhaustive list of commands that are currently available in Navi.

### Navigation

`next-item`

Go to the next item

`prev-item`

Go to the previous item

`select-item`

Select the current item

`middle-item`

Go to the middle item

`up-directory`

Go to the parent directory

`first-item`

Go to the first item

`last-item`

Go to the last item

### Echo (Printing) Commands

`echo-info`

Echos or prints the information given as argument with the *info-face* font

`echo-warn`

Echos or prints the information given as argument with the *warning-face* font

`echo-error`

Echos or prints the information given as argument with the *error-face* font

### Marking Item

`mark`

Marks the current item.

`toggle-mark`

Toggle the mark of the current item.

`toggle-mark-dwim`

DWIM version of `toggle-mark` command

`mark-all`

Marks all the items in the current directory.

`mark-inverse`

Inverses the marks in the current directory.

`mark-dwim`

DWIM version of `toggle-mark`

### Unmarking Item

`unmark`

Unmarks the current item.

`unmark-local`

Unmarks **Local Marks**

`unmark-global`

Unmarks **Global Marks**

### Selecting Item

`visual-select`

Enters into *visual selection* mode. Navigation is followed by selection until the mode is toggled off.

### Change Permission

`chmod`

Change the permission of the current item using three digit numbers

Example: 777, 666, 000 etc.

`chmod-local`

Change permission for **Local Marks**

`chmod-global`

Change permission for **Global Marks**

`chmod-dwim`

Change permission **DWIM** style

### Renaming Files

**NOTE: Renaming more than one item leads to *bulk renaming*. This is where a text editor of your choice (set it in the configuration) opens up a temporary "rename file" and enables you to rename all the marked files once you save and close the said "rename file"**

`rename`

Rename the highlighted item.

`rename-local`

Renames items in the **Local Marks**

`rename-global`

Renames items in the **Global Marks**

`rename-dwim`

Renames items in **DWIM** style

### Cutting Files

`cut`

Cut (prepare for moving) the current item.

`cut-local`

Register cut for **Local Marks**

`cut-global`

Register cut for **Global Marks**

`cut-dwim`

Register cut in **DWIM** style

### Copying Files

`copy`

Copy the current item.

`copy-local`

Register copy for **Local Marks**

`copy-global`

Register copy for **Global Marks**

`copy-dwim`

Register copy in **DWIM** style

### Pasting (Moving) Files

`paste`

Paste (Move) the current item.

### Deleting Files

**WARNING: Please be careful when using delete commands, this does not trash the items, it directly deletes them. If you want to trash use the *trash* command**

`delete`

Delete the highlighted items(s).


`delete-local`

Deletes items in the **Local Marks**

`delete-global`

Deletes items in the **Global Marks**

`delete-dwim`

Deletes items in **DWIM** style

### Trashing Files

`trash`

Trash the current item.

`trash-local`

Trashes items in **Local Marks**

`trash-global`

Trashes items in **Global Marks**

`trash-dwim`

Trashes items in **DWIM** style

### Filtering Items

`filter`

Set a filter to directory.

Example: `*` displays everything, `*.csv` displays only the csv files, `*.png` displays only the png files

`reset-filter`

Reset the appplied filter.

`hidden-files`

Toggles the hidden files.

**NOTE**: Hidden files are those items whose name start with a period like `.config`, `.gitignore` etc.

`dot-dot`

Toggles the .. file item.

### Panes

`bookmark-pane`

Opens the bookmarks list.

TODO: work in progress

`marks-pane`

Opens the marks list.

<img src="./screenshot/marks-pane-demo.gif" height="400px" width="600px" />

`messages-pane`

Opens the messages list.

<img src="./screenshot/messages_pane.png" height="400px" width="500px" />

`preview-pane`

Toggles the preview pane.

The preview pane handles previewing images (good number of formats) and PDF documents (first page) `asynchronously`. This means that the previewing experience will be seamless and without any lag. Navi uses `ImageMagick` library under the hood for previewing images and therefore any image formats supported my ImageMagick is supported by Navi.

`shortcuts-pane`

This displays the list of all the shortcuts.

<img src="./screenshot/shortcuts_pane.png" height="400px" width="600px" />


### Misc

`execute-extended-command`

This is the function that pops up the inputbar to enter the interactive commands.

`menu-bar`

Toggles the menu bar.

`focus-path`

Focuses the path widget and sets it in edit mode.

`item-property`

Display the property of the currently focused item.

`header`

Toggle the display of header information.

`cycle`

Toggle the cycle (last item to first item and vice-versa) during navigation.

`refresh`

Force refresh the current directory.

`syntax-highlight`

Toggle the syntax highlight for text preview

**NOTE: By default, Navi watches the directory for changes and loads them, so there is no requirement to refresh the directory. This command is there just in case something does not look right.**

`mouse-scroll`

Toggle mouse scroll support for file panel

`reload-config`

Re-reads the configuration file if it exists and loads the configurations.

`get-input`

Get input from the user and store the result.

Args:

* Prompt string `required`
* Default placeholder text `optional`
* Selection text (piece of text that has to be selected by default) `optional`

**NOTE**: This command does not return any input as of now.

**TL; DR**: This feature does not work

`tasks`

Opens the task widget showing all the running tasks like command execution with outputs and file operations like copying, moving and deleting/trashing.

![](./videos/tasks-demo.mp4)

### Shell Commands

`shell`

Run a shell command *synchronously* (blocking).

**NOTE: This blocks the main GUI thread until the command finished executing**

**NOTE: NOT YET IMPLEMENTED**

`shell-async`

Run a shell command *asynchronously* (non-blocking).

The running commands can be seen in the `Task Widget` using the `tasks` command

## Tutorial

Using navi is easy. You have bunch of commands that do certain things and sometimes they depend on the context (see DWIM commands). The default settings are set up for you to use navi without any configurations needed. The default keybindings are listed below:

* `h` to go to the parent directory
* `j` to go the next item
* `k` to go the previous item
* `l` to select the current item
* `gg` to go to the first item
* `Shift+g` to go the last item
* `Space` to toggle marking for the current item
* `Shift+Space` to toggle inverse marking for the current directory
* `Ctrl+a` to mark all the items in the current directory
* `Shift+r` to rename current item if no marks are present or all the marked items if marks are present (this is what DWIM means)
* `Shift+d` to delete DWIM
* `yy` to copy DWIM
* `p` to paste cut/copy marked items to the current directory
* `Shift+u` to unmark all the local marks
* `.` toggle hidden/dot files
* `/` Search for item in the current directory
* `n` Go to the next match for the search
* `Shift+n` Go to the previous match for the search
* `Ctrl+m` toggle menubar
* `Ctrl+p` toggle preview panel
* `Ctrl+l` focus the path widget
* `Shift+v` Toggle visual line selection mode

**NOTE: These keybindings are not loaded if there is a `keybindings` table in the lua configuration file**

# Linux Only

Sorry, this software is built keeping in mind Linux and it's derivatives only.

# Theming

Since navi is a Qt GUI library based application, it can be styled using the `qt6ct` package available on linux. The colors mentioned in the configuration file will override the respective control colors of the theme.

# Scripting with Lua [scripting-with-lua]

Navi can be scripted to include custom actions by subscribing to hooks and perform custom actions in response to those hooks.

# Configuration with Lua

Navi can be configured using lua. The default configuration file is provided in this git repository. Navi looks for the configuration file in the XDG standard config directory (`~/.config`). You can place your configuration file in the `~/.config/navi/` directory. The configuration file should be named `config.lua`.

Configuration includes all the settings that can be changed for Navi and keybindings can also be changed.

**TL;DR: Navi configuration file (config.lua) should be placed at this location ( `~/.config/navi/config.lua`)**

Copy and paste the following template config file into the configuration directory.

``` lua
settings = {
    ui = {

        tabs = {
            show_on_multiple = true, -- don’t show tab bar if there is only one tab
        },

        preview_pane = {
            shown = false,
            max_file_size = "10M", -- max file size to preview
            fraction = 0.2,
            syntax_highlight = true,
        },

        menu_bar = {
            shown = false
        },

        status_bar = {
            shown = true
        },

        input_bar = {
            background = "#FF5000",
            foreground = "#FFFFFF",
            font = "JetBrainsMono Nerd Font Mono",
        },

        path_bar = {
            shown = true,
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
                { name = "NAME", type = "file_name" },
                { name = "PERM", type = "file_permission"},
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

keybindings = {
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
}
```

# Acknowledgement

Navi uses the following header-only C++ libraries. Thanks to the authors of the following libraries:

- [ArgParse](https://github.com/p-ranav/argparse) (for parsing command
line arguments)
- [Sol2](https://github.com/ThePhD/sol2) (for lua integration)
