core Flight System (cFS) Memory Dwell Application (MD)
======================================================

Open Source Release Readme
==========================

MD Release 2.3.3

Date:
10/7/2020

Introduction
-------------
  The Memory Dwell application (MD) is a core Flight System (cFS) application
  that is a plug in to the Core Flight Executive (cFE) component of the cFS.

  The MD application monitors memory addresses accessed by the CPU. This task
  is used for both debugging and monitoring unanticipated telemetry that had
  not been previously defined in the system prior to deployment.

  The MD application is written in C and depends on the cFS Operating System
  Abstraction Layer (OSAL) and cFE components. There is additional MD application
  specific configuration information contained in the application's Doxygen
  user's guide available in https://github.com/nasa/MD/tree/master/docs.

  This software is licensed under the Apache 2.0 license.


Software Included
------------------
  Memory Dwell application (MD) 2.3.3


Software Required
------------------

 Operating System Abstraction Layer 5.0.0 or higher can be
 obtained at https://github.com/nasa/osal

 core Flight Executive 6.8.0 or higher can be obtained at
 https://github.com/nasa/cfe

 Note: An integrated bundle including the cFE, OSAL, and PSP can
 be obtained at https://github.com/nasa/cfs


About cFS
-----------
  The cFS is a platform and project independent reusable software framework and
  set of reusable applications developed by NASA Goddard Space Flight Center.
  This framework is used as the basis for the flight software for satellite data
  systems and instruments, but can be used on other embedded systems.  More
  information on the cFS can be found at http://cfs.gsfc.nasa.gov
