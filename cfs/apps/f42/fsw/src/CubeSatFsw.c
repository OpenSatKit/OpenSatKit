/*
** Purpose: Implement a spacecraft control law
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. The goal is to be able to port
**      the 42 code code unchanged and have it run as a cFS app.
**   2. 
**
** References:
**   1. See 42 open source repository at https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** License:
**   This code is licensed under the NASA Open Source Agreement (NOSA).
*/
#include "42fsw.h"
#include "fswkit.h"
#include "iokit.h"
#include "mathkit.h"

#define FALSE 0
#define TRUE 1

//#ifdef __cplusplus
//namespace _42 {
//using namespace Kit;
//#endif

/**********************************************************************/
/* This function copies needed parameters from the SC structure to    */
/* the FSW structure.                                                 */
void InitFSW(struct FSWType *FSW)
{
      long i,j;
      double Awhl[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
      double Amtb[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
      double Mmax[3] = {0.2,0.2,0.2};
      double MOI[3] = {1.198352e-1,1.477797e-1,4.49084e-2};
      double SqrtHalf = sqrt(0.5);

      FSW->Init = 1;

      FSW->Nwhl = 3;
      FSW->Hw = (double *) calloc(FSW->Nwhl,sizeof(double));
      FSW->Hwcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
      FSW->Awhl = CreateMatrix(3,FSW->Nwhl);
      FSW->AwhlPlus = CreateMatrix(FSW->Nwhl,3);
      FSW->Twhlcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
      for (i=0;i<FSW->Nwhl;i++) {
         for (j=0;j<3;j++) {
            FSW->Awhl[j][i] = Awhl[i][j];
         }
      }
      PINVG(FSW->Awhl,FSW->AwhlPlus,3,FSW->Nwhl);

      FSW->Nmtb = 3;
      FSW->Amtb = CreateMatrix(3,FSW->Nmtb);
      FSW->AmtbPlus = CreateMatrix(FSW->Nmtb,3);
      FSW->Mmtbcmd = (double *) calloc(FSW->Nmtb,sizeof(double));
      FSW->Mmtbmax = (double *) calloc(FSW->Nmtb,sizeof(double));
      for (i=0;i<FSW->Nmtb;i++) {
         for (j=0;j<3;j++) {
            FSW->Amtb[j][i] = Amtb[i][j];
         }
         FSW->Mmtbmax[i] = Mmax[i];
      }
      PINVG(FSW->Amtb,FSW->AmtbPlus,3,FSW->Nmtb);

      /* Crude MOI estimate */
      for (i=0;i<3;i++) {
            FSW->MOI[i] = MOI[i];
      }
      FSW->mass = 8.3858878;

      /* Coarse Sun Sensors */
      for(i=0;i<4;i++) {
         FSW->Css[i].CountsToIllum = 1.0/1024.0;  /* Max illum = 1.0 */
         FSW->Css[i].Axis[1] = SqrtHalf;
      }
      FSW->Css[0].Axis[0] =  SqrtHalf;
      FSW->Css[0].Axis[2] =  0.0;
      FSW->Css[1].Axis[0] =  0.0;
      FSW->Css[1].Axis[2] =  SqrtHalf;
      FSW->Css[2].Axis[0] = -SqrtHalf;
      FSW->Css[2].Axis[2] =  0.0;
      FSW->Css[3].Axis[0] =  0.0;
      FSW->Css[3].Axis[2] = -SqrtHalf;
}
/**********************************************************************/
void CssProc(struct FSWType *FSW)
{
      struct FswCssType *Css;
      double Threshold = 0.2; /* Min Illum threshold */
      double SqrtHalf = sqrt(0.5);
      double TempDbl;  //dcm
      long Nillum;
      long i;

/* .. Individual CSS Processing */
      Nillum = 0;
      for(i=0;i<4;i++) {
         Css = &FSW->Css[i];
         Css->Illum = Css->CountsToIllum*((double) Css->Counts);
         if (Css->Illum > Threshold) Nillum++;
      }

/* .. Build Sun Vector */
      if (Nillum > 0) FSW->SunValid = TRUE;
      else FSW->SunValid = FALSE;
      /* X Component */
      FSW->svb[0] = SqrtHalf*(FSW->Css[0].Illum - FSW->Css[2].Illum);
      /* Z Component */
      FSW->svb[2] = SqrtHalf*(FSW->Css[1].Illum - FSW->Css[3].Illum);
      /* Y Component */
      FSW->svb[1] = sqrt(1.0-FSW->svb[0]*FSW->svb[0]-FSW->svb[2]*FSW->svb[2]);
      //dcmTempDbl = 1.0 - (FSW->svb[0]*FSW->svb[0]-FSW->svb[2]*FSW->svb[2]);
      //dcmOS_printf("TempDbl = %.10f\n",TempDbl);
      //dcmif (TempDbl > 0.0000001) {
      //dcmFSW->svb[1] = sqrt(TempDbl); 
      //dcm   FSW->svb[1] = 1.0; 
      //dcm}
      //dcmelse {
      //dcm   FSW->svb[1] = 1.0;
      //dcm}

}
/**********************************************************************/
/* Model of CubeSat FSW                                               */
void CubeSatFSW(struct FSWType *FSW)
{
      long i;

      if (FSW->Init) {
         FSW->Init = FALSE;
         for(i=0;i<3;i++) FindPDGains(FSW->MOI[i],0.1*6.28,0.7,
            &FSW->Kr[i],&FSW->Kp[i]);
      }

      CssProc(FSW);

      /* Point +Y axis at Sun */
      if (FSW->SunValid) {
         FSW->wrn[0] = 0.0;
         FSW->therr[0] = -FSW->svb[2];
         FSW->therr[2] =  FSW->svb[0];
      }
      else {
         /* Sun-seeking slow roll */
         FSW->wrn[0] = 360.0/2600.0*3.1416/180.0;
         FSW->therr[0] = 0.0;
         FSW->therr[2] = 0.0;
      }
      FSW->therr[1] = 0.0;
      FSW->wrn[1] = 0.0;
      FSW->wrn[2] = 0.0;

      for(i=0;i<3;i++) {
         FSW->Tcmd[i] = -FSW->Kr[i]*(FSW->wbn[i]-FSW->wrn[i])
                        -FSW->Kp[i]*FSW->therr[i];
         FSW->Twhlcmd[i] = -FSW->Tcmd[i];
      }
}
/**********************************************************************
long ReadSensorSignalsFromSocket(struct FSWType *FSW, FILE* Stream)
{
      char line[512];


      fgets(line,511,Stream);
      if (sscanf(line,"%ld %lf %lf %lf %ld %ld %ld %ld",
         &FSW->SunValid,
         &FSW->wbn[0],&FSW->wbn[1],&FSW->wbn[2],
         &FSW->Css[0].Counts,&FSW->Css[1].Counts,
         &FSW->Css[2].Counts,&FSW->Css[3].Counts) == 8) return(1);
      else return(0);
}
**********************************************************************
void WriteActuatorCmdsToSocket(struct FSWType *FSW, int Socket)
{
      char line[512];
      int Success;

      sprintf(line,"%lf %lf %lf\n",
         FSW->Twhlcmd[0],FSW->Twhlcmd[1],FSW->Twhlcmd[2]);
      //printf("%s",line);

      Success = send(Socket,line,strlen(line),0);
}
*********************************************************************
int main(int argc, char **argv)
{

      struct FSWType FSW;
      int FswSocket;
      FILE *FswStream;
      long Isc;

      if (argc == 2) Isc = atoi(argv[1]);
      else Isc = 0;

      InitFSW(&FSW);

      FswSocket = InitSocketClient("localhost",42420+Isc,TRUE);
      FswStream = fdopen(FswSocket,"r+");

      while(1) {
         ReadSensorSignalsFromSocket(&FSW,FswStream);
         CubeSatFSW(&FSW);
         WriteActuatorCmdsToSocket(&FSW,FswSocket);
      }
      return(0);
}
*/

//#ifdef __cplusplus
//}
//#endif

