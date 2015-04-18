binary_diff: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	reset && gcc src/main.c src/pythonCode.c -Iinclude -lgmp `python-config --ldflags --cflags`
	cp trunc.py bin

