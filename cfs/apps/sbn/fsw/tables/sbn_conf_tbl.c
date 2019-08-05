#include "sbn_tbl.h"
#include "cfe_tbl_filedef.h"

SBN_ConfTbl_t SBN_ConfTbl =
{
		{ /* Mods */
				{ /* Mods[0] */
						"UDP", /* Name */
						"/cf/sbn_udp.so", /* LibFileName */
						"SBN_UDP_Ops" /* LibSymbol */
				},
				{ /* Mods[1] */
						"TCP", /* Name */
						"/cf/sbn_tcp.so", /* LibFileName */
						"SBN_TCP_Ops" /* LibSymbol */
				}
		},
		2, /* ModCnt */
		{ /* Peers */
				{ /* Peers[0] */
						1, /* ProcessorID */
						42, /* SpacecraftID */
						0, /* NetNum */
						0, /* ModIdx */
						"127.0.0.1:1234" /* Address */
				},
				{ /* Peers[1] */
						2, /* ProcessorID */
						42, /* SpacecraftID */
						0, /* NetNum */
						0, /* ModIdx */
						"127.0.0.1:1235" /* Address */
				},
				{ /* Peers[2] */
						3, /* ProcessorID */
						42, /* SpacecraftID */
						0, /* NetNum */
						0, /* ModIdx */
						"127.0.0.1:1236" /* Address */
				}
		},
		3 /* PeerCnt */
};

CFE_TBL_FILEDEF(SBN_ConfTbl, SBN.SBN_ConfTbl, SBN Configuration Table, sbn_conf_tbl.tbl)
