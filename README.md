lsipdx - Synchronization and Inter-Process Data eXchage
Co-Simulation Engine
===============================================================

lsipdx is a utility which enables synchronization of data transfer between 
different processes. It can work as a co-simulation engine. 
Upon start, the server utility opens a specified number of connections
which are each given its own identification name. Then it creates a specified number
of hubs for each connection.
The client processes connect to the server by using the hostname and port number 
and specify name of connection which they will use to transfer the data. 
Each client has to specify its role in the transfer chain - ie. either Sender
or Receiver. Once all client processes arrive, lsipdx starts transferring the data. 

The library uses libm3l TCP/IP protocol which needs to be installed prior installing lsipdx. 
Libm3l is available at https://github.com/libm3l/libm3l

Installation
-----------

In Source directory type
./configure PATH

where PATH is a path to libm3l.so library

then 

gmake

Linking
-----------

To link the shared library use aither

-L/PATHTOLIBM3L/ -lm3l -Wl,-rpath=/PATHTOLIBM3L/ -L/PATHTOLSIPDX/ -llsipdx -Wl,-rpath=/PATHTOLSIPDX/ 

where PATHTOLIBM3L is a path to location of libm3l.so file and PATHTOLSIPDX is a location to liblsipdx.so


or 

link libm3lsipdx.so library which contains all libm3l and lsipdx objects:

-L/PATHTOLSIPDX/ -lm3lsipdx -Wl,-rpath=/PATHTOLSIPDX/ 

This is preferred if you link to c++ compiled programs




Portability
-----------

lsipdx has been tested on Linux only.

Dependency
----------

`lsipdx` depends on the `libm3l` library


References
-----------

Adam Jirasek and Arthur W. Rizzi: "libm3l and lsipdx - Utilities for Inter-Process Data Transfer and Synchronization", 52nd Aerospace Sciences Meeting, AIAA SciTech Forum, (AIAA 2014-1045)
https://doi.org/10.2514/6.2014-1045, https://www.researchgate.net/publication/269248697_libm3l_and_lsipdx_-_Utilities_for_Inter-Process_Data_Transfer_and_Synchronization

 Adam Jirasek, Olivier Amoignon, Peter Eliasson, mats Dalenbring and Urban Falk: "Coupling of the Edge CFD Solver with External Solvers", 53rd AIAA Aerospace Sciences Meeting, AIAA SciTech Forum, (AIAA 2015-0769)
https://doi.org/10.2514/6.2015-0769, https://www.researchgate.net/publication/273462889_Coupling_of_the_Edge_CFD_Solver_with_External_Solvers

Karlsson A. and Ringertz U.: Experimental and Analytical Investigation of Aircraft-Like Wind Tunnel Model with External Stores, XVII International Forum on Aeroelasticity and Structrural Dynamics, 25-28 June, 2017, Como, Italy


[![Analytics](https://ga-beacon.appspot.com/UA-47978935-2/libm3l/lsipdx)](https://github.com/igrigorik/ga-beacon)

