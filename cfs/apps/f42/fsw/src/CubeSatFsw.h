#ifndef __CUBESATFSW_H__
#define __CUBESATFSW_H__

/*
** These are defined for demonstration purposes.
*/

#define CUBESATFSW_SUN_TARGET_X_AXIS_PLUS    0
#define CUBESATFSW_SUN_TARGET_X_AXIS_MINUS   1
#define CUBESATFSW_SUN_TARGET_Y_AXIS_PLUS    2
#define CUBESATFSW_SUN_TARGET_Y_AXIS_MINUS   3
#define CUBESATFSW_SUN_TARGET_Z_AXIS_PLUS    4
#define CUBESATFSW_SUN_TARGET_Z_AXIS_MINUS   5
#define CUBESATFSW_SUN_TARGET_ID_MAX         5

void InitFSW(struct FSWType *FSW);
void CssProc(struct FSWType *FSW);
void CubeSatFSW(struct FSWType *FSW, unsigned SunTarget, unsigned CssFault);

#endif
