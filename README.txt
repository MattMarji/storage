STORAGE-SERVER W/ CLIENT SHELL

This program creates a storage server that allows authenticated users to add, remove, and query data.
The server details are found in 'conf.kern' and the details can be changed.

Documentation can be found in storage->doc->doxygen->latex->refman.pdf

As well, you can launch the server by going into the src folder and in your shell, launch './server conf.kern'
Note, the arguement is expecting a .conf file with specific parameters. The server will NOT start without the correct parameters.

To launch the client shell, run './client' and ensure that you enter the proper credentials or else you will not be able to setup a TCP connection to the server.
You have the ability to run multiple clients at the same time as mutex's have been properly inserted into the code.

NOTE: All parsing has been completed using Yacc and Lex. Those files can also be found in the storage->src directory.


BIG THANKS TO MY PARTNER HEINDRIK FOR HIS CONTRIBUTIONS AS WELL!!

Have any questions? E-mail me: matthew.marji@utoronto.ca