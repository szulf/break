### basic understanding
program interprets breakfile
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
#### concepts
- lists - defined by ` [ item1, item2 ] `  
    used to define lists of files
- variables - defined by ` var variable_name = value `  
    can be used for pretty much anything
- program variables - defined by ` variable_name = value `  
    used by the program, and not by the user  
    list of program variables [here](#program%20variables)
- comments - defined by ` # `

#### actual syntax

#### program variables
- project_name
- source_directory  
    if left empty defaults to root directory
- source_files  
    looks for files in source_directory
- header_directory  
    if left empty defaults to root directory
- header_files  
    looks for files in header_directory
- c_compiler  
    if left empty break will use the compiler specified in the 'CC' environmental variable
    if that is also not set break will error
- cpp_compiler  
    if left empty break will use the compiler specified in the 'CXX' environmental variable
    if that is also not set break will error
- c_flags
- cpp_flags

### todo
- add cli commands to change the breakfile  
    so it works both as a cli tool and with a config file
- it should also be able to export compile commands
- add some way to parse command line inputs so something like this works  
    ` break build --debug ` or ` break build --release `
    and let the user somehow specify what these mean
