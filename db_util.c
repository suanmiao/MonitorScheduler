/**
 * Created by hongkunl on 28/02/2017.
 * this header and it's source file contains all interface for interacting with database
 *
 */

#include "db_util.h"


sqlite3 *init_db(int (*sql_callback)(void*,int,char**,char**)){
    sqlite3 *db;
	int rc;             // return code
	char *errmsg = 0;       // pointer to an error string

	/*
     * open SQLite database file matrix.db
     */
	rc = sqlite3_open("matrix.db", &db);
	if (rc != SQLITE_OK) {
		printf("ERROR opening SQLite DB in memory: %s\n", sqlite3_errmsg(db));
        return NULL;
	}
	printf("[INFO] Opened SQLite handle successfully.\n");

    if(create_table(db, "REQ_TIME", errmsg, sql_callback)){
		fprintf(stderr, "Unable to create table %s, error message %s \n", "REQ_TIME", errmsg);
	}

    if(create_table(db, "VM_TOTAL", errmsg, sql_callback)){
		fprintf(stderr, "Unable to create table %s, error message %s \n", "VM_TOTAL", errmsg);
	}

    if(create_table(db, "VM_USED", errmsg, sql_callback)){
		fprintf(stderr, "Unable to create table %s, error message %s \n", "VM_USED", errmsg);
	}
	return db;
}

int create_table(sqlite3 *db, char *table_name, char *errmsg, int (*sql_callback)(void*,int,char**,char**)){

	char sql[200];
	/* Create TABLE statement */
	sprintf(sql, "CREATE TABLE IF NOT EXISTS %s ("
         "TIME         DECIMAL ," \
         "VALUE        DECIMAL 	 NOT NULL," \
         "TYPE         INT 	);", table_name);

	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql, sql_callback, 0, &errmsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
        return 1;
	}
	return 0;
}

int insert_record(sqlite3 *db, char * table_name, long time, float value, int type, int (*sql_callback)(void*,int,char**,char**)){
	char sql[200];
	char *zErrMsg = 0;
	sprintf(sql, "INSERT INTO %s (TIME, VALUE, TYPE) VALUES (%ld, %f, %d); ", table_name, time, value, type);
	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
        return 1;
	}
	printf("[INFO] Insert Record Success (table %s, type %d, value %f)\n", table_name, type, value);
	return 0;
}


int delete_record(sqlite3 *db, char * table_name, int type, int (*sql_callback)(void*,int,char**,char**)){
	char sql[200];
	char *zErrMsg = 0;
	sprintf(sql, "DELETE FROM %s WHERE TYPE = %d; ", table_name, type);
	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
        return 1;
	}
	printf("[INFO] Delete Record Success (table %s, type %d )\n", table_name, type);
	return 0;
}


int insert_or_update_record(sqlite3 *db, char * table_name, long time, float value, int type, int (*sql_callback)(void*,int,char**,char**)){
 	char sql_delete[200];
	char *zErrMsg = 0;
	sprintf(sql_delete, "DELETE FROM %s WHERE TYPE = %d; INSERT INTO %s (TIME, VALUE, TYPE) VALUES (%ld, %f, %d); ", table_name, type, table_name, time, value, type);
	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql_delete, sql_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
        return 1;
	}

	printf("[INFO] Update Record Success (table %s, type %d, value %f)\n", table_name, type, value);
    return 0;
}


int select_record(sqlite3 *db, char * table_name,int type, int (*sql_callback)(void*,int,char**,char**)){
	char sql[200];
	char *zErrMsg = 0;
	sprintf(sql, "SELECT * FROM %s WHERE TYPE = %d; ", table_name, type);
	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
        return 1;
	}
	return 0;
}


int select_all(sqlite3 *db, char * table_name, int (*sql_callback)(void*,int,char**,char**)){
	char sql[200];
	char *zErrMsg = 0;
	sprintf(sql, "SELECT * FROM %s;", table_name);
	/* Execute SQL statement */
	int rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
        return 1;
	}
	return 0;
}



