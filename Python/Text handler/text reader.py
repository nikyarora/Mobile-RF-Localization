f=open('anchorPos.txt', 'r')
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

    
print (x)
print (y)

