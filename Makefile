CC=gcc

virtualMachine: virtualMachine.c
	$(CC) virtualMachine.c -o runVirtualMachine -Wall -Wextra -pedantic