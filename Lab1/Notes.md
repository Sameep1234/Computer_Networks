# Steps for Server
* Import Libraries
* Define Server Port, maximum waiting requests, maximum buffer size as global variables.
* Define required variables
    * sockaddr_in struct
    * buffer
    * sockets (old and new)
    * Initialize the structure
    * Modify address, address_family and port (using htons() function)
    * Create a socket
    * Bind the socket with address and port
    * listen to each socket
    * create new socket using accept() function
    * Send or recieve the data/files.

# Steps for Client
* Import Libraries
* Define Server IP Address, Port number and Maximum buffer size.
* Define the required variables
    * Hostent Structure
    * sockets
    * buffer array
    * sockaddr_in structure
* Get argument from command line if any
* Invoke gethostbyname()
* Initialize sockaddr struc to memory byte 0.
* copy the address of host in hostent to sockaddr
* Modify the port
* Create the socket
* Connect to the server
* Send or recieve the data/files.