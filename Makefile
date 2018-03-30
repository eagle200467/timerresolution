all:
	gcc timercreator.c -o testtimer -lrt
clean:
	rm -rf *.o testtimer
