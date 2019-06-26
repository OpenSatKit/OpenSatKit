NOTE: By default ION uses SIGALRM, as does PSP, which causes a conflict.
A version of PSP was created in branch trac-76-posix_timer that replaces
the logic with posix timers; or if you can build ION without the alarm-based
logic, you should have success. (I was unable to get ION to build with
the POSIX_SEMAPHORES option...tricky with a *lot* of compiler option switches.)
