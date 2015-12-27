#!/usr/bin/sh

sudo mkdir /opt/touchpad-disabler
sudo cp bin/touchpad-disabler /opt/touchpad-disabler/

sudo cp schemas/apps.touchpad-disabler.gschema.xml /usr/share/glib-2.0/schemas/
sudo glib-compile-schemas /usr/share/glib-2.0/schemas/

cp touchpad-disabler.desktop $HOME/.config/autostart/
sudo cp touchpad-disabler.desktop /usr/share/applications/
