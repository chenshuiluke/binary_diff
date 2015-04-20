binary_diff: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	reset && gcc src/main.c src/pythonCode.c -g -o bin/binary_diff -Iinclude -lgmp `python-config --ldflags --cflags`
	cp trunc.py bin
windows: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	cls && gcc src/main.c src/pythonCode.c -g -o bin/binary_diff.exe -Iinclude -lgmp -LC:\Python34\libs -IC:\Python34\libs
	cp trunc.py bin

