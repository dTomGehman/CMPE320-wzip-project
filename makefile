wzip: wzip.o main.o
	gcc -o wzip wzip.o main.o -Wall -Werror -pthread

main.o: main.c
	gcc -c main.c -Wall -Werror -pthread

wzip.o: wzip.c
	gcc -c wzip.c -Wall -Werror -pthread

