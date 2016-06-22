import turtle
                # make sure python is in the system path.
import serial   # On cmd type python -m pip install
import math
import time
from struct import unpack
#################   Converts signed byte to unsigned byte   ###############
def convert_to_signed(x):
    if x >= 127:
        x -= 255
    return x

def calculateRadius(levelInDb, freqInMHz):

    #exp = (27.55 - (20 * math.log10(freqInMHz)) + abs(levelInDb+19)) / 20.0
    exp = (abs(levelInDb)/20) - 1.26
    #exp = (abs(levelInDb)/20) - 1.26
    return pow(10.0, exp)

def RSSI_to_dB(RSSI):
    dB= 0.5227*RSSI - 125.43
    return dB


#https://confluence.slac.stanford.edu/display/IEPM/TULIP+Algorithm+Alternative+Trilateration+Method
def Trilateration(x1,y1, x2,y2, x3, y3, r1, r2, r3):


    x= ( ( ( (pow(r1,2)-pow(r2,2)) + (pow(x2,2)-pow(x1,2)) + (pow(y2,2)-pow(y1,2)) ) * (2*y3-2*y2) - ( (pow(r2,2)-pow(r3,2)) + (pow(x3,2)-pow(x2,2)) + (pow(y3,2)-pow(y2,2)))*(2*y2-2*y1) )/( (2*x2-2*x3)*(2*y2-2*y1)-(2*x1-2*x2)*(2*y3-2*y2 )))
    #x= ( ( (pow(r1,2)-pow(r2,2)) + (pow(x2,2)-pow(x1,2)) + (pow(y2,2)-pow(y1,2)) ) * (2*y3-2*y2) - ( (pow(r2,2)-pow(r3,2)) + (pow(x3,2)-pow(x2,2)) + (pow(y3,2)-pow(y2,2)))*(2*y2-2*y1) )/( (2*x2-2*x3)*(2*y2-2*y1)-(2*x1-2*x2)*(2*y3-2*y2 ))
    y=( (pow(r1,2)-pow(r2,2)) + (pow(x2,2)-pow(x1,2)) + (pow(y2,2)-pow(y1,2)) + x*(2*x1-2*x2)) / (2*y2-2*y1)
    return (x,y)


#setting up the serial port
ser = serial.Serial(3, 115200, timeout=0)    #port 0, baudrate 19200, no timeout
i=0
#setting up screen
wn = turtle.Screen()
wn.title("Localization simulation software")
wn.bgpic("grid.gif")
turtle.setworldcoordinates(-500, -500, 500, 500)
#setting up nodes
turtle.register_shape("Capture1.gif")   #Only supports .gif  !!!
turtle.register_shape("Capture2.gif")
turtle.register_shape("Capture3.gif")
turtle.register_shape("mobile.gif")


#x1= int(input ("x1="))
#y1= int(input ("y1="))
x1= 0
y1= 0

station1 = turtle.Turtle()
station1.shape("Capture1.gif")
station1.penup()
station1.setpos((x1,y1))       ###position (120, 80)
station1.write(station1.pos(), False, align= "center")


#x2= int(input ("x2="))
#y2= int(input ("y2="))
x2= 0
y2= 200

station2 = turtle.Turtle()
station2.shape("Capture2.gif")
station2.penup()
station2.setpos((x2,y2))     ###position (-80, -120)
station2.write(station2.pos(), False, align= "center")

#x3= int(input ("x3="))
#y3= int(input ("y3="))
x3= 200
y3= 0

station3 = turtle.Turtle()
station3.shape("Capture3.gif")
station3.penup()
station3.setpos((x3,y3))     ###position (-150, 160)
station3.write(station3.pos(), False, align= "center")




if ser.isOpen():        #Good networking practice to close before opening
    ser.close()
ser.open()
ser.isOpen()
n=0
line=[]
data = "test"
freqInMHz= 434
#ser.write(bytes(data, 'UTF-8'))
ser.write(data.encode())


mobile_station = turtle.Turtle()
mobile_station.shape("mobile.gif")
#mobile_station.shape("circle")
#mobile_station.shapesize(0.5)
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
        #r1_dB= RSSI_to_dB(C[1])     # radius in dB
        #r2_dB= RSSI_to_dB(C[3])
        #r3_dB= RSSI_to_dB(C[5])

        r1_dB= convert_to_signed(C[1])     # radius in dB
        #r1_dB = r1_dB*31/32
        r1_dB = r1_dB*31/37
        r2_dB= convert_to_signed(C[2])
        #r2_dB = r2_dB*31/
        r2_dB = r2_dB*31/43
        r3_dB= convert_to_signed(C[3])
        #if(r3_dB<20):
        r3_dB = r3_dB *31/24
        #r3_dB = r3_dB*31/
        print("r1 in dB", r1_dB)
        print("r2 in dB", r2_dB)
        print("r3 in dB", r3_dB)

        r1= calculateRadius(r1_dB, freqInMHz) *100
        r2= calculateRadius(r2_dB, freqInMHz)*100
        r3= calculateRadius(r3_dB, freqInMHz)*100

        print("r1", r1)
        print("r2", r2)
        print("r3", r3)

#readout = ser.read(13)        # read up to ten bytes (timeout)
#r1=readout[0:1]
#r2=readout[1:2]
#r3=readout[1:3]
##
        ##station1.pendown()
##        station1.circle(r1)
##        station2.pendown()
##        station2.circle(r2)
##        station3.pendown()
##        station3.circle(r3)


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
