lsipdx - Synchronization and Inter-Process Data eXchage Utility
===============================================================

lsipdx is a utility which enables synchronization of data transfer between 
different processes. Upon start, the server utility opens a specified number of connections
which are each given its own identification name. Then it creates a specified number
of hubs for each connection.
The client processes connect to the server by using the hostname and port number 
and specify name of connection which they will use to transfer the data. 
Each client has to specify its role in the transfer chain - ie. either Sender
or Receiver. Once all client processes arrive, lsipdx starts transferring the data. 

Installation
-----------

In Source directory type
./configure PATH

where PATH is a path to libm3l.so library

then 

gmake


Portability
-----------

lsipdx has been tested on Linux only.

Dependency
----------

`lsipdx` depends on the `libm3l` library

[![Analytics](https://ga-beacon.appspot.com/UA-47978935-2/lsipdx/readme)](https://github.com/igrigorik/ga-beacon)

