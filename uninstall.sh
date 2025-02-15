#!/bin/sh

echo "Uninstalling navifm and it's libraries."
echo "The config folder for navifm (~/.config/navifm/) is not deleted"
echo "if you have created it"
sudo rm -rf /usr/bin/navifm
sudo rm -rf /usr/share/navifm
sudo rm -rf /usr/include/kddockwidgets-qt6/
sudo rm -rf /usr/share/applications/navifm.desktop
