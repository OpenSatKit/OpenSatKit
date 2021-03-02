core Flight System (cFS) Data Storage Application (DS)
======================================================

Open Source Release Readme
==========================

DS Release 2.5.2

Date: 10/5/2020

Introduction
-------------
  The Data Storage application (DS) is a core Flight System (cFS) application 
  that is a plug in to the Core Flight Executive (cFE) component of the cFS.  
  The DS application is used for storing software bus messages in files. These 
  files are generally stored on a storage device such as a solid state recorder 
  but they could be stored on any file system. Another cFS application such as 
  CFDP (CF) must be used in order to transfer the files created by DS from 
  their onboard storage location to where they will be viewed and processed.

  The DS application is written in C and depends on the cFS Operating System
  Abstraction Layer (OSAL) and cFE components.  There is additional DS application
  specific configuration information contained in the application user's guide
  available in https://github.com/nasa/DS/tree/master/docs/users_guide

  This software is licensed under the Apache 2.0 license.


Software Included
------------------

  Data Storage application (DS) 2.5.2


Software Required
------------------

 Operating System Abstraction Layer 5.0 or higher can be
 obtained at https://github.com/nasa/osal

 core Flight Executive 6.8.0 or higher can be obtained at
 https://github.com/nasa/cfe

 Note: An integrated bundle including the cFE, OSAL, and PSP can
 be obtained at https://github.com/nasa/cfs

About cFS
----------
  The cFS is a platform and project independent reusable software framework and
  set of reusable applications developed by NASA Goddard Space Flight Center.
  This framework is used as the basis for the flight software for satellite data
  systems and instruments, but can be used on other embedded systems.  More
  information on the cFS can be found at http://cfs.gsfc.nasa.gov

EOF
