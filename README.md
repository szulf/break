### basic understanding
program interprets break.json  
based on that runs the necessary compilation commands  
and creates a '.break-cache' file that holds info about last modified change to a file  
defaults to using a cpp compiler unless file has a '.c' ending

### cli commands
` break init project-name `  
creates a template break.json file

` break build `  
compiles all files

` break run `  
runs the program

` break build run `  
compiles all files and runs the program

### breakfile syntax
json  
switched because i think it is better to use a standardized syntax then create my own  
dont know how much better would it be to use a normal language instead of just storing data with some format  
since I dont know how much logic is involved in real building scripts

### todo
- add cli commands to change the break.json file  
    so it works both as a cli tool and with a config file
- it should also be able to export compile commands
- add vcpkg somehow to manage dependencies
- add just specifying a source directory  
    so break would just use all the files inside it  
    same for a header directory
- think of something for include directories  
    maybe automatically include all directories from 'header_files' variable and from dependencies
- let the user make custom macros(use of -D compiler flag)
