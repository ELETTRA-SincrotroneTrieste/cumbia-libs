# cumbia-tango dedicated threads with individual device factories

Since 1.4, generic *CuThread* has been extended into *CuTThread* (instantiated by its dedicated factory).
*CuTThread* owns a personal Tango *device factory* in order to avoid the lock protected CuDeviceFactoryService
that was shared across all threads. 

The new *CuTThreadDevices* is the factory providing access to *Tango devices*, that are shared only within the same thread.
A reference to an instance of *CuTThreadDevices* is held by *CuTThread*.

This improves speed and simplifies the code. 

### Note

Remember that by default cumbia-tango creates *one thread per device*, unless CuTThreadTokenGen is used to group threads 
differently.

## qumbia-tango-controls CuTangoRegisterEngine

The *qumbia-tango-controls CuTangoRegisterEngine* class instantiates a *CumbiaTango* object using the new *CuTThread*
threads. 

## Impact on existing applications

Existing applications based on *CumbiaPool* will be automatically *upgraded* in order to use *CuTThreads + CuTThreadDevices* 
instead of simple *CuThread + CuDeviceFactoryService*.

This is made possible because *CumbiaPool* based applications use CuTangoRegisterEngine.
