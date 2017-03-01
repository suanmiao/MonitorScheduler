/**
 * Created by hongkunl on 28/02/2017.
 * this header and it's source file contains all interface for interacting with database
 *
 */
#include <sqlite3.h>
#include <err.h>
#include <stdio.h>


sqlite3 *init_db(int (*sql_callback)(void*,int,char**,char**));

int create_table(sqlite3 *db, char *table_name, char *errmsg,  int (*sql_callback)(void*,int,char**,char**));

int insert_record(sqlite3 *db, char * table_name, long time, float value, int type,  int (*sql_callback)(void*,int,char**,char**));

int delete_record(sqlite3 *db, char * table_name, int type,  int (*sql_callback)(void*,int,char**,char**));

int insert_or_update_record(sqlite3 *db, char * table_name, long time, float value, int type,  int (*sql_callback)(void*,int,char**,char**));

int select_record(sqlite3 *db, char * table_name,int type,  int (*sql_callback)(void*,int,char**,char**));

int select_all(sqlite3 *db, char * table_name,  int (*sql_callback)(void*,int,char**,char**));


