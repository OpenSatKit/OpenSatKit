#ifndef _sbn_tcp_if_struct_h_
#define _sbn_tcp_if_struct_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "sbn_platform_cfg.h"
#include "cfe.h"

/**
 * If I haven't sent a message in SBN_TCP_PEER_HEARTBEAT seconds, send an empty
 * one just to maintain the connection. If this is set to 0, no heartbeat
 * messages will be generated.
 */
#define SBN_TCP_PEER_HEARTBEAT 5

/**
 * If I haven't received a message from a peer in SBN_TCP_PEER_TIMEOUT seconds,
 * consider the peer lost and disconnect. If this is set to 0, no timeout is
 * checked.
 */
#define SBN_TCP_PEER_TIMEOUT 10

typedef struct
{
    char Host[16];
    int Port;
    uint8 BufNum;
    int Socket;
} SBN_TCP_Net_t;

typedef struct
{
    char Host[16];
    int  Port;
    int Socket;
    /* 0 = this peer connects to me, 1 = I connect to this peer */
    uint8 /** flags */
            /** \brief recv the header first */
            ReceivingBody,
            /** \brief Do I connect to this peer or do they connect to me? */
            ConnectOut,
            /** \brief Is this peer currently connected? */
            Connected,
            BufNum;
    OS_time_t LastConnectTry;
    int RecvSz;
} SBN_TCP_Peer_t;

#endif /* _sbn_tcp_if_struct_h_ */
