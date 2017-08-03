#include "42.h"

//#ifdef __cplusplus
//namespace _42 {
//using namespace Kit;
//#endif

/**********************************************************************/
long FswCmdInterpreter(char CmdLine[512],double *CmdTime)
{
      long NewCmdProcessed = FALSE;
      long Isc,Ib,Ig,Iw,It,i,Isct,Ibt;
      char FrameChar;
      long Frame;
      struct FSWType *Fsw;
      struct CmdType *Cmd;
      struct CmdVecType *CV;
      double q[4],Ang[3],C[3][3],VecR[3],Vec[3];
      double RA,Dec;
      double Lng,Lat,Alt;
      long RotSeq;
      char VecString[20],TargetString[20];

      if (sscanf(CmdLine,"%lf SC[%ld] qrn = [%lf %lf %lf %lf]",
         CmdTime,&Isc,&q[0],&q[1],&q[2],&q[3]) == 6) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         Cmd = &Fsw->Cmd;
         Cmd->Parm = PARM_QUATERNION;
         Cmd->Frame = FRAME_N;
         for(i=0;i<4;i++) Cmd->qrn[i] = q[i];
      }

      else if (sscanf(CmdLine,"%lf SC[%ld] qrl = [%lf %lf %lf %lf]",
         CmdTime,&Isc,&q[0],&q[1],&q[2],&q[3]) == 6) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         Cmd = &Fsw->Cmd;
         Cmd->Parm = PARM_QUATERNION;
         Cmd->Frame = FRAME_L;
         for(i=0;i<4;i++) Cmd->qrl[i] = q[i];
      }

      else if (sscanf(CmdLine,"%lf SC[%ld] Cmd Angles = [%lf %lf %lf] Seq = %ld wrt %c Frame",
         CmdTime,&Isc,&Ang[0],&Ang[1],&Ang[2],&RotSeq,&FrameChar) == 7) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         Cmd = &Fsw->Cmd;
         Cmd->Parm = PARM_EULER_ANGLES;
         if (FrameChar == 'L') Cmd->Frame = FRAME_L;
         else Cmd->Frame = FRAME_N;
         for(i=0;i<3;i++) Cmd->Ang[i] = Ang[i]*D2R;
         Cmd->RotSeq = RotSeq;
         A2C(RotSeq,Ang[0]*D2R,Ang[1]*D2R,Ang[2]*D2R,C);
         if (Cmd->Frame == FRAME_L) C2Q(C,Cmd->qrl);
         else C2Q(C,Cmd->qrn);
      }

      else if (sscanf(CmdLine,"%lf SC[%ld].G[%ld] Cmd Angles = [%lf %lf %lf]",
         CmdTime,&Isc,&Ig,&Ang[0],&Ang[1],&Ang[2]) == 6) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         Cmd = &Fsw->GimCmd[Ig];
         Cmd->Parm = PARM_EULER_ANGLES;
         for(i=0;i<3;i++) Cmd->Ang[i] = Ang[i]*D2R;
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at RA = %lf Dec = %lf",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&RA,&Dec) == 9) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_DIRECTION;
         CV->Frame = FRAME_N;
         UNITV(VecR);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
         CV->N[0] = cos(RA*D2R)*cos(Dec*D2R);
         CV->N[1] = sin(RA*D2R)*cos(Dec*D2R);
         CV->N[2] = sin(Dec*D2R);
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at World[%ld] Lng = %lf Lat = %lf Alt = %lf",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&Iw,&Lng,&Lat,&Alt) == 11) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         CV->TrgType = TARGET_WORLD;
         CV->TrgWorld = Iw;
         UNITV(VecR);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
         CV->W[0] = (World[Iw].rad+1000.0*Alt)*cos(Lng*D2R)*cos(Lat*D2R);
         CV->W[1] = (World[Iw].rad+1000.0*Alt)*sin(Lng*D2R)*cos(Lat*D2R);
         CV->W[2] = (World[Iw].rad+1000.0*Alt)*sin(Lat*D2R);
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at World[%ld]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&Iw) == 8) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         CV->TrgType = TARGET_WORLD;
         CV->TrgWorld = Iw;
         UNITV(VecR);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
         for(i=0;i<3;i++) CV->W[i] = 0.0;
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at GroundStation[%ld]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&It) == 8) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         CV->TrgType = TARGET_WORLD;
         CV->TrgWorld = GroundStation[It].World;
         UNITV(VecR);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
         for(i=0;i<3;i++) CV->W[i] = GroundStation[It].PosW[i];
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at SC[%ld].B[%ld] point [%lf %lf %lf]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&Isct,&Ibt,&Vec[0],&Vec[1],&Vec[2]) == 12) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         CV->TrgType = TARGET_BODY;
         CV->TrgSC = Isct;
         CV->TrgBody = Ibt;
         CopyUnitV(VecR,CV->R);
         for(i=0;i<3;i++) CV->T[i] = Vec[i];
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at SC[%ld]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&Isct) == 8) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         CV->TrgType = TARGET_SC;
         CV->TrgSC = Isct;
         CopyUnitV(VecR,CV->R);
      }

      else if (sscanf(CmdLine,"%lf Point SC[%ld].B[%ld] %s Vector [%lf %lf %lf] at %s",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],TargetString) == 8) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_TARGET;
         CV->Frame = FRAME_N;
         if (!strcmp(TargetString,"EARTH")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = EARTH;
         }
         else if (!strcmp(TargetString,"MOON")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = LUNA;
         }
         else if (!strcmp(TargetString,"LUNA")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = LUNA;
         }
         else if (!strcmp(TargetString,"MERCURY")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = MERCURY;
         }
         else if (!strcmp(TargetString,"VENUS")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = VENUS;
         }
         else if (!strcmp(TargetString,"MARS")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = MARS;
         }
         else if (!strcmp(TargetString,"JUPITER")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = JUPITER;
         }
         else if (!strcmp(TargetString,"SATURN")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = SATURN;
         }
         else if (!strcmp(TargetString,"URANUS")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = URANUS;
         }
         else if (!strcmp(TargetString,"NEPTUNE")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = NEPTUNE;
         }
         else if (!strcmp(TargetString,"PLUTO")) {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = PLUTO;
         }
         else if (!strcmp(TargetString,"VELOCITY")) {
            CV->TrgType = TARGET_VELOCITY;
         }
         else {
            CV->TrgType = TARGET_WORLD;
            CV->TrgWorld = SOL;
         }
         UNITV(VecR);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
      }

      else if (sscanf(CmdLine,"%lf Align SC[%ld].B[%ld] %s Vector [%lf %lf %lf] with SC[%ld].B[%ld] vector [%lf %lf %lf]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&Isct,&Ibt,&Vec[0],&Vec[1],&Vec[2]) == 12) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         Cmd->Frame = FRAME_N;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_DIRECTION;
         CV->Frame = FRAME_B;
         CV->TrgType = TARGET_BODY;
         CV->TrgSC = Isct;
         CV->TrgBody = Ibt;
         CopyUnitV(VecR,CV->R);
         for(i=0;i<3;i++) CV->T[i] = Vec[i];
      }

      else if (sscanf(CmdLine,"%lf Align SC[%ld].B[%ld] %s Vector [%lf %lf %lf] with %c-frame Vector [%lf %lf %lf]",
         CmdTime,&Isc,&Ib,VecString,&VecR[0],&VecR[1],&VecR[2],&FrameChar,&Vec[0],&Vec[1],&Vec[2]) == 11) {
         NewCmdProcessed = TRUE;
         Fsw = &SC[Isc].FSW;
         if(FrameChar == 'L') Frame = FRAME_L;
         else Frame = FRAME_N;
         if (Ib == 0) {
            Cmd = &Fsw->Cmd;
         }
         else {
            Ig = SC[Isc].B[Ib].Gin;
            Cmd = &Fsw->GimCmd[Ig];
         }
         Cmd->Parm = PARM_VECTORS;
         if (!strcmp(VecString,"Primary")) CV = &Cmd->PriVec;
         else CV = &Cmd->SecVec;
         CV->Mode = CMD_DIRECTION;
         CV->Frame = Frame;
         UNITV(VecR);
         UNITV(Vec);
         for(i=0;i<3;i++) CV->R[i] = VecR[i];
         if (Frame == FRAME_L) {
            for(i=0;i<3;i++) CV->L[i] = Vec[i];
         }
         else {
            for(i=0;i<3;i++) CV->N[i] = Vec[i];
         }
      }


      return(NewCmdProcessed);
}
/**********************************************************************/
/* Given a relative position and velocity vector, find the angular    */
/* velocity at which the relative position vector is rotating.        */
void RelMotionToAngRate(double RelPosN[3], double RelVelN[3],
                        double wn[3])
{
      double magp,phat[3],Axis[3],Vpar,Vperp[3],magvp;
      long i;

      magp = CopyUnitV(RelPosN,phat);

      VxV(RelPosN,RelVelN,Axis);
      UNITV(Axis);

      Vpar = VoV(RelVelN,phat);
      for(i=0;i<3;i++) Vperp[i] = RelVelN[i]-Vpar*phat[i];
      magvp = MAGV(Vperp);
      for(i=0;i<3;i++) wn[i] = magvp/magp*Axis[i];
}
/**********************************************************************/
void FindCmdVecN(struct SCType *S, struct CmdVecType *CV)
{
      struct WorldType *W;
      double RelPosB[3],vb[3];
      double RelPosN[3],RelPosH[3],RelVelN[3],RelVelH[3];
      double pcmn[3],pn[3],vn[3],ph[3],vh[3];
      double CosPriMerAng,SinPriMerAng;
      long i;

      switch (CV->TrgType) {
         case TARGET_WORLD:
            W = &World[CV->TrgWorld];
            CosPriMerAng = cos(W->PriMerAng);
            SinPriMerAng = sin(W->PriMerAng);
            pn[0] =  CV->W[0]*CosPriMerAng - CV->W[1]*SinPriMerAng;
            pn[1] =  CV->W[0]*SinPriMerAng + CV->W[1]*CosPriMerAng;
            pn[2] =  CV->W[2];
            vn[0] = -CV->W[0]*SinPriMerAng - CV->W[1]*CosPriMerAng;
            vn[1] =  CV->W[0]*CosPriMerAng - CV->W[1]*SinPriMerAng;
            vn[2] = 0.0;
            if (CV->TrgWorld == Orb[SC->RefOrb].center) {
               for(i=0;i<3;i++) {
                  RelPosN[i] = pn[i] - S->PosN[i];
                  RelVelN[i] = vn[i] - S->VelN[i];
               }
            }
            else {
               MTxV(W->CNH,pn,ph);
               MTxV(W->CNH,vn,vh);
               for(i=0;i<3;i++) {
                  RelPosH[i] = (W->PosH[i]+ph[i])-S->PosH[i];
                  RelVelH[i] = (W->VelH[i]+vh[i])-S->VelH[i];
               }
               MxV(World[Orb[S->RefOrb].center].CNH,RelPosH,RelPosN);
               MxV(World[Orb[S->RefOrb].center].CNH,RelVelH,RelVelN);
            }
            CopyUnitV(RelPosN,CV->N);
            RelMotionToAngRate(RelPosN,RelVelN,CV->wn);
            break;
         case TARGET_SC:
            if (SC[CV->TrgSC].RefOrb == S->RefOrb) {
               for(i=0;i<3;i++) {
                  RelPosN[i] = SC[CV->TrgSC].Rrel[i]-S->Rrel[i];
                  RelVelN[i] = SC[CV->TrgSC].Vrel[i]-S->Vrel[i];
               }
            }
            else if (Orb[SC[CV->TrgSC].RefOrb].center == Orb[S->RefOrb].center) {
               for(i=0;i<3;i++) {
                  RelPosN[i] = SC[CV->TrgSC].PosN[i]-S->PosN[i];
                  RelVelN[i] = SC[CV->TrgSC].VelN[i]-S->VelN[i];
               }
            }
            else {
               for(i=0;i<3;i++) {
                  RelPosH[i] = SC[CV->TrgSC].PosH[i]-S->PosH[i];
                  RelVelH[i] = SC[CV->TrgSC].VelH[i]-S->VelH[i];
               }
               MxV(World[Orb[S->RefOrb].center].CNH,RelPosH,RelPosN);
               MxV(World[Orb[S->RefOrb].center].CNH,RelVelH,RelVelN);
            }
            CopyUnitV(RelPosN,CV->N);
            RelMotionToAngRate(RelPosN,RelVelN,CV->wn);
            break;
         case TARGET_BODY:
            MTxV(SC[CV->TrgSC].B[0].CN,SC[CV->TrgSC].cm,pcmn);
            MTxV(SC[CV->TrgSC].B[CV->TrgBody].CN,CV->T,pn);
            for(i=0;i<3;i++) RelPosB[i] = CV->T[i] - SC[CV->TrgSC].B[CV->TrgBody].cm[i];
            VxV(SC[CV->TrgSC].B[CV->TrgBody].wn,RelPosB,vb);
            MTxV(SC[CV->TrgSC].B[CV->TrgBody].CN,vb,vn);
            for(i=0;i<3;i++) {
               pn[i] += SC[CV->TrgSC].B[CV->TrgBody].pn[i]-pcmn[i];
               vn[i] += SC[CV->TrgSC].B[CV->TrgBody].vn[i];
            }
            if (SC[CV->TrgSC].RefOrb == S->RefOrb) {
               for(i=0;i<3;i++) {
                  RelPosN[i] = SC[CV->TrgSC].Rrel[i] + pn[i] - S->Rrel[i];
                  RelVelN[i] = SC[CV->TrgSC].Vrel[i] + vn[i] - S->Vrel[i];
               }
            }
            else if (Orb[SC[CV->TrgSC].RefOrb].center == Orb[S->RefOrb].center) {
               for(i=0;i<3;i++) {
                  RelPosN[i] = SC[CV->TrgSC].PosN[i] + pn[i] - S->PosN[i];
                  RelVelN[i] = SC[CV->TrgSC].VelN[i] + vn[i] - S->VelN[i];
               }
            }
            else {
               MTxV(World[Orb[SC[CV->TrgSC].RefOrb].center].CNH,pn,ph);
               MTxV(World[Orb[SC[CV->TrgSC].RefOrb].center].CNH,vn,vh);
               for(i=0;i<3;i++) {
                  RelPosH[i] = SC[CV->TrgSC].PosH[i] + ph[i] - S->PosH[i];
                  RelVelH[i] = SC[CV->TrgSC].VelH[i] + vh[i] - S->VelH[i];
               }
               MxV(World[Orb[S->RefOrb].center].CNH,RelPosH,RelPosN);
               MxV(World[Orb[S->RefOrb].center].CNH,RelVelH,RelVelN);
            }
            CopyUnitV(RelPosN,CV->N);
            RelMotionToAngRate(RelPosN,RelVelN,CV->wn);
            break;
         case TARGET_VELOCITY:
            for(i=0;i<3;i++) CV->N[i] = S->VelN[i];
            UNITV(CV->N);
            break;
         default:
            break;
      }
}
/**********************************************************************/
void ThreeAxisAttitudeCommand(struct SCType *S)
{
      struct JointType *G;
      struct BodyType *B;
      struct FSWType *Fsw;
      struct CmdType *Cmd;
      struct CmdVecType *PV, *SV;
      double CRN[3][3],C[3][3],qln[4],Cdot[3][3];
      double PriVecBi[3],SecVecBi[3],PriVecGi[3],SecVecGi[3];
      double PriVecGo[3],SecVecGo[3],CGoGi[3][3];
      long Ig,Bi,i,j;

      Fsw = &S->FSW;

      Cmd = &Fsw->Cmd;
      PV = &Cmd->PriVec;
      SV = &Cmd->SecVec;

      switch (Cmd->Parm) {
         case PARM_EULER_ANGLES:
            A2C(Cmd->RotSeq,Cmd->Ang[0],Cmd->Ang[1],Cmd->Ang[2],C);
            if (Cmd->Frame == FRAME_L) C2Q(C,Cmd->qrl);
            else C2Q(C,Cmd->qrn);
         case PARM_QUATERNION:
            C2Q(S->CLN,qln);
            if (Cmd->Frame == FRAME_L) {
               QxQ(Cmd->qrl,qln,Cmd->qrn);
               QxV(Cmd->qrn,S->wln,Cmd->wrn);
            }
            break;
         case PARM_VECTORS:
            if (PV->Mode == CMD_TARGET) FindCmdVecN(S,PV);
            else if (PV->Frame == FRAME_N) {
               for(i=0;i<3;i++) PV->wn[i] = 0.0;
            }
            else if (PV->Frame == FRAME_L) {
               MTxV(S->CLN,PV->L,PV->N);
               for(i=0;i<3;i++) PV->wn[i] = S->wln[i];
            }
            else if (PV->Frame == FRAME_B) {
               MTxV(SC[PV->TrgSC].B[PV->TrgBody].CN,PV->T,PV->N);
               MTxV(SC[PV->TrgSC].B[PV->TrgBody].CN,
                  SC[PV->TrgSC].B[PV->TrgBody].wn,PV->wn);
            }

            if (SV->Mode == CMD_TARGET) FindCmdVecN(S,SV);
            else if (SV->Frame == FRAME_N) {
               for(i=0;i<3;i++) SV->wn[i] = 0.0;
            }
            else if (SV->Frame == FRAME_L) {
               MTxV(S->CLN,SV->L,SV->N);
               for(i=0;i<3;i++) SV->wn[i] = S->wln[i];
            }
            else if (SV->Frame == FRAME_B) {
               MTxV(SC[SV->TrgSC].B[SV->TrgBody].CN,SV->T,SV->N);
               MTxV(SC[SV->TrgSC].B[SV->TrgBody].CN,
                  SC[SV->TrgSC].B[SV->TrgBody].wn,SV->wn);
            }
            if (MAGV(PV->N) == 0.0 || MAGV(PV->R) == 0.0)
               printf("Warning: Primary Vector not defined for SC[%ld]\n",S->Tag);
            if (MAGV(SV->N) == 0.0 || MAGV(SV->R) == 0.0)
               printf("Warning: Secondary Vector not defined for SC[%ld]\n",S->Tag);
            TRIAD(PV->N,SV->N,PV->R,SV->R,CRN);
            C2Q(CRN,Cmd->qrn);
            for(i=0;i<3;i++) {
               for(j=0;j<3;j++) {
                  Cdot[i][j] = (CRN[i][j]-Cmd->OldCRN[i][j])/Fsw->DT;
               }
            }
            CDOT2W(CRN,Cdot,Cmd->wrn);
            for(i=0;i<3;i++) {
               for(j=0;j<3;j++) {
                  Cmd->OldCRN[i][j] = CRN[i][j];
               }
            }
            break;
         default:
            break;
      }

      for(Ig=0;Ig<S->Ng;Ig++) {
         G = &S->G[Ig];
         Bi = G->Bin;
         B = &S->B[Bi];
         Cmd = &Fsw->GimCmd[Ig];
         PV = &Cmd->PriVec;
         SV = &Cmd->SecVec;

         if (Cmd->Parm == PARM_VECTORS) {
            if (PV->Mode == CMD_TARGET) FindCmdVecN(S,PV);
            else if (PV->Frame == FRAME_L) MTxV(S->CLN,PV->L,PV->N);
            if (SV->Mode == CMD_TARGET) FindCmdVecN(S,SV);
            else if (SV->Frame == FRAME_L) MTxV(S->CLN,SV->L,SV->N);


            if (G->RotDOF == 3) {
               MxV(B->CN,PV->N,PriVecBi);
               MxV(B->CN,SV->N,SecVecBi);
               MxV(G->CGiBi,PriVecBi,PriVecGi);
               MxV(G->CGiBi,SecVecBi,SecVecGi);
               MTxV(G->CBoGo,PV->R,PriVecGo);
               MTxV(G->CBoGo,SV->R,SecVecGo);
               TRIAD(PriVecGi,SecVecGi,PriVecGo,SecVecGo,CGoGi);
               C2A(G->RotSeq,CGoGi,&Cmd->Ang[0],
                  &Cmd->Ang[1],&Cmd->Ang[2]);
            }
            else {
               MxV(B->CN,PV->N,PriVecBi);
               PointGimbalToTarget(G->RotSeq,G->CGiBi,G->CBoGo,PriVecBi,
                  PV->R,Cmd->Ang);
            }
         }
      }
}
/**********************************************************************/
/* This function copies needed parameters from the SC structure to    */
/* the FSW structure.                                                 */
void InitFSW(struct SCType *S)
{
      long Ig,i,j;
      struct FSWType *FSW;

      FSW = &S->FSW;

      FSW->Init = 1;

      if (S->Ng > 0) {
         FSW->Ngim = S->Ng;
         FSW->Gim = (struct FswGimType *) calloc(S->Ng,sizeof(struct FswGimType));
         FSW->GimCmd = (struct CmdType *) calloc(S->Ng,sizeof(struct CmdType));
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
         }
      }

      if (S->Nw > 0) {
         FSW->Nwhl = S->Nw;
         FSW->Hw = (double *) calloc(FSW->Nwhl,sizeof(double));
         FSW->Hwcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
         FSW->Awhl = CreateMatrix(3,FSW->Nwhl);
         FSW->AwhlPlus = CreateMatrix(FSW->Nwhl,3);
         FSW->Twhlcmd = (double *) calloc(FSW->Nwhl,sizeof(double));
         for (i=0;i<S->Nw;i++) {
            for (j=0;j<3;j++) {
               FSW->Awhl[j][i] = S->Whl[i].A[j];
            }
         }
         PINVG(FSW->Awhl,FSW->AwhlPlus,3,FSW->Nwhl);
      }

      if (S->Nmtb > 0) {
         FSW->Nmtb = S->Nmtb;
         FSW->Amtb = CreateMatrix(3,FSW->Nmtb);
         FSW->AmtbPlus = CreateMatrix(FSW->Nmtb,3);
         FSW->Mmtbcmd = (double *) calloc(FSW->Nmtb,sizeof(double));
         FSW->Mmtbmax = (double *) calloc(FSW->Nmtb,sizeof(double));
         for (i=0;i<S->Nmtb;i++) {
            for (j=0;j<3;j++) {
               FSW->Amtb[j][i] = S->MTB[i].A[j];
            }
            FSW->Mmtbmax[i] = S->MTB[i].Mmax;
         }
         PINVG(FSW->Amtb,FSW->AmtbPlus,3,FSW->Nmtb);
      }

      if (S->Nthr > 0) {
         FSW->Nthr = S->Nthr;
         FSW->Athr = CreateMatrix(3,FSW->Nthr);
         FSW->AthrPlus = CreateMatrix(FSW->Nthr,3);
         FSW->Thrcmd = (double *) calloc(FSW->Nthr,sizeof(double));
         PINVG(FSW->Athr,FSW->AthrPlus,3,FSW->Nthr);
      }

      /* Crude MOI estimate */
      for (i=0;i<3;i++) {
            FSW->MOI[i] = S->I[i][i];
      }
      FSW->mass = S->mass;

}
/**********************************************************************/
/* The effective inertia for a gimbal is assumed to be the moment of  */
/* inertia of the appendage depending from the joint (that is, all    */
/* bodies for which that joint is in the JointPathTable) about that   */
/* joint, with all joints undeflected.                                */
void FindAppendageInertia(long Ig, struct SCType *S,double Iapp[3])
{
      struct DynType *D;
      struct JointType *G;
      double rho[3],CBoBi[3][3],Coi[3][3],Cr[3],Csofar[3][3];
      double CBoG[3][3],IBoG[3][3];
      long Ib,Jg,j,k;

      D = &S->Dyn;

      for(k=0;k<3;k++) Iapp[k] = 0.0;
      for (Ib=1;Ib<S->Nb;Ib++) {
         if (D->JointPathTable[Ib][Ig].InPath) {
            /* Build undeflected rho */
            Jg = S->B[Ib].Gin;
            for(k=0;k<3;k++) rho[k] = 0.0;
            for(j=0;j<3;j++) {
               for(k=0;k<3;k++) CBoBi[j][k] = 0.0;
               CBoBi[j][j] = 1.0;
            }
            while (Jg > Ig) {
               G = &S->G[Jg];
               MxM(G->CBoGo,G->CGiBi,Coi);
               for(k=0;k<3;k++) rho[k] -= G->rout[k];
               MTxV(Coi,rho,Cr);
               for(k=0;k<3;k++) rho[k] = Cr[k] + G->rin[k];
               for(j=0;j<3;j++) {
                  for(k=0;k<3;k++) Csofar[j][k] = CBoBi[j][k];
               }
               MxM(Csofar,Coi,CBoBi);
               Jg = S->B[G->Bin].Gin;
            }
            G = &S->G[Ig];
            for(k=0;k<3;k++) rho[k] -= G->rout[k];
            MTxV(G->CBoGo,rho,Cr);
            MxM(CBoBi,G->CBoGo,CBoG);
            /* Parallel axis theorem */
            PARAXIS(S->B[Ib].I,CBoG,S->B[Ib].mass,rho,IBoG);
            /* Accumulate */
            for(k=0;k<3;k++) Iapp[k] += IBoG[k][k];
         }
      }

}
/**********************************************************************/
/*  This simple control law is suitable for rapid prototyping.        */
void PrototypeFSW(struct SCType *S)
{
      struct FSWType *FSW;
      double alpha[3],Iapp[3];
      long Ig,i,j;

      FSW = &S->FSW;

      if (FSW->Init) {
         FSW->Init = 0;
         FSW->DT = DTSIM;
         for(Ig=0;Ig<FSW->Ngim;Ig++) {
            FindAppendageInertia(Ig,S,Iapp);
            for(j=0;j<3;j++) {
               FindPDGains(Iapp[j],0.05,1.0,
                  &FSW->Gim[Ig].RateGain[j],
                  &FSW->Gim[Ig].AngGain[j]);
               FSW->Gim[Ig].MaxRate[j] = 0.5*D2R;
               FSW->Gim[Ig].MaxTrq[j] = 0.1;
            }
         }
      }

/* .. Find qrn, wrn and joint angle commands */
      ThreeAxisAttitudeCommand(S);

/* .. Form attitude error signals */
      QxQT(FSW->qbn,FSW->Cmd.qrn,FSW->qbr);
      Q2AngleVec(FSW->qbr,FSW->therr);
      for(i=0;i<3;i++) FSW->werr[i] = FSW->wbn[i] - FSW->Cmd.wrn[i];

/* .. Closed-loop attitude control */
      VectorRampCoastGlide(FSW->therr,FSW->werr,20.0,1.0E-2,0.5*D2R,
         alpha);
      for(i=0;i<3;i++) FSW->IdealTrq[i] = FSW->MOI[i]*alpha[i];

}
/**********************************************************************/
void IssFSW(struct SCType *S)
{
      long Ig,i,j;
      struct FSWType *FSW;
      double Identity[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
      double Zvec[3] = {0.0,0.0,1.0};
      double GimCmd[3];
      double svb[3];
      double Iapp[3];
      double r[3],rb[3],tvb[3],MinRoZ,RoZ;
      double CRL[3][3],CBL[3][3];

      FSW = &S->FSW;

      if (FSW->Init) {
         FSW->Init = 0;
         for(Ig=0;Ig<FSW->Ngim;Ig++) {
            for(j=0;j<3;j++) {
               FSW->GimCmd[Ig].Rate[j] = 0.0;
               FSW->GimCmd[Ig].Ang[j] = 0.0;
               FSW->Gim[Ig].MaxRate[j] = 0.5*D2R;
            }
            FindAppendageInertia(Ig,S,Iapp);
            for(j=0;j<FSW->Gim[Ig].RotDOF;j++) {
               FindPDGains(Iapp[j],0.02*TwoPi,1.0,
                  &FSW->Gim[Ig].RateGain[j],
                  &FSW->Gim[Ig].AngGain[j]);
               FSW->Gim[Ig].MaxTrq[j] = 0.1*FSW->Gim[Ig].AngGain[j];
            }
         }
         for(i=0;i<3;i++)
            FindPDGains(S->I[i][i],0.02*TwoPi,0.7,
               &FSW->Kr[i],&FSW->Kp[i]);
         FSW->Tmax = 0.1*MAX(FSW->Kp[0],MAX(FSW->Kp[1],FSW->Kp[2]));
      }

/* .. Hold LVLH */
      A2C(213,0.0*D2R,0.0,0.0,CRL);
      MxMT(S->B[0].CN,S->CLN,CBL);
      MxMT(CBL,CRL,FSW->CBR);
      /* XVV */
      C2A(321,FSW->CBR,&FSW->therr[2],&FSW->therr[1],&FSW->therr[0]);
      for(i=0;i<3;i++) {
         FSW->werr[i] = FSW->wbn[i] - S->wln[i];
         FSW->IdealTrq[i] = -FSW->Kp[i]*FSW->therr[i]
                            -FSW->Kr[i]*FSW->werr[i];
       }

/* .. Point Main Solar Arrays */
      MxV(S->B[0].CN,FSW->svn,svb);
      PointGimbalToTarget(21,Identity,Identity,svb,Zvec,GimCmd);
      GimCmd[0] += 5.0*D2R; /* Avoid lighting artifacts from on-edge polys */
      FSW->GimCmd[0].Ang[0] = GimCmd[0];
      FSW->GimCmd[1].Ang[0] = -GimCmd[0];
      FSW->GimCmd[0].Rate[0] = -S->wln[1];
      FSW->GimCmd[1].Rate[0] = S->wln[1];

      FSW->GimCmd[2].Ang[0] = -GimCmd[1];
      FSW->GimCmd[3].Ang[0] =  GimCmd[1];
      FSW->GimCmd[4].Ang[0] = -GimCmd[1];
      FSW->GimCmd[5].Ang[0] =  GimCmd[1];

      FSW->GimCmd[6].Ang[0] =  GimCmd[1];
      FSW->GimCmd[7].Ang[0] = -GimCmd[1];
      FSW->GimCmd[8].Ang[0] =  GimCmd[1];
      FSW->GimCmd[9].Ang[0] = -GimCmd[1];

/* .. Point SM Solar Array */
      FSW->GimCmd[12].Ang[0] = GimCmd[0];
      FSW->GimCmd[13].Ang[0] = -GimCmd[0];

/* .. Point Radiators */
      PointGimbalToTarget(1,Identity,Identity,svb,Zvec,GimCmd);
      FSW->GimCmd[10].Ang[0] =  GimCmd[0] + 90.0*D2R;
      FSW->GimCmd[11].Ang[0] =  GimCmd[0] + 90.0*D2R;

/* .. Point HGA */
      /* Select TDRS nearest Zenith */
      MinRoZ = 2.0;
      for(i=0;i<10;i++) {
         if (Tdrs[i].Exists) {
            for(j=0;j<3;j++) r[j] = Tdrs[i].PosN[j]-S->PosN[j];
            UNITV(r);
            MxV(S->B[0].CN,r,rb);
            RoZ = VoV(rb,Zvec);
            if (RoZ < MinRoZ) {
               MinRoZ = RoZ;
               for(j=0;j<3;j++) tvb[j] = rb[j];
            }
         }
      }
      PointGimbalToTarget(21,S->G[14].CGiBi,Identity,tvb,Zvec,GimCmd);

      FSW->GimCmd[14].Ang[0] = Limit(GimCmd[0],-120.0*D2R,120.0*D2R);
      FSW->GimCmd[14].Ang[1] = Limit(GimCmd[1],-65.0*D2R,65.0*D2R);

}
/**********************************************************************/
/* Put your custom controller here                                    */
void AdHocFSW(struct SCType *S)
{
}
/**********************************************************************/
void CssProc(struct FSWType *FSW)
{
      struct FswCssType *Css;
      double Threshold = 0.2; /* Min Illum threshold */
      long Nillum;
      long i;

/* .. Individual CSS Processing */
      Nillum = 0;
      for(i=0;i<4;i++) {
         Css = &FSW->Css[i];
         Css->Counts = ((long) Css->AmpsToCounts*Css->Amps);
         if (Css->Counts < 0) Css->Counts = 0;
         if (Css->Counts > 4095) Css->Counts = 4095;
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
}
/**********************************************************************/
/* Model of CubeSat FSW                                               */
void CubeSatFSW(struct FSWType *FSW)
{
      long i;

      if (FSW->Init) {
         FSW->Init = FALSE;
         for(i=0;i<3;i++) FindPDGains(FSW->MOI[i],0.1*TwoPi,0.7,
            &FSW->Kr[i],&FSW->Kp[i]);
      }

      CssProc(FSW);

      /* Point +Y axis at Sun */
      if (FSW->SunValid) {
         FSW->therr[0] = -FSW->svb[2];
         FSW->therr[2] =  FSW->svb[0];
         FSW->wrn[0] = 0.0;
         FSW->wrn[1] = 0.0;
         FSW->wrn[2] = 0.0;
      }
      else {
         /* Sun-seeking slow roll */
         FSW->wrn[0] = 360.0/2600.0*D2R;
         FSW->therr[0] = 0.0;
         FSW->therr[2] = 0.0;
      }
      FSW->therr[1] = 0.0;

      for(i=0;i<3;i++) {
         FSW->Tcmd[i] = -FSW->Kr[i]*(FSW->wbn[i]-FSW->wrn[i])
                        -FSW->Kp[i]*FSW->therr[i];
         FSW->Twhlcmd[i] = -FSW->Tcmd[i];
      }
}
/**********************************************************************/
void WriteSensorSignalsToSocket(struct FSWType *FSW,int Socket)
{
      char line[512];
      int Success;

      /* Don't forget the \n at the end.  It's important. */
      sprintf(line,"%ld %lf %lf %lf %ld %ld %ld %ld\n",
         FSW->SunValid,
         FSW->wbn[0],FSW->wbn[1],FSW->wbn[2],
         FSW->Css[0].Counts,FSW->Css[1].Counts,
         FSW->Css[2].Counts,FSW->Css[3].Counts);

      Success = send(Socket,line,strlen(line),0);
}
/**********************************************************************/
long ReadActuatorCmdsFromSocket(struct FSWType *FSW,FILE *Stream)
{
      char line[512];

      fgets(line,511,Stream);
      if (sscanf(line,"%lf %lf %lf",
         &FSW->Twhlcmd[0],&FSW->Twhlcmd[1],&FSW->Twhlcmd[2]) == 3) {
         //printf("%s",line);
         return(1);
      }
      else return(0);
}
/**********************************************************************/
/*  This function is called at the simulation rate.  Sub-sampling of  */
/*  control loops should be done on a case-by-case basis.             */
/*  Mode handling, command generation, error determination, feedback  */
/*  and failure detection and correction all fall within the scope of */
/*  this file.                                                        */
/**********************************************************************/
void FlightSoftWare(struct SCType *S)
{
      long FswSocketEnabled = 1;
      struct FSWType *FSW;

      FSW = &S->FSW;

      if (FSW->Init) {
         FSW->Init = 0;
         if (IpcMode != IPC_RX && FswSocketEnabled) {
            FSW->Socket = InitSocketServer(42420+S->Tag,TRUE);
            FSW->Stream = fdopen(FSW->Socket,"r+");
         }
      }

      switch(S->FswTag){
         case PASSIVE_FSW:
            break;
         case PROTOTYPE_FSW:
            PrototypeFSW(S);
            break;
         case AD_HOC_FSW:
            AdHocFSW(S);
            break;
         case CUBESAT_FSW:
            if (IpcMode != IPC_RX && FswSocketEnabled) {
               WriteSensorSignalsToSocket(FSW,FSW->Socket);
               ReadActuatorCmdsFromSocket(FSW,FSW->Stream);
            }
            else {
               CubeSatFSW(FSW);
            }
            break;
      }


}

//#ifdef __cplusplus
//}
//#endif

