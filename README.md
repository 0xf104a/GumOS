# Credits
Bootlogo by <a href="https://www.flaticon.com/authors/roundicons">RoundIcons</a>

# Notice 
GUI is not yet ready

# microramfs
## Brief
microramfs is builtin virtual filesystem that store resources and some of sensors states. It has fixed maximum size and in case of exceeding this space you will get error 8 (see https://github.com/Andrewerr/GumOS/blob/master/src/kernel/error.h ).
## Initialization
microramfs initializes from static array placed in ```src/kernel/microramfs/init.h```. It could be generated from any folder with <a href="https://github.com/Andrewerr/microramfs-creator"> this </a> tool.
## Functions
Any function return ```int``` which is error code(0 means no error).<br>
Each function accepts filesystem instance(```microramfs *fs```) as first parameter.<br>


| Function                                   | Description                        |
|--------------------------------------------|------------------------------------|
|create_file(microramfs *fs,char *path)      | Creates file with given path(path  |
|                                            | should include file name).         |
|write_file(microramfs *fs,char *path,uint8_t *data, uint64_t sz)| Write ```data``` to file(file will be truncated).```sz``` is size of data|
|read_file(microramfs *fs,char *path, uint8_t \**data, uint64_t *sz)|Read data from file. <br> **IMPORTANT**:```*data``` should be NULL before calling this function.<br>Size of read data will be written to ```sz```|
|rm_file(microramfs *fs,char *path)          | Remove file by given path.         |
|ls_dir(microramfs *fs, char *path, array_t \**array)| List dir and put all object instances to ```array```|
|create_dir(microramfs *fs,char *path, char *name)| Create directory with ```name``` in directory ```path```|

## Not yet implemented
Following functions are not yet implemented(and even not added to headers): ```rm_dir```, ```append_file```
