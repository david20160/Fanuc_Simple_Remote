# Fanuc Simple Remote



## What is?

​	It is a very simple program to send a numeric control program (NC) to a Fanuc controlled machine over serial port (RS232) in remote mode. Also may be used to send a program normally to Fanuc machine.



## Why?

​	Because exists programs to that function, near all are commercial and need to pay for this simple task. Why not make an open source simple program?



## Can be more complete?

​	Of course that can be. But It is need time and interest. In time may be going to make more complete.



## Windows, Linux, ...

​	It is make cross-platform. Uses two cross-platform libraries as base:

RS-232 Library from https://www.teuniz.net/RS-232/ (Teunis van Beelen contact: teuniz@protonmail.com)

and

Iup Framework from https://www.tecgraf.puc-rio.br/iup/ (Tecgraf Institute of Technical-Scientific Software Development of PUC-Rio contact: iup@tecgraf.puc-rio.br)



## How build

Clone this repository or download the `.ZIP` version. It will create the basic structure.

~~~bash
git clone https://github.com/david20160/Fanuc_Simple_Remote.git
~~~

Need to add IUP Framework. Download the need version (Windows or Linux) to the cloned repository directory create a directory called `IUP` and unzip / tar inside this `IUP` directory.

Run `./comp.linux` (in Linux) or `comp.bat` (in Windows). In Windows and if You does not have a lot need of a complete compile system to use, choose the great alternative of TCC (https://bellard.org/tcc/)

After run this, in the `bin` directory You will get the program. In the Windows case, must to be companied with `iup.dll` copied from `IUP` directory.

