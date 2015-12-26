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
**
**    You should have received a copy of the GNU General Public License
**    along with touchpad-disabler.
**    If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "touchpad.h"


/****************************************************************************
** Function Name: get_touchpad_state
**
** Inputs: none
**
** Output: A integer value that indicates the state of the TouchpadOff
**         setting.
** 	   0	Touchpad is enabled
**         1	Touchpad is switched off
**         2	Only tapping and scrolling is switched off
**
** Description: This function retrieves the value of the TouchpadOff
** 		from synclient.
**
****************************************************************************/
int get_touchpad_state()
{
   int state = -1;
   char output_buffer[100];

   FILE *synclient_data = popen("synclient | grep TouchpadOff", "r");

   if (synclient_data != NULL)
    {
    if (fgets(output_buffer, sizeof(output_buffer), synclient_data) != NULL)
      {
	char value[10];

	// get the last character of the character array
	value[0] = output_buffer[(strlen(output_buffer) - 2)];

	// convert the character to an integer
	state = atoi(value);
      }
    }

  pclose(synclient_data);

  return state;
}

/****************************************************************************
** Function Name: change_touchpad_sate
**
** Inputs: const char *state. The requested state of the touchpad
**
** Output: A integer value that indicates if the command was accepted
 *         0 = success, -1 = failed
**
** Description: This function changes the state of the touchpad using
**		using the synclient TouchpadOff property
**
****************************************************************************/
int set_touchpad_sate(const char *state)
{
  int sucess = -1;

  if (strcmp(state, "ON") == 0)
   {
      // enable the touchpad
      system("synclient TouchpadOff=0");
      sucess = 0;
   }
  else
   {
      // disable the touchpad
      system("synclient TouchpadOff=1");
      sucess = 0;
   }

  return sucess;
}

/****************************************************************************
** Function Name: get_click_pad_setting
**
** Inputs: None
**
** Output: A integer value that indicates the current clickpad setting of
** 	   synclient.
**         0 = Clickpad fucntionality Disabled
** 	   0 = Clickpad fucntionality Enabled
**         -1 = There was an error processing the request
**
** Description: This function changes the state of the touchpad using
**		using the synclient TouchpadOff property
**
****************************************************************************/
int get_clickpad_setting()
{
   int setting = -1;
   char output_buffer[100];

   FILE *synclient_data = popen("synclient | grep ClickPad", "r");

   if (synclient_data != NULL)
    {
    if (fgets(output_buffer, sizeof(output_buffer), synclient_data) != NULL)
      {
	char value[10];

	// get the last character of the character array
	value[0] = output_buffer[(strlen(output_buffer) - 2)];

	// convert the character to an integer
	setting = atoi(value);
      }
    }

  pclose(synclient_data);

  return setting;
}

/****************************************************************************
** Function Name: set_clickpad_sate
**
** Inputs: const char *state. The requested state of the clickpad
**
** Output: A integer value that indicates if the command was accepted
 *         0 = success, -1 = failed
**
** Description: This function changes the state of the clickPad using
**		using the synclient clickPad property
**
****************************************************************************/
int set_clickpad_state(const char *state)
{
  int sucess = -1;

  if (strcmp(state, "ON") == 0)
   {
      // enable the clickpad feature
      system("synclient ClickPad=1");
      sucess = 0;
   }
  else
   {
      // disable the clickpad feature
      system("synclient ClickPad=0");
      sucess = 0;
   }

  return sucess;
}

/****************************************************************************
** Function Name: get_clickpad_xinput_id
**
** Inputs: None
**
** Output: A integer value that indicates the xinput id of the touchpad
**         -1 = There was an error processing the request
**	   Any other value is the xinput device id
**
** Description: This function gets the xinput id of the touchpad
**
****************************************************************************/
int get_clickpad_xinput_id()
{
   int id = -1;
   char output_buffer[100];

   FILE *xinput_data = popen("xinput --list --id-only 'SynPS/2 Synaptics TouchPad'", "r");

   if (xinput_data != NULL)
    {
       if (fgets(output_buffer, sizeof(output_buffer), xinput_data) != NULL)
        {
	  // convert the character to an integer
	  id = atoi(output_buffer);
        }
    }

  pclose(xinput_data);

  return id;
}

/****************************************************************************
** Function Name: set_touchpad_xinput_state
**
** Inputs: const char *state. The requested state of the touchpad
**
** Output: A integer value that indicates if the command was accepted
 *         0 = success, -1 = failed
**
** Description: This function changes the state of the touchpad using
**		using the xinput --enable or --disable
**
****************************************************************************/
int set_touchpad_xinput_state(const char *state)
{
  int sucess = -1;
  int cmd = 1;
  char xinput_cmd[45];

  // get the xinput id of the touchpad
  int id = get_clickpad_xinput_id();

  if (id != -1)
    {
  	if (strcmp(state, "ON") == 0)
   	 {
	    cmd = 1;
         }
        else
         {
            cmd = 0;
         }

        // create the complete command string
        snprintf(xinput_cmd, 45, "xinput set-prop %d 'Device Enabled' %d", id, cmd);

        // send the command to the terminal
        system(xinput_cmd);

        sucess = 0;
    }

  return sucess;
}

