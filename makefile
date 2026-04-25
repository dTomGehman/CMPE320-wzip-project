wzip: wzip.o
	gcc -o wzip wzip.o -Wall -Werror

wzip.o: wzip.c
	gcc -c wzip.c -Wall -Werror -pthread

