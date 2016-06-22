from __future__ import division
import numpy.linalg
import math
import turtle
                # make sure python is in the system path.
import serial   # On cmd type python -m pip install
import time
from struct import unpack
#############################################################################
###########################   Predefined Values   ###########################
NumberOFNodes = 3
Brodcast_dB = 1


#################   Converts signed byte to unsigned byte   ###############
def convert_to_signed(x):
    if x >= 127:
        x -= 255
    return x
#######################   Calculates x_m and y_m   ########################
def Linearized_Multi_Lateration():
    A= numpy.ones((NumberOFNodes,2)) #initialization
    RSSI= numpy.ones((NumberOFNodes,1)) #initialization
    logs= numpy.ones((NumberOFNodes,1)) #initialization
    B= numpy.ones((NumberOFNodes,1)) #initialization
    for i in range (0, NumberOFNodes):
        A[i] = numpy.array([-x[i]/(pow(x[i],2) + pow(y[i],2)), -y[i]/(pow(x[i],2) + pow(y[i],2))])
        logs[i]= (19.1314 * math.log(pow(x[i],2) + (pow(y[i],2)) ) ) - ((Brodcast_dB + 125.45)/0.5227)
        B[i]= (RSSI[i]-logs[i])/16.6173236

    #print ("B=", B)
    invA = numpy.linalg.pinv(A)
    #print ("inverse of A= ", invA)
    colomn_mat = numpy.matrix(invA) * numpy.matrix(B)
    print (colomn_mat)
    return colomn_mat

#########################################################################
#setting up screen
wn = turtle.Screen()
wn.title("RF Network Localization-Multilateral linearization")
wn.bgpic("grid.gif")
turtle.setworldcoordinates(-500, -500, 500, 500)
#setting up nodes
turtle.register_shape("Capture1.gif")   #Only supports .gif  !!!
turtle.register_shape("Capture2.gif")
turtle.register_shape("Capture3.gif")
turtle.register_shape("mobile.gif")
#Has to be changed
x=[0,0,200]
y=[0,200,0]

#Hard coding positions of the nodes
x1= 0
y1= 0
station1 = turtle.Turtle()
station1.shape("Capture1.gif")
station1.penup()
station1.setpos((x1,y1))       ###position (120, 80)
station1.write(station1.pos(), False, align= "center")

x2= 0
y2= 200
station2 = turtle.Turtle()
station2.shape("Capture2.gif")
station2.penup()
station2.setpos((x2,y2))     ###position (-80, -120)
station2.write(station2.pos(), False, align= "center")

x3= 200
y3= 0
station3 = turtle.Turtle()
station3.shape("Capture3.gif")
station3.penup()
station3.setpos((x3,y3))     ###position (-150, 160)
station3.write(station3.pos(), False, align= "center")

mobile_station = turtle.Turtle()
mobile_station.shape("mobile.gif")

#setting up the serial port
ser = serial.Serial(3, 115200, timeout=0)    #port 3, baudrate 115200, no timeout
if ser.isOpen():        #Good networking practice to close before opening
    ser.close()
ser.open()
ser.isOpen()
n=0
line=[]
data = "test"
#ser.write(bytes(data, 'UTF-8'))
ser.write(data.encode())
while(ser.isOpen()):
    time.sleep(.01)
    C= ser.read(1)#.decode()
    if(len(C)>0):
        C = C + ser.readline()#.decode()
        #line = C + ser.readline().decode()
        #line.append(C)
        #n=n+1
        #C.decode("utf-8")

        print (C)
        print ("1", C[1])
        print ("2", C[2])
        print ("3", C[3])

        mobile_station.speed(0)
        coordinate=Trilateration(x1,y1, x2,y2, x3, y3, r1, r2, r3)
        print(coordinate[0])# x
        mobile_station.penup()
        mobile_station.setpos((coordinate[0],coordinate[1]))
        #mobile_station.stamp()
        mobile_station.write(mobile_station.pos(), False, align= "center")
        #print(coordinate[0]*100, coordinate[1]*100)
        time.sleep(2)
        mobile_station.clear()
        i=i+1
        print (i)

wn.mainloop()
ser.close()


###############################
