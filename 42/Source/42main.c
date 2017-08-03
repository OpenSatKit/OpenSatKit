/**********************************************************************/
/*                                                                    */
/*                               42                                   */
/*                                                                    */
/*                  The Mostly Harmless Simulation                    */
/*                                                                    */
/**********************************************************************/
/*  Written by Eric Stoneking, beginning August 6, 2001               */
/*  Based on the VW, Pegasus, TK family of simulations                */
/**********************************************************************/

/* This file contains a minimal "main" function to facilitate         */
/* compiling 42 as a library to be called from other applications.    */
/* This "main" serves as the entry point for the stand-alone          */
/* application.  It calls 42's top-level executive function, "exec".  */

//#ifdef __cplusplus
//namespace _42 {
//#endif

int exec(int argc,char **argv);

int main(int argc,char **argv)
{
 	   return(exec(argc,argv));
}

//#ifdef __cplusplus
//}
//#endif

