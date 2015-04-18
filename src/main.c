#include <stdio.h>
#include <limits.h>
#include <gmp.h>//For files of arbitrary size
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
	}
	else
	{
		while(!feof(reader))
		{
			fread(&temp,sizeof(char), 1, reader);
			mpz_add_ui(*count,*count,1);//Add one
		}
		fclose(reader);
	}
}
void addToDiffFile(mpz_t pos)
{
	FILE * diffWriter;
	if((diffWriter = fopen("diffs","a"))== NULL)
	{
		puts("Error opening difference file!");
	}
	else
	{
		mpz_out_str(diffWriter,10,pos);
		fprintf(diffWriter,"\n");
		fclose(diffWriter);
	}
}
void isDiff(char origFile[], char otherFile[],char disp[])
{
	int compare1 = 0;
	fpos_t pos;

	FILE * origReader;
	FILE * otherReader;
	unsigned char origChar;
	unsigned char otherChar;

	mpz_t counter;
	mpz_t origSize;
	mpz_t otherSize;
	mpz_t divCheck;

	mpz_init(counter);
	mpz_init(origSize);
	mpz_init(otherSize);
	mpz_init(divCheck);

	mpz_set_ui(counter,0);
	mpz_set_ui(origSize,0);
	mpz_set_ui(otherSize,0);
	mpz_set_ui(divCheck,0);

	printf("%s has ",origFile);
	getFileSize(origFile,&origSize);
	mpz_out_str(stdout,10,origSize);
		
	printf(" bytes while %s has ", otherFile);
	getFileSize(otherFile,&otherSize);
	mpz_out_str(stdout,10,otherSize); 
	printf(" bytes.\n");
	compare1 = mpz_cmp(otherSize,origSize);
	if(compare1 > 0)
	{
		truncat(otherFile,origSize);
		puts("Now");
		truncat(otherFile,origSize);
		printf("%s has ",origFile);
		getFileSize(origFile,&origSize);
		mpz_out_str(stdout,10,origSize);
			
		getFileSize(origFile,&otherSize);
		printf(" bytes while %s has ", otherFile);
		mpz_out_str(stdout,10,otherSize); 
		printf(" bytes.\n");
	}
	if((origReader = fopen(origFile,"r+b")) == NULL || (otherReader = fopen(otherFile,"r+b"))== NULL)
	{
		puts("Error opening file!");
	}
	else
	{
		puts("This may take several minutes depending on the size of the files. Don't be alarmed if it looks like it hangs.");
		while(!feof(origReader) || !feof(otherReader))
		{
			
			if(!strcmp(disp,"y"))
			{
				mpz_mod_ui(divCheck,counter,100);
				if(mpz_cmp_ui(divCheck,0))
				{
					mpz_out_str(stdout,10,counter);
					printf(" / ");
					mpz_out_str(stdout,10,origSize);
					printf("\r");
				}
			}
			compare1 = mpz_cmp(counter,origSize);
			if(!(compare1 >0))
			{
				fread(&origChar,sizeof(unsigned char),1,origReader);	
				fgetpos(otherReader,&pos);
				fread(&otherChar,sizeof(unsigned char),1,otherReader);	
				if(origChar!=otherChar)
				{
					addToDiffFile(counter);
					fsetpos(otherReader,&pos);
					fwrite(&origChar,sizeof(unsigned char),1,otherReader);
					fsetpos(otherReader,&pos);
					fread(&otherChar,sizeof(unsigned char),1,otherReader);	
				}
//				printf("%u\t%u\n",origChar,otherChar);
				mpz_add_ui(counter,counter,1);
			}
			else
			{
				break;
			}
			//mySleep(0);
		}
		puts("");
		puts("Done!");
		fclose(origReader);
		fclose(otherReader);
	}
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

