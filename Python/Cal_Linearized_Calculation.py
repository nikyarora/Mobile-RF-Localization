from __future__ import division
import numpy.linalg
import math
import turtle
                # make sure python is in the system path.
import serial   # On cmd type python -m pip install
import time
from struct import unpack
from scipy import stats
import csv
import ssLocalizationLib

#port = '/dev/tty.usbmodemfd121'
port = 2
Brodcast_dB = 1
baud= 9600
timeout= 0
exitBool= True
xi = 0.33
yi = 1.6

#Initialize variables
x_anchor,y_anchor,numAnchor = ssLocalizationLib.textReader('D:\Research\Localization\Pos.txt') #import anchor positions #import anchor positions
calX,calY,nums = ssLocalizationLib.textReader('D:\Research\Localization\cal.txt')
cal = numpy.array([calX,calY]).T
ser = ssLocalizationLib.initSerial(port, 9600,0) #set up serial port
RSSI = numpy.empty((0,3),int)
pos = numpy.zeros((1,2))
pos[0][0] = xi
pos[0][1] = yi
line=[]
data = "test"

#########################################################################
#setting up screen
'''
wn = turtle.Screen()
wn.title("RF Network Localization-Multilateral linearization")
wn.bgpic('grid.gif')
turtle.setworldcoordinates(-500, -500, 500, 500)

#setting up nodes
turtle.register_shape("ali.gif")   #Only supports .gif  !!!
turtle.register_shape("ali.gif")

for i in range (0, len(x_anchor)):
    station1 = turtle.Turtle()
    station1.shape("ali.gif")
    station1.penup()

    station1.setpos((int(x_anchor[i]*100),int(y_anchor[i]*100)))
    station1.write(station1.pos(), False, align= "center")

mobile_station = turtle.Turtle()
mobile_station.shape("ali.gif")'''


#ser.write(bytes(data, 'UTF-8'))
ser.write(data.encode())
stime = time.time()
while(ser.isOpen()&((time.time()-stime)<10)):
    time.sleep(.05)
    C= ser.read(numAnchor + 1)#.decode()
    time.sleep(.05)
    if(len(C)>0):
        #print ("buffer length")
        #print(len(C))
        C = C + ser.readline()

        #print ("1", ord(C[1]))
        #print ("2", ord(C[2]))
        #print ("3", ord(C[3]))

        j=0
        tempRSSI = numpy.zeros((1,numAnchor),int)
        #print (len(RSSI))
        for j in range (0, numAnchor):
            if ord(C[j+1]) == 0:
                (C[j+1])=chr(255)
            tempRSSI[0][j-1]=ord((C[j+1]))
        #mobile_station.speed(0)
        RSSI = numpy.append(RSSI,tempRSSI,axis = 0)

        #function to interpret anchor locations and calibrations into matrices
        [A, B, C] = ssLocalizationLib.generateMatrices(numpy.matrix(x_anchor), numpy.matrix(y_anchor),cal,xi,yi)
        #print numpy.linalg.pinv(A)
        #print B
        #print C
        #print tempRSSI
        t_matrix = numpy.dot(numpy.linalg.pinv(A),(tempRSSI - C - B * Brodcast_dB).reshape(nums,1))
        pos = numpy.append(pos,t_matrix.T,axis = 0)
        print t_matrix

        '''
        print("x_m is", t_matrix[0])
        print("y_m is", t_matrix[1])
        
        mobile_station.penup()
        mobile_station.setpos(t_matrix[0], t_matrix[1])
        #mobile_station.stamp()
        mobile_station.write(mobile_station.pos(), False, align= "center")
        #print(coordinate[0]*100, coordinate[1]*100)
        time.sleep(2)
        mobile_station.clear()'''
        i=0
        i=i+1
        #print (i)

#wn.mainloop()
ser.close()

timestamp= ssLocalizationLib.timestamp()
logFileName = 'log'.join(timestamp).join('.csv')
#save data as csv
#with open(logFileName,'wb') as csvfile:
with open('testat5.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(pos)):
        spamwriter.writerow(pos[i][:])
        
with open('RSSIat5.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(RSSI)):
        spamwriter.writerow(RSSI[i][:])