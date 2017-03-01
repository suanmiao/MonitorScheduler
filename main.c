/**
 * Created by hongkunl on 28/02/2017.
 *
 */
#include "main.h"
#include<time.h>

sqlite3 *db;
float agg_min[3];
float agg_max[3];
float agg_mean[3];
float agg_count[3];
int update_index = -1;

//the handler to handle user's exit
void sig_handler(int signo)
{
    //when user exit this app, or when the app is killed
	if (signo == SIGINT || signo == SIGKILL){
		//close db
		printf("DB closed, Exiting \n");
		sqlite3_close(db);
		exit(0);
	}
}

/**
 * the main function for this file
 * @param argc count of arguments
 * @param argv list arguments
 * @return the status of execution
 */
int main(int argc, char **argv) {
	db = init_db(sql_callback);
    if(db == NULL){
		return -1;
	}
	load_aggregated_matrix();

    //handle signal
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);

    //start scheduling tasks according to input options
    int i = 1;
	pthread_t tid[(argc - 1)/2];
    for(; i + 1 < argc; i += 2){
		char *option = argv[i];
		float frequency = (float)atof(argv[i + 1]);
		struct task_t *task = malloc(sizeof(struct task_t));
		task->frequency = frequency;
		if(strcmp(option, "-r") == 0){
			task->task_type = TASK_TYPE_REQUEST_TIME;
			fprintf(stdout, "[INFO] start task '%s', frequency %f \n", "Monitor Request Time", frequency);
		}else if(strcmp(option, "-v") == 0){
			fprintf(stdout, "[INFO] start task '%s', frequency %f \n", "Monitor Virtual Memory Total/Usage", frequency);
			task->task_type = TASK_TYPE_VM_SIZE;
		}
		pthread_create(&tid[(i - 1) / 2], NULL, (void *) func_thread, (void *) task);
	}
	//join these thread, thus waiting for their finish
    for(i = 0; i < (argc - 1)/2; i++){
		pthread_join(tid[i], NULL);
	}

	return 0;
}

/**
 * the thread method for handing concurrency
 * @param arg the argument passed when creating the thread
 * @return void result
 */
void *func_thread(void *arg) {
	struct task_t *task = (struct task_t *) (arg);

	//in millisecond
	long target_interval = 1000000 / task->frequency;
    while(1){
		long time_start = get_timestamp_ms();
	    execute_task(task->task_type);
        long time_end = get_timestamp_ms();

        long duration = time_end - time_start;
		if(duration >= target_interval){
			continue;
		}

        //time rest in this interval, in microsecond
		long t_sleep = target_interval - duration;

    	usleep(t_sleep);
	}
}

/**
 * execute different kinds of tasks
 * @param task_type the type of task
 */
void execute_task(int task_type){
	switch(task_type){
		case TASK_TYPE_REQUEST_TIME:
			task_request_time();
			break;
		case TASK_TYPE_VM_SIZE:
			task_memory_monitor();
			break;
	}
}

/**
 * load aggregated matrix into memory at the application startup. Then we don't have to read it from DB everytime
 * It's like a write-back cache
 */
void load_aggregated_matrix(){
	    //load aggregated data into memory
	int i = 0;
	for(;i < 3; i++){
		agg_min[i] = FLT_MAX;
		agg_max[i] = FLT_MIN;
		agg_mean[i] = 0;
		agg_count[i] = 0;
	}

	update_index = MATRIX_REQUEST_TIME;

	select_record(db, get_table_from_type(MATRIX_REQUEST_TIME), RECORD_TYPE_AGG_MAX, sql_callback);
	select_record(db, get_table_from_type(MATRIX_REQUEST_TIME), RECORD_TYPE_AGG_MIN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_REQUEST_TIME), RECORD_TYPE_AGG_MEAN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_REQUEST_TIME), RECORD_TYPE_AGG_COUNT, sql_callback);

	update_index = MATRIX_VM_TOTAL_SIZE;
	select_record(db,get_table_from_type(MATRIX_VM_TOTAL_SIZE), RECORD_TYPE_AGG_MAX, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_TOTAL_SIZE), RECORD_TYPE_AGG_MIN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_TOTAL_SIZE), RECORD_TYPE_AGG_MEAN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_TOTAL_SIZE), RECORD_TYPE_AGG_COUNT, sql_callback);

	update_index = MATRIX_VM_USED_SIZE;
 	select_record(db,get_table_from_type(MATRIX_VM_USED_SIZE), RECORD_TYPE_AGG_MAX, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_USED_SIZE), RECORD_TYPE_AGG_MIN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_USED_SIZE), RECORD_TYPE_AGG_MEAN, sql_callback);
	select_record(db,get_table_from_type(MATRIX_VM_USED_SIZE), RECORD_TYPE_AGG_COUNT, sql_callback);

	//reset the index to avoid further update
	update_index = -1;

}

/**
 * the task to check request time to "google.com"
 */
void task_request_time(){
    long start_time = get_timestamp_ms();

	int sockfd, rv;
	struct sockaddr_in srv;

	char *hostname = "google.com";

	struct hostent  *he;

	/* resolve hostname */
	if ( (he = gethostbyname(hostname) ) == NULL ) {
		exit(1); /* error */
	}

	unsigned short port = 80;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);    // TCP/IP socket
	if (sockfd < 0) {
		fprintf(stderr, "Initialize socket error\n");
		err(1, 0);            // in case of error
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
		err(1, 0);
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &(int) {1}, sizeof(int)) < 0)
		err(1, 0);

	// setup address structure to point to server
	memset(&srv, 0, sizeof(srv));            // clear it first
	srv.sin_family = AF_INET;            // IP family

	/* copy the network address to sockaddr_in structure */
	memcpy(&srv.sin_addr, he->h_addr_list[0], he->h_length);


	srv.sin_port = htons(port);            // server port

	// actually connect to the server
	rv = connect(sockfd, (struct sockaddr *) &srv, sizeof(struct sockaddr));
	if (rv < 0) {
		fprintf(stderr, "Socket connect error with errno %d\n", errno);
		err(1, 0);
	}
    close(sockfd);
	long end_time = get_timestamp_ms();

    //duration in millisecond
	float duration = (float)(end_time - start_time);
    store_result(MATRIX_REQUEST_TIME, duration);
}

/**
 * the task to monitor current memory usage and total memory
 */
void task_memory_monitor(){
	struct sysinfo memInfo;

	sysinfo (&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;

	//total virtual memory, in Megabyte
	float totalVM =  (float)(totalVirtualMem / (1024 * 1024));
	store_result(MATRIX_VM_TOTAL_SIZE, totalVM);

	long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
	virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
	virtualMemUsed *= memInfo.mem_unit;

	//used virtual memory, in Megabyte
	float usedVM =  (float)(virtualMemUsed / (1024 * 1024));

	store_result(MATRIX_VM_USED_SIZE, usedVM);
}


/**
 * store matrix into the database. The input is raw matrix, which means it doesn't input aggregated matrix
 * because aggregated matrix is handled inside the function
 * @param matrix_type the type of the matrix
 * @param value the decimal value of the matrix
 */
void store_result(int matrix_type, float value){
	long current_time = get_timestamp_ms();
	//insert the record
    insert_record(db, get_table_from_type(matrix_type), current_time, value, RECORD_TYPE_NORMAL, sql_callback);

	//update aggregate
	float current_min = agg_min[matrix_type];
	float current_max = agg_max[matrix_type];
	float current_mean = agg_mean[matrix_type];
	float current_count = agg_count[matrix_type];

	if(value < current_min){
		//update min value
		agg_min[matrix_type] = value;
		insert_or_update_record(db, get_table_from_type(matrix_type), current_time, value, RECORD_TYPE_AGG_MIN, sql_callback);
	}
	if(value > current_max){
		//update max value
		agg_max[matrix_type] = value;
		insert_or_update_record(db, get_table_from_type(matrix_type), current_time, value, RECORD_TYPE_AGG_MAX, sql_callback);
	}
	//update count value
	agg_count[matrix_type] += 1;
	insert_or_update_record(db, get_table_from_type(matrix_type), current_time, agg_count[matrix_type], RECORD_TYPE_AGG_COUNT, sql_callback);

	//update mean
	//divide first, to avoid overflow
	agg_mean[matrix_type] = current_mean / (current_count + 1) * current_count + value / (current_count + 1);

	insert_or_update_record(db, get_table_from_type(matrix_type), current_time, agg_mean[matrix_type], RECORD_TYPE_AGG_MEAN, sql_callback);
}

/**
 * return the actual table name of that matrix type
 * @param matrix_type the type of the matrix
 * @return the name of the corresponding table
 */
char *get_table_from_type(int matrix_type){
	switch(matrix_type){
		case MATRIX_REQUEST_TIME:
			return "REQ_TIME";
		case MATRIX_VM_USED_SIZE:
			return "VM_USED";
		case MATRIX_VM_TOTAL_SIZE:
			return "VM_USED";
	}
    return NULL;
}

/**
 * the callback for the sql execution
 * @param data
 * @param argc
 * @param argv
 * @param azColName
 * @return
 */
static int sql_callback(void *data, int argc, char **argv, char **azColName){
	int i;
	bool hit = false;
    float value;
	int matrix_type = -1;
	fprintf(stderr, "%s: ", (const char*)data);
	for(i=0; i<argc; i++){
		printf("[INFO] Callback message from DB: %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		if(argv[i]){
			if(strcmp(azColName[i], "VALUE") == 0){
    			value = (float)atof(argv[i]);
    			hit = true;
    		}else if(strcmp(azColName[i], "TYPE") == 0){
                matrix_type = atoi(argv[i]);
    		}
		}
	}
    if(update_index != -1 && matrix_type != -1 && hit){
		switch(matrix_type){
			case RECORD_TYPE_AGG_MAX:
				agg_max[update_index] = value;
				break;
			case RECORD_TYPE_AGG_MIN:
				agg_min[update_index] = value;
				break;
			case RECORD_TYPE_AGG_MEAN:
				agg_mean[update_index] = value;
				break;
			case RECORD_TYPE_AGG_COUNT:
				agg_count[update_index] = value;
				break;
		}
	}
	printf("\n");
	return 0;
}

long get_timestamp_ms() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*(unsigned long)1000+tv.tv_usec/1000;
}


