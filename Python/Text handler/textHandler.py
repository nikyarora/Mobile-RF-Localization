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
