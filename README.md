# TalkMe messanger
This is simple messenger for conversation.

### Requirements
- gcc compiler (tested with version 6.3.0) 
- gtk2 library (to install use 'sudo apt-get install gtk-2.0')

### How to install?
Clone repository to your computer and execute following commands in folder with cloned files :
1) Create runnable client file :
```gcc -o gclient gclient.c client.c client.h `pkg-config --cflags --libs gtk+-2.0` ```
2) Create runnable server file :
```gcc -o newserver newserver.c newserver.h```
3) Create entry point for client start (It is possible to start client without it!) :
```gcc -o talkme startTalkMe.c `pkg-config --cflags --libs gtk+-2.0` ```

### How to run server?
To start server run command : 
  ```./newserver```

### How to run client?
There are available two ways:
  1) Via GUI:
      ```./talkme```
  2) From terminal:
      ```./gclient [user_name] [server_address]```
