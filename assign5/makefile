CCFLAGS = -ansi -Wall

assign5: assign5.o PhysicalMemory.o PageTable.o
	g++ $(CCFLAGS) -o assign5 assign5.o PhysicalMemory.o PageTable.o

assign5.o: assign5.cpp PhysicalMemory.h PageTable.h
	g++ $(CCFLAGS) -c assign5.cpp

PageTable.o: PageTable.cpp PageTable.h
	g++ $(CCFLAGS) -c PageTable.cpp

PhysicalMemory.o: PhysicalMemory.cpp PhysicalMemory.h
	g++ $(CCFLAGS) -c PhysicalMemory.cpp

clean:
	-rm *.o assign5
