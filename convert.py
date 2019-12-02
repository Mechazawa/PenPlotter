#!/usr/local/bin/env python3
#todo make it a lot better

import sys

if len(sys.argv) < 2:
    print("Missing file argument")
    sys.exit(1)

width = 55
height = 55

def normalize(x):
    x = round(x * 2) / 2

    if x % 1 == 0.5:
        return "{}.".format(int(round(x)))
    
    return str(int(round(x)))

high = low = 0

def parseGroup(coordinates):
    coordinates = [
        [
            normalize((coord[0] - low) * (width / high)),
            normalize(height - ((coord[1] - low) * (height / high))),
        ]
        for coord in coordinates]

    lastX = -1
    lastY = -1
    output = ""
    penDown = False

    for coord in coordinates:
        endLine = False

        if lastX != coord[0]:
            lastX = coord[0]
            endLine = True
            output += 'X' + coord[0]

        if lastY != coord[1]:
            lastY = coord[1]
            endLine = True
            output += 'Y' + coord[1]

        if endLine:
            output += ';\n'
        
            if not penDown:
                penDown = True
                output += 'D;\n'

    return output + 'U;\n'


with open(sys.argv[1], 'r') as f:
    coordinates = [
            [float(coord) for coord in str(line).strip().split(',')] if '#' not in line else 'BREAK'
        for line in f.readlines()]

flatCoord = [y for x in coordinates for y in x if x != 'BREAK'] 

low = min(flatCoord)
high = max(flatCoord) - low

print("U;\n")
cluster = []
for coord in coordinates:
    if coord == 'BREAK':
        print(parseGroup(cluster))
        cluster = []
    else:
        cluster.append(coord)

if len(cluster) > 0:
    print(parseGroup(cluster))

print("X{}Y{};".format(width, height));