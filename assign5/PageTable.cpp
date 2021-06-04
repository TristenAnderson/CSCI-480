/***************************************************************
PageTable.cpp

Programmer: Tristen Anderson
zID: z1820036

Use: Class to manage the functions of the page table
***************************************************************/
#include <string>
#include <vector>
#include "PageTable.h"
#include "PhysicalMemory.h"
#include <iostream>
#include <iomanip>

using namespace std;

/***************************************************************
Constructor: PageTable

Use: Initializes the page table and keeps a copy of the physical memory

Arguments: An integer to set the size of the page table and an instance 
	of virtual memory
***************************************************************/
PageTable::PageTable(PhysicalMemory* pmIn, int tableSize) {
	//initialize page table and physical memory
	numFaults = 0;
	//set all entries to not is use
	for(int i = 0; i < tableSize; i++) {
		PageTableEntry entry = {false, -1};
		entryList.push_back(entry);
	}
	//keep a copy of the physical memory
	mainPhysicalMemory = pmIn;
}

/***************************************************************
Method: reference

Use: writes to the shared memory for numIterations if the string 
	is not empty

Arguments: param, holds the id of the writer

Returns: nothing
***************************************************************/
void PageTable::reference(int pageID, string item) {
	//check if entry is valid. if false pagefault, otherwise access the memory
	if(entryList.at(pageID).valid == false)
		pageFault(pageID, item);
	else
		mainPhysicalMemory->access(entryList.at(pageID).physicalMemoryIndex);
}

/***************************************************************
Method: getFaults

Use: returns the the current number of page faults

Arguments: none

Returns: the number of page faults
***************************************************************/
int PageTable::getFaults() const {
	return numFaults;
}

/***************************************************************
Method: pageFault

Use: increments the page fault count and swaps in the item to physical memory. 
	Updates the page table after replacement.

Arguments: the pageID and the item to store in physical memory

Returns: nothing
***************************************************************/
void PageTable::pageFault(int pageID, string item) {
	//increment number of page faults
	numFaults++;
	//declare a page fault has occurred
	cout << "PageTable: page fault has occurred" << endl;
	//swap item into physical memory
	int frameID = mainPhysicalMemory->swapIn(item);
	
	for(size_t i = 0; i < entryList.size(); i++)
		if(frameID == entryList.at(i).physicalMemoryIndex) {
			entryList.at(i).valid = false;
			entryList.at(i).physicalMemoryIndex = -1;
		}
	//update the page table 
        updateReplacedEntry(frameID, pageID);	
}

/***************************************************************
Method: updateReplacedEntry

Use: updates the page table to reflect the new state of physical memory

Arguments: the new location in physical memory (ie. the frame id) and the pageID 

Returns: nothing
***************************************************************/
void PageTable::updateReplacedEntry(int replacedIndex, int pageID) { 
	//update the page table and set valid to true
	entryList.at(pageID).physicalMemoryIndex = replacedIndex;
	entryList.at(pageID).valid = true;
}

/***************************************************************
Method: printTable

Use: prints out the current contents of the page table

Arguments: none

Returns: nothing
***************************************************************/
void PageTable::printTable() const {
	//print the page table layout
	for(size_t i = 0; i < entryList.size(); i++) {
		cout << "PageTable: Index " << i << " : Physical Index: " << setw(2) << right << entryList.at(i).physicalMemoryIndex << " : In Use:  " << (entryList.at(i).valid ? "true" : "false") << endl;
	}
	cout << "PageTable: Current number of page faults: " << numFaults << endl;
}

/***************************************************************
Method: reset

Use: return the page table to an empty state and page faults to 0

Arguments: none

Returns: nothing
***************************************************************/
void PageTable::reset() {
	//reset number of faults to 0 and validity flags to false for each entry
	numFaults = 0;
	for(size_t i = 0; i < entryList.size(); i++) {
		entryList.at(i).valid = false;
		entryList.at(1).physicalMemoryIndex = -1;
	}
}
