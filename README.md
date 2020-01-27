![Alt text](/Screenshots/NES_Space_Checker_QT_windows.png?raw=true "NES Screen Checker Windows screenshot")
![Alt text](/Screenshots/NES_Space_Checker_QT_KDE.png?raw=true "NES Screen Checker Debian screenshot")

# Nes-Space-Checker-QT

 This small tool is intended to help you with controlling free ROM space 
in NES development, if you don't provided with this information by your 
dev tools. The tool loads a NES file and generates a simple visual graph 
that shows how much memory in the PRG and CHR banks is used up. 

For PRG ROMs it first finds out a value that is considered 'empty', 
through counting sequences of repeating values that are at least 8 bytes 
long. If there is some free space filled with the same byte, this works. 
This won't work if the free space is filled with random data, or if 
there is not much of free space. For CHR ROMs it consider non-zero bytes 
as empty, this does not provide accurace information, but gives the 
idea. 

You can open a NES file by Open item of the main menu, or through the 
command line, or with drag and drop of the file to the program window. 

You can enable watching for changes in last opened file, the program 
will update the graph when the file is changed. 

You can save the graph as a PNG file. 

You also can to use this tool to estimate how much of empty space is
in some other file types, just by selecting the file type in file open
dialog to 'binary' or 'any files'. In this case contents of a file
will be just treated like a set of 16K data chunks.


Versions:

v1.2            23.01.20

v1.2.5          27.01.20


mailto:shiru@mail.ru
http://shiru.untergrund.net/

Qt conversion: mr287cc
mailto: wecleman@gmail.com
