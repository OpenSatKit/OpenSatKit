#ifndef _sbn_serial_if_struct_h_
#define _sbn_serial_if_struct_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "sbn_serial_platform_cfg.h"
#include "sbn_platform_cfg.h"
#include "cfe.h"

typedef struct
{
    char Filename[SBN_SERIAL_MAX_CHAR_NAME];
    int FD;
    /* 0 = this peer connects to me, 1 = I connect to this peer */
    uint8 /** flags */
            /** \brief recv the header first */
            ReceivingBody,
            /** \brief am I connected to the serial device? */
            SerialConn,
            BufNum;
    int RecvSz;

    /** See SBN_SERIAL_CONNTRY_TIME. */
    OS_time_t LastConnTry;
} SBN_SERIAL_Peer_t;

#endif /* _sbn_serial_if_struct_h_ */
