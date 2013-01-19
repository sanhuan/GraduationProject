INC_ACE = -I/home/sanhuan/ACE/include
LIB_ACE = -L/home/sanhuan/ACE/lib -lACE

client : main.o client.o
	g++ $^ -o $@ $(INC_ACE) $(LIB_ACE)

%.o : %.cpp
	g++ -c $^ -o $@ $(INC_ACE) $(LIB_ACE)

clean_obj :
	rm *.o
clean_all:
	rm *.o client
