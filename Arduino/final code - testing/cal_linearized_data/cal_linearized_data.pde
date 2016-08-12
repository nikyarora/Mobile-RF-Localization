import processing.serial.*;

Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port
PrintWriter output;

void setup()
{
  // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  //Str ing portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, "/dev/cu.usbmodem1421", 9600);
  output = createWriter( "CalLinearized4NodesPos1GaussNewton.txt" ); 
}

void draw()
{
  while (myPort.available() > 0 ) 
  {
    val = myPort.readStringUntil('\n');
    if ( val != null )
    {
      output.println( val );
    }
  }
  println(val);
}

void keyPressed() {
    output.flush();  // Writes the remaining data to the file
    output.close();  // Finishes the file
    exit();  // Stops the program
}