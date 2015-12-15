/*This file is part of touchpad-disabler.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with touchpad-disabler.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "touchpad-disabler.h"

char* concat(char *string_one, char *string_two)
{
    size_t string_one_length = strlen(string_one);
    size_t string_two_length = strlen(string_two);

    char *result = malloc(string_one_length + string_two_length + 1);

    strcpy(result, string_one);
    strcat(result, string_two);

    return result;
}

void read_saved_device(char array[])
{
    FILE *file_handle;
    char read_buffer[MAX_BUFFER_SIZE];

    // open the saved device file as readonly
    file_handle = fopen(saved_file_path, "r");

    if (file_handle != NULL)
    {
      fgets(read_buffer, MAX_BUFFER_SIZE, file_handle);
      strcpy(array, read_buffer);

      printf("%s\n", read_buffer);

      fclose(file_handle);
    }
}

void write_saved_device(char array[])
{
    FILE *file_handle;

    // open the saved device file foe writing
    file_handle = fopen(saved_file_path, "w");

    if (file_handle != NULL)
    {
       fputs(array, file_handle);
       fclose(file_handle);
    }
}

bool check_for_mouse(struct udev_device *dev)
{
    bool result = false;
    char dev_path_udev[MAX_BUFFER_SIZE];
    int char_match_cnt = 0;

    // make sure that the data we are looking at is valid
    if (udev_device_get_devnode(dev) != NULL)
    {
        // length of the device path string
        int dev_path_string_len = strlen(dev_path_string);

        // get a copy of the udev device path string
        strcpy(dev_path_udev, udev_device_get_devnode(dev));

        for (int i = 0; i < dev_path_string_len; i++)
        {
            if (dev_path_string[i] == dev_path_udev[i])
            {
                char_match_cnt++;
            }
        }

        if (char_match_cnt == dev_path_string_len)
        {
            // make a copy of the device path so we know when it has been removed
            write_saved_device(dev_path_udev);
            result = true;
        }
    }

    return result;
}

bool check_device(struct udev_device *dev, char buffer[])
{
    int compare = 0;
    bool result = false;

    if (udev_device_get_devnode(dev) != NULL)
    {
        //printf("%s %s", udev_device_get_devnode(dev), buffer);
        compare = strcmp(udev_device_get_devnode(dev), buffer);

        if (compare == 0)
        {
            result = true;
        }
    }

    return result;
}

int main (int argc, char *argv[])
{
    int act_compare = 0;
    char file_contents_buffer[MAX_BUFFER_SIZE];
    char saved_file_name[] = "/.saved_device";

    char* users_home_path = getenv("HOME");
    saved_file_path = concat(users_home_path, saved_file_name);
    //printf("%s\n", saved_file_path);

    read_saved_device(file_contents_buffer);

    // create the udev object
    udev = udev_new();

    if (!udev)
    {
        exit(1);
    }

    // setup a udev monitor for input devices
    device_monitor = udev_monitor_new_from_netlink(udev, "udev");

    // setup a filter for the monitor so that we only get notified of input devices
    udev_monitor_filter_add_match_subsystem_devtype(device_monitor, "input", NULL);

    // start recieving events
    udev_monitor_enable_receiving(device_monitor);

    // create a list of the input devices
    dev_enum = udev_enumerate_new(udev);

    // setup a enumerate filter of the input devices
    udev_enumerate_add_match_subsystem(dev_enum, "input");

    // get the device list
    udev_enumerate_scan_devices(dev_enum);

    // the device list
    devices = udev_enumerate_get_list_entry(dev_enum);

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path;

        path = udev_list_entry_get_name(dev_list_entry);
        device = udev_device_new_from_syspath(udev, path);

        if (device)
        {
            if (check_for_mouse(device) == true)
            {
                if (check_device(device, file_contents_buffer) == true)
                {
                   // disable the touchpad since a mouse is plugged in
                   system("synclient TouchpadOff=1");
                   break;
                }
            }
        }
    }

    while (1)
    {
        // recieve the usb event
        // this function is blocking. since this is the sole purpose of this
        // applicaiton block is ok
        device = udev_monitor_receive_device(device_monitor);

        if (device)
        {
            // check if a usb input device has been added
            act_compare = strcmp(udev_device_get_action(device), dev_action_add_string);

            // a usb device has been added
            if (act_compare == 0)
            {
                // if the device is a mouse then disable the touchpad
                if (check_for_mouse(device) == true)
                {
                    // disable the touchpad
                    system("synclient TouchpadOff=1");
                }
            }
            else
            {
                // check if the device has been removed
                act_compare = strcmp(udev_device_get_action(device), dev_action_remove_string);

                if (act_compare == 0)
                {
                    // check if it was the same device removed
                    if (check_device(device, file_contents_buffer) == true)
                    {
                        // enable the touchpad
                        system("synclient TouchpadOff=0");
                    }
                }
            }

            udev_device_unref(device);
        }

        // 250 millisecond delay
        usleep(250 * 1000);
    }

    free(saved_file_path);
    free(udev);

    return 0;
}
