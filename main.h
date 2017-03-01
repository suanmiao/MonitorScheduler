/**
* Created by hongkunl on 25/01/2017.
*/
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <sqlite3.h>
#include <sys/time.h>
#include <semaphore.h>
#include <float.h>
#include <stdbool.h>
#include<signal.h>
#include "db_util.h"


#define TASK_TYPE_REQUEST_TIME 2
#define TASK_TYPE_VM_SIZE 3

#define MATRIX_REQUEST_TIME 0
#define MATRIX_VM_TOTAL_SIZE 1
#define MATRIX_VM_USED_SIZE 2

#define RECORD_TYPE_NORMAL 7
#define RECORD_TYPE_AGG_MIN 8
#define RECORD_TYPE_AGG_MAX 9
#define RECORD_TYPE_AGG_MEAN 10
#define RECORD_TYPE_AGG_COUNT 11

/**
 * the data structure to store the task information
 */
struct task_t{
    double frequency;
    int task_type;

};

static int sql_callback(void *data, int argc, char **argv, char **azColName);

void task_request_time();

void task_memory_monitor();

void execute_task(int task_type);

void store_result(int task_type, float value);


char *get_table_from_type(int matrix_type);

long get_timestamp_ms();

void *func_thread(void *arg);

void load_aggregated_matrix();


