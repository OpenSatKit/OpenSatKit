/*
** $Id: $
**
** Title: Simulator (SIM) Version Information Header File
**
** Purpose: Specification for the CFS SIM application version label
**          definitions
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**    CFS Flight Software Development Standards Version 0.11
**
** $Log: $Changed major revision number to 2 to reflect major changes
**	  to HSIM app
**	  cgesterling (22.Nov.2016)
**
*/

#ifndef _sim_version_h_
#define _sim_version_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/*
**  Application Version Information:
**
**  Major.Minor.Revision.Mission_Rev
**
**  Major: Major update.  This would include major changes or new functionality.
**         Most likely will include database schema changes and interface changes.
**         Probably not backwards compatible with older versions
**
**	Minor: Minor change, may introduce new features, but backwards compatibility is mostly
**         retained.  Likely will include schema changes.
**
**  Revision: Minor bug fixes, no significant new features implemented, though a few small
**            improvements may be included.  May include a schema change.
**
**  Mission_Rev:  Used by users of the applications (nominally missions) to denote changes made
**                by the mission.  Releases from the Flight Softare Reuse Library (FSRL) should
**                use Mission_Rev zero (0).
**
*/
#define SIM_MAJOR_VERSION     2
#define SIM_MINOR_VERSION     0
#define SIM_REVISION          0

#endif /* _sim_version_h_ */

/************************/
/*  End of File Comment */
/************************/

