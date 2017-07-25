The OSAL black box unit tests tests were run in the following pc-linux environment:

Linux wirbelwind 4.3.3-gentoo #2 SMP PREEMPT Sun Dec 20 15:26:01 EST 2015
x86_64 Intel(R) Core(TM) i7-6700K CPU @ 4.00GHz GenuineIntel GNU/Linux

unit tests tests built with gcc-5.3 and glibc 2.21-r1

The expected results of the black box unit tests in a pc-linux environment are 
provide in the _log.txt file for each test.

The Init-not-call-first failures are expected due to the missing call to 
OS_API_Init() in the pc-linux BSP.

Two additional failures are produced in the oscore test:

FAILED [ ] OS_FPUExcSetMask - #1 Nominal 
FAILED [ ] OS_FPUExcGetMask - #1 Null-pointer-arg

These tests are not applicable to pc-linux environments

====================================================================================