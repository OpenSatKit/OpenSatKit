#ifndef __IOKIT_H__
#define __IOKIT_H__

//#ifdef __cplusplus
//namespace Kit {
//#endif

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef _ENABLE_SOCKETS_
   #include <sys/socket.h>
   //#include <sys/un.h>
   #include <netinet/in.h>
   #include <netdb.h>
   #include <fcntl.h>
#endif

FILE *FileOpen(const char *Path, const char *File, const char *CtrlCode);
void ByteSwapDouble(double *A);

#ifdef _ENABLE_SOCKETS_
int InitSocketServer(int Port, int AllowBlocking);
int InitSocketClient(const char *hostname, int Port, int AllowBlocking);
#endif

//#ifdef __cplusplus
//}
//#endif

#endif /* __IOKIT_H__ */
