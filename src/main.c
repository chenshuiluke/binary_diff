#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <gmp.h>//For files of arbitrary size
#include <string.h>
#include "pythonCode.h"

#ifdef __gnu_linux__
	#include <unistd.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif

#define BUFFER_SIZE 1048576
#define BUFFER_SIZE_IND 1048575

void mySleep(int sleepMs)
{
	#ifdef __gnu_linux__
	    usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
	#endif

	#ifdef _WIN32
	    Sleep(sleepMs);
	#endif
}
void dumpArrs(unsigned char * arr, unsigned char * arr1)
{
	int counter = 0;
	for(counter = 0; counter < BUFFER_SIZE_IND;counter++)
	{
		if(arr[counter] != arr1[counter])
		{
			printf("0x%x|0x%x", arr[counter],arr1[counter]);
			puts("");
			puts("");
		}
 	}
}
void keepWritingUntilEqual(FILE ** orig, FILE ** other, unsigned char * arr, unsigned char * arr1,fpos_t * origPos, fpos_t * otherPos, unsigned long int offset)
{

	do
	{
		if(fsetpos(*other,otherPos))
		{
			perror("Error setting file position: ");
		}
		if((fwrite(arr,sizeof(unsigned char),offset,*other))!= offset)
		{
			perror("Error writing differences");
		}
		if(fsetpos(*other,otherPos))
		{
			perror("Error setting file position: ");
		}
		if((fread(arr1,sizeof(unsigned char),offset,*other))!= offset)
		{
			perror("Error writing differences");
		}
	}while(memcmp(arr,arr,offset));

}
void getFileSize(char file[],mpz_t * count)
{
	FILE * reader;
	mpz_init(*count);
	mpz_set_ui(*count,0);
	unsigned long int offset = BUFFER_SIZE;
	unsigned int numRead =0;
	unsigned char * temp = NULL;
	unsigned char * otherTemp = NULL;
	otherTemp = calloc(BUFFER_SIZE,sizeof(unsigned char));

	if(!otherTemp)
	{
		puts("Memory error!");
		abort();
	}
	temp = otherTemp;
	memset(temp,'\0',BUFFER_SIZE_IND);	
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
			numRead = fread(temp,sizeof(unsigned char),offset,reader);
			if(numRead != offset)
			{
				if(feof(reader))
				{
					mpz_add_ui(*count,*count,numRead);
					break;
				}	
				else if(ferror(reader))
				{
					puts("Error occurred. Don't trust the output.");	
					mpz_set_ui(*count,0);
					break;
				}
			}
			mpz_add_ui(*count,*count,offset);
			mySleep(1);
		}
		fclose(reader);
	}
	free(otherTemp);
}
void fill(char file[], mpz_t origSize, mpz_t otherSize)
{
	FILE * filler;
	mpz_t counter;
	mpz_t difference;
	unsigned long int offset = BUFFER_SIZE;	
	unsigned char * empty = NULL;
	unsigned char * temp = calloc(BUFFER_SIZE,sizeof(unsigned char));
	if(!temp)
	{
		puts("Memory error!");
		abort();
	}
	empty = temp;
	memset(empty,'\0',BUFFER_SIZE_IND);
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
			if(mpz_cmp_ui(difference,BUFFER_SIZE) < 0)
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
			}
			mySleep(1);
		}while((mpz_cmp(counter,origSize)<0));
		mpz_out_str(stdout,10,counter);
		printf(" / ");
		mpz_out_str(stdout,10,origSize);
		puts("");
		fclose(filler);
	}
	free(temp);
}
void isDiff(char origFile[], char otherFile[],char disp[])
{
	 unsigned long int offset = BUFFER_SIZE;	

	fpos_t origPos;
	fpos_t otherPos;

	 FILE * origReader;
	 FILE * otherReader;
	unsigned char * origChar = NULL;
	unsigned char * otherChar = NULL;
	
	unsigned char * temp = calloc(BUFFER_SIZE,sizeof(unsigned char));
	unsigned char * temp1 = calloc(BUFFER_SIZE,sizeof(unsigned char));
	if(!temp || !temp1)
	{
		puts("Memory error!");
		abort();
	}
	origChar = temp;
	otherChar = temp1;
	memset(origChar,'\0',BUFFER_SIZE);
	memset(otherChar,'\0',BUFFER_SIZE);

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
	
	do
	{
		getFileSize(origFile,&origSize);
		getFileSize(otherFile,&otherSize);
		mpz_sub(difference,origSize,otherSize);
		printf("*****");
		printf("%s has ",origFile);
		mpz_out_str(stdout,10,origSize);
			
		printf(" bytes while %s has ", otherFile);
		mpz_out_str(stdout,10,otherSize); 
		printf(" bytes.");
		printf("*****\n");
		if(mpz_cmp(otherSize,origSize) > 0)
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
		else if(mpz_cmp(otherSize,origSize) < 0)
		{
			fill(otherFile,origSize,otherSize);
		}
	}while(mpz_cmp(otherSize,origSize));
	origReader = fopen(origFile,"r+b");
	otherReader = fopen(otherFile,"r+b");
	if( otherReader == NULL || origReader == NULL)
	{
		puts("Error opening file!");
		perror("Error:");
		return;
	}
	else
	{
		puts("=====Scanning for and merging differences=====");
		while(mpz_cmp(counter,origSize) < 0)
		{
			mpz_sub(difference,origSize,counter);
			if(fgetpos(otherReader,&otherPos))
			{
				perror("Error getting file position: ");
			}
			if(fgetpos(origReader,&origPos))
			{
				perror("Error getting file position: ");
			}
			if(mpz_cmp_ui(difference,BUFFER_SIZE) <= 0)
			{
				offset = mpz_get_ui(difference);
				mpz_out_str(stdout,10,difference); 
				printf("\n");
				mpz_out_str(stdout,10,counter); 
				printf("\n");
			}
//				printf("Read %s %ld\n",origFile,ftell(origReader));
			if(fread(origChar,sizeof(unsigned char),offset,origReader) != offset)
			{
				perror("Error reading! ");
			}
//				printf("Read %s %ld\n",otherFile,ftell(otherReader));
			if(fread(otherChar,sizeof(unsigned char),offset,otherReader) != offset) 
			{
				perror("Error reading! ");
			}

			if(memcmp(origChar,otherChar,offset))
			{
//				puts("Difference found.");
				//dumpArrs(origChar, otherChar);
				if(fsetpos(otherReader,&otherPos))
				{
					perror("Error setting file position: ");
				}
				if(fsetpos(origReader,&origPos))
				{
					perror("Error setting file position: ");
				}
				
//				printf("Writing to :%ld\n", ftell(otherReader));	

				if((fread(origChar,sizeof(unsigned char),offset,origReader))!= offset)
				{
					perror("Error writing differences");
				}
				
				keepWritingUntilEqual(&origReader, &otherReader,origChar,otherChar,&origPos,&otherPos,offset);
			}
//			puts("=======");
//			puts("=======");
			memset(origChar,'\0',BUFFER_SIZE);
			memset(otherChar,'\0',BUFFER_SIZE);
//				printf("%u\t%u\n",origChar,otherChar);
			mpz_add_ui(counter,counter,offset);
			mySleep(1);
		}
		mpz_out_str(stdout,10,counter); 
		printf(" / ");
		mpz_out_str(stdout,10,origSize); 
		puts("");
		printf("Offset :%lu\n",offset);
		puts("Done!");
		fclose(origReader);
		fclose(otherReader);
	}
	mpz_clear(counter);
	mpz_clear(origSize);
	mpz_clear(otherSize);
	mpz_clear(difference);
	free(origChar);
	free(otherChar);
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

