#include "mystruct.h"

// Hold all the malicius users


// mal_user_t* init(){
//     mal_user_t* mal
// }


int insert(mal_user_t** mal_users, int new_size, uid_t uid){
    
    (*mal_users) = (mal_user_t*)realloc((*mal_users), new_size*sizeof(mal_user_t));
    
    if((*mal_users) == NULL) 
        return -1;

    
    (*mal_users)[new_size-1].user_id = uid;
    (*mal_users)[new_size-1].ac_num = 0;

    
  
    return 0;

}


mal_user_t* find(mal_user_t* mal_users, int cur_size, uid_t user_id){

    if(cur_size==0)
        return NULL;
    
    for(int i=0; i<cur_size; i++){
        if(mal_users[i].user_id == user_id)
            return &mal_users[i];
    }

    return NULL;

}

int inc_value(mal_user_t* mal_user){
    mal_user->ac_num++;
}

/* Returns 0 if it's a new file otherwise 1 */
int is_file_logged(mal_user_t* mal_user, char* filename){

    int l = mal_user->ac_num;
    int cnt = 0;
    for(int i=0; i<l; i++){
        if(strcmp(mal_user->files[i], filename)!=0)
            cnt++;
    }

    return ((cnt==l) ? 0:1);
}


void log_file(mal_user_t** mal_user, char* filename){

    int size = strlen(filename);

    
    (*mal_user)->ac_num++;
    (*mal_user)->files = (char**)realloc((*mal_user)->files, (*mal_user)->ac_num*sizeof(char*));
    (*mal_user)->files[(*mal_user)->ac_num-1] = (char*)realloc((*mal_user)->files[(*mal_user)->ac_num-1], size*sizeof(char));
    (*mal_user)->files[(*mal_user)->ac_num-1] = filename;
    
}


void print_users(mal_user_t* mal_users, int cur_size, int mode){

    for(int i=0; i<cur_size; i++){
        if(mode == 0){
            if(mal_users[i].ac_num>=7)
                printf("%d. UID: %d \t Times: %d\n", i+1, mal_users[i].user_id, mal_users[i].ac_num);
        }else
        {
            printf("%d. UID: %d \t Times: %d\n", i+1, mal_users[i].user_id, mal_users[i].ac_num);
        }
        
    }
}



/*
int main(int argc, char const *argv[])
{
    mal_user_t* mal_users = NULL;
    int size = 0;
    uid_t uid = 1000;
    int ac_num = 5;

    mal_users = (mal_user_t*)malloc(sizeof(mal_user_t));

    for(int i=0; i<5; i++){
        size++;
        insert(mal_users, size, uid++, ac_num);
    }

   mal_user_t* myuser = find(mal_users, size, 1001);
   inc_value(myuser);
   printf("New value %d\n", myuser->ac_num); 


    free(mal_users);

    return 0;
}
*/