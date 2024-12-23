Installation
------------

Navi can be built from source. The following libraries are dependencies for navi:

- `Qt6 <https://www.qt.io/product/qt6>`_
- `Lua <https://www.lua.org/start.html>`_
- `ImageMagick(Magick++) <https://imagemagick.org/script/magick++.php>`_
- `Poppler (Poppler-Qt6) <https://poppler.freedesktop.org/api/qt6/>`_
- `Udisks <https://www.freedesktop.org/wiki/Software/udisks/>`_

If you're on ArchLinux you can copy paste the following `pacman` code to install these dependencies:

.. code-block:: bash

    sudo pacman -S qt6-base lua imagemagick poppler poppler-qt6 udisks2 libarchive

If you're on Ubuntu/Debian based distribution, you can paste the following `apt` code to install the dependencies:

.. code-block:: bash

    sudo apt install qt6-base-dev lua5.4 libmagick++-dev libpoppler-dev libpoppler-cpp-dev libpoppler-qt6-dev udisks2 libarchive

**NOTE**: I have not tested Navi on an Ubuntu machine. If there are any problems, please open an issue.

After installing these dependencies on your system and cloning this project repository, you can build navi by using the following command (inside the project’s directory):

.. code-block:: bash

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)
    sudo make install

One liner:

.. code-block:: bash

    mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc) && sudo make install

If everything went well, you should have navi installed on your system. You can launch navi using the command `navi`.

Next, you can check out :doc:`getting_started` to know more about the configuration options and customizations in navi.
