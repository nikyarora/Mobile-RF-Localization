'''textReader
inputs: fileName (with .txt)
outputs: List of x and y and the number of nodes
Preconditions: numpy is imported
'''
import numpy
def textReader (fileName):
    f=open(fileName, 'r')
    x=[]
    y=[]
    i=0
    for line in f :
        xCurr, yCurrAndNewLine = line.split(',')
        yCurr, newLine= yCurrAndNewLine.split('\n')
        x.append(xCurr)
        y.append(yCurr)
        if (line == ''):
            break
        i=i+1
    x= numpy.asarray(x)
    y= numpy.asarray(y)
    return(x, y, i)
    


fileName = 'anchorPos.txt'
textReader(fileName)
x, y, numberOfAnchors = textReader(fileName)

print (x)
print (y)
print (numberOfAnchors)
