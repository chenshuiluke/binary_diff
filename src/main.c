#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <gmp.h>//For files of arbitrary size
#include <string.h>
#include "pythonCode.h"
void getFileSize(char file[],mpz_t * count)
{
	FILE * reader;
	mpz_init(*count);
	mpz_set_ui(*count,0);
	unsigned char temp;
	if((reader = fopen(file,"rb")) == NULL)
	{
		puts("Error opening file to get size!");
		return;
	}
	else
	{
		printf("=====Getting file size of %s=====\n", file);
		while(!feof(reader))
		{
			if(fread(&temp,sizeof(char), 1, reader) != 1)
			{
				break;
			}
			mpz_add_ui(*count,*count,1);//Add one
		}
		fclose(reader);
	}
}
void fill(char file[], mpz_t origSize, mpz_t otherSize)
{
	FILE * filler;
	mpz_t counter;
	mpz_t difference;
	int compare;
	unsigned long int offset = 100;	
	unsigned char empty[100];
	memset(empty,'\0',99);
	fpos_t pos;
	mpz_init(counter);
	mpz_init(difference);
//	mpz_sub_ui(origSize,origSize,1);

	mpz_set(counter,otherSize);
	if(!(filler = fopen(file,"r+b")))
	{
		puts("Error!");
	}
	else
	{
		//TODO do mpz_fseek
		fseek(filler,0,SEEK_END);
		printf("=====Filling %s=====\n", file);
		do
		{
			mpz_sub(difference,origSize,counter);
			if(mpz_cmp_ui(difference,100) < 0)
			{
				offset = mpz_get_ui(difference);
			}
			fgetpos(filler,&pos);
			if(fwrite(empty,sizeof(unsigned char), offset, filler) != offset)
			{
				puts("Error filling file!");
				fsetpos(filler,&pos);
			}
			else
			{
				mpz_add_ui(counter,counter,offset);
				compare = mpz_cmp(counter,origSize);
			}
		}while((compare<0));
		mpz_out_str(stdout,10,counter);
		printf(" / ");
		mpz_out_str(stdout,10,origSize);
		puts("");
		fclose(filler);
	}
}
void isDiff(char origFile[], char otherFile[],char disp[])
{
	int compare1 = 0;
	unsigned long int offset = 100;	

	fpos_t pos;
	FILE * origReader;
	FILE * otherReader;
	unsigned char origChar[100];
	unsigned char otherChar[100];
	
	memset(origChar,'\0',99);
	memset(otherChar,'\0',99);

	mpz_t counter;
	mpz_t difference;
	mpz_t origSize;
	mpz_t otherSize;

	mpz_init(counter);
	mpz_init(difference);
	mpz_init(origSize);
	mpz_init(otherSize);

	mpz_set_ui(counter,0);
	mpz_set_ui(origSize,0);
	mpz_set_ui(otherSize,0);
	
	compare1 = 1;
	while(compare1)
	{
		getFileSize(origFile,&origSize);
		getFileSize(otherFile,&otherSize);
		mpz_sub(difference,origSize,otherSize);
		compare1 = mpz_cmp(otherSize,origSize);
		printf("*****");
		printf("%s has ",origFile);
		mpz_out_str(stdout,10,origSize);
			
		printf(" bytes while %s has ", otherFile);
		mpz_out_str(stdout,10,otherSize); 
		printf(" bytes.");
		printf("*****\n");
		if(compare1 > 0)
		{
			puts("=====Truncating File: =====");
			truncat(otherFile,origSize);

			getFileSize(origFile,&origSize);
			getFileSize(origFile,&otherSize);

			printf("*****");
			printf("%s has ",origFile);
			mpz_out_str(stdout,10,origSize);
				
			printf(" bytes while %s has ", otherFile);
			mpz_out_str(stdout,10,otherSize); 
			printf(" bytes.");
			printf("*****\n");
		}
		else if(compare1 < 0)
		{
			fill(otherFile,origSize,otherSize);
		}
	}
	if((origReader = fopen(origFile,"r+b")) == NULL || (otherReader = fopen(otherFile,"r+b"))== NULL)
	{
		puts("Error opening file!");
		return;
	}
	else
	{
		puts("=====Scanning for and merging differences=====");
		while(mpz_cmp(counter,origSize) < 0)
		{
			mpz_sub(difference,origSize,counter);
			fgetpos(otherReader,&pos);
			if(mpz_cmp_ui(difference,100) < 0)
			{
				offset = mpz_get_ui(difference);
			}

			fread(origChar,sizeof(unsigned char),offset,origReader);
			fread(otherChar,sizeof(unsigned char),offset,otherReader); 

			if(memcmp(origChar,otherChar,offset))
			{
				fsetpos(otherReader,&pos);
				fwrite(&origChar,sizeof(unsigned char),offset,otherReader);
			}
//				printf("%u\t%u\n",origChar,otherChar);
			mpz_add_ui(counter,counter,offset);
		}
		mpz_out_str(stdout,10,counter); 
		printf(" / ");
		mpz_out_str(stdout,10,origSize); 
		puts("");
//		printf("Offset :%lu\n",offset);
		puts("Done!");
		fclose(origReader);
		fclose(otherReader);
	}
	mpz_clear(counter);
	mpz_clear(origSize);
	mpz_clear(otherSize);
	mpz_clear(difference);
}
int main(int argc, char * argv[])
{
	remove("diffs");
	if(argc != 4)
	{
		puts("Launch using the following form: ./diff file1 file2 [y/n]");
		puts("Put y if u want to display progress information.");
	}
	else
	{
		isDiff(argv[1],argv[2],argv[3]);
	}
	return 0;
}

