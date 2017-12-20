#ifndef _CR_DUMP_H_FL_
#define _CR_DUMP_H_FL_

#include "Config.h"

#include <stddef.h>

#ifdef AlvaPublic
#include "DebugTools_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _DUMP_BMP_TYPE_{
	DUMP_FLOAT,
	DUMP_UCHAR,
}DUMPDATATYPE;

typedef enum _DUMP_MODE_ {
    DUMP_APPEND,//������׷�ӵ��ļ�δβ
    DUMP_CLEAR, //����ļ����ڣ�����ļ�
}DUMP_MODE;

#if DUMP_SWITCH
#define DUMP_NAME_LENGTH 2048
#define DUMP_NAME(name) char name[DUMP_NAME_LENGTH];
#define DUMP_FORMAT_NAME(name, ...) sprintf_fl(name, DUMP_NAME_LENGTH, __VA_ARGS__)
#define DUMP_INIT(size) dump_init_temp(size);
#define DUMP(filename, buf, size) dump_temp(filename, buf, size);

#define DUMP_BMP(filename, buf, width, height, channels, DATATYPE) dump_bmp(filename, buf, width, height, channels, DATATYPE);

#define Start_FileDump(filename, mode) start_filedump_temp(filename, mode);
#define DumpBuffer2File(file, buf, size) dumpbuffer2file_temp(file, buf, size);
#define LOG2File(file, ...) log2file_temp(file, __VA_ARGS__);
#define End_FileDump(file) end_filedump_temp(file);
#else
#define DUMP_NAME_LENGTH 0
#define DUMP_NAME(name) ;
#define DUMP_FORMAT_NAME(buf, ...)
#define DUMP_INIT(size) ;
#define DUMP(filename, buf, size) ;

#define DUMP_BMP(filename, buf, width, height, channels, DATATYPE);

#define Start_FileDump(filename, mode) NULL;
#define DumpBuffer2File(file, buf, size) ;
#define LOG2File(file, ...) ;
#define End_FileDump(file) ;
#endif

extern char* dumpbuffer;

int dump_init_temp(size_t buffersize);
int dump_temp(char* filename, void* buf, size_t size);
int dump_bmp(char* filename, void* buf, int width, int height, int channels, DUMPDATATYPE DATATYPE);
void* start_filedump_temp(char* filename, DUMP_MODE mode);
void dumpbuffer2file_temp(void* file, void* buf, size_t size);
void log2file_temp(void* file, char* format, ...);
int end_filedump_temp(void* file);

#ifdef __cplusplus
}
#endif

#endif
