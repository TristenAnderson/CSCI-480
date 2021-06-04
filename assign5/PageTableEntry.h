#ifndef PAGE_TABLE_ENTRY_H
#define PAGE_TABLE_ENTRY_H

struct  PageTableEntry
{
	bool valid;
	int physicalMemoryIndex;
};

#endif

