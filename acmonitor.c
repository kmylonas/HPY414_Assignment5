#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/md5.h>
#include "mystruct.h"

#define FIELDS 8

typedef struct entry {

	int uid; /* user id (positive integer) */
	int access_type; /* access type values [0-2] */
	int action_denied; /* is action denied values [0-1] */

	char date[11]; /* file access date */
	char time[9]; /* file access time */

	char *file; /* filename (string) */
	int fingerprint[16]; /* file fingerprint */

	/* add here other fields if necessary */
	/* ... */
	/* ... */

} entry_t;

/* TOOLS */





void
usage(void)
{
	printf(
	       "\n"
	       "usage:\n"
	       "\t./monitor \n"
		   "Options:\n"
		   "-m, Prints malicious users\n"
		   "-i <filename>, Prints table of users that modified "
		   "the file <filename> and the number of modifications\n"
		   "-h, Help message\n\n"
		   );

	exit(1);
}

int entries_num(FILE* file){

	int lines=0;
	char ch;

	while(!feof(file)){
		ch = fgetc(file);
		if(ch == '\n')
			lines++;

	}

	fseek(file, 0, SEEK_SET);
	return (lines+1)/FIELDS;

}

int read_file(FILE* file, entry_t* entries, int nentries){


	char dummy[50];
	int wtf;
	for(int i=0; i<nentries; i++){
		entries[i].file = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
		fscanf(file, "%s %d", dummy, &entries[i].uid);
		fscanf(file, "%s %s %s", dummy, dummy, entries[i].file);
		fscanf(file, "%s %s", dummy, entries[i].date);
		fscanf(file, "%s %s", dummy, entries[i].time);
		fscanf(file, "%s %s %d", dummy, dummy, &entries[i].access_type);
		fscanf(file, "%s %s %s %s %d", dummy, dummy, dummy, dummy, &entries[i].action_denied);
		fscanf(file, "%s " , dummy);
		for(int j=0; j<MD5_DIGEST_LENGTH; j++)
			fscanf(file, "%d", &entries[i].fingerprint[j]);
		fscanf(file, "%s", dummy);

	}

	return nentries;
}

void copy_fingerprint(int dst[], int src[]){
	
	for(int i=0; i<MD5_DIGEST_LENGTH; i++){
		dst[i] = src[i];
	}


}

/* 0 when they are the same */
int compare_fingerprint(int f1[], int f2[]){

	for(int i=0; i<MD5_DIGEST_LENGTH; i++){
		if(f1[i]!=f2[i])
			return 1;
	}
	
	return 0;
}

void init_prev_f(int f[]){
	for(int i=0; i<MD5_DIGEST_LENGTH; i++){
		f[i] = 0;
	}
}


void 
list_unauthorized_accesses(FILE *log)
{
	int nentries = entries_num(log);
	entry_t* entries = (entry_t *)malloc(sizeof(entry_t)*nentries);
	read_file(log, entries, nentries);
	
	int nmal_users=0;
	mal_user_t* mal_users = NULL;
	mal_user_t* mal_user = NULL;

	for(int i=0; i<nentries; i++){
		
		if(entries[i].action_denied == 1){
			
			if((mal_user = find(mal_users, nmal_users, entries[i].uid)) == NULL){
				nmal_users++;
				insert(&mal_users, nmal_users, entries[i].uid);
				mal_user = &mal_users[nmal_users-1];
				log_file(&mal_user, entries[i].file);
			}
			else{
				if(!is_file_logged(mal_user, entries[i].file)){
					log_file(&mal_user, entries[i].file);
				}
			}
		}
		
	}

	print_users(mal_users, nmal_users, 0);

	free(entries);
	return;

}


void
list_file_modifications(FILE *log, char *file_to_scan)
{
	int prev_f[MD5_DIGEST_LENGTH], cur_f[MD5_DIGEST_LENGTH];
	int nentries = entries_num(log), nmal_users=0;
	
	entry_t* entries = (entry_t *)malloc(sizeof(entry_t)*nentries);
	read_file(log, entries, nentries);
	
	
	mal_user_t* mal_users = NULL;
	mal_user_t* mal_user = NULL;
	
	init_prev_f(prev_f);

	for(int i=0; i<nentries; i++){
		if(entries[i].action_denied == 1 || entries[i].access_type!=2 || strcmp(entries[i].file, file_to_scan)!=0)
			continue;
		
		copy_fingerprint(cur_f, entries[i].fingerprint);		

		if((mal_user = find(mal_users, nmal_users, entries[i].uid)) == NULL){
			nmal_users++;
			insert(&mal_users, nmal_users, entries[i].uid);
			mal_user = &mal_users[nmal_users-1];			
		}

		if(compare_fingerprint(prev_f, cur_f)!=0){
			mal_user->ac_num++;
			copy_fingerprint(prev_f, cur_f);
		}


	}

	print_users(mal_users, nmal_users,1);
	free(entries);

	return;

}

int check_time(char* timestamp, int* count){


	int h, m, s, curh, curm, curs;
	time_t rawtime;
 	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
		
	curh = timeinfo->tm_hour;
	curm = timeinfo->tm_min;
	curs = timeinfo->tm_sec;
	
	h = atoi(strtok(timestamp, ":"));
	m = atoi(strtok(NULL, ":"));
	s = atoi(strtok(NULL, ":"));
	

	if(curh == h){
		if(curm - m < 20)
			*count = *count + 1;
	}
	else if(curh - h == 1){
		if(m-curm>0 && m-curm>40)
			*count = *count + 1;
	}

	return 0;
}

void list_files_num(FILE* log, int nfiles){
	
	int nentries = entries_num(log);
	char timestamp[9];
	int count = 0;
	
	entry_t* entries = (entry_t *)malloc(sizeof(entry_t)*nentries);
	read_file(log, entries, nentries);

	for(int i=0; i<nentries; i++){
	
		if(entries[i].access_type == 0){
			
			check_time(entries[i].time, &count);

		}
		
	}
	printf("Number of files created in the last 20 minutes: %d\n", count);
	if(count>nfiles) printf("Suspect malicious activity\n");
	else printf("Not suspicious\n");
}


void list_encrypted_files(FILE* log){

	int nentries = entries_num(log);
	entry_t* entries = (entry_t *)malloc(sizeof(entry_t)*nentries);
	read_file(log, entries, nentries);
	char* token, *prev;

	mal_user_t* mal_user = NULL;
	insert(&mal_user,1, 0);
	
	for(int i=0; i<nentries; i++){
		token = strtok(entries[i].file, ".");
		while(token!=NULL){
			prev = token;
			token = strtok(NULL,".");
		}
		if(strcmp(prev, "encrypt")==0){
			if(is_file_logged(mal_user,entries[i].file))
				continue;
			else{
				log_file(&mal_user, entries[i].file);
				printf("%s\n", entries[i].file);
			}

			
		}
			
	}

}

int 
main(int argc, char *argv[])
{

	int ch;
	int num_of_files;
	FILE *log;

	if (argc < 2)
		usage();

	log = fopen("./file_logging.log", "r");
	if (log == NULL) {
		printf("Error opening log file \"%s\"\n", "./log");
		return 1;
	}
	

	while ((ch = getopt(argc, argv, "hi:v:me")) != -1) {
		switch (ch) {		
		case 'i':
			list_file_modifications(log, optarg);
			break;
		case 'm':
			list_unauthorized_accesses(log);
			break;
		case 'v':
			num_of_files = atoi(optarg);
			list_files_num(log, num_of_files);
			//prints total number of files created in the last 20 min
			break;
		case 'e':
			list_encrypted_files(log);
			// prints the files that were encrytped
			break;
		default:
			usage();
		}

	}


	fclose(log);
	argc -= optind;
	argv += optind;	
	

	return 0;

}



