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

#Initialize variables
x,y,numAnchor = ssLocalizationLib.textReader('Pos.txt') #import anchor positions
ser = ssLocalizationLib.initSerial(port, 9600,0) #set up serial port
R_dB = numpy.empty((0,3),float)
dis = numpy.empty((0,3),float)
RSSI = numpy.empty((0,3),int)
posM = numpy.empty((0,2),float)
B_dB = 1
exitBool = True
print ("Press enter 'q' when calibration is complete\n")

while(exitBool&ser.isOpen()):
    w = raw_input("Enter the x and y components of the mobile node with a comma:\n")
    #check if user quit
    if w.find('q')!=-1:
        exitBool = False
    else:
        #get user input x and y
        pos = numpy.zeros((1,2),float)
        xm,ym = w.split(',')
        xm = float(xm)
        ym = float(ym)
        pos[0][0] = xm
        pos[0][1] = ym

        #calculate distance of mobile node from the rest of the anchors
        d = numpy.zeros((1,numAnchor),float)
        rdb = numpy.zeros((1,numAnchor),float)
        for i in range(0,numAnchor):
            d[0][i] = numpy.sqrt(pow(x[i]-xm,2)+pow(y[i]-ym,2))
            rdb[0][i] = B_dB-20*math.log(numpy.sqrt(pow(x[i]-xm,2)+pow(y[i]-ym,2)),10)-25.2
        
        #gather data for 60 seconds
        stime = time.time()
        while((time.time()-stime)<10):
            C = ser.readline()

            if len(str) > 0:
                tempString = C.split(',')
                #collect RSSI and place into matrix
                tempRSSI = numpy.zeros((1,numAnchor),int)
                for j in range (0, numAnchor):
                    if tempString[j] == 0:
                        tempRSSI[0][j]=255
                    else:
                        tempRSSI[0][j]= int(tempString[j])

                dis = numpy.append(dis,d,axis = 0)
                R_dB = numpy.append(R_dB,rdb,axis = 0)
                RSSI = numpy.append(RSSI,tempRSSI,axis = 0)
                posM = numpy.append(posM,pos,axis = 0)

ser.close()

#linear regress values
slope = numpy.zeros((numAnchor,1))
intercept = numpy.zeros((numAnchor,1))
for i in range(0,numAnchor):
    slope[i], intercept[i], r_value, p_value, std_err = stats.linregress(RSSI[:][i],R_dB[:][i])
numpy.savetxt('cal.txt',numpy.concatenate((slope,intercept),axis = 1),delimiter=',') # save in calibration file form

#save data as csv
with open('calibrationRSSI.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(RSSI)):
        spamwriter.writerow(RSSI[i][:])
with open('calibrationRdB.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(R_dB)):
        spamwriter.writerow(R_dB[i][:])
with open('calibrationDis.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(dis)):
        spamwriter.writerow(dis[i][:])
with open('calibrationPos.csv','wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',quotechar='|',quoting=csv.QUOTE_MINIMAL)
    for i in range(0,len(posM)):
        spamwriter.writerow(posM[i][:])
