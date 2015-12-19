/****************************************************************************
** This file is part of touchpad-disabler.
**
**    Foobar is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Foobar is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.

**    You should have received a copy of the GNU General Public License
**    along with touchpad-disabler.
**    If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#ifndef _touchpad_disabler_h
#define _touchpad_disabler_h

#include <stdbool.h>
#include <libudev.h>

// udev structure pointers
struct udev *udev;
struct udev_monitor *device_monitor;
struct udev_enumerate *dev_enum;
struct udev_list_entry *devices;
struct udev_list_entry *dev_list_entry;
struct udev_device *device;

char *get_saved_file_path();
bool check_file(char file_path[]);
bool check_device(struct udev_device *dev, char buffer[]);
bool check_for_mouse(struct udev_device *dev);
char *read_saved_device(char file_path[]);
void write_saved_device(const char data[], char file_path[]);

#endif
