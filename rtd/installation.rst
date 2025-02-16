Installation
------------

Navi can be built from source. The following libraries are dependencies for navi:

- `Qt6 <https://www.qt.io/product/qt6>`_ GUI framework
- `Lua <https://www.lua.org/start.html>`_ Scripting language
- `ImageMagick(Magick++) <https://imagemagick.org/script/magick++.php>`_ Image library (used for preview and thumbnailing image files)
- `Poppler (Poppler-Qt6) <https://poppler.freedesktop.org/api/qt6/>`_ PDF library (used for previewing and thumbnailing PDF files)
- `Udisks <https://www.freedesktop.org/wiki/Software/udisks/>`_ Disk management
- `ffmpegthumbnailer <https://github.com/dirkvdb/ffmpegthumbnailer>`_ Thumbnailing video files

If you're on ArchLinux:

Install from the AUR
~~~~~~~~~~~~~~~~~~~~

You can install navi from the AUR. The package is called ``navifm-git``.

Building manually
~~~~~~~~~~~~~~~~~

ArchLinux
=========

You can copy paste the following `pacman` code to install these dependencies:

.. code-block:: bash

    sudo pacman -S make ninja cmake qt6-base lua imagemagick poppler poppler-qt6 udisks2 libarchive ffmpegthumbnailer

Debian/Ubuntu
=============

If you're on Ubuntu/Debian based distribution, you can paste the following `apt` code to install the dependencies:

.. code-block:: bash

    sudo apt install qt6-base-dev lua5.4 libmagick++-dev libpoppler-dev libpoppler-cpp-dev libpoppler-qt6-dev udisks2 libarchive ffmpegthumbnailer

.. note:: I have not tested Navi on an Ubuntu machine. If there are any problems, please open an issue.

After installing these dependencies on your system, clone the repository and then execute the ``build.sh`` script.

If everything went well, you should have navi installed on your system. You can launch navi using the command `navi`.

Next, you can check out :doc:`getting_started` to know more about the configuration options and customizations in navi.

Uninstallation
~~~~~~~~~~~~~~

If you have installed navi using PKGBUILD then simply run the following command to uninstall navi and it's related files **except for your configuration files**:

``sudo pacman -Rns navi``

Or you can run the ``uninstall.sh`` script if you had cloned the repo and built navi, both of these methods do the same thing.
