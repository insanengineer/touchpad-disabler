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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "touchpad-disabler.h"

/****************************************************************************
** Function Name: get_saved_file_path
**
** Inputs: None
**
** Output: Ponter to concatenated sting of the users home directory and the
**         saved file name.
**
** Description: This function get the users home directory path from the
**		the envronment vairables. After the home directory path has
** 		been fetched then it get concatenated with the saved device
**		file name and then returned.
**
****************************************************************************/
char *get_saved_file_path()
{
    char saved_file_name[] = "/.saved_device";
    char *users_home_path;

    users_home_path = getenv("HOME");

    strcat(users_home_path, saved_file_name);

    return users_home_path;
}

/****************************************************************************
** Function Name: get_saved_file_path
**
** Inputs: string file_path[]
**
** Output: A boolean value that idicates if the requested file is present or
**         not. A value of true indicates the file exisits.
**
** Description: This function uses the access command to see if the selected
**  		file exisits and is readable and writable.
**
****************************************************************************/
bool check_file(char file_path[])
{
    int file_check_result = -1;
    bool result = false;

    // check if the file exists and is readable and writable
    file_check_result = access(file_path, F_OK | R_OK | W_OK);

    if (file_check_result == 0)
    {
       result = true;
    }

    return result;
}

/****************************************************************************
** Function Name: read_saved_device
**
** Inputs: string file_path[]
**
** Output: A pointer to a string that contains the contents of the file.
**
** Description: This function checks to see if the saved device file is open
** 		then gets the data from the file and returns a pointer to it.
**
****************************************************************************/
char *read_saved_device(char file_path[])
{
    FILE *file_handle;
    char read_buffer[25];
    char *read_data;

    if (check_file(file_path) == true)
    {
    	// open the saved device file as readonly
        file_handle = fopen(file_path, "r");

        if (file_handle != NULL)
        {
           fgets(read_buffer, 25, file_handle);
           //strcpy(data, read_buffer);

           read_data = read_buffer;

           fclose(file_handle);
        }
    }

    return read_data;
}

/****************************************************************************
** Function Name: write_saved_device
**
** Inputs: string data[], string file_path[]
**
** Output: None
**
** Description: This function writes the information from the data string into
** 		the saved device file at the file path string.
**
****************************************************************************/
void write_saved_device(const char data[], char file_path[])
{
    FILE *file_handle;

    // open the saved device file foe writing
    file_handle = fopen(file_path, "w");

    if (file_handle != NULL)
    {
       fputs(data, file_handle);
       fclose(file_handle);
    }
}

/****************************************************************************
** Function Name: check_for_mouse
**
** Inputs: udev_device dev
**
** Output: A boolean value if the device is a mouse or not. A value of true
**         means the device is mouse.
**
** Description: This function gets the device path from the device passed
**  		to it and checks to see if it is a mouse based off a const
** 		string that holds the location and type of the device.
**
****************************************************************************/
bool check_for_mouse(struct udev_device *dev)
{
    bool result = false;
    const char *dev_path_udev;
    const char *dev_path_string = "/dev/input/mouse";
    int char_match_cnt = 0;

    dev_path_udev = udev_device_get_devnode(dev);

    // make sure that the data we are looking at is valid
    if (dev_path_udev != NULL)
    {
        // length of the device path string
        int dev_path_string_len = strlen(dev_path_string);

        for (int i = 0; i < dev_path_string_len; i++)
        {
            if (dev_path_string[i] == dev_path_udev[i])
            {
                char_match_cnt++;
            }
        }

        if (char_match_cnt == dev_path_string_len)
        {
            result = true;
        }
    }

    return result;
}

/****************************************************************************
** Function Name: compare_device
**
** Inputs: udev_device dev,  char buffer
**
** Output: A boolean value if the current plugged in device matched that of
**         the device that is contained int he buffer variable. A value of
** 	   true means the curent device matches the buffer.
**
** Description: This function uses strcmp to compare the buffer string to
**  		the current device detected from udev
**
****************************************************************************/
bool compare_device(struct udev_device *dev, char buffer[])
{
    int compare = 0;
    bool result = false;

    if (udev_device_get_devnode(dev) != NULL)
    {
        compare = strcmp(udev_device_get_devnode(dev), buffer);

        if (compare == 0)
        {
            result = true;
        }
    }

    return result;
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
int main (int argc, char *argv[])
{
    char *file_contents_buffer; //[25];
    char *saved_file_path;

    saved_file_path = get_saved_file_path();

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
                file_contents_buffer = read_saved_device(saved_file_path);

	        if (compare_device(device, file_contents_buffer) == true)
                {
                   // disable the touchpad since a mouse is plugged in
                   system("synclient TouchpadOff=1");
                   break;
                }
            }
        }
    }

    free(devices);
    free(dev_list_entry);

    while (1)
    {
        // recieve the usb event
        // this function is blocking. since this is the sole purpose of this
        // applicaiton block is ok
        device = udev_monitor_receive_device(device_monitor);

        if (device)
        {
            const char *read_dev_path;
	    const char *read_dev_action;

            read_dev_path = udev_device_get_devnode(device);
	    read_dev_action = udev_device_get_action(device);

            // a usb device has been added
            if (strcmp(read_dev_action, "add") == 0)
            {
                // if the device is a mouse then disable the touchpad
                if (check_for_mouse(device) == true)
                {
		    // this section of code is to avoid exessive writes of the
		    // saved device file. The basic logic is that if the file
		    // exists the check if the contents are the same as the
		    // current plugged in device. If the contents to not match
		    // current device update the file.
		    if (check_file(saved_file_path) == true)
		    {
	 		 char *saved_device;

		         saved_device = read_saved_device(saved_file_path);

		         if (strcmp(saved_device, read_dev_path) != 0)
			 {
			    // write the saved device file since a device was plugged in
		    	    write_saved_device(read_dev_path, saved_file_path);
			 }

		         free(saved_device);
		    }
		    else
		    {
			write_saved_device(read_dev_path, saved_file_path);
		    }

		    // disable the touchpad
                    system("synclient TouchpadOff=1");
                }
            }

	    // a usb device has been removed
            if (strcmp(read_dev_action, "remove") == 0)
            {
                // check if the device has been removed
		if (check_for_mouse(device) == true)
		{
		   system("synclient TouchpadOff=0");
		}
            }
        }

        // release the udev device
        udev_device_unref(device);

        // 250 millisecond delay
        usleep(250 * 1000);
    }

    free(udev);
    free(device_monitor);
    free(device);
    free(dev_enum);

    free(saved_file_path);
    free(file_contents_buffer);

    return 0;
}
