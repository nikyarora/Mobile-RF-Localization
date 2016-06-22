import serial   # On cmd type python -m pip install
#setting up the serial port
ser = serial.Serial(3, 115200, timeout=0)    #port 3, baudrate 115200, no timeout
if ser.isOpen():        #good practice to close before opening
    ser.close()
ser.open()
ser.isOpen()

data = "test"
#ser.write(bytes(data, 'UTF-8'))
ser.write(data.encode())
    
while(ser.isOpen()):
    #line = ser.readline().decode()
    line = ser.read(1).decode()
    #readout = ser.read(1)        # read up to ten bytes (timeout)
    if(len(line)>0):
        #print (readout, end="")
        print(line[0])
#        print(line[2:4])
#        print(line[3:4])
#        print (line, end = "")
#    if (line == 'q' ):
#        break
ser.close()
