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
#ifndef _touchpad_h
#define _touchpad_h

int get_touchpad_state();
int set_touchpad_sate(const char *state);

int get_clickpad_setting();
int set_clickpad_state(const char *state);

int get_clickpad_xinput_id();
int set_touchpad_xinput_state(const char *state);

#endif
