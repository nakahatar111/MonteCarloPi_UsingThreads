all: pi
pi: est_pi.o
	g++ -pthread est_pi.o -o pi
est_pi.o: est_pi.cpp
	g++ -c est_pi.cpp 