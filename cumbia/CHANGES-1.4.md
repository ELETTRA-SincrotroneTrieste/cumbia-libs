# cumbia-libs 1.4 code optimization

## cumbia

In order to remove locks and make the code simpler, faster and more reliable, data structures managed by CuActivityManager
and CuThreadService, that used to be shared among CuThread and the rest of the world, have now been duplicated so that 
CuThread owns a copy of what is needed and does not access removeConnection and CuThreadService.

This implies:

#### Cumbia
- Cumbia::unregisterActivity removes activity connections immediately and removes thread from CuThreadService. The CuThread is
  notified in order to exit the activity and release private resources linked to thread's activities.
  
#### CuActivity
- CuActivity holds a reference to the owning thread instead of the whole activity manager. This is possible because one activity
  belongs to one thread only.
 
#### CuThread
- CuThread has no references to neither CuThreadService nor CuActivityManager
- An additional multimap holding activity --> listeners pairs has been added to CuThread to replicate the connections that once were
  fetched from CuActivityManager (that would require locks)

##### Notes
Timer based CuThreads with the same period shall use one instance of the same timer.
CuThread must refer to the global CuTimerService, which members shall still be lock protected.

  
### Benefits

- Locks removed from CuActivityManager
- Locks removed from CuThreadService
- CuActivity holds a reference to its thread instead of the entire activity manager: no need to perform searches across the activity manager.
