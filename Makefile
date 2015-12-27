APPNAME=touchpad-disabler
CFLAGS=`pkg-config --cflags gtk+-3.0`
GTKFLAGS=`pkg-config --libs gtk+-3.0 libnotify`
FILES=src/touchpad-disabler.c	src/touchpad.c
CC=gcc	-g	-Wall

all:	app

app:	$(FILES)
	$(CC)	$(CFLAGS)	-o	bin/$(APPNAME)	$(FILES)	$(GTKFLAGS)	-ludev

clean:
	/bin/rm	-rf	bin/$(NAME)
