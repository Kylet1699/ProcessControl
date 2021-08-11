main: main.c
	gcc -g main.c ProcessControl.c -o main

clean:
	rm main
