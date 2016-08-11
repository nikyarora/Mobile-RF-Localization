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

port = '/dev/cu.usbmodem1411'
#port = 2
Brodcast_dB = 1
baud= 9600
timeout= 0
exitBool= True
xi = 0.33
yi = 1.6

#Initialize variables
x_anchor,y_anchor,numAnchor = ssLocalizationLib.textReader('Pos.txt') #import anchor positions #import anchor positions
calX,calY,nums = ssLocalizationLib.textReader('cal.txt')
cal = numpy.array([calX,calY]).T
ser = ssLocalizationLib.initSerial(port, 9600,0) #set up serial port
RSSI = numpy.empty((0,3),int)
pos = numpy.zeros((1,2))
pos[0][0] = xi
pos[0][1] = yi
line=[]
data = "test"


#ser.write(bytes(data, 'UTF-8'))
ser.write(data.encode())
stime = time.time()
while(ser.isOpen()&((time.time()-stime)<30)):
    C = ser.read()

    if len(C) > 0:
    	tempString = C.split(',')
    	print tempString
    	if len(tempString) == 3:
    		pos = numpy.append(pos,[tempString[0],tempString[1]],axis = 0)

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
