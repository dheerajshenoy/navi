**NOTE: This page is still a work in progress.**

# Naming Conventions

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

# Bookmarks

Bookmarks allows you to instantly go to your favourite or frequently
visited directories. You can add, edit, remove and even load bookmarks
directly from a text file. Bookmarks are read and stored in plain text
files that has the following structure:

​# This is a comment – Any lines that begin with \# are ignored and empty
lines are ignored as well.

bookmarktitle1 bookmarkdirectorypath1 bookmarktitle2
bookmarkdirectorypath2

# Interactive Commands

These are commands that can be executed using the `inputbar` UI
component. By default, pressing `: (colon)` key pops up the inputbar.
You can type a name of the command in this bar or a line number to go to
in the current directory of the focused pane. Example: `mark` or `42`,
where `mark` would mark the current item under focus or highlighted
items (more than one items).

## Types of Command

There are four variants for most of the commands: `Current`, `Local` and
`Global` and `DWIM` (Do What I Mean).

### Current Scope

This applies to the currently selected item. Any command that is
`Current` scoped will run only on the currently highlighted item.

**This includes mouse selection (also called as Highlighting)**.

### Local Scope

This applies to the marks that are in the current directory which is
conveniently called as a **Local Mark**. Any command that is `Local`
scoped will run on the local marks.

**This does not include mouse highlighting.**

### Global Scope

This applies to the marks that are present in any directory. Unlike the
`Local` scope the command that is `Global` scoped will run on the marks
that are present in any directory.

**This does not include mouse highlighting**.

### DWIM

[DWIM wikipedia page](https://en.wikipedia.org/wiki/DWIM)

Do What I Mean version of commands. If no local marks are present, run
the command on the current item, otherwise run the commands on the
marks.

# Commands List

Below is an exhaustive list of commands that are currently available in
Navi.

## Marking Item

### mark

Marks the current item.

### toggle-mark

Toggle the mark of the current item.

### mark-all

Marks all the items in the current directory.

### mark-inverse

Inverses the marks in the current directory.

### mark-dwim

DWIM version of `toggle-mark`

## Unmarking Item

### unmark

Unmarks the current item.

### unmark-local

`unmark` for **Local Marks**

### unmark-global

`unmark` for **Global Marks**

## Selecting Item

### visual-select

Enters into *visual selection* mode. Navigation is followed by selection
until the mode is toggled off.

## Change Permission

### chmod

Change the permission of the current item using three digit numbers

Example: 777, 666, 000 etc.

### chmod-local

Similar to `chmod` for **Local Marks**

### chmod-global

Similar to `chmod` for **Global Marks**

### chmod-dwim

## Renaming Files

\*NOTE: Renaming more than one item leads to *bulk renaming*. This is
where a text editor of your choice (set it in the configuration) opens
up a temporary "rename file" and enables you to rename all the marked
files once you save and close the said "rename file"\*

### rename

Rename the highlighted item.

### rename-local

`rename` for **Local Marks**

### rename-global

`rename` for **Global Marks**

### rename-dwim

## Cutting Files

### cut

Cut (prepare for moving) the current item.

### cut-local

`cut` for **Local Marks**

### cut-global

`cut` for **Global Marks**

### cut-dwim

## Copying Files

### copy

Copy the current item.

### copy-local

`copy` for **Local Marks**

### copy-global

`copy` for **Global Marks**

### copy-dwim

## Pasting (Moving) Files

### paste

Paste (Move) the current item.

## Deleting Files

### delete

Delete the highlighted items(s).

\*WARNING: Please be careful when using this command, this does not
trash the items, it directly deletes them. If you want to trash use the
*trash* command\*

### delete-local

`delete` for **Local Marks**

### delete-global

`delete` for **Global Marks**

### delete-dwim

## Trashing Files

### trash

Trash the current item.

### trash-local

`trash` for **Local Marks**

### trash-global

`trash` for **Global Marks**

### trash-dwim

## Filtering Items

### filter

Set a filter to directory.

Example: `*` displays everything, `*.csv` displays only the csv files,
`*.png` displays only the png files

### reset-filter

Reset the appplied filter.

### hidden-files

Toggles the hidden files.

NOTE: Hidden files are those items whose name start with a period like
`.config`, `.gitignore` etc.

## Panes

### bookmark-pane

Opens the bookmarks list.

### marks-pane

Opens the marks list.

### messages-pane

Opens the messages list.

### preview-pane

Toggles the preview pane.

The preview pane handles previewing images (good number of formats) and
PDF documents (first page) `asynchronously`. This means that the
previewing experience will be seamless and without any lag. Navi uses
`ImageMagick` library under the hood for previewing images and therefore
any image formats supported my ImageMagick is supported by Navi.

### shortcuts-pane

This displays the list of all the shortcuts that have been assigned.

## Misc

### execute-extended-command

This is the function that pops up the inputbar to enter the interactive
commands.

### menu-bar

Toggles the menu bar.

### focus-path

Focuses the path widget and sets it in edit mode.

### item-property

Display the property of the currently focused item.

### toggle-header

Toggle the display of header information.

### toggle-cycle

Toggle the cycle (last item to first item and vice-versa) during
navigation.

### refresh

Force refresh the current directory.

\*NOTE: By default, Navi watches the directory for changes and loads
them, so there is no requirement to refresh the directory. This command
is there just in case something does not look right.\*

## Shell Commands

### <span class="todo TODO">TODO</span> shell-command [shell-command]

Run a shell command *synchronously* (blocking).

**NOTE: This blocks the main GUI thread until the command finished
executing**

### <span class="todo TODO">TODO</span> shell-command-async [shell-command-async]

Run a shell command *asynchronously* (non-blocking).

The running commands can be seen in the `Task Widget`

## Bookmarks

### bookmark-add

Add a new non-existing bookmark

### bookmark-remove

Remove an existing bookmark

### bookmark-edit

Edit an existing bookmark

-   Args:

    `title` - this tells Navi to edit the bookmark title `path` - this
    tells Navi to edit the bookmark directory that the bookmark points
    to

### bookmark-go

Go to the directory pointed by the bookmark

-   Args:

    `bookmark-name` - a unique bookmark name

### bookmark-save

Save the bookmarks that have been added.

\*NOTE: Saving of the bookmarks added is done when the application
exits. If you feel insecure about losing your bookmarks, then you can
run this command manually.\*

## Searching Items

Searching is **Regular Expression** enabled. So you can pinpoint exactly
the file that you want to look for.

### search

Searches the current directory for the search term

### search-next

Goes to the next best match for the search term

### search-prev

Goes to the previous best match for the search term

## Sorting Items

### sort-name

Sorts the directory by *name* in **ascending order** with the
directories listed first.

### sort-name-desc

Sorts the directory by *name* in **descending order** with the
directories listed first.

### sort-date

Sorts the directory by *date* in **ascending order** with the
directories listed first.

### sort-date-desc

Sorts the directory by *date* in **descending order** with the
directories listed first.

### sort-size

Sorts the directory by *size* in **ascending order** with the
directories listed first.

### sort-size-desc

Sorts the directory by *size* in **descending order** with the
directories listed first.

# Linux Only

Sorry, this software is built keeping in mind Linux and it's derivatives
only.

# Configuration with Lua

Navi can be configured using lua. The default configuration file is
provided in this git repository. Navi looks for the configuration file
in the XDG standard config directory (`~/.config`). You can place your
configuration file in the `~/.config/navi/` directory. The configuration
file should be named `config.lua`.

Configuration includes all the settings that can be changed for Navi and
keybindings can also be changed.

**TL;DR: Navi configuration file (config.lua) should be placed at this
location ( `~/.config/navi/config.lua`)**

Copy and paste the following template config file into the configuration
directory.

``` lua
settings = {
    ui = {
        preview_pane = {
            shown = false,
            max_file_size = "2.4M", -- max file size to preview
            fraction = 0.2,
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

            columns = {
                name = "NAME",
                -- permission = "PERM",
                -- modified_date = "Date",
                -- size = "SIZE"
            },
            headers = false,
            cycle = true,
            mark = {
                foreground = "#FF5000",
                background = nil,
                italic = true,
                bold = nil,
                font = "JetBrainsMono Nerd Font Mono",
                header = {
                    foreground = "#FF5000",
                    background = "#FFFFFF",
                    italic = nil,
                    bold = true,
                }
            },
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
    { key = "Space", command = "toggle-mark", desc = "Mark item(s)" },
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
    { key = "Ctrl+m", command = "toggle-menu-bar", desc = "Toggle menu bar" },
    { key = "Ctrl+p", command = "preview-pane", desc = "Toggle preview pane" },
    { key = "Ctrl+l", command = "focus-path", desc = "Focus path bar" },
    { key = "Shift+t", command = "trash-dwim", desc = "Trash item(s)" },
    { key = ".", command = "toggle-hidden-files", desc = "Toggle hidden items" },
}
```

# <span class="todo TODO">TODO</span> Scripting with Lua [scripting-with-lua]

Navi can be scripted to include custom actions by subscribing to hooks
and perform custom actions in response to those hooks.
