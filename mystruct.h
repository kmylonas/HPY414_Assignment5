#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILE_SIZE 50

typedef struct mal_user{

    uid_t user_id;              // User ID
    int ac_num;                 // How many times denied access
    
    char** files;               // All the different files that the user accesed -- files[i] is a string (filename)
           

} mal_user_t;


//mal_user_t* init();

/* Inserts a new element at the structure.
    -1 on error -- 0 on success */
int insert(mal_user_t** mal_users, int new_size, uid_t user_id);

/* Find the element with key = user_id .
   If not found return NULL else return address of user */
mal_user_t* find(mal_user_t* mal_users, int cur_size, uid_t user_id);

// /* Set the value of the element with key = user_id */
// int set_value(uid_t user_id, int a_num);

/* Increment the value of the element with key = user_id by 1 */
int inc_value(mal_user_t* mal_user);

/* Returns 0 if its a new file, different than before */
int is_file_logged(mal_user_t* mal_user, char* filename);

void log_file(mal_user_t** mal_user, char* filename);


void print_users(mal_user_t* mal_users, int cur_size, int mode);


