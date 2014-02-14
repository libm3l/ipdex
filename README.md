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


Portability
-----------

lsipdx has been tested on Linux only.

Dependency
----------

`lsipdx` depends on the `libm3l` library


[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/4066d03ffed8d2b3f1672861489f440e "githalytics.com")](http://githalytics.com/libm3l/lsipdx)