# avoid_priority_inversion

This project focuses on the implementation of an embedded application capable of performing different tasks based on user input. One of the problems the system faces implementing something like this is resource management. More specifically, in concurrent systems where multitasking is involved, resources are shared. When tasks simultaneously try to access the resource, the system might generate unexpected and the wrong results or even crash. The critical sections are the part where the shared resources are accessed from and these need to be protected to ensure the system runs error-free. The problem can be referred to as priority inversion where a lower priority task inadvertently blocks a higher priority task from running since it is accessing the critical section (shared resource). The methodology used to solve this problem is called dynamic priority elevation. As the name suggests, what this does is dynamically change the priority of the task before calling the next one to avoid priority inversion. After the next task is done executing, the priority is reverted to allow for normal execution of the system. The system implements this in two ways, using mutexes and semaphores based on the user input. The expected results would be for the highest priority task to run without being blocked by the lower priority task. The other part of this system involves joinable threads where one job can be divided into different threads which can run in parallel then be combined at the end. This is implemented though multithreading.

The first problem the embedded application solves is that of priority inversion. This occurs when for example a lower priority thread T3, blocks a higher priority thread T1 because T3 is using the shared critical section. While this is happening, a normal priority thread T2 pre-empts T3 so it can execute until it gets blocked by the higher priority thread T1. However, T1 is still waiting on T3 to finish so T1 will continue to be blocked thus causing priority inversion. The methods used to overcome this problem are mutexes and semaphores. RTX has both already built in which makes it very easy to implement through the available functions.

The second method this system implements is joinable threads. In this, a single job is divided into multiple different threads, in this system specifically 3 threads, and then are ran concurrently. Again, this is built into RTX where the thread management allows the joining of threads with each other. Threads wait until all the joined threads are finished and enter a waiting period if the thread they are trying to join is not finished executing yet. Joinable threads is also implemented in two different ways which the user selects via the joystick. The two are using Rate Monotonic Scheduling and Round Robin to test the difference by using different scheduling techniques.