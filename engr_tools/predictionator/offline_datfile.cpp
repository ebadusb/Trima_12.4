#include <semaphore.h>
#include "datfile.cpp"

#define S_IRUSR 0
#define S_IRGRP 0
#define S_IROTH 0
#define open(name,_mode,mode) ((int)fopen(name,"r"))
#define write(stream,buffer,size) fwrite(buffer,size,size,(struct _iobuf *)stream)
#define close(x) fclose((struct _iobuf *)x)
#include "filehelp.cpp"