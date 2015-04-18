#include "pythonCode.h"

void truncat(char fileName[], mpz_t size)
{
	//https://gmplib.org/manual/Converting-Integers.html#Converting-Integers
	//https://docs.python.org/3/c-api/long.html
	//http://stackoverflow.com/questions/8229597/embedding-python
	//https://docs.python.org/3/c-api/object.html
	//http://www.codeproject.com/Articles/11805/Embedding-Python-in-C-C-Part-I

	
	char * tempNumStr = NULL;
	char * runString = NULL;

	mpz_sub_ui(size,size,1);

	/*The getFileSize() function says
	there is one more byte than there really is
	so we account for that here
	*/
	
	tempNumStr = malloc(sizeof(char) * (mpz_sizeinbase(size,10)+2));

	/*tempNumStr will hold the char[] version 
	of mpz_t size that's returned from 
	mpz_get_str()
	*/
	
	runString = calloc( (mpz_sizeinbase(size,10)+2)+strlen(fileName)+strlen("trunc.PyTruncate") + 2,sizeof(char) );
	
	//runString will hold the python command to call the truncate() function
	Py_Initialize();
	mpz_get_str(tempNumStr,10,size); 
	strcpy(runString,"trunc.PyTruncate('");
	strcat(runString,fileName);
	strcat(runString,"'");
	strcat(runString,",");
	strcat(runString,tempNumStr);
	strcat(runString,")");
	
	puts(runString);
	/*
	Should look something like:
	trunc.PyTruncate('example',size)
	*/
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
//	PyRun_SimpleString("print(os.getcwd())");
	PyRun_SimpleString("sys.path.append(os.getcwd())");
//	PyRun_SimpleString("print(sys.path)");
	PyRun_SimpleString("import trunc");
	PyRun_SimpleString(runString);
	Py_Finalize();
	free(tempNumStr);
	free(runString);
}
