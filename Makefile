all: simulator

clean:
	rm simulator

simulator:
	g++ project2.cpp -o simulator
