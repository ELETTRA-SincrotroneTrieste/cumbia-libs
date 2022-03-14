1.4:

# CuThread

1. is it ok to call mActivityExit when repeat <=0 in CuThread::run ?

2. remove reference to thread in CuActivity: move postEvent to thread from onExit to 
   CuThread itself
