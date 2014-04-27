#ifndef LOGGING_H
#include "logging.h"
#endif

namespace logging{

void log(ErrorLevel level, const char *fmt, ...){
    va_list ap;
    FILE *where = stderr;

    va_start(ap, fmt);
    switch(level){
        case ERROR:{
            fprintf(where, KRED "[ERROR] ");
            break;        
        }

        case WARNING:{
            fprintf(where, KRED "[WARNING] ");
            break;
        }

        case INFO:{
            fprintf(where, KBLU "[INFO] ");
            break;
        }

        case EVENT:{
            fprintf(where, KYEL "[EVENT] ");
            break;
        }
    }
    vfprintf(where, fmt, ap);
    va_end(ap);
}

}
