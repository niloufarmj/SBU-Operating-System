//
//  main.cpp
//  CA2
//
//  Created by Parsa Hejabi on 12/8/18.
//  Copyright © 2018 Parsa Hejabi. All rights reserved.
//

#include <bits/stdc++.h>
using namespace std;

// constants
const int tlbEntries = 16;
const int pageTableEntries = 256;
const int pageSize = 256;
const int pageTableSize = pageTableEntries * pageSize;
const int physicalMemoryEntries = 265;
const int frameSize = 256;
const int ramSize = physicalMemoryEntries * frameSize;

int TLB[tlbEntries][2];
int pageTable[pageTableEntries];
int physicalMemory[ramSize];

// files
FILE * addressesFile;
FILE * backingStoreFile;

// main vars
int physicalAddress;
int pageNumber;
int pageOffset;
//int pageTableFullIndex;
int physicalMemoryFullIndex;
int tlbFullIndex;
double pageFaultCount;
double tlbHitCount;
double addressCount;

// temp vars
const char * addressesFileAddress;
const char * backingStoreFileAddress = "BACKING_STORE.txt";
char inputString[10];
int inputNumber;
bool tlbHit;
bool pageTableHit;

void init(){
    for (int i = 0; i < pageTableEntries; i++) {
        pageTable[i] = -1;
    }
    for (int i = 0; i < ramSize; i++) {
        physicalMemory[i] = -1;
    }
    for (int i = 0; i < tlbEntries; i++) {
        TLB[i][0] = -1;
        TLB[i][1] = -1;
    }
}

void debugExtracting(){
    cout << "inputNumber:\t" << inputNumber << endl;
    cout << "pageNumber:\t" << pageNumber << endl;
    cout << "pageOffset:\t" << pageOffset << endl;
}

bool searchTLB(){
    for (int i = 0; i < tlbEntries; i++) {
        if (TLB[i][0] == pageNumber) {
            physicalAddress = (TLB[i][1] * frameSize) + pageOffset;
//            DEBUG
            cout << "TLB hit" << endl;
            tlbHitCount++;
            return true;
        }
    }
    return false;
}

bool searchPageTable(){
    if (pageTable[pageNumber] != -1) {
//        DEBUG
        cout << "Page table hit" << endl;
        physicalAddress = (pageTable[pageNumber] * frameSize) + pageOffset;
        TLB[tlbFullIndex][0] = pageNumber;
        TLB[tlbFullIndex][1] = pageTable[pageNumber];
        tlbFullIndex++;
        tlbFullIndex = tlbFullIndex == 16 ? 0 : tlbFullIndex;
        return true;
    }
    return false;
}

void recoverFromDisk(){
//    DEBUG
    cout << "Recover from disk" << endl;
//    fseek(backingStoreFile, pageNumber * frameSize, SEEK_SET);
    fseek(backingStoreFile, 6 * pageNumber * frameSize, SEEK_SET);
    char bkString[7];
    for (int i = 0; i < frameSize; i++) {
        fgets(bkString, sizeof(bkString), backingStoreFile);
        int bkNumber = atoi(bkString);
//        DEBUG
//        cout << "Disk read number:\t" << bkNumber << endl;
//        cout << (physicalMemoryFullIndex * frameSize) + i << endl;
        physicalMemory[(physicalMemoryFullIndex * frameSize) + i] = bkNumber;
    }
    physicalAddress = (physicalMemoryFullIndex * frameSize) + pageOffset;
    pageTable[pageNumber] = physicalMemoryFullIndex;
    TLB[tlbFullIndex][0] = pageNumber;
    TLB[tlbFullIndex][1] = physicalMemoryFullIndex;
    physicalMemoryFullIndex++;
//    pageTableFullIndex++;
    tlbFullIndex++;
//    physicalMemoryFullIndex = physicalMemoryFullIndex == 16 ? 0 : physicalMemoryFullIndex;
//    pageTableFullIndex = pageTableFullIndex == 16 ? 0 : pageTableFullIndex;
    tlbFullIndex = tlbFullIndex == 16 ? 0 : tlbFullIndex;
}

void debug(){
    cout << pageFaultCount << "\t" << tlbHitCount << endl;
}

int main(int argc, const char * argv[]) {
    
    if (argc == 1) {
        cerr << "No extra command line argument passed!" << endl;
        cerr << "Try again running with input file address please." << endl;
        return 1;
    }
    
    init();
    
    addressesFileAddress = argv[1];
//    DEBUG
//    cout << inputFileAddress << endl;
    addressesFile = fopen(addressesFileAddress, "r");
    
    if (addressesFile == NULL) {
        cerr << "Can't read addresses file!" << endl;
        return 1;
    }
    
    backingStoreFile = fopen(backingStoreFileAddress, "r");
    
    if (backingStoreFile == NULL) {
        cerr << "Can't read BACKING STORE file!" << endl;
        return 1;
    }
    
    while (fgets(inputString, sizeof(inputString), addressesFile)) {
        addressCount++;
        inputNumber = atoi(inputString);
        pageNumber = inputNumber & 65280;
        pageNumber = pageNumber >> 8;
        pageOffset = inputNumber & 255;
        
//        debugExtracting();
        
        tlbHit = searchTLB();
        
        if (!tlbHit) {
            pageTableHit = searchPageTable();
            if (!pageTableHit) {
                pageFaultCount++;
                recoverFromDisk();
            }
        }
        
        cout << "The logical address:\t" << inputNumber << endl;
        cout << "The physical address:\t" << physicalAddress << endl;
//        fseek(backingStoreFile, physicalAddress, SEEK_SET);
//        char bkString[1];
//        fgets(bkString, sizeof(bkString), backingStoreFile);
//        int bkNumber = atoi(bkString);
        cout << "tlb index:\t" << tlbFullIndex << endl;
        cout << "The signed byte value at the physical address:\t" << physicalMemory[physicalAddress] << endl;
        cout << endl;
    }
    
//    debug();
    
    cout.precision(17);
    cout << "Page-fault rate:\t" << fixed << ((double)(pageFaultCount/addressCount)) * 100 << endl;
    cout << "TLB hit rate:\t" << fixed << ((double)(tlbHitCount/addressCount)) * 100 << endl;
    
    fclose(addressesFile);
    fclose(backingStoreFile);
    return 0;
}
