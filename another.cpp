#include <iostream>
#include <fstream>
#include <queue>
#include <cstdio>
#include <vector>
#include <string>
#include <cmath>
#include <deque>
#include <set>
#include <map>
#include <cstdlib>

using namespace std;
bool debug = false;
class Memory;
class CPU;
class MemoryBase;
class Scheduler;

/*
 * Some const values we will use
 */
typedef pair<long long, int> interrupt_t;
typedef string Interrupt;
typedef pair<int, long long> ProcessInfo; //for scheduler
typedef pair<long long, string> reverseAccessType;
#define reverseAccessElement(x,y) make_pair((x), (y))

#define maxLL(x,y) (((x) > (y))? (x):(y))
#define TimerInterrupt(t) make_pair(t,1)
#define DiskInterrupt(t) make_pair(t,2)
#define ProcessCreationInterrupt(t) make_pair(t,3)
#define DiskDiscardInterrupt(t) make_pair(t,4)
#define EOP "End of program"
#define SOP "Start of program"
#define IDLE "IDLE_PROCESS"
#define ERR printf("VirtualError\n");
#define NOBUFFER "NO BUFFER"
#define quantumLeft first
#define nextTimer second
#define TimerMessage() ("")
#define DiskMessage(pageName)(pageName)
#define ProcessCreationMessage(proceeName) (processName)
#define messageParseProcessName(msg) (msg)
#define messageParsePageName(msg) (msg)

string FIFO = "fifo";
string LRU = "lru";
string SCA = "2ch-alg";

map<string, double> processStartTime;

int globalQuantum = 1000;
int globalContextSwitch = 50;
int globalPages = 75;
int globalSwap = 1000;
int globalCyclesPerSec = 100000;

void printerror(string msg) {
	cout << endl << msg << endl;
	exit(0);
}

/*
 * This is the base class of the page-replacement algorithm
 * The main purpose of this component is to decide which page should be in the main memory and how to replace them.
 */
class MemoryBase {
public:
	//check whether the memory pool is full.
	virtual bool memoryFull() {
		ERR
	}
	//Access the page in the physical memory the update the information.
	virtual bool accessPage(long long time, string pgName) {
		ERR
	}
	//Insert the page into the physical memory and update the information.
	virtual bool insertPage(long long time, string pgName) {
		ERR
	}
	//Kick out a memory in the memory pool.
	virtual bool kickOut(long long time) {
		ERR
	}
	//mark the page to be busy.
	virtual void markBusy(string pgName) {
		ERR
	}
	//unmark the page.
	virtual void unmarkBusy(string pgName) {
		ERR
	}



	virtual int pgNumIncrease(){
		ERR
	}
	virtual int pgNumDecrease(){
		ERR
	}
	virtual void debug()
	{
		ERR
	}
	virtual int pageNum()
	{
		ERR
	}
};

/*
 * The declaration of Memory
 * The memory is connected to the CPU to handle the memory fetch.
 */
class Memory {
	map<string, string> lastFaultProcess;
	set<string> waitingPages;
	MemoryBase* mmu; //This pointer decides which page replacement algorithm we will use.
	Scheduler* scheduler;
	long long maxtime, busyCount;

public:
	void initialize(Scheduler* s, MemoryBase *m) {
		scheduler = s;
		busyCount = 0;
		maxtime = -1;
		mmu = m;
	}

	void erasewaiting(string page) {
		waitingPages.erase(page);
		mmu->unmarkBusy(page);
	}

	bool fetch(long long time, string processName, string pgName);

	bool swapPage(long long time, string faulingProcess, string faultingPage);
	void kickOut(long long time);
	void pgNumIncrease()
	{
		mmu->pgNumIncrease();
	}
	void pgNumDecrease()
	{
		mmu->pgNumDecrease();
	}
	void debug()
	{
		set<string>::iterator it;
		cout<<"The waiting pages are:"<<endl;
		for (it = waitingPages.begin(); it != waitingPages.end(); it ++)
			cout<<*it<<"    ";
		cout<<endl;
		cout<<"The busy count is: "<<busyCount<<endl;
		mmu->debug();
	}
	int pgNum()
	{
		return mmu->pageNum();
	}
};

/*
 * The declaration of scheduler
 */

class Scheduler {
	//process Name ------>   blocked pages of the process
	//map<string, string> blockedPages;
	//Process's infomation : The rest time of the quantum and the process' next timer interrupt
	map<string, ProcessInfo> infoTable;
	//interrupts' time and type ----> the detailed information the interrupt, such as the process name or the interrupted page name
	map<interrupt_t, Interrupt> interrupts;
	//The queues which stores the faulting, hanged, ready process.
	deque<string> faultQueue, readyQueue;
	//blocked process
	//set<string> blockedQueue;
	int blockedNum;


	CPU* cpu;
	Memory* memory;
public:
	/*
	 * return the closest interrupt from now.
	 */
	long long cloestInterrupt() {
		if (interrupts.begin() == interrupts.end())
			return -1;

		return (interrupts.begin()->first).first;
	}

	void initilize(CPU* c, Memory* m) {
		cpu = c;
		memory = m;
	}

	void creationInterrupt(long long time, string processName);
	void diskInterrupt(long long time, string pgName);
	void diskDiscardInterrupt(long long time);
	void handleDiskDiscardInterrupt(long long time);
	void handleCreationInterrupttion(long long time, string currentProcess);
	void handleDiskInterruption(long long time, string currentProcess);
	void handleTimerInterruption(long long time, string currentProcess);

	bool handleInterrupts(long long time, string currentProcess);

	void processTermination(long long time, string currentProcess);

	void pgFault(long long time, string faultingProcess, string faultingPage);

	bool hasInterrupts(long long time);
};

/*
 * The declaration of the CPU.
 * The cpu is the main component of the simulator, which performs the simulating of feteching memory and context switch.
 */
class CPU {
	Scheduler *scheduler;
	Memory *memory;

	//Process Name -----> The process memory trace file descriptor
	map<string, ifstream*> fd;
	//process Name -----> cycleNum, page fault number, terminate time.
	map<string, long long> cycleNum, pgFaultNum, terminateTime;
	//The process buffer.
	map<string, string> buffer;

	long long idleCycle;
	long long currentTime, currentProcessStartTime;

	string currentProcess, nextMem;

	bool readBuffer(string processName);

public:
	void initialize(Scheduler* s, Memory* m) {
		scheduler = s;
		memory = m;
	}
	void simulate();
	void ContextSwitch(string newProcess, long long newProcessStartTime);
	void pgFaultIncrease(string processName) {
		pgFaultNum[processName] += 1;
	}
	void cycleIncrese(string processName) {
		cycleNum[processName] += 1;
	}
};

/*
 * Read this process's next memory. If there is a buffered memory, then extract it.
 */
bool CPU::readBuffer(string processName) {
	if (buffer.find(processName)
			== buffer.end() || buffer[processName] == NOBUFFER) {
		ifstream *fileDes = fd[processName];
		bool ret = ((*fileDes) >> nextMem);
		nextMem += "b" + processName;
		return ret;
	}
	nextMem = buffer[processName];
	buffer[processName] = NOBUFFER;
	return true;
}

/*
 * Switch the contex from this process to next process
 */
void CPU::ContextSwitch(string newProcess, long long newProcessStartTime) {
	//Next Memory will not be fetched, restore it.
	if (currentProcess != IDLE && nextMem != SOP && nextMem != EOP) {
		buffer[currentProcess] = nextMem;
	}

	currentProcess = newProcess;
	currentProcessStartTime = newProcessStartTime;
	nextMem = SOP;
}

void CPU::simulate() {
	currentProcess = IDLE;
	idleCycle = 0;
	currentTime = -1;
	currentProcessStartTime = -1;
	while (true) {
		if (!scheduler->handleInterrupts(++currentTime,
				currentProcess) && currentProcess == IDLE) {
			//finished
			break;
		}

		//check which process is running now
		if (currentProcess == IDLE) {
			if (currentTime < scheduler->cloestInterrupt() - 1) {
				idleCycle += scheduler->cloestInterrupt() - currentTime;
				currentTime = scheduler->cloestInterrupt() - 1;
				continue;
			}
			++idleCycle;
			continue;
		}

		//if the CPU is on a context switch,
		if (currentProcessStartTime > currentTime) {
			if (scheduler->cloestInterrupt() - 1 < currentProcessStartTime){
				idleCycle += scheduler->cloestInterrupt() - 1;
				currentTime = scheduler->cloestInterrupt() - 1;
				continue;
			}
			++idleCycle;
			continue;
		}

		//check out whether it is a new coming process.
		if (fd.find(currentProcess) == fd.end()) {

			ifstream *newfd = new ifstream;
			string file = "./Testdata/dataL/" + currentProcess + ".mem";
			newfd->open(file.c_str());
			cout << "File " << file << " opened\n.";
			fd[currentProcess] = newfd;
			nextMem = SOP;
		}

		if (nextMem == SOP) {
			//nothing
		} else {
			cycleNum[currentProcess] += 1;
		}

		if (!readBuffer(currentProcess)) {
			cout << "There is no memory to read, the program goes to the end."
					<< endl;
			//We reach the end of the program
			nextMem = EOP;

			//switch to idle
			cout << currentProcess << " terminated at " << currentTime
					<< "with total cycles: " << cycleNum[currentProcess]
					<< "and page faults" << pgFaultNum[currentProcess] << endl;
			terminateTime[currentProcess] = currentTime;
			scheduler->processTermination(currentTime, currentProcess);
		} else {

			//go to read the memory
			bool isIn = memory->fetch(currentTime, currentProcess, nextMem);
			if (debug)
				if (isIn) {
					cout << "Fetch memory success! " << nextMem << " "
							<< currentProcess << cycleNum[currentProcess]
							<< endl;
				} else {
					cout << "Fetch memory failed! " << nextMem << " "
							<< currentProcess << cycleNum[currentProcess]
							<< endl;
				}

			if (isIn) {
				//do nothing
			} else {
				//next memory is not in memory
				//disk swap
				if (debug)
					cout << "Starting page fault at " << currentTime << " for "
							<< currentProcess << " and " << nextMem << endl;
				scheduler->pgFault(currentTime, currentProcess, nextMem);
				if (debug)
					cout << "Fetch finished" << endl;
			}
		}

	}

	cout << "Simulation finished at cycle " << currentTime
			<< " with total idle time: " << idleCycle << endl;
	cout << "The conclusion:" << endl;
	long long total = 0;
	//print out the conclusion.
	map<string, long long>::iterator it;
	for (it = terminateTime.begin(); it != terminateTime.end(); it++) {
		cout << it->first << " ends at" << it->second << ", with"
				<< cycleNum[it->first] << " cycles for "
				<< (it->second) * 1.0 / globalCyclesPerSec
						- processStartTime[it->first] << "s with"
				<< pgFaultNum[it->first] << " page faults" << endl;
		total += pgFaultNum[it->first];
	}
	cout << "Total page faults:" << total << endl;
}


/*
 * This is the model of FIFO.
 */
class FIFOMemory: MemoryBase {
	//page Name ------> page available time.
	map<string, long long> pageTime;
	//page Name ------> whether the page is marked.
	map<string, bool> pageMarked;
	//FIFO pages.
	deque<string> pages;
	int pgNum;
public:
	void debug()
	{
		cout<<"The page num is: "<<pgNum<<endl;
		cout<<"PageMarked: "<<endl;
		map<string,bool>::iterator it;
		for (it = pageMarked.begin(); it != pageMarked.end(); it++)
			cout<<it->first<<' '<<it->second<<'\t';
		cout<<endl;
		cout<<"Page pool:"<<endl;
		for (deque<string>::iterator iter = pages.begin() ; iter != pages.end() ; iter ++)
			cout<<*iter<<'\t';
		cout<<endl;
	}
	bool memoryFull();
	bool accessPage(long long time, string pgName);
	bool insertPage(long long time, string pgName);
	bool kickOut(long long time);
	void markBusy(string pgName);
	void unmarkBusy(string pgName);
	int pgNumIncrease()
	{
		return ++pgNum;
	}
	int pgNumDecrease()
	{
		return --pgNum;
	}
	int pageNum()
	{
		return pgNum;
	}
};

bool FIFOMemory::accessPage(long long time, string pgName) {
	if (pageTime.find(pgName) == pageTime.end()) {
		return false;
	} else if (pageTime[pgName] > time) {
		return false;
	} else if (pageMarked[pgName])
		return false;
	return true;
}

bool FIFOMemory::memoryFull() {
	return (pgNum == globalPages);
}

bool FIFOMemory::insertPage(long long time, string pgName) {
	/*if (memoryFull()) {
		cout << "!!!!!!You want to insert when the pages pool is full !!!"
				<< endl;
		exit(0);
	}*/
	pages.push_back(pgName);
	pageTime[pgName] = time;
	pageMarked[pgName] = false;
	return true;
}

void FIFOMemory::markBusy(string pgName) {
	pageMarked[pgName] = true;
}

void FIFOMemory::unmarkBusy(string pgName) {
	pageMarked[pgName] = false;
}

/*
 * The main idea of this function is to kick out the header of the queue.
 */
bool FIFOMemory::kickOut(long long time) {
	/*if (!this->memoryFull())
		return false;*/
	deque<string>::iterator it = pages.begin();
	while (pageMarked[(*it)] || pageTime[(*it)] > time) {
		it++;
		if (it == pages.end()) {
			//It reached the end of the queue.
			printerror("Memory kick out reached end of the queue.");
		}
	}
	//cout<<"The page which is kicked out is: "<<(*it)<<endl;
	//cout<<"Now in the page pool is "<<endl;
	string kickout = (*it);
	pages.erase(it);

	pageMarked.erase(pageMarked.find(kickout));
	pageTime.erase(pageTime.find(kickout));
	/*for (map<string, bool>::iterator it = pageMarked.begin(); it != pageMarked.end(); it++)
		cout<<it->first<<' '<<it->second<<"\t";
	cout<<endl;*/
	return true;
}

//This is the model of least recently used memory.
class LRUMemory: MemoryBase {
	map<string, bool> pgMarked;
	map<string, long long> pgAvailTime, pgAccessTime;
	int pgNum;
public:

	bool memoryFull();
	bool accessPage(long long time, string pgName);
	bool insertPage(long long time, string pgName);
	bool kickOut(long long time);
	void markBusy(string pgName);
	void unmarkBusy(string pgName);
	int pgNumIncrease()
	{
		return ++pgNum;
	}
	int pgNumDecrease()
	{
		return --pgNum;
	}
	int pageNum()
	{
		return pgNum;
	}
};

void LRUMemory::markBusy(string pgName) {
	pgMarked[pgName] = true;
}

void LRUMemory::unmarkBusy(string pgName) {
	pgMarked[pgName] = false;
}

bool LRUMemory::accessPage(long long time, string pgName) {
	if (pgAvailTime.find(pgName) == pgAvailTime.end())
		return false;
	if (pgAvailTime[pgName] > time)
		return false;
	if (pgMarked[pgName])
		return false;

	if (pgAccessTime[pgName] < time) {
		pgAccessTime[pgName] = time;
	}
	return true;
}

bool LRUMemory::insertPage(long long time, string pgName) {
	pgAvailTime[pgName] = time;
	pgMarked[pgName] = false;
	pgAccessTime[pgName] = time;
	return true;
}

bool LRUMemory::memoryFull() {
	return (pgNum == globalPages);
}

/*
 * The main idea of this function is to find out the page whose access time is the smallest and kick it out.
 */
bool LRUMemory::kickOut(long long time) {
	if (!memoryFull())
		return false;

	string pgName = (pgAccessTime.begin())->first;
	long long leastTime = (pgAccessTime.begin())->second;
	map<string, long long>::iterator it;
	for (it = pgAccessTime.begin(); it != pgAccessTime.end(); it++) {
		if (pgMarked[it->first] || pgAvailTime[it->first] > time) {
			continue;
		}
		if (it->second < leastTime) {
			leastTime = it->second;
			pgName = it->first;
		}
	}
	/*cout<<"At time "<<time<<". The pages in the memory is: "<<endl;
	 map<string, long long>::iterator i;
	 for (i = pgAccessTime.begin(); i != pgAccessTime.end();i++)
	 {
	 cout<<"pageName: "<<i->first<<" Last Access time: "<<i->second<<endl;
	 }
	 cout<<"We will kick out: "<<pgName<<endl;*/

	pgAccessTime.erase(pgName);
	pgMarked.erase(pgName);
	pgAvailTime.erase(pgName);
	return true;
}

/*
 * This is the model of second chance algorithm.
 */
class SCAMemory: MemoryBase {
	deque<string> pages;
	map<string, bool> pgMarked, pgRef;
	map<string, long long> pgAvailTime;
	int pgNum;
public:
	bool memoryFull();
	bool accessPage(long long time, string pgName);
	bool insertPage(long long time, string pgName);
	bool kickOut(long long time);
	void markBusy(string pgName);
	void unmarkBusy(string pgName);
	int pgNumIncrease()
	{
		return ++pgNum;
	}
	int pgNumDecrease()
	{
		return --pgNum;
	}
	int pageNum()
	{
		return pgNum;
	}
};

bool SCAMemory::memoryFull() {
	return (pgNum == globalPages);
}

void SCAMemory::markBusy(string pgName) {
	pgMarked[pgName] = true;
}

void SCAMemory::unmarkBusy(string pgName) {
	pgMarked[pgName] = false;
}

bool SCAMemory::accessPage(long long time, string pgName) {
	if (pgAvailTime.find(pgName) == pgAvailTime.end()) {
		return false;
	}
	if (pgAvailTime[pgName] > time) {
		return false;
	}
	if (pgMarked[pgName])
		return false;
	pgRef[pgName] = true;
	return true;
}

bool SCAMemory::insertPage(long long time, string pgName) {
	pages.push_back(pgName);
	pgAvailTime[pgName] = time;
	pgMarked[pgName] = false;
	pgRef[pgName] = true;
	return true;
}

/*
 * Use the 2nd chance algorithm to kick the page out.
 */
bool SCAMemory::kickOut(long long time) {
	if (!memoryFull())
		return false;

	deque<string>::iterator it = pages.begin();

	while (pgMarked[(*it)] || pgAvailTime[(*it)] > time || pgRef[(*it)]) {
		if (!(pgMarked[(*it)] || pgAvailTime[(*it)] > time))
			pgRef[(*it)] = false;
		pages.push_back(*it);
		pages.erase(it);
		it = pages.begin();
	}

	string kopg = *it;
	pages.erase(it);
	pgRef.erase(kopg);
	pgMarked.erase(kopg);
	pgAvailTime.erase(kopg);
	return true;
}

/*
 * acess the page and unmark it, then call accesspage.
 */
bool Memory::fetch(long long time, string processName, string pgName) {
	/*if (lastFaultProcess.find(pgName) != lastFaultProcess.end()) {
		if (lastFaultProcess[pgName] == processName) {
			mmu->unmarkBusy(pgName);
			busyCount--;
			lastFaultProcess.erase(pgName);
		}
	}*/
	return mmu->accessPage(time, pgName);
}

/*
 * The page is not in the main memory. So we need to kick one page in the main memory out. Then extract the page in the disk and place it in the main memory.
 * The pages replacement algorithm depends on the mmu.
 */
bool Memory::swapPage(long long time, string faultingProcess,
		string faultingPage) {
	lastFaultProcess[faultingPage] = faultingProcess;
	if (waitingPages.find(faultingPage) != waitingPages.end()) {
		return true;
	} else  {
		long long arrivalTime = maxLL(time, maxtime) + globalSwap;
		waitingPages.insert(faultingPage);
		maxtime = arrivalTime;
		if (time == arrivalTime - globalSwap)
			scheduler->diskDiscardInterrupt(arrivalTime - globalSwap + 1);
		else
			scheduler->diskDiscardInterrupt(arrivalTime - globalSwap);
		mmu->insertPage(arrivalTime, faultingPage);
		mmu->markBusy(faultingPage);
		busyCount++;
		scheduler->diskInterrupt(arrivalTime, faultingPage);
		return true;
	}
}

void Memory::kickOut(long long time)
{
	if (!mmu->memoryFull())
		return;
	busyCount--;
	mmu->kickOut(time);
	if (mmu->pageNum()==globalPages)
		return;
	pgNumDecrease();
}

/*
 * When we meet a new process, we need to make a creation interrupt and handle it at that time.
 */
void Scheduler::creationInterrupt(long long time, string processName) {
	while (interrupts.find(ProcessCreationInterrupt(time)) != interrupts.end())
		time++;
	interrupts[ProcessCreationInterrupt(time)] = processName;
}

/*
 * Creat a disk interrupt, and we will handle it at that time.
 */
void Scheduler::diskInterrupt(long long time, string pgName) {
	//cout<<"The disk interrupt is caused at "<<time<<", page name is "<<pgName<<endl;
	interrupts[DiskInterrupt(time)] = pgName;
}

/*
 * The scheduler handles the creation interrupt, if there is a creation interrupt at this time, the scheduler will push the process onto the ready queue.
 */
void Scheduler::handleCreationInterrupttion(long long time,
		string currentProcess) {
	map<interrupt_t, Interrupt>::iterator it = interrupts.begin();
	for (;;it++)
	{
		if ((it->first).first != time)
		{
			return ;
		}
		if ((it->first).first == time && (it->first).second == 3)
			break;
	}

	//cout << "Handle creation interrupts at time, " << time
			//<< " process name is " << currentProcess << endl;
	readyQueue.push_back(interrupts[ProcessCreationInterrupt(time)]);

	infoTable[interrupts[ProcessCreationInterrupt(time)]] = make_pair(
			globalQuantum, 0);

	interrupts.erase(ProcessCreationInterrupt(time));

}

/*
 * Handle the disk interrupt, this means
 */
void Scheduler::handleDiskInterruption(long long time, string currentProcess) {
	map<interrupt_t, Interrupt>::iterator it = interrupts.begin();
	for (;;it++)
	{
		if ((it->first).first != time)
		{
			return ;
		}
		if ((it->first).first == time && (it->first).second == 2)
			break;
	}
	//cout<<"Handle disk interrupt at "<<time<<" ";
		string faultingPage = interrupts[DiskInterrupt(time)];
		if (memory->pgNum() != globalPages)
			memory->pgNumIncrease();
		string faultingProcess = faultingPage.substr(faultingPage.rfind("b") + 1);
		blockedNum -= 1;
		faultQueue.push_back(faultingProcess);
		/*set<string>::iterator iter = blockedQueue.begin();
		for (; iter != blockedQueue.end();) {
			if (blockedPages[(*iter)] == faultingPage) {
				string faultingProcess = *iter;
				faultQueue.push_back(faultingProcess);

				blockedPages.erase(faultingProcess);

				set<string>::iterator it = iter;
				++iter;
				if (iter != blockedQueue.end()) {
					string tmp = *iter;
					blockedQueue.erase(it);
					iter = blockedQueue.find(tmp);
				} else {
					blockedQueue.erase(it);
					break;
				}
			} else {
				iter++;
			}
		}*/
		memory->erasewaiting(faultingPage);

		interrupts.erase(DiskInterrupt(time));

}

void Scheduler::handleTimerInterruption(long long time, string currentProcess) {
	if (currentProcess != IDLE)
	{
        map<interrupt_t, Interrupt>::iterator it = interrupts.begin();
        for (;;it++)
        {
            if ((it->first).first != time)
            {
                return ;
            }
            if ((it->first).first == time && (it->first).second == 1)
                break;
        }
	}

	if (faultQueue.size() + readyQueue.size() == 0) {
		if (currentProcess != IDLE) {
			infoTable[currentProcess].first = globalQuantum;
			infoTable[currentProcess].second = time + globalQuantum;
			interrupts[TimerInterrupt(time + globalQuantum)] =		TimerMessage();
		} else {
			//do nothing
		}
	} else {
		string nextProcess = IDLE;
		if (faultQueue.size() != 0) {
			nextProcess = faultQueue.front();
			faultQueue.pop_front();
		} else {
			nextProcess = readyQueue.front();
			readyQueue.pop_front();
		}
			//kickout the current process

		if (currentProcess != IDLE) {
			infoTable[currentProcess].first = globalQuantum;
			readyQueue.push_back(currentProcess);
		}
		long long restTime = time + globalContextSwitch
				+ infoTable[nextProcess].first;
		if (infoTable[nextProcess].first == 0) {
			printerror("quantum Error!");
		} else {
			infoTable[nextProcess].second = restTime;
			interrupts[TimerInterrupt(restTime)] = TimerMessage();
		}

		cpu->ContextSwitch(nextProcess, time + globalContextSwitch);
	}
	if (interrupts.find(TimerInterrupt(time)) != interrupts.end())
		interrupts.erase(interrupts.find(TimerInterrupt(time)));

}

bool Scheduler::handleInterrupts(long long time, string currentProcess) {
	if (interrupts.size() == 0 && faultQueue.size() == 0
			&& readyQueue.size() == 0 && blockedNum == 0)
		return false;
	/*cout<<"At cycle: "<< time<<endl;
	map<interrupt_t, Interrupt>::iterator it = interrupts.begin();
	for(; it != interrupts.end(); it++)
		cout<<(it->first).first<<' '<<(it->first).second<<' '<<(it->second)<<"\t";
	cout<<endl;*/
	handleCreationInterrupttion(time, currentProcess);
	handleDiskDiscardInterrupt(time);
	handleDiskInterruption(time, currentProcess);
	handleTimerInterruption(time, currentProcess);

	return true;
}

void Scheduler::processTermination(long long time, string currentProcess) {
	int rest = infoTable[currentProcess].second;

	if (interrupts.find(TimerInterrupt(rest)) != interrupts.end()) {
		interrupts.erase(TimerInterrupt(rest));
	}


	cpu->ContextSwitch(IDLE, time + 1);
}

bool Scheduler::hasInterrupts(long long time) {
	if (interrupts.find(ProcessCreationInterrupt(time)) != interrupts.end()
			|| interrupts.find(DiskInterrupt(time)) != interrupts.end())
		return true;
	else
		return false;
}

void Scheduler::pgFault(long long time, string faultingProcess,
		string faultingPage) {
	if ((infoTable[faultingProcess].second == time)
			&& (hasInterrupts(time)
					|| faultQueue.size() + readyQueue.size() != 0)) {
		return;
	}
	//cout<<"The page fault is at "<<time<<" which is caused by"<<faultingProcess<<", the page is "<<faultingPage<<endl;
	cpu->pgFaultIncrease(faultingProcess);
	if (infoTable[faultingProcess].second == time) {
		interrupts.erase(TimerInterrupt(time));

		infoTable[faultingProcess].first = globalQuantum;
	} else {
		interrupts.erase(TimerInterrupt(infoTable[faultingProcess].second));
		infoTable[faultingProcess].first = infoTable[faultingProcess].second - time;
	}

	if (memory->swapPage(time, faultingProcess, faultingPage)) {
		blockedNum++;

		//blockedPages[faultingProcess] = faultingPage;
		cpu->ContextSwitch(IDLE, time);
	} /*else {
		printerror("5555");
		hangedQueue.push_back(faultingProcess);

		hangedPage.push_back(faultingPage);
		cout<<"The memory slot is full!"<<endl;
		for (deque<string>::iterator it = hangedQueue.begin(); it != hangedQueue.end(); it++)
			cout<<(*it)<<"    ";
		cout<<endl;
		for (deque<string>::iterator it = hangedPage.begin(); it != hangedPage.end();it++)
			cout<<(*it)<<"    ";
		cout<<endl;
		cout<<"The blocked pages and queues: "<<endl;
		for (map<string,string>::iterator it = blockedPages.begin(); it != blockedPages.end();it++)
					cout<<it->first<<"->"<<it->second<<"    ";
		cout<<endl;
		cout<<"The ready queue: "<<endl;
		for (deque<string>::iterator it = readyQueue.begin(); it != readyQueue.end(); it++)
			cout<<*it<<"    ";
		cout<<endl;
		cout<<"The fault queue: "<<endl;
		for (deque<string>::iterator it = faultQueue.begin(); it != faultQueue.end(); it++)
			cout<<*it<<"    ";
		memory->debug();
		cout<<endl;
		cpu->ContextSwitch(IDLE, time);
	}*/
}

void Scheduler::diskDiscardInterrupt(long long time)
{
	//cout<<"Set up a disk discard at time: "<<time<<endl;
	interrupts[DiskDiscardInterrupt(time)] = "Kick Out one page";
}

void Scheduler::handleDiskDiscardInterrupt(long long time)
{
	map<interrupt_t, Interrupt>::iterator it = interrupts.begin();
	for (;;it++)
	{
		if ((it->first).first != time)
		{
			return ;
		}
		if ((it->first).first == time && (it->first).second == 4)
			break;
	}
	//cout<<"Handle disk discard interrupt at time: "<<time<<endl;
	memory->kickOut(time);
	interrupts.erase(it);
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		printerror("The arguments' number is incorrect!");
	}
	freopen(argv[4], "r", stdin);
	globalPages = atoi(argv[1]);
	globalQuantum = atoi(argv[2]);
	CPU *c = new CPU;
	Memory *m = new Memory;
	Scheduler* s = new Scheduler;

	MemoryBase* base;
	if (argv[3] == FIFO)
		base = (MemoryBase*) (new FIFOMemory);
	else if (argv[3] == LRU)
		base = (MemoryBase*) (new LRUMemory);
	else if (argv[3] == SCA)
		base = (MemoryBase*) (new SCAMemory);

	s->initilize(c, m);
	c->initialize(s, m);
	m->initialize(s, base);

	char buff[255];
	double createtime;
	double burst;
	double IO;
	cout << "I will go to read!" << endl;
	while (scanf("%s %lf %lf %lf", buff, &createtime, &burst, &IO) != EOF) {
		cout << "I read!" << endl;
		s->creationInterrupt(createtime * globalCyclesPerSec, buff);
		string process = buff;
		processStartTime[buff] = createtime;
	}
	c->simulate();

}
