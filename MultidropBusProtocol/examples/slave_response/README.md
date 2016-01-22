# Slave Response Example

This example demonstrates receiving data from slave nodes.

Master sends a message asking a slave node what it's button value 
is. The slave node then puts a 0 or 1 into that message body as 
the response. This is mostly handled by the library automatically, 
all the slave program code needs to do is provide a callback function 
that will provide this value at runtime.