#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#include <vector>
#include <queue>
#include <string>

using namespace std;

class PhysicalMemory
{
    public:
        PhysicalMemory(int memorySize, int algorithmIn); //Initialize all the data and data structure in the class (e.g. set clock to 0). The second argument is for testing different page replacement algorithms if you will implement the bonus credits for LRU.
        void access(int frameID);   //access the frame, update the time stamp and time list 
        void printMemory() const;  //print out the physical memory layout
	int swapIn(string item);  //returns the frame ID of the item just swapped in
        
    private:
	vector <string> memoryList;   //The physical memory!
	vector <int> timeList;	//storage for LRU
	queue <int> fifo;	//storage for FIFO
	int algorithmFlag; //indicate if using FIFO or LRU
        int getNextAvailableFrame();  //get a frame, either available or via replacement
	int capacity;	//the number of frames of physical memory
};

#endif
