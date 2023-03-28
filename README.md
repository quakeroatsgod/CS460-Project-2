# CS460-Project-2
## Group Members
- Griffen Agnello
- Cyrus Santiago
##  How you share data between parts of the program
There are two doubly linked lists used to share data: a ready queue and an I/O queue named `ready_queue` and `io_queue` respectively. They are initialized in the main thread and used in the input, CPU, and I/O threads. There are also a number of global variables used such as:
- `alg_type`: The type of algorithm used
- `quantum_time`: The length of the quantum being used in Round Robin
- `total_jobs`: The total number of processes logged by the input thread.
- `jobs_completed`: The number of processes completed. It has no more bursts left.
- `cpu_finished`: A flag to determine if the CPU thread has finished running.
- `input_finished`: A flag to determine if the input thread has finished running.
- `total_wait_time`: A counter to keep track of the wait time for processes on the ready queue.
- `total_turnaround_time`: A counter to keep track of the turnaround time for processes on the ready queue.
There are also two mutexes:
- `ready_mutex`: Mutex used to gain control of the ready queue.
- `io_mutex`: Mutex used to gain control of the I/O queue.
## Your approach to synchronization issues
The two aforementioned mutexes are used to prevent synchronization issues. If
a thread already has control over some mutex that another thread needs, then
the thread will continuously try to gain the mutex.
### Input Thread
This thread will try to gain control over the ready queue mutex when reading in
a `proc` from the input file. The thread will then add a node to the queue. The
thread will hold onto the mutex if there are more processes to add. If the input
thread reads `sleep` or `stop`, then the thread will release the mutex.
### CPU Thread
The thread will first try to get the ready queue mutex. If the thread cannot get the mutex, then the thread will skip the rest of the while loop and start the iteration again. When the thread gets the mutex, the thread will remove a node, update wait times on the ready queue, and then release the ready queue mutex. After performing a CPU burst, the thread will try to get the ready queue mutex (if using Round Robin and the quantum expired) or the I/O queue if the burst has finished. Either mutex will be released after adding to the respective queue.
### I/O Thread
The thread will first try to get the I/O queue mutex. If the thread cannot get the mutex, then the thread will skip the rest of the while loop and start the iteration again. When the thread gets the mutex, the thread will remove the first node and then release the ready queue mutex. After performing the I/O burst, the thread will try to get the ready queue mutex. The mutex will be released after adding to the ready queue.
## How you switch between scheduling algorithms
The arguments follow the same format as the assignment directions. It looks like this: `./bin/exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]`. You may do `-alg FCFS`, `-alg SJF`, `-alg PR`, or `-alg RR`.
## How you generate data for the required measurements
- Waiting Time: When performing a CPU burst, the ready queue will be iterated through. For each iteration, the length of the CPU burst will be added to the total waiting time. At the end, the total waiting time is divided by the number of jobs.
- Turnaround Time: For each CPU burst, the time is updated by adding the length of the burst time. The waiting times are also added to the turnaround time. 
- Throughput: The total number of jobs divided by the total runtime. 
## The purposes of any threads beyond the three required ones
N/A