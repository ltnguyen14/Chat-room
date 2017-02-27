Lam Nguyen - CS410
I. File roster:
  1. c-s-socket.h : Headers that are needed for server and client code
  1. Server.c : The source code to set up the chat server.
  2. Client.c : The source code to compile the chat client.
  3. Makefile : A makefile to make the server and client binaries (this was based on the same makefile that you provided us for last lab)
  4. Screenshot : A screenshot of three clients chatting with each other using the client program above.
  5. README file : You are reading it right now.
II. Instruction:
  1. To build the code, simply type make or make all.
  2. Run the server by typing "./server portnumber"
  3. Run the client by typing "./client hostname portnumber"
III. Functionality:
  For this project, I used cursed library in combination of pthread to divide the client terminal screen into two menus, one for input and one for output. The user would type the message at the bottom menu and see the messages coming in at the top menu at the same time. I think this is a pretty neat addition to the program that makes it more interactive and much cleaner, since inputs and outputs are separated.
  When an user send a message to the server (using the client program), anything that doesn't start with "/" would be considered as a message and would be distributed to everybody else. Everybody would see that message with the following structure: "Username: Message". The sender however would see "You: Message".
  The chat protocol includes following requests:
    . /user username: login with the given name
    . /list : list users that are connected to the server
    . /whoami : print out your username
    . /help : bring up the help menu that contains infos about requests
  
