# aritdev
Kernel module for providing basic arithmetic operations, and associated userspace server and client applications.

### Dependencies
- Linux environment (tested with kernel 5.15.13)
- GCC (tested with 11.2.0)
- Make (tested with 4.3)

### Building
To build, simply run
`make`
at the root of the repo.

To clean, run
`make clean`
at the root of the repo.

### Kernel module
Load the module with:

`# insmod module/aritdev.ko`

This creates character device /dev/aritdev, with which applications can interact to request basic arithmetic operations. The module logs messages on the kernel log (dmesg).

The module awaits for write operations that describe arithmetic operations. Messages written must be exactly 17 bytes long. Byte 0 must be one of the following ASCII characters that define an arithmetic operation:
- \+ (sum)
- \* (multiplication)
- \- (subtraction)
- \/ (division)

Bytes 1 through 8 contain the first 64-bit operand and bytes 9 through 16 contain the second 64-bit operand. The operands are little endian in the x86-64 architecture.

Upon reception of a write operation, the module responds (read operation) with one confirmation byte. If the message is valid, the byte returned is a zero. Otherwise, the byte returned is -EINVAL.

The module may be unloaded with
`# rmmod aritdev`
while no application holds the device open.

By default, when the module is loaded, device /dev/aritdev is created with read/write permissions for root only. To allow other users to interface with the device, copy file udev/96-aritdev.rules to directory /etc/udev/rules.d/, then run:

`# udevadm control -R`

to reload udev rules.
