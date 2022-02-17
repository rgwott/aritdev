#!/usr/bin/env python3

# \file client.c
# \date 2022/02/15
# \author Rodolfo Wottrich
# \version 1.0
#
# \brief A userspace application that requests basic arithmetic operations to
#        to server application and reads back a success code and the result.


import os
import socket

OPERAND_LEN  = 8
RESPONSE_LEN = 1
SERVER_HOST  = '127.0.0.1'
SERVER_PORT  = 8080

print('(1) Add 2 numbers')
print('(2) Multiply 2 numbers')
print('(3) Subtract 2 numbers')
print('(4) Divide 2 numbers')
print('(5) Exit')

# Read operation
try:
    command = int(input("Enter command: "))
except ValueError:
    print("Invalid input.")
    os._exit(-1)

if command == 0 or command > 5:
    print("Invalid input.")
    os._exit(-1)

# Get ASCII symbol corresponding to operation
if command == 1:
    operation = '+'
elif command == 2:
    operation = '*'
elif command == 3:
    operation = '-'
elif command == 4:
    operation = '/'
if command == 5:
    print("Exiting...")
    os._exit(-1)

# Read first operand
try:
    operand1 = int(input("Enter operand 1: "))
except ValueError:
    print("Invalid input.")
    os._exit(-1)

# Read second operand
try:
    operand2 = int(input("Enter operand 2: "))
except ValueError:
    print("Invalid input.")
    os._exit(-1)

# Assemble request
request = bytes(operation, 'utf-8') + \
          operand1.to_bytes(OPERAND_LEN, 'little', signed=True) + \
          operand2.to_bytes(OPERAND_LEN, 'little', signed=True)

# Open socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # Connect to server
    s.connect((SERVER_HOST, SERVER_PORT))

    # Send request
    print("Sending request...");
    s.sendall(request)

    # Receive status response
    response = s.recv(RESPONSE_LEN)
    response = int.from_bytes(response, 'little')
    if response == 0:
        print("Request OKAY...");
    elif response == 1:
        print("Request was invalid (invalid length).");
        os._exit(-1)
    elif response == 2:
        print("Request was invalid (invalid operation).");
        os._exit(-1)
    else:
        print("Request was invalid (unknown error).");
        os._exit(-1)

    # We are here because request was successful. Read result
    print('Receiving result...')
    result = s.recv(OPERAND_LEN)
    print('Result is ', int.from_bytes(result, 'little'), '.', sep='')
