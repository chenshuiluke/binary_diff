binary_diff: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	reset && gcc src/main.c src/pythonCode.c -g -o bin/binary_diff -Iinclude -lgmp `python-config --ldflags --cflags`
	cp trunc.py bin

