/*
** $Id: $
**
** Title: Heater Control (HC) Version Information Header File
**
** Purpose: Specification for the CFS HC application version label
**          definitions
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes: Major revision by Christel A. Gesterling (18.Nov.2016)
** 	  Changes to how HC Component Table is organized,
**	  how temperature information is parsed from HSIM,
**	  how average temperatures are computed and stored,
**	  and added capability for Propulsion control
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**    CFS Flight Software Development Standards Version 0.11
**
** $Log: $
*/

#ifndef _hc_version_h_
#define _hc_version_h_

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
#define HC_MAJOR_VERSION     2
#define HC_MINOR_VERSION     0
#define HC_REVISION          0

#endif /* _hc_version_h_ */

/************************/
/*  End of File Comment */
/************************/

