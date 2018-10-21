#ifndef CJRINGBUFFER
#define CJRINGBUFFER

#define RING_LENGTH (102400)
#define BYTE unsigned char

//#define __DEBUG
class CJRing
{
public:
    CJRing(void){}
    
public:
    ~CJRing(void){}


public:
    int   head;
    int   tail;
    BYTE  buf[RING_LENGTH];

public:
    void  Clear()
    {
        head = 0;
        tail = 0;
    }

    void PushBack(BYTE cb)
    {
        #ifdef __DEBUG
            printf("head--push:%x\n", head);
            printf("tail--push:%x\n", tail);
            printf("Push --ring-head-tail:%d - %d\n", &head, &tail);
        #endif
        tail %= RING_LENGTH;
        buf[tail++] = cb;    
    }
    
    bool Get(BYTE &cb)
    {
        #ifdef __DEBUG
            printf("head-get:%x\n", head);
            printf("tail-get:%x\n", tail);
            printf("Get --ring-head-tail:%d - %d\n", &head, &tail);
        #endif
        head %= RING_LENGTH;
        tail %= RING_LENGTH;

        if (tail == head)
        {
            return false;
        }

        cb = buf[head++];
        return true;
    }    
};

#endif