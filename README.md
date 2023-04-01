# CS460-Project-2
## Group Members
- Griffen Agnello
- Cyrus Santiago
##  How you share data between parts of the program
There are two doubly linked lists used to share data: a ready queue and an I/O queue named `ready_queue` and `io_queue` respectively. They are initialized in
the main thread and used in the input, CPU, and I/O threads. There are also a number of global variables used such as:
- `total_jobs`: The total number of processes logged by the input thread.
- `jobs_completed`: The number of processes completed. It has no more bursts left.
- `input_finished`: A flag to determine if the input thread has finished running.
- `total_wait_time`: A counter to keep track of the wait time for processes on the ready queue.
- `total_turnaround_time`: A counter to keep track of the turnaround time for processes on the ready queue.
There are also several mutexes:
- `ready_mutex`: Mutex used to gain control of the ready queue.
- `io_mutex`: Mutex used to gain control of the I/O queue.
- `tot_job_mutex`: Mutex used to gain access to `total_jobs`
- `job_complete_mutex`: Mutex used to gain access to `jobs_completed`
- `in_fin_mutex`: Mutex used to gain access to `input_finished`
## Your approach to synchronization issues
The aforementioned mutexes are used to prevent synchronization issues. If
a thread already has control over some mutex that another thread needs, then
the thread will continuously try to gain the mutex.
### Input Thread
This thread will try to gain control over the ready queue mutex when reading in
a `proc` from the input file. The thread will then add a node to the queue. The
thread will hold onto the mutex if there are more processes to add. If the input
thread reads `sleep` or `stop`, then the thread will release the mutex.
### CPU Thread
The thread will first try to get the ready queue mutex. If the thread cannot get the mutex, then the thread will skip the rest of the while loop and start the
iteration again. When the thread gets the mutex, the thread will remove a node based on the algorithm specified, update wait times on the ready queue, release
the ready queue mutex, and begin a CPU Burst. After performing a CPU burst, the thread will try to get the ready queue mutex (if using Round Robin and the
quantum expired) or the I/O queue if the burst has finished. Either mutex will be released after adding to the respective queue.
### I/O Thread
The thread will first try to get the I/O queue mutex. If the thread cannot get the mutex, then the thread will skip the rest of the while loop and start the
iteration again. When the thread gets the mutex, the thread will remove the first node and then release the ready queue mutex. After performing the I/O burst,
the thread will try to get the ready queue mutex. The mutex will be released after adding to the ready queue.
## How you switch between scheduling algorithms
The arguments follow the same format as the assignment directions. It looks like this: `./bin/exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]`. You may do `-alg FCFS`, `-alg SJF`, `-alg PR`, or `-alg RR`. 

These selections then determine how nodes are chosen from the ready queue before performing a CPU burst, based on the algorithm specified.

You may also use `make fcfs/pr/rr/sjf` to run an algorithm with the input file `input`.
## How you generate data for the required measurements
- Avg. Waiting Time: The waiting time is measured by the cpu and io threads, and is calculated by taking the difference between the time a process entered the
ready queue, and the time it runs a CPU burst; this ensures that it only calculates time in the ready queue. These numbers are added to a tally stored within
the node, which is added to `total_wait_time` before deletion. The average waiting time is that total divided by the number of jobs.
- Avg. Turnaround Time: The arrival time is stored when a `proc`is added to the ready queue by the input thread. Right before deletion, the turnaround time for
that `proc` is calculated by taking the difference between the arrival time and the time of completion. This number is added to the `total_turnaround_time`
and divided by the number of jobs to get the average turnaround time.
- Throughput: The total number of jobs divided by the total runtime calculated by the main thread. 
## The purposes of any threads beyond the three required ones
N/A