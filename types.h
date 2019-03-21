#ifndef TYPE_H
#define TYPE_H
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

#define MSGSIZE          8
#define MAX_MESSAGES     50

struct Message {
    float data;
    int sender;
};
#endif
