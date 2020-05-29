all: create

create: main.o algorithm.o io_handler.o
	g++ -o "create" main.o algorithm.o io_handler.o

main.o:
	g++ -c team_picker/src/main.cpp

algorithm.o:
	g++ -c team_picker/src/algorithm.cpp

io_handler.o:
	g++ -c team_picker/src/io_handler.cpp

clean:
	rm main.o algorithm.o io_handler.o create
