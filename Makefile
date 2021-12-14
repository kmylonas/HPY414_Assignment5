all: logger acmonitor test_aclog

logger: logger.c
	gcc -g -O0 -Wall -fPIC -shared -o logger.so logger.c -lcrypto -ldl 

acmonitor: acmonitor.o mystruct.o 
	gcc -g acmonitor.o mystruct.o -o acmonitor

acmonitor.o: acmonitor.c mystruct.h
	gcc -g -c acmonitor.c

mystruct.o: mystruct.c mystruct.h
	gcc -g -c mystruct.c

test_aclog: test_aclog.c 
	gcc -g -g test_aclog.c -o test_aclog


run: logger.so test_aclog
	LD_PRELOAD=./logger.so ./test_aclog

clean:
	rm -rf logger.so
	rm -rf test_aclog
	rm -rf acmonitor

