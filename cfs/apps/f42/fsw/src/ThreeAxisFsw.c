/*
** Purpose: Implement 42's ThreeAxisFsw control law
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. The goal is to be able to port
**      the 42 code code unchanged and have it run as a cFS app.
**   2. The functions in this file were taken directly from 42fsw.c. 42's FSW
**      data structure is used for all I/O. See function prologues for 
**      change details.
**
** References:
**   1. See 42 open source repository at https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** License:
**   This code is licensed under the NASA Open Source Agreement (NOSA).
*/

#include <stdlib.h>

#include "fswtypes.h"
#include "fswkit.h"
#include "mathkit.h"
#include "dcmkit.h"
#include "ThreeAxisFsw.h"

#define FALSE 0
#define TRUE 1

#define Pi 3.141592653589793
#define TwoPi 6.283185307179586
#define D2R 1.74532925199433E-2

/******************************************************************************
** Function: InitFSW
**
** Notes:
**   1. This function was adapted from 42fsw.c's InitFSW(). The following
**      changes were made:
**      A. Changed function input parameter from SCType *S to FSWType *FSW
**      B. 42 reads SC definition text file for initial values. Removed 
**         generalized logic (e.g. number of wheels) and hard coded initial
**         values using the Aura S/C definition file SC_Aura.txt.
**   2. This is demonstration code and not flight code. There are many aspects 
**      of this code that would not be appropriate for flight. For example,
**      typical FSW uses tables to define default parameter values and does
**      not use dynamic memory allocation. One goal of transferring the 42 
**      controller to a FSW app was to minimize changes to the exist 42 code.    
**
*/
void InitFSW(struct FSWType *FSW)
{
 
    long Ig,i,j;
    
    /* Align components with s/c axes */
    double Awhl[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
    double Amtb[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
    double Mmax[3] = {300.0,300.0,300.0};      /* A-m^2  */
    double MOI[3] = {2759.831373, 2117.500000, 3835.272549};  /* kg-m^2 - moved to table */
	
   
    FSW->Init = 1;

    /* Gimbals */
    FSW->Ngim = 1;
    FSW->Gim = (struct FswGimType *) calloc(FSW->Ngim,sizeof(struct FswGimType));
    FSW->GimCmd = (struct CmdType *) calloc(FSW->Ngim,sizeof(struct CmdType));
    /* dcm
	 for(Ig=0;Ig<FSW->Ngim;Ig++) {
        FSW->Gim[Ig].IsSpherical = S->G[Ig].IsSpherical;
        FSW->Gim[Ig].RotDOF = S->G[Ig].RotDOF;
        FSW->Gim[Ig].TrnDOF = S->G[Ig].TrnDOF;
        FSW->Gim[Ig].IsUnderActiveControl = TRUE;
        FSW->Gim[Ig].RotSeq = S->G[Ig].RotSeq;
        FSW->GimCmd[Ig].RotSeq = S->G[Ig].RotSeq;
        for(i=0;i<3;i++) {
            for(j=0;j<3;j++) {
                FSW->Gim[Ig].CGiBi[i][j] = S->G[Ig].CGiBi[i][j];
                FSW->Gim[Ig].CBoGo[i][j] = S->G[Ig].CBoGo[i][j];
            }
        }
    } /* End inner gimbal loop */
    FSW->Gim[0].RotSeq = 231;
	 /* dcm - Copied from 42init.c using SC_Aura.txt definitions*/
	 A2C(312,0.0*D2R,0.0*D2R,0.0*D2R,FSW->Gim[0].CGiBi);
    A2C(312,0.0*D2R,0.0*D2R,0.0*D2R,FSW->Gim[0].CBoGo);
    

    /* Wheels */
    FSW->Nwhl = 3;
    FSW->Hw = (double *) calloc(FSW->Nwhl,sizeof(double));
    FSW->Hwcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
    FSW->Awhl = CreateMatrix(3,FSW->Nwhl);
    FSW->AwhlPlus = CreateMatrix(FSW->Nwhl,3);
    FSW->Twhlcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
    for (i=0;i<FSW->Nwhl;i++) {
        for (j=0;j<3;j++) {
           FSW->Awhl[j][i] = Awhl[j][i];
        }
        FSW->Hwcmd[i] = 0.0;  /* Commanded wheel momentum - No bias */	
    }
    if (FSW->Nwhl >= 3) PINVG(FSW->Awhl,FSW->AwhlPlus,3,FSW->Nwhl);

    /* MTBs */
    FSW->Nmtb = 3;
    FSW->Amtb = CreateMatrix(3,FSW->Nmtb);
    FSW->AmtbPlus = CreateMatrix(FSW->Nmtb,3);
    FSW->Mmtbcmd = (double *) calloc(FSW->Nmtb,sizeof(double));
    FSW->Mmtbmax = (double *) calloc(FSW->Nmtb,sizeof(double));
    for (i=0;i<FSW->Nmtb;i++) {
        for (j=0;j<3;j++) {
            FSW->Amtb[j][i] = Amtb[j][i];
        }
        FSW->Mmtbmax[i] = Mmax[i];
    }
    PINVG(FSW->Amtb,FSW->AmtbPlus,3,FSW->Nmtb);

    /* Thrusters removed */

    /* Crude MOI estimate */
    for (i=0;i<3;i++) {
        FSW->MOI[i] = MOI[i];
    }
    CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, "InitFSW MOI: %lf %lf %lf\n",FSW->MOI[0],FSW->MOI[1],FSW->MOI[2]);
    FSW->mass = 1000.0;

    /* For RampCoastGlide.  See Inp_Cmd.txt for easy modification. */
    FSW->RcgWC   = 0.05*TwoPi;
    FSW->RcgAmax = 0.01;
    FSW->RcgVmax = 0.5*D2R;

} /* End InitFSW() */


/******************************************************************************
** Function: ThreeAxisFSW
**
** Notes:
**   1. This function was adapted from 42fsw.c's ThreeAxisFSW(). The following
**      changes were made:
**      A. Changed input parameter from SCType *S to FSWType *FSW
**      B. Added SaMOI. Value from SC_Aura.txt
**
*/
void ThreeAxisFSW(struct FSWType *FSW)
{

    double wln[3],CRN[3][3];
    double qrn[4],svr[3]; // Removed qbr[4] & used FSW->qbr
    double Herr[3],HxB[3];
    double Zvec[3] = {0.0,0.0,1.0};
    double SaMOI = 20.0; /* kg-m^2: S->B[1].I[1][1] */
    long i,j; //Bout;

    if (FSW->Init) {
        FSW->Init = 0;
        FSW->DT = 0.5;  /* Sensor data will overwrite this value */
        FSW->Gim[0].IsSpherical = FALSE;
        FSW->Gim[0].RotDOF = 1;
        FSW->Gim[0].RotSeq = 231;
        for(j=0;j<3;j++) {
            FSW->GimCmd[0].Rate[j] = 0.0;
            FSW->GimCmd[0].Ang[j] = 0.0;
            FSW->Gim[0].MaxRate[j] = 0.2*D2R;
            FSW->Gim[0].MaxTrq[j] = 100.0;
            FindPDGains(SaMOI,0.02*TwoPi,1.0,
                        &FSW->Gim[0].RateGain[j],&FSW->Gim[0].AngGain[j]);
		}

        for(i=0;i<3;i++) {
            FSW->wc[i] = 0.1;
            FSW->zc[i] = 0.7;
            FindPDGains(FSW->MOI[i],FSW->wc[i],FSW->zc[i], &FSW->Kr[i],&FSW->Kp[i]);
        }
        FSW->Kunl = 1.0E6;
        CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, "ThreeAxis Init MOI: %lf %lf %lf\n",FSW->MOI[0],FSW->MOI[1],FSW->MOI[2]);
    }

      /* Find Attitude Command */
      FindCLN(FSW->PosN,FSW->VelN,CRN,wln);
      C2Q(CRN,qrn);
      MxV(CRN,FSW->svn,svr);

      /* Form Error Signals */
      QxQT(FSW->qbn,qrn,FSW->qbr);
      RECTIFYQ(FSW->qbr);

      /* PD Control */
      for(i=0;i<3;i++) {
         FSW->Tcmd[i] = -FSW->Kr[i]*FSW->wbn[i]-FSW->Kp[i]*(2.0*FSW->qbr[i]);
         FSW->Twhlcmd[i] = -FSW->Tcmd[i];
      }

      CFE_EVS_SendEvent(THREEAXISFSW_DEBUG_EID, CFE_EVS_DEBUG, "Mom Mgmt: Hw: %.6e, %.6e, %.6e, Hwcmd: %.6e, %.6e, %.6e\n", 
                        FSW->Hw[0], FSW->Hw[1], FSW->Hw[2], FSW->Hwcmd[0], FSW->Hwcmd[1], FSW->Hwcmd[2]);
      
      /* Momentum Management */
      for(i=0;i<3;i++) {
          Herr[i]=FSW->Hw[i]-FSW->Hwcmd[i];
      }
      VxV(Herr,FSW->bvb,HxB);
      for(i=0;i<3;i++) {
          FSW->ierr[i] = Herr[i];   // dcm - Hack to get intermediate error to TLM without modifying fswtypes.h
          FSW->Mmtbcmd[i] = FSW->Kunl*HxB[i];
      }

      /* Gimbals */
      FSW->GimCmd[0].Rate[0] = wln[1];

	   CFE_EVS_SendEvent(THREEAXISFSW_DEBUG_EID, CFE_EVS_DEBUG, "Before PointGimbalToTarget(): %ld, %.6e, %.6e\n", 
	                     FSW->Gim[0].RotSeq, FSW->Gim[0].CGiBi[0][0], FSW->Gim[0].CBoGo[0][0]);
	  
      if (FSW->SunValid) {
           PointGimbalToTarget(FSW->Gim[0].RotSeq, FSW->Gim[0].CGiBi,
               FSW->Gim[0].CBoGo, FSW->svb, Zvec,FSW->GimCmd[0].Ang);
      }
      else {
         FSW->GimCmd[0].Ang[0] += wln[1]*FSW->DT;
      }
      if (FSW->Gim[0].Ang[0] - FSW->GimCmd[0].Ang[0] > Pi)
         FSW->GimCmd[0].Ang[0] += TwoPi;
      if (FSW->Gim[0].Ang[0] - FSW->GimCmd[0].Ang[0] < -Pi)
         FSW->GimCmd[0].Ang[0] -= TwoPi;
      
 
} /* End ThreeAxisFSW() */

