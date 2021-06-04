/***************************************************************
PhysicalMemory.cpp

Programmer: Tristen Anderson
zID: z1820036

Use: Manages the functionality of physical memory using FIFO
	or Least Recently Used demand paging 
***************************************************************/
#include <string>
#include "PhysicalMemory.h"
#include <iostream>
#include <algorithm>

using namespace std;

/***************************************************************
Constructor: PhysicalMemory

Use: Sets the algorithm to use and capacity of physical memory. 
	Fills physical memory with dummy frames to start.

Arguments: An integer for the number of frames of physical memory and a flag
	to designate what algorithm to use. 
***************************************************************/
PhysicalMemory::PhysicalMemory(int memorySize, int algorithmIn) {
	//intialize algorithm to use
	algorithmFlag = algorithmIn;
	capacity = memorySize;
	//fill the physical memory with empty empty frames
	for(int i = 0; i < memorySize; i++)
		memoryList.push_back("");
}

/***************************************************************
Method: access

Use: prints the contents of physical memory at the requested frame

Arguments: the frameID, tells the method which frame to access

Returns: nothing
***************************************************************/
void PhysicalMemory::access(int frameID) {
	//print the content of the frame 
	cout << "Physical: Accessed frameID: " << frameID << " contains: " << memoryList.at(frameID) << endl;

	if(algorithmFlag == 2) {
		//find the frame in the vector
		vector<int>::iterator it = find(timeList.begin(), timeList.end(), frameID);
		//remove it from current postion and push to the back of queue
		timeList.erase(it);
		timeList.push_back(frameID);
	}	
}

/***************************************************************
Method: printMemory

Use: prints the current contents of physical memory

Arguments: none

Returns: nothing
***************************************************************/
void PhysicalMemory::printMemory() const {
	for(size_t i = 0; i < memoryList.size(); i++) {
		cout << "Physical: Memory Frame: " << i << " contains: " << memoryList.at(i) << endl;
	}
}

/***************************************************************
Method: swapIn

Use: finds the next available frame and replaces the content in physical memory
	at that index with the new item provided

Arguments: item, an item to store in physical memory

Returns: the frame at which the item is stored in physical memory
***************************************************************/
int PhysicalMemory::swapIn(string item) {
	
	int nextFrame = getNextAvailableFrame(); 
	memoryList.at(nextFrame) = item;

	cout << "Physical: Stored: " << memoryList.at(nextFrame) << endl;

	//use the storage for the correct algorithm
	if(algorithmFlag == 2)
		timeList.push_back(nextFrame);
	else
		fifo.push(nextFrame);

	return nextFrame;
}

/***************************************************************
Method: getNextAvailableFrame

Use: Returns the next empty frame if any are empty, otherwise uses FIFO to
	return the next frame to replace unless LRU is specified

Arguments: none

Returns: the id of the frame to be replaced
***************************************************************/
int PhysicalMemory::getNextAvailableFrame() {
	
	//Check for any empty frame
	for(size_t i = 0; i < memoryList.size(); i++) {
		if(memoryList.at(i) == "")
			return i; 
	}
	//LRU if specified
	if(algorithmFlag == 2) {
		int next = timeList.front();
		timeList.erase(timeList.begin());
		return next;
	}
        //default to FIFO	
	else {
		int next = fifo.front();
		fifo.pop();
		return next;
	}
}
