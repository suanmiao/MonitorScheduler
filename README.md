Periodic Task Scheduler
================================

This is a tiny periodic task scheduler in C++. It can monitor several matrix such as the trip time from the computer to "google.com". Or it can monitor the memory usage

All these features are implemented within 4 hours. There are some potential optimizations (such as a more exact time format to support more exact interval. More options on displaying log message .etc..). But given the limit of time, I finished the main framework, upon which, other optimizations can be implemented quickly

### How to use it

It has to be compiled on Ubuntu platform. And we can specify the kind of task and it's frequency by commandline arguments
option "-r" means the task "Request Time to 'google.com' ", option "-v" means the task "Virtual Memory Size and Usage"
After each option, we should specify a frequency, which can be a integer or float

* Ubuntu 16.04
* (optional) make clean
* make
* ./main -v 0.3 -r 0.4 (this means executing task "Request Time to 'google.com' " at the frequency 0.3 (3.3333s per execution) and "Virtual Memory Size and Usage" at the frequency of 0.4 (2.5s per execution))


### Supported Features

#### 1. Periodic Task 

* Customized periodic for different tasks 


#### 2. Measurement Matrix 

* Time to connect to a TCP server, e.g. google.com port 80
* Total virtual memory size and current usage
* Aggregated matrix (min, max, average)

#### 3. Resume of Tasks

* It's possible to resume previous unfinished tasks (that means continuing inserting new data for each matrix and update aggregated matrix)


#### 4. Supported Storage

* SQLite database



### Overall Design

#### Task scheduling:
* Utilize thread to support multiple tasks
* For each thread, set customized periodic parameter and use while loop to execute task after each interval

#### Aggregated Matrix
* First, I use one table to store all record related with that data
* Since each aggregated matrix is derived from a certain raw matrix, we can certain new records into represent these aggregated matrix 
* 

#### Data Storage Format
* For each record there are several attributes: [time (long), value(float), type (integer)(default, none)]
* For aggregated matrix: [time (long), value(float), type (integer)]

### References:

* [SQLite Official Reference](https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm)
* [Get Memory Info](http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process)
* [Check Time](http://stackoverflow.com/questions/8558625/how-to-get-the-current-time-in-milliseconds-in-c-programming)

More detailed explanation is in the annotation of the code

Best wishes to you, my friend (content inside the image below :)

<img src="https://raw.github.com/suanmiao/MonitorScheduler/master/readme_imgs/my_friend.jpg" width="300">


Developed By
------------

* suanmiao (Hongkun Leng)


License
-------

    Copyright 2017 suanmiao

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
