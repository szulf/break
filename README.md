### basic understanding
program interprets break.json  
based on that runs the necessary compilation commands  
and creates a '.break-cache' file that holds info about last modified change to a file

### cli commands
` break init project-name `  
creates a template breakfile

` break build `  
compiles all files

` break run `  
runs the program

` break build run `  
compiles all files and runs the program

### breakfile syntax
json  
switched because i think it is better to use a standardized syntax then create my own

### todo
- add cli commands to change the breakfile  
    so it works both as a cli tool and with a config file
- it should also be able to export compile commands
- add vcpkg somehow to manage dependencies
