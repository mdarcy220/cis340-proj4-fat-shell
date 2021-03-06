# CIS 340 Project 4

## Contents (in order)
 - Description
 - Compilation and running
 - Sample ouputs
 - High-level Design
 - Known bugs
 - Extra Features

## Description 
This project implements a command-line interface for a set of floppy-disk
utilies. It was completed by Mike D'Arcy as part of a school project with Joe
Komperda and Kyle Lunter, so please don't use it for anything important (many
much more mature and thoroughly-tested tools are available for FAT
manipulation). The shell handles many common features such as I/O redirection
and piping (see the "Extra Features" section).


## Compiling and running
To compile the shell, simply use the makefile at the command prompt in the
directory where the files are located by typing 'make' at the command prompt.
The executable is named `a.out`. NOTE: The image file name for the built-in
commands has been hard-coded as `imagefile.img` (in the current directory, so
`./imagefile.img`).  A sample image file is included with the source for your
convenience.


## Sample output

path import, path commands
path import appends the currents the current path to the path command
path shows the current path

```
flop:/home/student/midarcy/shared/proj4/src$ path import
flop:/home/student/midarcy/shared/proj4/src$ path
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games
flop:/home/student/midarcy/shared/proj4/src$ 
```

cd -  changes current directory

```
flop:/home/student/midarcy/shared/proj4/src$ cd ..
flop:/home/student/midarcy/shared/proj4$ cd src
flop:/home/student/midarcy/shared/proj4/src$ 
```

we included the current directory path into the flop prompt.

```
path +, traverse
```

appended the path to include the directory where out executables were stored ../src

```
flop:/home/student/midarcy/shared/proj4/test$ path import
flop:/home/student/midarcy/shared/proj4/test$ path + ../src
flop:/home/student/midarcy/shared/proj4/test$ traverse
/A.CLA                                                 
/ALOOP1.CLA                                            
/ALOOP1.JAV                                            
/ALOOP2.CLA                                            
/ALOOP2.JAV                                            
/ALOOP3.CLA                                            
/ALOOP3.JAV                                            
/B.CLA                                                 
/CONDIT.CLA                                            
/CONDIT.JAV                                            
/D.CLA                                                 
/DEMO.CLA                                              
/DEMO.JAV                                              
/DIR1                                             <DIR>
/DIR1/.                                           <DIR>
/DIR1/..                                          <DIR>
/DIR1/DIR2                                        <DIR>
/DIR1/DIR2/.                                      <DIR>
/DIR1/DIR2/..                                     <DIR>
/DIR1/DIR2/FILE3                                       
/DIR1/C.CLA                                            
/POLYTEST.CLA                                          
/POLYTEST.JAV                                          
/SOME.CLA                                              
```

 traverse -l (fixed to include starting sector, includes variable size buffer for different sized windows)

```
flop:/home/student/midarcy/shared/proj4/test$ traverse -l
        *****************************
        ** FILE ATTRIBUTE NOTATION **
        **                         **
        ** R ------ READ ONLY FILE **
        ** S ------ SYSTEM FILE    **
        ** H ------ HIDDEN FILE    **
        ** A ------ ARCHIVE FILE   **
        *****************************

-A---     10/27/2014 13:13:05               670     /A.CLA                                                         3
-A---     10/27/2014 13:13:05               763     /ALOOP1.CLA                                                    5
-A---     10/27/2014 13:13:05               333     /ALOOP1.JAV                                                    7
-A---     10/27/2014 13:13:05               786     /ALOOP2.CLA                                                    8
-A---     10/27/2014 13:13:05               404     /ALOOP2.JAV                                                   10
-A---     10/27/2014 13:13:05               786     /ALOOP3.CLA                                                   11
-A---     10/27/2014 13:13:05               408     /ALOOP3.JAV                                                   13
-A---     10/27/2014 13:13:05               358     /B.CLA                                                        14
-A---     10/27/2014 13:13:05              1067     /CONDIT.CLA                                                   15
-A---     10/27/2014 13:13:05               444     /CONDIT.JAV                                                   18
-A---     10/27/2014 13:13:05               508     /D.CLA                                                        19
-A---     10/27/2014 13:13:05               967     /DEMO.CLA                                                     20
-A---     10/27/2014 13:13:05               648     /DEMO.JAV                                                     22
-----     10/27/2014 13:13:05        <DIR>          /DIR1                                                         24
-----     10/27/2014 13:13:05        <DIR>          /DIR1/.                                                       24
-----     10/27/2014 13:13:05        <DIR>          /DIR1/..                                                       0
-----     10/27/2014 13:13:05        <DIR>          /DIR1/DIR2                                                    25
-----     10/27/2014 13:13:05        <DIR>          /DIR1/DIR2/.                                                  25
-----     10/27/2014 13:13:05        <DIR>          /DIR1/DIR2/..                                                 24
-A---     10/27/2014 13:13:05                11     /DIR1/DIR2/FILE3                                              26
-A---     10/27/2014 13:13:05               427     /DIR1/C.CLA                                                   27
-A---     10/27/2014 13:13:05               630     /POLYTEST.CLA                                                 28
-A---     10/27/2014 13:13:05              1063     /POLYTEST.JAV                                                 30
-A---     10/27/2014 13:13:05               226     /SOME.CLA                                                     33
```

showfat

```
flop:/home/student/midarcy/shared/proj4/test$ showfat
         0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
                FREE    4  fff    6  fff  fff    9  fff  fff    c  fff  fff  fff   10
   10   11  fff  fff  fff   15  fff   17  fff  fff  fff  fff  fff   1d  fff   1f   20
   20  fff  fff FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   30 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   40 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   50 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   60 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   70 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   80 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   90 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   a0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   b0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   c0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   d0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   e0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
   f0 FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE FREE
flop:/home/student/midarcy/shared/proj4/test$ 
```

showsector 

```
flop:/home/student/midarcy/shared/proj4/test$ showsector 1
       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F

   0  F0  FF  FF   0  40   0  FF  6F   0  FF  FF  FF   9  F0  FF  FF  
  10  CF   0  FF  FF  FF  FF   F   1  11  F0  FF  FF  FF  FF  15  F0  
  20  FF  17  F0  FF  FF  FF  FF  FF  FF  FF  1D  F0  FF  1F   0   2  
  30  FF  FF  FF   0   0   0   0   0   0   0   0   0   0   0   0   0  
  40   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  50   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  60   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  70   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  80   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  90   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  A0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  B0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  C0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  D0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  E0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  F0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 100   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 110   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 120   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 130   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 140   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 150   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 160   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 170   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 180   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 190   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1A0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1B0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1C0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1D0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1E0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
 1F0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
```


showfile

```
flop:/home/student/midarcy/shared/proj4/test$ showfile /dir1

   2e   20   20   20   20   20   20   20   20   20   20   10    0    0   a5   69
   5b   45   5b   45    0    0   a5   69   5b   45   18    0    0    0    0    0
   2e   2e   20   20   20   20   20   20   20   20   20   10    0    0   a5   69
   5b   45   5b   45    0    0   a5   69   5b   45    0    0    0    0    0    0
   41   64    0   69    0   72    0   32    0    0    0    f    0   aa   ff   ff
   ff   ff   ff   ff   ff   ff   ff   ff   ff   ff    0    0   ff   ff   ff   ff
   44   49   52   32   20   20   20   20   20   20   20   10    0    0   a5   69
   5b   45   5b   45    0    0   a5   69   5b   45   19    0    0    0    0    0
   41   63    0   2e    0   63    0   6c    0   61    0    f    0   ff    0    0
   ff   ff   ff   ff   ff   ff   ff   ff   ff   ff    0    0   ff   ff   ff   ff
   43   20   20   20   20   20   20   20   43   4c   41   20    0    0   a5   69
   5b   45   5b   45    0    0   a5   69   5b   45   1b    0   ab    1    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
```

structure

```
flop:/home/student/midarcy/shared/proj4/test$ structure
                number of FAT:                     2
                number of sectors used by FAT:     9
                number of sectors per cluster:     1
                number of of ROOT entries:       224
                number of bytes per sector:      512
                ---Sector #---     ---Sector Types---
                      0                  BOOT        
                   01 -- 09              FAT1        
                   10 -- 18              FAT2        
                   19 -- 32              ROOT DIRECTORY
```


normal commands with an arbitrary number of pipes:

```
flop:~/shared/proj4/src$ ls -l|cat|cat| cat |grep exec
-rw-rw-r--  1 midarcy  users  7972 Nov 30 21:56 exec_command.c
-rw-rw-r--  1 midarcy  users   172 Nov 11 09:42 exec_command.h
-rw-rw-r--  1 jokomper users  7512 Dec  1 11:07 exec_command.o
```



## High-level Design
The general design for this project was as follows:

Get user input --> Parser --> Executer

The user-prompting code (in `flopshell.c` and `shellinput.c`) simply prompts
the user and retreives a line of text from the console and passes it to the
parser. The parser (in `lex.c` and `parse.c`) converts the given text to
a FlopCommand struct that holds all relevent information about the command,
including the argv array, input file name, and output file name. It also holds
a pointer to another FlopCommand that represents the command being piped to (if
there was a pipe). After being parsed, the command is executed (code in
`exec_command.c`). The execution code first checks if the command is an
internal comamnd, and if so calls the appropriate function. Otherwise, it
searches the path variable for the executable file, forks, performs I/O
redirection, and waits for the child to terminate before prompting again.


## Known bugs

1. Command editing doesn't work properly when command wraps to a new line.  The
   buffer is updated correctly, but the display is incorrect. To reproduce this
   bug, simply type a very long command that wraps to a new line, then
   repeatedly press `Backspace` or the `Left Arrow` key. The cursor will not
   wrap back to the original line.

2. Commands will fail to execute if there is a file in the PATH that has the
   correct name, but does not have execute permission. For example, if there is
   a file in the `scratch` directory named `ls`, with permissions `-rw-r--r--`,
   and PATH is set to `scratch:/bin`, running the command `ls` will produce an
   error, even though a working ls executable is in /bin.

3. Single quotes don't work. Only double quotes are functional.



## Extra Features

1. A mixture of pipes and redirection can be used, like so:
```
flop:~/shared/proj4/tmp$ cat<input.txt|wc>output.txt
flop:~/shared/proj4/tmp$ cat output.txt
      2      19      95
```

2. Arrow keys are functional on the command line, and commands can be edited.

3. Quotes work:
```
flop:~/shared/proj4/tmp$ touch "name with spaces.txt"
flop:~/shared/proj4/tmp$ ls -l
total 8
-rw-rw-r-- 1 midarcy users 95 Dec  1 19:10 input.txt
-rw-rw-r-- 1 midarcy users  0 Dec  1 19:27 name with spaces.txt
-rw-rw-r-- 1 midarcy users 24 Dec  1 19:24 output.txt
```

4. Backslashes can also be used to escape spaces and quotes:
```
flop:~/shared/proj4/tmp$ touch \"name\ with\ quotes\ and\ spaces\".txt
flop:~/shared/proj4/tmp$ ls -l
total 8
-rw-rw-r-- 1 midarcy users 95 Dec  1 19:10 input.txt
-rw-rw-r-- 1 midarcy users  0 Dec  1 19:30 "name with quotes and spaces".txt
-rw-rw-r-- 1 midarcy users  0 Dec  1 19:27 name with spaces.txt
-rw-rw-r-- 1 midarcy users 24 Dec  1 19:24 output.txt
```

5. Pipes (|) and redirectors (<, >) don't need to be separated from commands 
with spaces:
```
flop:~/shared/proj4/src$ ps aux|grep a.out
arndt     5062  0.0  0.0   4192     0 ?        S    Nov09   0:00 ./a.out
midarcy  31224  0.0  0.0   4344   612 pts/31   S+   19:41   0:00 ./a.out
midarcy  32096  0.0  0.0  11744   924 pts/31   S+   20:13   0:00 grep a.out
```


6. There is a "path import" command to automatically copy the PATH environment 
variable to the shell's path variable:
```
flop:~/shared/proj4/src$ path

flop:~/shared/proj4/src$ path import
flop:~/shared/proj4/src$ path
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/home/student/midarcy/usr/bin
```


# License

Copyright (C) 2017 Mike D'Arcy

The program is released under the GNU GPL. See the `COPYING` file for details.

Please note that while the code is GPL-licensed, **you must still comply with
applicable law and with the policies at your workplace or university** when
using it. In particular, you may not use any part of this code in a school
project if your university or instructor has an anti-plagiarism policy. In
fact, if you are developing such a school project, I highly recommend that you
do not even look at this source code, to avoid plagiarizing accidentally.
I only released the code because, to the best of my knowledge, the course
I took has now replaced this assignment with a different one.

