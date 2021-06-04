/***************************************************************
CSCI 480 - Assignment 2 - Spring 2021

Programmer: Tristen Anderson
Section:    1
TA:	    Stya Keerthi
Date Due:   2/15/2021

Purpose: A command interpretter that makes use of system calls such as fork,
	 exec, and wait to execute traditional unix commands as well a two special
         commands (ls_sys and fcfs <num_processes>). 	
***************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;
using std::stoi;

void ls_sys();
int getInfo(string line);
string getInfoString(string line);

int main(void) {

	char buf[1024];			//buffer to read in command line
	pid_t pid;
	int status;			//the command status
	int rs;			
	const int maxArgs = 16;		//set a maximum for the number of command line arguments, 16 should be enough
	const int SEED = 10;		//set the seed for the fcfs simulation
	
	printf("myshell> ");		//prompt the user for the first command
	while (fgets(buf,1024,stdin) != NULL)
	{
		//set last char to null
		buf[strlen(buf)-1] = '\0';
		//initiatize an array to store arguments
		char* argv[maxArgs];
		argv[0] = strtok(buf, " ");

		//parse the command line string into tokens if there is content to parse
	        if (argv[0] != NULL) 
		{	
			for(int i = 1; i < maxArgs; i++)
			{
				argv[i] = strtok(NULL, " ");
				if(argv[i] == NULL)
				{
					argv[i] = NULL;
					break;
				}
			}
		
			//check for the exit command
 			if(strcmp(argv[0], "quit") == 0)
				exit(0);
			
			//create a child process to execute the command 
			pid = fork();
			if (pid == -1) 
			{
				perror("Error on fork");
				exit(EXIT_FAILURE);
			}
			else if (pid == 0) 
			{
				//check if user requested fcfs command
				if(strcmp(argv[0], "fcfs") == 0) 
				{
					int W_total = 0;
					int W_avg = 0;
					int numProcesses = stoi(argv[1]);
					srand(SEED);
					cout << "FCFS CPU scheduling simulation with " << numProcesses << " processes." << endl;
					//write each burst and calculate the total wait time 
					for(int i = 0; i < numProcesses; i++) 
					{
						int rnum = rand() % 100 + 1;
						cout << "CPU burst: " << rnum << " ms" << endl;
						if(i < numProcesses - 1)
							W_total = W_total + rnum;
					}
					W_avg = W_total / numProcesses;
					cout << "Total waiting time in the ready queue: " << W_total << endl;
					cout << "Average waiting time in the ready queue: " << W_avg << endl;
					exit(0);	//teminate the child
				}
				else if(strcmp(argv[0], "ls_sys") == 0)
				{
					ls_sys();
					exit(0);	//terminate the child
				}
				//if not a special command then try to execute it as a regular one
				else {
					rs = execvp(argv[0], argv);
					if(rs == -1) 
					{
						perror(argv[0]);
						exit(EXIT_FAILURE);	
					}
				}
			}
			else 
			{
				//parent process: wait for the child to finish 
				if((pid = waitpid(pid, &status, 0)) < 0) 
					std::cerr << "Error on waitpid" << endl;
			}
		}
		//reprompt the user for a cmd again 
		cout << "myshell> ";
	}

	exit(0);
} 

/***************************************************************
Function: ls_sys

Use: Accesses a series of system files to retreive information
	that answers various questions 

Arguments: none

Returns: nothing
***************************************************************/
void ls_sys() {
	
	//open and retrieve the ostype
	ifstream fs("/proc/sys/kernel/ostype");
	string ostype, hostname, osrelease, version;
	getline(fs, ostype);
	fs.close();
	//open and retrieve the hostname
	fs.open("/proc/sys/kernel/hostname");
	getline(fs, hostname);
	fs.close();
	//open and retrieve the osrelease
	fs.open("/proc/sys/kernel/osrelease");
	getline(fs, osrelease);
	fs.close();
	//opene and retrieve the os version
	fs.open("/proc/sys/kernel/version");
	getline(fs, version);
	fs.close();
	
	//print the above information as answers to the questions
	std::cout << "**** A: Questions About Turing's OS ****" << std::endl;
	std::cout << "1. The type of turing's OS is: " << ostype << std::endl;
	std::cout << "2. The hostname is: " << hostname << std::endl;
	std::cout << "3. The osrelease is: " << osrelease << std::endl;
	std::cout << "4. version: " << version << std::endl;
	
	//iniatlize some storage variables for data we will be grabbing
	string line, vendor, model, physAddrSize, virtualAddrSize;
	int numCores = 0;	//number of cpu cores
        int numChips = 0;	//number of cpu chips
	int idNum = 0;		//temporary var to hold phys id 
	int processorNum;	//processor number

	vector<int> ids;	//a vector of unique physical id numbers 

	//open the cpuinfo file
	fs.open("/proc/cpuinfo");
	while(!fs.eof()) {
		getline(fs, line);
		size_t found;				//index of item to find in a string
		found = line.find("physical id");
		if(found != string::npos) {
			//increment num cpu cores
			numCores++;
			//convert phys id to int 	
			idNum = getInfo(line);
			
			bool exists = false;		//var to indicate whether physical id is already known
			
			//if no chips are in the vector add the first one
			if(numChips == 0) {	
				ids.push_back(idNum);
				numChips++;				
			}
			//for all other processors check the physical ids against those we already have
			for(size_t i = 0; i < ids.size(); i++) {
				if(ids[i] == idNum) {
					exists = true;
				} 
			}
			//if we dont have record of the physical id add it vector and chip count +1
			if(exists == false) {
				ids.push_back(idNum);
				numChips++;
			}	
		}
		//look for an instance of the word processor and take note of its value 
		string processor;	
		found = string::npos;
		found = line.find("processor");
		if (found != string::npos) {
			processorNum = getInfo(line);
		}
		//if the processor if the right number then check for these values
		if(processorNum == 0) {
			found = line.find("vendor_id");
			if(found != string::npos) {
				vendor = getInfoString(line);
			}
			found = string::npos;
			found = line.find("model");
			if(found != string::npos) {
				model = getInfoString(line);
			}
			found = string::npos;
			found = line.find("address size");
			if(found != string::npos) {
				physAddrSize = getInfoString(line);
				found = physAddrSize.find(",");
				virtualAddrSize = physAddrSize.substr(found + 2, string::npos);
				physAddrSize = physAddrSize.substr(0, found);
			}
		}
		       	
	}
	fs.close();
	//end cpuinfo file
	
	//open /proc/stat file
	string cpuNumStr, idleTime, userModeTime, sysModeTime;
	int cpuNum;
	fs.open("/proc/stat");
	while(!fs.eof()) {
		getline(fs, line);
		size_t found;
		//look for the term cpu and then cut to the number only
		found = line.find("cpu");
		if(found != string::npos) {
			found = line.find(" ");
			cpuNumStr = line.substr(0, found);
			cpuNumStr = cpuNumStr.substr(3, string::npos);
			found = cpuNumStr.find(" ");
			cpuNumStr = cpuNumStr.substr(0, found);
			//if the cpu number exitss make it an integer
	       		if (cpuNumStr.size() >= 1)
				cpuNum = stoi(cpuNumStr);
			//if the cpu number is 1 then get the values we need from that row
			if (cpuNum == 1) {
				found = line.find(" ");
				idleTime = line.substr(found + 1, string::npos);
				found = idleTime.find(" ");
				userModeTime = idleTime.substr(0, found);
				sysModeTime = idleTime.substr(found + 1, string::npos);
				found = sysModeTime.find(" ");
				sysModeTime = sysModeTime.substr(found + 1, string::npos);
				found = sysModeTime.find(" ");
				idleTime = sysModeTime.substr(found + 1, string::npos);
				sysModeTime = sysModeTime.substr(0, found);
				found = idleTime.find(" ");
				idleTime = idleTime.substr(0, found);	
			}
		}
	}
	fs.close();
	//end /proc/stat file
	
	//open /proc/uptime file
	//get string from file and extract the upTime portion
	string upTime;
	fs.open("/proc/uptime");
	getline(fs, line);
	upTime = line.substr(0, line.find(" "));
	fs.close();
	//end /proc/uptimefile
	
	//convert upTime seconds into days, hours, minutes, and seconds
	int days, hours, minutes, seconds, temp, upTimeNum;
	upTimeNum = std::stoi(upTime);
	days = upTimeNum / (24 * 3600);
	temp = upTimeNum % (24 * 3600);
	hours = temp / 3600;
	temp = temp % 3600;
	minutes = temp / 60;
	temp = temp % 60;
        seconds = temp;
	
	//open the /proc/swaps file
	string swapSize;
	fs.open("/proc/swaps");
	while(!fs.eof()) {
		getline(fs, line);
		size_t found = line.find("/dev");
		//if we are at the swap file data push data until size column
		if(found != string::npos) {
			std::istringstream sizeStream(line);
			sizeStream >> swapSize;
			sizeStream >> swapSize;
			sizeStream >> swapSize;
		}
	}
	fs.close();
	//end of /proc/swaps file
	
	//convert jiffies to seconds 
	double userModeTime_seconds, sysModeTime_seconds, idleTime_seconds, swapSize_MB;
	int idleTimeDays, idleTimeHours, idleTimeMinutes, idleTimeSeconds;

	userModeTime_seconds = stoi(userModeTime) / 100;
	sysModeTime_seconds = stoi(sysModeTime) / 100;
	idleTime_seconds = stoi(idleTime) / 100;
	
	//break down the total seconds to days, hours, minutes and seconds
	idleTimeDays = (int)idleTime_seconds / (24 * 3600);
	temp = (int)idleTime_seconds % (24 * 3600);
	idleTimeHours = temp / 3600;
	temp = temp % 3600;
	idleTimeMinutes = temp / 60;
	temp = temp % 60;
	idleTimeSeconds = temp;

	//convert swapsize from KB to MB
	swapSize_MB = stoi(swapSize) / 1024;


	//print out all the data points gathered above 
	cout << "\n**** B: Questions About Turing's Processors ****" << endl;
	cout << "1. The number of turing's processors(cpu cores) is: " << numCores << endl;
	cout << "2. The number of physical mutlicore chips is: " << numChips << endl;
	cout << "3. The amount of time turing has been up in seconds: " << upTime << endl;
	cout << "4. The uptime expresses in days, hours, minutes and seconds: " << days << " days, "
		 << hours << " hours, " << minutes << " minutes, " << seconds << " seconds" << endl;

	cout << "\n**** C: For Processor 0, Answer theses Questions ****" << endl;
	cout << "1. Who is the vendor: " << vendor << endl;
	cout << "2. What is the Model Name: " << model << endl;
	cout << "3. What is it's Physical Address Size: " << physAddrSize << endl;
	cout << "4. What is it's Virtual Address Size: " << virtualAddrSize << endl;

	cout << "\n**** D: For Processor 1, Answer these Questions ****" << endl;
        cout << "1. How long has it spent time in user mode in seconds (not including low-priority user mode): " << userModeTime_seconds << endl;
	cout << "2. How long has it spend it system mode in seconds: " << sysModeTime_seconds << endl;
	cout << "3. How long has it been idle in seconds: " << idleTime_seconds << endl;
	cout << "4. How long has it been idle in days, minutes, hours and seconds: " << idleTimeDays << " days, " << idleTimeHours
	       	<< " hours, " << idleTimeMinutes << " minutes, " << idleTimeSeconds << " seconds" << endl;

	cout << "\n**** E: Question about Turing's Swap Device ****" << endl;
	cout << "1. What is the size of turing's swap devices: " << swapSize_MB << endl;	

}

/***************************************************************
Function: getInfo

Use:      takes a string and returns the remaining string thats 
	follows a ":" as an integer

Arguments: 1. line: a string to trim and convert to int

Returns:   an integer version of the string
***************************************************************/
int getInfo(string line) {
	
	string strNum;
	size_t strIndex = line.find(":");

        if(strIndex != string::npos) {
		strNum = line.substr(strIndex + 2, string::npos);
	}

	int num = stoi(strNum);
	return num; 
}

/***************************************************************
Function: getInfoString

Use:      Returns the string following a ":" after trimming the
	preceding text from the passed in string

Arguments: 1. line: the string to trim

Returns:   strNum: the string after trimming
***************************************************************/
string getInfoString(string line) {

	string str;
	size_t strIndex = line.find(":");

        if(strIndex != string::npos) {
		str = line.substr(strIndex + 2, string::npos);
	}

	return str;
}


