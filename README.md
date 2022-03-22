# IPC communication

Description
-----------
The project is a collection of 4 concurrent programs whose aim is to measure the speed of data transferring between a producer and a consumer using 4 different IPC transfer primitives, evaluating then their efficiency. 
The four programs may be run by one interface after an interactive choice.

How the communication works
---------------------------
The data will be transferred by an `array A` filled in the producer with random data and transferred in an array B of the same dimension in the consumer process; in shared memory IPC a circular buffer CB is implemented and is with that that the user will fill the array B in consumer process. 
All this is controlled by the master process which will be a UI for the user.

Sturcture
----------
Each program runs separately form the others, this way the user can choose different features of this efficiency test, namely:

* The dimension of the array A and B
* The dimension of the circular buffer (CB) 

The Master process is just creating and taking track of the consumer and producer activities.

In both producer and consumer, we find this function:
* `writetime()` that calculates the time needed by each transferring method. It writes the time of the transmission on a file called `t0` from the producer and the ending time of transmission in another file called “t1” from consumer. The time considered is the real life time taken with the already existing function `gettimeofday(&current_time, NULL)`.

While:
* `timediff()` is a consumer function that open the two files with starting and ending time of transmission and returns the difference between them.
* `Randomdata(unsigned char* data)` is a producer function that fills the array file of random data until the dimension specified form the user are reached.

Results
-------
The final result is a quite fun way to compare the different IPC and here you can find the sorting in order of efficiency:
* named pipes
* unnamed pipes
* sockets 
* shared memory
