# rshell

rshell is a simple Unix command shell with the ability to:

1. run all commands found in /bin   example: pwd

2. use semi-colon (;)               example: pwd; ls

3. use or (||)                      example: pwd || ls

4. use and (&&)                     example: pwd && ls

5. use arguments without spaces     example: ls-a-l

###exit command
The exit command (exit) allows user to quit rshell

###installation
to download and use rshell:
```
$ git clone  http://github.com/yliu075/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

###known bugs

1. cannot use || before &&          example: pwd || ls && pwd

2. cannot use || before ||          example: pwd || ls || pwd

3. cannot use gethostname() and getlogin() when running on Windows

4. sometimes requires two exits to quit FIXED
 


#ls command

My ls command supports:

1. -a flag
2. -R flag
3. -l flag
4. optional files and folders
5. using multiple flags at once such as -al
 
###installation
to download and use ls:
```
$ git clone  http://github.com/yliu075/rshell.git
$ cd rshell
$ git checkout hw1
$ make
$ bin/ls
```
###known bugs

1. using the -R flag with -a in any instance creates recursive bombs if user has root access, recursion is limited to 10 for user without root access
2. error with optional files and -l flag