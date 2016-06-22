def fileWriter ():
    f=open('anchorPos.txt', 'w')
    i=0
    print ("Press enter q when the last node is inputed")
    while(1):
        #x,y = input("Enter the x and y components of the location seperated with a space").split()
        w=  input("Enter the x and y components of the location seperated with a space")
        if (w ==''):
            break
        else:
            x, y = w.split()
            f.write(x+','+y+'\n')
    f.close()

fileWriter()
f1=open('anchorPos.txt', 'r')
x=[]
y=[]
numberOfNodes= 0
for line in f1 :
    xCurr, yCurrAndNewLine = line.split(',')
    yCurr, newLine= yCurrAndNewLine.split('\n')
    x.append(xCurr)
    y.append(yCurr)
    if (line == ''):
        break
    numberOfNodes = numberOfNodes+1
f1.close()
print("Number of nodes: ", numberOfNodes)
print(x)
print(y)
