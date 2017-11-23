# TalkMe messanger
This is simple messenger for conversation.

### Requirements
- gcc compiler (tested with version 6.3.0) 
- gtk2 library (to install use 'sudo apt-get install gtk-2.0')

### How to install?
Clone repository to your computer and execute following commands in folder with cloned files :
1) ```gcc -o gclient gclient.c client.c client.h `pkg-config --cflags --libs gtk+-2.0` ```
   > Tip: command will create runnable client file.
2) ```gcc -o newserver newserver.c newserver.h```
   > Tip: command will create runnable server file
3) ```gcc -o talkme startTalkMe.c `pkg-config --cflags --libs gtk+-2.0` ```
   > Tip: command will create entry point for client start (It is possible to start client without it!)

### How to run server?
To start server run command : 
  ./newserver
   > Tip: it does not require any information to input. Server starts on default network interface on port 4547.

### How to run client?
There are available two ways:
  1) Via GUI:
      ```./talkme```
  2) From terminal:
      ```./gclient [user_name] [server_address]```
