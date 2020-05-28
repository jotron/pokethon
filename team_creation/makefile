all: create

create: team_picker.o
	g++ -o create team_picker.o

team_picker.o: team_picker/src/team_picker.cpp
	g++ -c team_picker/src/team_picker.cpp 

clean:
	rm team_picker.o create
