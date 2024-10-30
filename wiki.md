
# Table of Contents

1.  [Naming Conventions](#orgc814c69)
2.  [Bookmarks](#org9f395af)
3.  [Interactive Commands](#org1a042e0)
    1.  [Types of Command](#orgaec2477)
        1.  [Current Scope](#org8629e92)
        2.  [Local Scope](#org39149ee)
        3.  [Global Scope](#orgb78d294)
4.  [Commands List](#org5c21c25)
    1.  [Marking Item](#org974d332)
        1.  [mark](#org1b44641)
        2.  [toggle-mark](#org8d2a97c)
        3.  [mark-all](#orge4fc60f)
        4.  [mark-inverse](#org6f2e51b)
    2.  [Unmarking Item](#orge27ac5a)
        1.  [unmark](#org446444f)
        2.  [unmark-local](#orgdea8a1c)
        3.  [unmark-global](#org7536505)
    3.  [Change Permission](#org82fb1f7)
        1.  [chmod](#org20d2856)
        2.  [chmod-local](#orge5527b1)
        3.  [chmod-global](#org9417620)
    4.  [Renaming Files](#orgd4962e5)
        1.  [rename](#orgbaba631)
        2.  [rename-local](#orge4ff5c8)
        3.  [rename-global](#orgce5e65b)
    5.  [Cutting Files](#orgf4776af)
        1.  [cut](#orgdf9e51e)
        2.  [cut-local](#orgedbc985)
        3.  [cut-global](#org6f036e9)
    6.  [Copying Files](#orgaac9440)
        1.  [copy](#org47aae10)
        2.  [copy-local](#org3372106)
        3.  [copy-global](#orgce33b39)
    7.  [Pasting (Moving) Files](#org09ab66c)
        1.  [paste](#orgc42876b)
    8.  [Deleting Files](#org4220986)
        1.  [delete](#org0948704)
        2.  [delete-local](#orgc34b86a)
        3.  [delete-global](#org9cee89c)
    9.  [Trashing Files](#org5a9b7e9)
        1.  [trash](#orga1ef814)
        2.  [trash-local](#org74a0d55)
        3.  [trash-global](#orgd9a056e)
    10. [filter](#orgf0a7300)
    11. [unfilter](#org438a5fe)
    12. [refresh](#org0105a15)
    13. [hidden-files](#org0ab786f)
    14. [Panes](#orgc1365cc)
        1.  [bookmark-pane](#orgc7cd395)
        2.  [marks-pane](#orgd6948e9)
        3.  [messages-pane](#orgb076372)
        4.  [preview-pane](#org4a14d08)
    15. [Misc](#org850d18f)
        1.  [menu-bar](#orgd748518)
        2.  [focus-path](#org10ac22d)
        3.  [item-property](#org50acaaf)
    16. [Shell Commands](#org71dea1f)
        1.  [shell-command](#org6f2bb10)
        2.  [shell-command-async](#orgd43288b)
    17. [Bookmarks](#orga64ff53)
        1.  [bookmark-add](#org3a52244)
        2.  [bookmark-remove](#orgab4ddf4)
        3.  [bookmark-edit](#orgbdc3f90)
        4.  [bookmark-go](#orgde4c484)
        5.  [bookmark-save](#org3cd70aa)
    18. [Searching Items](#orgb47402c)
        1.  [search](#orgc6ca662)
        2.  [search-next](#orga493148)
        3.  [search-prev](#orgfb2a7ad)
5.  [Linux Only](#org7887096)
6.  [Configuration with Lua](#orga36c27f)
7.  [Scripting with Lua](#orgb9fae21)

**NOTE: This page is still a work in progress.**

<a id="orgc814c69"></a>

# Naming Conventions

1.  `Item` refers to a file or a directory.

2.  `Highlighting` refers to the action of clicking a directory item (one or more than one) once. This means that you can use your mouse to select and highlight multiple items in the directory.

3.  `Marking` refers to the action of the `mark` or `toggle-mark` command to "select" one or more currently highlighted directory items. You can see the list of all the files that you have marked in the `Marks List` using `View -> Marks List` or by using the `show-marks` command. When you call any of the commands, the commands will be aware of these markings and do the action accordingly. For example, if we use the `rename` command and if there are marked files, your choice of text editor is opened with the list of all the file names. You can change these and write to the file to rename them accordingly.

<a id="org9f395af"></a>

# Bookmarks

Bookmarks allows you to instantly go to your favourite or frequently visited directories. You can add, edit, remove and even load bookmarks directly from a text file. Bookmarks are read and stored in plain text files that has the following structure:

\\# This is a comment &#x2013; Any lines that begin with # are ignored and empty lines are ignored as well.

bookmark<sub>title1</sub> bookmark<sub>directory</sub><sub>path1</sub>
bookmark<sub>title2</sub> bookmark<sub>directory</sub><sub>path2</sub>


<a id="org1a042e0"></a>

# Interactive Commands

These are commands that can be executed using the `inputbar` UI component. By default, pressing `: (colon)` key pops up the inputbar. You can type a name of the command in this bar or a line number to go to in the current directory of the focused pane. Example: `mark` or `42`, where `mark` would mark the current item under focus or highlighted items (more than one items).


<a id="orgaec2477"></a>

## Types of Command

There are three variants for most of the commands: `Current`, `Local` and `Global`.
If any interactive command has scope available, they are named accordingly. For example, the `rename` command is actually the `Current` scoped command. It’s other variants are `rename-local` and `rename-global` for `Local` and `Global` scope respectively.


<a id="org8629e92"></a>

### Current Scope

This applies to the currently selected item. Any command that is `Current` scoped will run only on the currently highlighted item.

**This includes mouse selection (also called as Highlighting)**.


<a id="org39149ee"></a>

### Local Scope

This applies to the marks that are in the current directory which is conveniently called as a **Local Mark**. Any command that is `Local` scoped will run on the local marks.

**This does not include mouse highlighting.**


<a id="orgb78d294"></a>

### Global Scope

This applies to the marks that are present in any directory. Unlike the `Local` scope the command that is `Global` scoped will run on the marks that are present in any directory.

**This does not include mouse highlighting**.


<a id="org5c21c25"></a>

# Commands List

Below is an exhaustive list of commands that are currently available in Navi. The default keybindings associated with them are mentioned. Below is a mapping of what some keys mean.

1.  `C` - Control
2.  `M` - Meta (Alt Key)
3.  `S` - Shift
4.  `SPC` - Space


<a id="org974d332"></a>

## Marking Item


<a id="org1b44641"></a>

### mark

Marks the current item.


<a id="org8d2a97c"></a>

### toggle-mark

Toggle the mark of the current item.

Default keybinding: `SPC`


<a id="orge4fc60f"></a>

### mark-all

Marks all the items in the current directory.

Default keybinding: `C-a`


<a id="org6f2e51b"></a>

### mark-inverse

Inverses the marks in the current directory.

Default keybinding: `S-SPC`


<a id="orge27ac5a"></a>

## Unmarking Item


<a id="org446444f"></a>

### unmark

Unmarks the current item.


<a id="orgdea8a1c"></a>

### unmark-local

`unmark` for **Local Marks**

Default keybinding: `S-u`


<a id="org7536505"></a>

### unmark-global

`unmark` for **Global Marks**

Default keybinding: `C-S-u`


<a id="org82fb1f7"></a>

## Change Permission


<a id="org20d2856"></a>

### chmod

Change the permission of the current item using three digit numbers

Example: 777, 666, 000 etc.


<a id="orge5527b1"></a>

### chmod-local

Similar to `chmod` for **Local Marks**


<a id="org9417620"></a>

### chmod-global

Similar to `chmod` for **Global Marks**


<a id="orgd4962e5"></a>

## Renaming Files

**NOTE: Renaming more than one item leads to *bulk renaming*. This is where a text editor of your choice (set it in the configuration) opens up a temporary "rename file" and enables you to rename all the marked files once you save and close the said "rename file"**


<a id="orgbaba631"></a>

### rename

Rename the highlighted item.

Default keybinding: `S-r`


<a id="orge4ff5c8"></a>

### rename-local

`rename` for **Local Marks**


<a id="orgce5e65b"></a>

### rename-global

`rename` for **Global Marks**


<a id="orgf4776af"></a>

## Cutting Files


<a id="orgdf9e51e"></a>

### cut

Cut (prepare for moving) the current item.

Default keybinding: `C-x`


<a id="orgedbc985"></a>

### cut-local

`cut` for **Local Marks**


<a id="org6f036e9"></a>

### cut-global

`cut` for **Global Marks**


<a id="orgaac9440"></a>

## Copying Files


<a id="org47aae10"></a>

### copy

Copy the current item.

Default keybinding: `C-c`


<a id="org3372106"></a>

### copy-local

`copy` for **Local Marks**


<a id="orgce33b39"></a>

### copy-global

`copy` for **Global Marks**


<a id="org09ab66c"></a>

## Pasting (Moving) Files


<a id="orgc42876b"></a>

### paste

Paste (Move) the current item.

Default keybinding: `p`


<a id="org4220986"></a>

## Deleting Files


<a id="org0948704"></a>

### delete

Delete the highlighted items(s).

**WARNING: Please be careful when using this command, this does not trash the items, it directly deletes them. If you want to trash use the *trash* command**

Default keybinding: `S-d`


<a id="orgc34b86a"></a>

### delete-local

`delete` for **Local Marks**


<a id="org9cee89c"></a>

### delete-global

`delete` for **Global Marks**


<a id="org5a9b7e9"></a>

## Trashing Files


<a id="orga1ef814"></a>

### trash

Trash the current item.

Default keybinding: `S-t`


<a id="org74a0d55"></a>

### trash-local

`trash` for **Local Marks**

Default keybinding: `S-t`


<a id="orgd9a056e"></a>

### trash-global

`trash` for **Global Marks**

Default keybinding: `S-t`


<a id="orgf0a7300"></a>

## filter

Set a filter to directory.

Example: `*` displays everything, `*.csv` displays only the csv files, `*.png` displays only the png files


<a id="org438a5fe"></a>

## unfilter

Reset the appplied filter.


<a id="org0105a15"></a>

## refresh

Force refresh the current directory.

**NOTE: By default, Navi watches the directory for changes and loads them, so there is no requirement to refresh the directory. This command is there just in case something does not look right.**


<a id="org0ab786f"></a>

## hidden-files

Toggles the hidden files.

Default keybinding: `C-h`

NOTE: Hidden files are those items whose name start with a period like `.config`, `.gitignore` etc.


<a id="orgc1365cc"></a>

## Panes


<a id="orgc7cd395"></a>

### bookmark-pane

Opens the bookmarks list.


<a id="orgd6948e9"></a>

### marks-pane

Opens the marks list.


<a id="orgb076372"></a>

### messages-pane

Opens the messages list.


<a id="org4a14d08"></a>

### preview-pane

Toggles the preview pane.

Default keybinding: `C-p`

The preview pane handles previewing images (good number of formats) and PDF documents (first page) `asynchronously`. This means that the previewing experience will be seamless and without any lag. Navi uses `ImageMagick` library under the hood for previewing images and therefore any image formats supported my ImageMagick is supported by Navi.


<a id="org850d18f"></a>

## Misc


<a id="orgd748518"></a>

### menu-bar

Toggles the menu bar.

Default keybinding: `C-m`


<a id="org10ac22d"></a>

### focus-path

Focuses the path widget and sets it in edit mode.

Default keybinding: `C-l`


<a id="org50acaaf"></a>

### item-property

Display the property of the currently focused item.


<a id="org71dea1f"></a>

## Shell Commands


<a id="org6f2bb10"></a>

### TODO shell-command

Run a shell command *synchronously* (blocking).

**NOTE: This blocks the main GUI thread until the command finished executing**


<a id="orgd43288b"></a>

### TODO shell-command-async

Run a shell command *asynchronously* (non-blocking).

The running commands can be seen in the `Task Widget`


<a id="orga64ff53"></a>

## Bookmarks


<a id="org3a52244"></a>

### bookmark-add

Add a new non-existing bookmark


<a id="orgab4ddf4"></a>

### bookmark-remove

Remove an existing bookmark


<a id="orgbdc3f90"></a>

### bookmark-edit

Edit an existing bookmark

1.  Args:

    `title` - this tells Navi to edit the bookmark title
    `path` - this tells Navi to edit the bookmark directory that the bookmark points to


<a id="orgde4c484"></a>

### bookmark-go

Go to the directory pointed by the bookmark

1.  Args:

    `bookmark-name` - a unique bookmark name


<a id="org3cd70aa"></a>

### bookmark-save

Save the bookmarks that have been added.

**NOTE: Saving of the bookmarks added is done when the application exits. If you feel insecure about losing your bookmarks, then you can run this command manually.**


<a id="orgb47402c"></a>

## Searching Items

Searching is **Regular Expression** enabled. So you can pinpoint exactly the file that you want to look for.


<a id="orgc6ca662"></a>

### search

Searches the current directory for the search term

Default Keybinding: `/`


<a id="orga493148"></a>

### search-next

Goes to the next best match for the search term

Default Keybinding: `n`


<a id="orgfb2a7ad"></a>

### search-prev

Goes to the previous best match for the search term

Default Keybinding: `S-n`


<a id="org7887096"></a>

# Linux Only

Sorry, this software is built keeping in mind Linux and it’s derivatives only.


<a id="orga36c27f"></a>

# TODO Configuration with Lua

Navi can be configured using lua. The default configuration file is provided in this git repository. Navi looks for the configuration file in the XDG standard config directory (`~/.config`). You can place your configuration file in the `~/.config/navi/` directory. The configuration file should be named `config.lua`.

Configuration includes all the settings that can be changed for Navi and keybindings can also be changed.

**TL;DR: Navi configuration file (config.lua) should be placed at this location ( `~/.config/navi/config.lua`)**


<a id="orgb9fae21"></a>

# TODO Scripting with Lua

Navi can be scripted to include custom actions by subscribing to hooks and perform custom actions in response to those hooks.
