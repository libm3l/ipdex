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


<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-47978935-1', 'github.com');
  ga('send', 'pageview');

</script>
