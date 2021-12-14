gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0

All the tasks were succesfully completed.


Step 1:

The file USAGE.txt contains information on how to use ransomware.sh
Briefly:

chmod +x ransomware.sh
./ransomware.sh <flag> <num_of_files> <directory>
e.g.
./ransomware.sh -e 5 /home/kmylonas ---> will create 5 encrypted files at /home/kmylonas/
./ransomware.sh -d 5 ./home/kmylonas ---> will decrypt 5 encrytped files at /home.kmylonas/

Encryption and decryption is done via opnessl aes 256 ecb and the password used is kostas.
This information is available inside the script.
LD_PRELOAD is embedded in the script so you don't have to worry about it.
Make sure ransomware.sh and the rest of the files (logger.so acmonitor etc.) are in the same
directory.

The files are created by executiong test_aclog which then calls fopen and fwrite.
Due to LD_PRELOAD, fopen and fwrites are logged in our logfile which will be
used later by acmonitor


Step 2:
Because openssl uses fopen64 we added function fopen64 in logger.c which calls fopen.


Step 3:
Enriched version of acmonitor contains the extra funcionality.
The same data structure is used to read the logfile's entries.
Specifically, the -v case is handled by the function list_files_num(FILE* log, int nfiles)
and the -e case is handled by list_encrypted_files(FILE* log).

A. list_files_num

First it reads the log file and saves all the entries.
Then it scans the entries to find those with Access Type == 0 which indicates files creation.
For those "creation" entries the time is checked. 
If "current" hour and "creation" hour are the same e.g 15:13:28 and 15:30:21 then 
we just check if the absolute difference of the minutes is less than or equal to 20
If yes we increment a counter to indicate the "one more file was created in the last 20 minutes"
If current hour is bigger than creation hour by 1 then we have to check whether
"creation_minute - current_minute > 40" as in this case the file was created
in the last 20 minutes. 
For example if a files was created at 15:45:10 and current time is 16:04:32
then 45-04 = 41 > 40 so the file was indeed created in the last 20 min
In any other case the file was for sure created more than 20 minutes ago.
Finally the counter is compared with <num_of_files> argument. If the
counter is bigger than num_of_files this is considered as a malicious activity.
If its not bigger its not considered as a malicious activity.
In any case the total number of files that were created in the last 20
minutes is printed.

B. list_encrypted_files
This function takes advantage of the fact that the encrypted files has the suffix ".encrypt"
So it parses the entries of the logfile and tokenizes the file names with delimiter "." (strtok is used)
If token .encrypt occurs then its obvious that the file is encrypted by the ransomware.
Because there might be many entries with the same File Name: file.encrypt, everytime a new file occurs
its logged in our data structure so we keep track of all previous files that were found. This way
we avoid having duplicate files. The name of the original file can be found by cutting the suffix
.encrypt. This way we print the names of the original files that were encrypted by the ransomware.

