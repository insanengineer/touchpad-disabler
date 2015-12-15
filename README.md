#touchpad-disabler
###Featrures
This applicaiton will disable the touchpad when a usb mouse has been plugged in. The detection is persistant.

###Package Requirements
* synaptics
* udev

###Building
````bash
gcc -Wall -g -o touchpad-disabler touchpad-disabler.c -ludev

````

###Todo
* Create a Makefile
* Create a systemd service script
* Configure app to be a daemon
* Create a AUR package
* Create a ubuntu and debian package
