# aritdev
Kernel module for providing basic arithmetic operations, and associated userspace server and client applications.

### Dependencies
- Linux environment (tested with kernel 5.15.13)
- GCC (tested with 11.2.0)
- Make (tested with 4.3)
- Python (tested with 3.10.0)

### Building
To build all aplications, simply run
`make`
at the root of the repo.

To clean all aplications, run
`make clean`
at the root of the repo.

### Kernel module
Load the module from the root of the repo with:

`# insmod module/aritdev.ko`

This creates character device /dev/aritdev, with which applications can interact to request basic arithmetic operations. The module logs messages on the kernel log (dmesg).

The module awaits for write operations that describe arithmetic operations. Messages written must be exactly 17 bytes long. Byte 0 must be one of the following ASCII characters that define an arithmetic operation:
- \+ (sum)
- \* (multiplication)
- \- (subtraction)
- \/ (division)

Bytes 1 through 8 contain the first 64-bit operand and bytes 9 through 16 contain the second 64-bit operand. The operands are little endian in the x86-64 architecture.

Upon reception of a write operation, the module returns the amount of bytes received (17) if the message is well-formed or -EINVAL if it is malformed (e.g. invalid length or invalid operation). The result is then computed.

If a result is available (meaning that there has been a write describing a valid operation and the result has not yet been read), it can be obtained through a read operation.

The module may be unloaded with

`# rmmod aritdev`

while no application holds the device open.

By default, when the module is loaded, device /dev/aritdev is created with read/write permissions for root only. To allow other users to interface with the device, copy file udev/96-aritdev.rules to directory /etc/udev/rules.d/, then run:

`# udevadm control -R`

to reload udev rules.

### Server application
Run the server application from the root of the repo with:

`$ server/server`

The server listens to requests from client applications on port 8080.
A request message must be composed identically to the operations expected by the aritdev device (one operation byte and two 64-bit operands).

Before forwarding the request to aritdev, the server performs validations and responds to the client with a status code. A valid message returns a zero, a message with invalid length returns a 1 and a message with and invalid operation returns a 2.

If the request is valid, it is written to aritdev and the result is read. The server then responds with the 64-bit result to the client, closes the connection, and awaits for a new connection.

### Client applications
Two client applications are provided, one in C and one in Python.
They are identical in functionality.
The C client is executed from the root of the repo with:

`$ client/client`

while the Python client is executed with:

`$ python python_client/python_client.py`

Both clients present four arithmetic operations for the user to choose from.
The user inputs the chosen application then inputs the two signed integer operands.

The clients perform validations then assemble a request which is sent to the server on port 8080.
The status code response from the server is then retrieved and, in case of success, so is the result of the arithmetic operation.
