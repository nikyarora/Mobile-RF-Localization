import numpy
import math
import serial
import turtle
import time
import datetime
import csv

#function to interpret anchor locations and calibrations into matrices
# pass all inputs as matrix
def generateMatrices(x, y, cal, xi, yi):
    A = numpy.zeros((x.size,2))
    C = numpy.zeros(x.size)
    B = numpy.zeros(x.size)

    xnew = numpy.array(x.T)
    ynew = numpy.array(y.T)

    for i in range (0, len(xnew)):
        linearM = numpy.array([2*(xi-xnew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2)), 2*(yi-ynew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2))])
        A[i][:] = -10.0 /cal[i][0]* numpy.reshape(linearM, (2,))
        C[i] = (-cal[i][1]-25.2-20*math.log(math.sqrt(pow(xi-xnew[i],2) + (pow(yi-ynew[i],2))),10)) / cal[i][0]
        B[i] = 1/cal[i][0]
    C = C+numpy.dot(A,numpy.array([-xi,-yi]))

    return [A,B,C]

#generates a weighted matrix from the calibration data that can be used to solve location   
def generateWeightedMatrix(posfileName,RSSIfileName,num):
    pos = numpy.genfromtxt(posfileName,delimiter=',')
    RSSI = numpy.genfromtxt(RSSIfileName,delimiter=',')
    wMatrix = numpy.dot(numpy.linalg.pinv(RSSI),pos).T
    
    return wMatrix

def initSerial(portNum,baud,time):
    ##setting up the seriadl port
    ser = serial.Serial(portNum, baud, timeout=time)    #port COM-1 = 5 (Com6), baudrate 9600, no timeout

    if ser.isOpen():        #Good networking practice to close before opening
        ser.close()
    ser.open()
    if ser.isOpen():
        print("port opened")
    return ser

'''readAnchor
inputs: fileName (with .txt)
outputs: List of x and y and the number of nodes
Preconditions: numpy is imported
'''
def textReader (fileName):
    f=open(fileName, 'r')
    x=[]
    y=[]
    i=0
    for line in f :
        xCurr, yCurr = line[:-1].split(',')
        x.append(xCurr)
        y.append(yCurr)
        if (line == ''):
            break
        i=i+1
    x= numpy.asarray(x, dtype=numpy.float32)
    y= numpy.asarray(y, dtype=numpy.float32)
    return(x, y, i)

'''timestamp
    Calculates the timestamp for file names
'''
def timestamp():
    ts = time.time()
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y_%m_%d_%H_%M')
    return (st)

'''Draw grid
input ()
output ()
preconditions: import turtle
'''

def Draw_grid():
    turtle.setup(600, 600)      # set the window size to 800 by 600 pixels
    wn = turtle.Screen()
    grid = turtle.Turtle()

    grid.speed(0)

    width = 320
    height = 320
    increment = 20

    grid.up()
    grid.setpos(-width, height)

    for i in range (0, 2 * height//increment + 1):

        grid.down()
        grid.forward(2*width)
        grid.up()
        grid.setpos(-width, (height - i * increment))

    grid.up()
    grid.setpos(-width, height)
    grid.left(-90)
    for i in range (0, 2 * width//increment + 1):

        grid.down()
        grid.forward(2*height)
        grid.up()
        grid.setpos((width - i * increment) +1 , height)
        
#############################################################################
#Unit Test Code
#############################################################################
'''[xPos,yPos,num] = textReader ('D:\Research\Localization\Pos.txt')
[calX,calY,num] = textReader ('D:\Research\Localization\Cal.txt')
#wMatrix = generateWeightedMatrix('D:\Research\Localization\RF_Localization\Python\calibrationPos.csv','D:\Research\Localization\RF_Localization\Python\calibrationRSSI.csv',num)
#print wMatrix

cal = numpy.array([calX,calY]).T

[A,B,C] = generateMatrices(numpy.matrix(xPos), numpy.matrix(yPos), cal,-1.336,2.95)

#print A
#print numpy.linalg.pinv(A)
#print B
print C
print numpy.dot(numpy.linalg.pinv(A),(numpy.array([218,218,226]) - C - B).reshape(num,1))
#print numpy.dot(wMatrix,(numpy.array([218,218,226]).reshape(num,1)))

with open('test.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(A)):
        spamwriter.writerow(numpy.concatenate((A[i][:],A[i][:]),axis = 1))'''