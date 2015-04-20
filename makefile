binary_diff: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	reset && gcc src/main.c src/pythonCode.c -g -O0 -pg -o bin/binary_diff -Iinclude -lgmp `python-config --ldflags --cflags`
	cp trunc.py bin
windows: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	cls && gcc src/main.c src/pythonCode.c -g -O0 -pg -o bin/binary_diff.exe -Iinclude -lgmp -IC:\Python34\include -LC:\Python34\libs -lpython34
	copy trunc.py bin
linux_test: src/main.c src/pythonCode.c include/pythonCode.h trunc.py
	reset && scan-build clang src/main.c src/pythonCode.c -O0 -g -pg -o bin/binary_diff -Iinclude -lgmp `python-config --ldflags --cflags`
	cp trunc.py bin
