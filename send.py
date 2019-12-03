#!/usr/local/bin/env python3
import sys
from serial import Serial
from time import sleep
import re 

DEBUG = True


def printd(message):
    if DEBUG:
        message = message.replace('POP', '');

        print(message)


if len(sys.argv) < 2:
    sys.stderr.write("Missing serial port argument")
    sys.exit(1)

sys.stderr.write("Opening {}\n".format(sys.argv[1]))
sys.stderr.flush()

serial = Serial(sys.argv[1], 9600, timeout=0.001)

sleep(1)


def expectResponse(*responses):
    data = ""

    while True not in [(resp in data) for resp in responses]:
        data += serial.readall().decode('ASCII')
        data = data.replace('\r', '')

        while '\n\n' in data:
            data = data.replace('\n\n', '\n')

        data = data.strip()

        if data:
            printd(data)
    return data

command = ""
c = sys.stdin.read(1)

while c:
    if c == ';':
        serial.write(command.encode('ASCII'))

        response = expectResponse('OK', 'FULL')

        if 'OK' in response and 'FULL' in response:
            ok = response.rindex('OK')
            fail = response.rindex('FULL')

            if fail > ok:
                expectResponse('POP')
                continue
        elif 'FULL' in response:
            expectResponse('POP')
            continue

        command = ""

    c = sys.stdin.read(1)
    command += c
