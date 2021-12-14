#define _GNU_SOURCE
#define MAX_FILENAME_SIZE 100

#include <time.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <errno.h>

#define logfile "file_logging.log"


void write_log(uid_t uid, const char* path, int at, int flag, unsigned char* md);
int fsize(FILE* fp);
void read_file(FILE* fp, unsigned char* cont, int s);
char* get_filename(FILE* fp);
unsigned char* generate_fingerprint(FILE* fp);
void to_string(unsigned char* cont, char* str);




/* This fopen is the first to be called as LD_PRELOAD is used */
FILE *
fopen(const char *path, const char *mode)
{
	char* name;

	FILE *original_fopen_ret;
	FILE *(*original_fopen)(const char*, const char*);
	
	int a_type;
	int d_flag;
	unsigned char* md = NULL;

	/* call the original fopen function */
	original_fopen = dlsym(RTLD_NEXT, "fopen");

	uid_t uid = getuid();
	a_type = access(path, F_OK) == 0 ? 1 : 0;
	// open the user requested file 
	if( (original_fopen_ret = (*original_fopen)(path, mode)) == NULL){
		name = get_filename(original_fopen_ret);
		if(errno == EACCES || errno == ENOENT){
			perror("");

			d_flag = 1;
			write_log(uid, name, a_type, d_flag, NULL);
		}
	}
	else{	
		// Access Allowed
		name = get_filename(original_fopen_ret);
		d_flag = 0;
		if(access(path, R_OK) == 0){			
			md = generate_fingerprint(original_fopen_ret);
			write_log(uid, name, a_type, d_flag, md);
			free(md);
		}
		else{
			write_log(uid, name, a_type, d_flag, NULL);
		}
	}

	return original_fopen_ret;
}

FILE *
fopen64(const char *path, const char *mode)
{

	return fopen(path, mode);

}


size_t
fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{




	fseek(stream, 0, SEEK_END);
	int a_type, d_flag;
	char* filename = get_filename(stream);
	
	
	unsigned char* md = NULL;

	size_t original_fwrite_ret;
	size_t (*original_fwrite)(const void*, size_t, size_t, FILE*);

	original_fwrite = dlsym(RTLD_NEXT, "fwrite");
	
	a_type = 2;
	uid_t uid = getuid();

	if( (original_fwrite_ret = (*original_fwrite)(ptr, size, nmemb, stream)) !=nmemb){
		if(errno = EBADFD){
			d_flag = 1;
			if(access(filename, R_OK) == 0){
			md = generate_fingerprint(stream);
			write_log(uid, filename, a_type, d_flag, md);
			free(md);
			}
			else{
				write_log(uid, filename, a_type, d_flag, NULL);
			}
		}
	}
	else{
		d_flag = 0;
		if(access(filename, R_OK) == 0){
			md = generate_fingerprint(stream);
			write_log(uid, filename, a_type, d_flag, md);
			free(md);
		}
		else{
			write_log(uid, filename, a_type, d_flag, NULL);
		}
		
	}
	
	free(filename);
	return original_fwrite_ret;
}


void write_log(uid_t uid, const char* path, int at, int flag, unsigned char* md){
	time_t tim = time(NULL);
	struct tm t = *localtime(&tim);
	char* delim = "-----------------------------------------";
	char* fprnt = "Fingerprint";
	FILE *original_fopen_ret;
	FILE *(*original_fopen)(const char*, const char*);

	original_fopen = dlsym(RTLD_NEXT, "fopen");
	original_fopen_ret = (*original_fopen)(logfile, "a");

	//to_string(md, md_str);
	if( original_fopen_ret == NULL )
		perror("Failed to open logfile");



	fprintf(original_fopen_ret, "UID: %d\n", uid);
	fprintf(original_fopen_ret, "File Name: %s\n", path);	
	fprintf(original_fopen_ret, "Date: %d/%d/%d\n", t.tm_mday, t.tm_mon+1, t.tm_year+1900);
	fprintf(original_fopen_ret, "Timestamp: %d:%d:%d\n", t.tm_hour, t.tm_min, t.tm_sec);
	fprintf(original_fopen_ret, "Access Type: %d\n", at);
	fprintf(original_fopen_ret, "Is action denied flag: %d\n", flag);
	fprintf(original_fopen_ret, "%s: ", fprnt);
	if(md!=NULL){
		for(int i=0; i<MD5_DIGEST_LENGTH; i++){
			fprintf(original_fopen_ret, "%d ", md[i]);
		}	
	}
	else{
		for(int i=0; i<MD5_DIGEST_LENGTH; i++){
			fprintf(original_fopen_ret, "%d ", 0);
		}	
	}
	fprintf(original_fopen_ret, "\n%s\n", delim);

	
	fclose(original_fopen_ret);
}


/* TOOLS */


int fsize(FILE* fp){

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	return (int)size; 
}

void read_file(FILE* fp, unsigned char* cont, int s){

	for(int i=0; i<s; i++){
		cont[i] = fgetc(fp);
	}
	fseek(fp, 0, SEEK_SET);

}

char* get_filename(FILE* fp){

	int fd;
	char path[20];
	char link[1000];
	char* ptr = (char*)malloc(100*sizeof(char));

	 if( (fd = fileno(fp)) == -1){
        perror("Fileno");
        exit(EXIT_FAILURE);
    }
    else{
        sprintf(path, "/proc/self/fd/%d", fd);
        int len = readlink(path, link, 100);
		link[len] = '\0';
        char* token = strtok(link, "/");
        
		do{
            strcpy(ptr, token);
        }while( (token = strtok(NULL,"/")) != NULL);

    }
	
	return ptr;
}

unsigned char* generate_fingerprint(FILE* fp){

	

	int size = fsize(fp);
	if(size==0)
		return NULL;
	
	unsigned char* content = (unsigned char*)malloc(sizeof(char)*size);
	unsigned char* md = (unsigned char*)malloc(MD5_DIGEST_LENGTH);
	read_file(fp, content, size);
	MD5(content, size, md);
	
	free(content);
	return md;


}
