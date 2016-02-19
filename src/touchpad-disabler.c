/****************************************************************************
** Touchpad Disabler is an application that will disable the touchpad when
** a usb mouse has been plugged in.
** Copyright (C) 2015 Keith Lewis
**
** This file is part of Touchpad Disabler.
**
**    Touchpad Disabler is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Touchpad Disabler  is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Touchpad Disabler.
**    If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <libudev.h>
#include <signal.h>
#include <gio/gio.h>
#include <libnotify/notify.h>

#include "touchpad.h"
#include "touchpad-disabler.h"

/****************************************************************************
** Function Name: send_notification
**
** Inputs: notify_pointer - pointer to the notfication object
**         show_notifications - setting indicating if the user would like to
**                              see the state notifications
**         notification_type - enabled or disabled notifcation types
**
** Output: None
**
** Description: This function updates and displays a notification to the user
**              if the show_notifications flag is true
**
****************************************************************************/
void send_notification(NotifyNotification *notify_pointer, bool show_notifications, char notification_type[])
{
  if (show_notifications == true)
    {
         if (strcmp(notification_type, "disable") == 0)
           {
             notify_notification_update(notify_pointer, "Touchpad Disabler",
				        "A USB mouse was added.\n Disabling Touchpad",
				        "/usr/share/icons/Adwaita/48x48/devices/input-touchpad.png");
           }
         else
          {
             notify_notification_update(notify_pointer, "Touchpad Disabler",
				        "A USB mouse was removed.\n Enabling Touchpad",
				        "/usr/share/icons/Adwaita/48x48/devices/input-touchpad.png");
          }

        notify_notification_show(notify_pointer, NULL);
    }
}

/****************************************************************************
** Function Name: check_for_mouse
**
** Inputs: udev_device dev
**
** Output: A integer value if the device is a mouse or not. A value of 0
**         means the device is mouse.
**
** Description: This function gets the device path from the device passed
**  		to it and checks to see if it is a mouse based off a const
** 		string that holds the location and type of the device.
**
****************************************************************************/
int check_for_mouse(struct udev_device *dev)
{
    int result = -1;

    const char *dev_path_udev = udev_device_get_devnode(dev);

    char *mouse_detect = strstr(dev_path_udev, "mouse");

    if (mouse_detect != NULL)
    {
      	result = 0;
    }

    return result;
}

/****************************************************************************
** Function Name: get_vendor_id_model_id_string
**
** Inputs: udev device
**
** Output: string
**
** Description: string containing vendor and model id
**
****************************************************************************/
char *get_vendor_id_model_id_string(struct udev_device *dev)
{
   char ven_mod_string[10];

   const char *vendor_id = udev_device_get_property_value(dev, "ID_MODEL_ID");
   const char *model_id = udev_device_get_property_value(dev, "ID_VENDOR_ID");

   snprintf(ven_mod_string, 10, "%s:%s", vendor_id, model_id);

   printf("%s\n", ven_mod_string);

   char *new_str = ven_mod_string;

   return new_str;
}

/****************************************************************************
** Function Name: main
**
** Inputs: application inputs
**
** Output: exit condition
**
** Description: main
**
****************************************************************************/
int main (void)
{
    const char *schema_id = "apps.touchpad-disabler";
    GSettings *settings = g_settings_new (schema_id);

    notify_init ("Touchpad Disabler");
    NotifyNotification *user_notfication = notify_notification_new ("", "", "");

    // create the udev object
    struct udev *udev = udev_new();

    if (!udev)
    {
        exit(1);
    }

    // setup a udev monitor for input devices
    struct udev_monitor *device_monitor = udev_monitor_new_from_netlink(udev, "udev");

    // setup a filter for the monitor so that we only get notified of input devices
    udev_monitor_filter_add_match_subsystem_devtype(device_monitor, "input", NULL);

    // start recieving events
    udev_monitor_enable_receiving(device_monitor);

    // create a list of the input devices
    struct udev_enumerate *dev_enum = udev_enumerate_new(udev);

    // setup a enumerate filter of the input devices
    udev_enumerate_add_match_subsystem(dev_enum, "input");

    // get the device list
    udev_enumerate_scan_devices(dev_enum);

    // the device list
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(dev_enum);
    struct udev_list_entry *dev_list_entry = 0;

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path = udev_list_entry_get_name(dev_list_entry);

        struct udev_device *device = udev_device_new_from_syspath(udev, path);

        if (device)
        {
            const char *dev_node = udev_device_get_devnode(device);
	    const char *name = udev_device_get_sysattr_value(device,"name");

	    if (name != NULL)
	    {
               if (strstr(name, "Synaptics") == NULL)
	       {
		  printf("Device Name: %s\n", name);

		  if (dev_node != NULL)
		  {
		      if (check_for_mouse(device) == 0)
                      {
		         bool notifications_enabled = g_settings_get_boolean(settings, "enable-notifications");

                         set_touchpad_xinput_state("OFF");
	                 send_notification(user_notfication, notifications_enabled, "disable");
		         break;
		      }
		  }
	       }
	    }
        }

        udev_device_unref(device);
    }

    udev_enumerate_unref(dev_enum);

    while (1)
    {
        // recieve the usb event
        // this function is blocking. since this is the sole purpose of this
        // application blocking is ok
        struct udev_device *device = udev_monitor_receive_device(device_monitor);

        if (device)
        {
            const char *read_dev_path = udev_device_get_devnode(device);
            const char *read_dev_action = udev_device_get_action(device);

            if (read_dev_path != NULL)
            {
	        bool notifications_enabled = g_settings_get_boolean(settings, "enable-notifications");

	        // a usb device has been added
                if (strcmp(read_dev_action, "add") == 0)
                {
                    // if the device is a mouse then disable the touchpad
                    if (check_for_mouse(device) == 0)
                    {
		        set_touchpad_xinput_state("OFF");
		        send_notification(user_notfication, notifications_enabled, "disable");
                    }
                }

                // a usb device has been removed
                if (strcmp(read_dev_action, "remove") == 0)
                {
                    // check if the device has been removed
                    if (check_for_mouse(device) == 0)
                    {
		        set_touchpad_xinput_state("ON");
			send_notification(user_notfication, notifications_enabled, "enable");
                    }
                }
            }
        }

        // release the udev device
        udev_device_unref(device);

        // 250 millisecond delay
        usleep(250 * 1000);
    }

    udev_monitor_unref(device_monitor);
    udev_unref(udev);

    g_object_unref(G_OBJECT(user_notfication));
    notify_uninit();

    free(udev);
    free(device_monitor);
    free(settings);

    return 0;
}

