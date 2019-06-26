#ifndef _sbn_pack_h_
#define _sbn_pack_h_

#include <stdlib.h> /* size_t */
#include <string.h> /* memcpy */
#include "osconfig.h"
#include "cfe.h"

#ifdef SOFTWARE_BIG_BIT_ORDER
    #define CFE_MAKE_BIG32(n) (n)
#else /* !SOFTWARE_BIG_BIT_ORDER */
    #define CFE_MAKE_BIG32(n) ( (((n) << 24) & 0xFF000000) | (((n) << 8) & 0x00FF0000) | (((n) >> 8) & 0x0000FF00) | (((n) >> 24) & 0x000000FF) )
#endif /* SOFTWARE_BIG_BIT_ORDER */

typedef struct
{   
    void *Buf;
    size_t BufSz, BufUsed;
} Pack_t;

static inline uint32 Pack_Init(Pack_t *PackPtr, void *Buf, size_t BufSz, int ClearFlag)
{
    PackPtr->Buf = Buf;
    PackPtr->BufSz = BufSz;
    PackPtr->BufUsed = 0;
    if(ClearFlag)
    {
        memset(Buf, 0, BufSz);
    }/* end if */
    return 1;
}/* end Pack_Init() */

static inline uint32 Pack_Data(Pack_t *PackPtr, void *DataBuf, size_t DataBufSz)
{
    if(PackPtr->BufUsed + DataBufSz > PackPtr->BufSz)
    {
        /* print an error? */
        return 0;
    }

    memcpy(PackPtr->Buf + PackPtr->BufUsed, DataBuf, DataBufSz);
    PackPtr->BufUsed += DataBufSz;

    return 1;
}

static inline uint32 Pack_UInt8(Pack_t *PackPtr, uint8 Data)
{
    return Pack_Data(PackPtr, &Data, sizeof(Data));
}

static inline uint32 Pack_UInt16(Pack_t *PackPtr, uint16 Data)
{
    uint16 D = CFE_MAKE_BIG16(Data);
    return Pack_Data(PackPtr, &D, sizeof(D));
}

static inline uint32 Pack_UInt32(Pack_t *PackPtr, uint32 Data)
{
    uint32 D = CFE_MAKE_BIG32(Data);
    return Pack_Data(PackPtr, &D, sizeof(D));
}

static inline uint32 Pack_Time(Pack_t *PackPtr, OS_time_t Data)
{
    OS_time_t D;
    D.seconds = CFE_MAKE_BIG32(Data.seconds);
    D.microsecs = CFE_MAKE_BIG32(Data.microsecs);
    return Pack_Data(PackPtr, &D, sizeof(D));
}

static inline uint32 Pack_MsgID(Pack_t *PackPtr, CFE_SB_MsgId_t Data)
{
    CFE_SB_MsgId_t D;
    D = CFE_MAKE_BIG16(Data);
    return Pack_Data(PackPtr, &D, sizeof(D));
}

static inline void *Pack_Get(Pack_t *PackPtr)
{
    return PackPtr->Buf;
}

typedef struct
{
    void *Buf;
    size_t Sz, Curr;
} Unpack_t;

static inline void Unpack_Init(Unpack_t *Unpack, void *Buf, size_t Sz)
{
    Unpack->Buf = Buf; Unpack->Sz = Sz; Unpack->Curr = 0;
}

static inline uint32 Unpack_Data(Unpack_t *Unpack, void *DataBuf, size_t Sz)
{
    if(Unpack->Curr + Sz > Unpack->Sz)
    {
        return 0;
    }

    memcpy(DataBuf, Unpack->Buf + Unpack->Curr, Sz);

    Unpack->Curr += Sz;

    return 1;
}

static inline uint32 Unpack_UInt8(Unpack_t *Unpack, uint8 *DataBuf)
{
    return Unpack_Data(Unpack, DataBuf, sizeof(*DataBuf));
}

static inline uint32 Unpack_UInt16(Unpack_t *Unpack, uint16 *DataBuf)
{
    uint16 D;
    if (!Unpack_Data(Unpack, &D, sizeof(D)))
    {
        return 0;
    }
    *DataBuf = CFE_MAKE_BIG16(D);
    return 1;
}

static inline uint32 Unpack_UInt32(Unpack_t *Unpack, uint32 *DataBuf)
{
    uint32 D;
    if (!Unpack_Data(Unpack, &D, sizeof(D)))
    {
        return 0;
    }
    *DataBuf = CFE_MAKE_BIG32(D);
    return 1;
}

static inline uint32 Unpack_MsgID(Unpack_t *Unpack, CFE_SB_MsgId_t *DataBuf)
{
    CFE_SB_MsgId_t D;
    if (!Unpack_Data(Unpack, &D, sizeof(D)))
    {
        return 0;
    }
    *DataBuf = CFE_MAKE_BIG16(D);
    return 1;
}

#endif /* _sbn_pack_h_ */
