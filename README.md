#touchpad-disabler
###Featrures
This applicaiton will disable the touchpad when a usb mouse has been plugged in. The detection is persistant. through the use of gsettings.

###Package Requirements
* X11 for xinput
* udev
* libnotify
* gio for gsettings

###Building
Run make in the cloned directory
````bash
make

````

###Manual Building
To manually build the application using gcc.
````bash
gcc `pkg-config --cflags gtk+-3.0` -o bin/touchpad-disabler src/touchpad-disabler.c src/touchpad.c `pkg-config --libs gtk+-3.0 libnotify` -ludev

````

###Building the Schema for GSettings
Make sure to do this step or the application will not start
````bash
sudo cp schemas/apps.touchpad-disabler.gschema.xml /usr/share/glib-2.0/schemas/
sudo glib-compile-schemas /usr/share/glib-2.0/schemas/

````

###Installing the application
Use the provided install.sh or to manually install run the commands below
````bash
sudo mkdir /opt/touchpad-disabler
sudo cp bin/touchpad-disabler
sudo cp touchpad-disabler.desktop /usr/share/applications/
cp touchpad-disabler.desktop $HOME/.config/autostart/

````

###Autostarting with the user interface
Copy the touchpad-disabler.desktop to $HOME/.config/autostart
````bash
cp touchpad-disabler.desktop $HOME/.config/autostart/

````

###Disabling Notifications
To disable touchpad state notifications run the command below.
````bash
gsettings set apps.touchpad-disabler enable-notifications false

````

To disable touchpad state notifications run the command below.
````bash
gsettings set apps.touchpad-disabler enable-notifications true

````

###Todo
* Create a AUR package
* Create a ubuntu and debian package
* Wayland support
* Gnome shell extention that exposes the gsettings and indicates if the
  application is running.
