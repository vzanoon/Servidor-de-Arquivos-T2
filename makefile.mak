all: sistema

sistema: ./sistema_arq.c
	gcc -o ./sistema_arq.exe ./sistema_arq.c
	./sistema_arq.exe