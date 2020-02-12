#!/usr/local/bin/env python3
#todo make it a lot better

import sys

if len(sys.argv) < 2:
    print("Missing file argument")
    sys.exit(1)

width = 45
height = 65

def normalize(x):
    x = round(x * 2) / 2

    if x % 1 == 0.5:
        return "{}.".format(int(round(x)))
    
    return str(int(round(x)))

lowX = lowY = factorX = factorY = factor = 0

def parseGroup(coordinates):
    coordinates = [
        [
            ((coord[0] - lowX) * factor),
            ((coord[1] - lowY) * factor),
        ]
        for coord in coordinates]

    lastX = -1
    lastY = -1
    output = ""
    penDown = False
    count = 0

    for coord in coordinates:
        endLine = False

        if lastX != coord[0]:
            lastX = coord[0]
            endLine = True
            output += 'X' + normalize(coord[0])

        if lastY != coord[1]:
            lastY = coord[1]
            endLine = True
            output += 'Y' + normalize(coord[1])

        if endLine:
            count += 1
            output += ';\n'
        
            if not penDown:
                penDown = True
                output += 'D;\n'

    if count < 2:
        return ''

    return output + 'U;\n'


with open(sys.argv[1], 'r') as f:
    coordinates = [
            [float(coord) for coord in str(line).strip().split(',')] if '#' not in line else 'BREAK'
        for line in f.readlines()]

flatCoordX = [coord[0] for coord in coordinates if coord != 'BREAK']
flatCoordY = [coord[1] for coord in coordinates if coord != 'BREAK']

lowX = min(flatCoordX)
lowY = min(flatCoordY)
factorX = width / (max(flatCoordX) - lowX)
factorY = height / (max(flatCoordY) - lowY)
factor = factorX if abs(factorX - 1) > abs(factorY - 1) else factorY

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