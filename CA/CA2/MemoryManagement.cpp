//Memory Manager
//Operating Systems - Computer Assignment 2
//Professor: Alireza Shameli
//Author: Niloufar Moradi Jam

#include <bits/stdc++.h>
#include<stdio.h>
#include <fcntl.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {

    const int page_size = 256;
    const int page_table_length = 256; 
    const int physical_memory_length = 256; 
    const int tlb_length = 16; 

    int physical_memory[physical_memory_length][page_size];
    int physical_memory_index = 0;
    int page_table[page_table_length];
    int tlb[tlb_length][2];

    //initialization

    for (int i = 0; i < page_table_length; i++) 
        page_table[i] = -1;
        
    
    for (int i = 0; i < tlb_length; i++){
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }

    char* in_file = argv[1];
    char* out_file = argv[2];
    char* store_file_ = argv[3];
    FILE* in_ptr; // Address file pointer.
    FILE* out_ptr; // Output file pointer.
    FILE* store_file;


     /* Open the address file. */
        if ((in_ptr = fopen(in_file, "r")) == NULL) {
            /* If fopen fails, print error and exit. */
            printf("Input file could not be opened.\n");

            exit(EXIT_FAILURE);
        }

        /* Open the output file. */
        if ((out_ptr = fopen(out_file, "a")) == NULL) {
            /* If fopen fails, print error and exit. */
            printf("Output file could not be opened.\n");

            exit(EXIT_FAILURE);
        }

    store_file = fopen(store_file_, "rb");

    char line[8];

    int page_number = 0;
    int physical_address = 0;
    int page_fault_number = 0;
    double pageFaultRate = 0.0;
    int tlb_hit_number = 0;
    int test_number = 0;
    double TLBHitRate = 0.0;

    while ( fgets(line, sizeof(line), in_ptr) ) {
        int virtual_address = atoi(line);
        int offset = virtual_address & 255;
        int page_table_number = virtual_address >> 8;
        test_number ++;
        int frame_number = -1;
        int value;
        int physical_address;

        //cout << offset << endl;

        //check tlb
        for (int i = 0; i < tlb_length; i++){
            if (tlb[i][0] == page_table_number){
                tlb_hit_number ++;
                frame_number = tlb[i][1];
                break;
            }
        }

        //cout << frame_number << endl;

        //tlb hit
        if (frame_number != -1) {
            physical_address = frame_number*page_size + offset;
            value = physical_memory[frame_number][offset];
        }

        //search in page table
        else {
            
            frame_number = page_table[page_table_number];

            //cout << frame_number << endl;
            //page fault
            if (frame_number == -1) {
                
                frame_number = physical_memory_index++ % 256;
                
                page_table[page_table_number] = frame_number;
        
                page_fault_number++;
     
                //navigate to the page number in the backing store
                fseek(store_file, page_table_number*256, SEEK_SET);
  
                //read FRAME_SIZE bytes into physical memory
                char val[7];

                for (int i = 0; i < page_size; i++) {
                    fgets(val, 7, store_file);
                    physical_memory[frame_number][i] = atoi(val);
                }


                // update tlb with fifo

                for (int i = tlb_length-2; i >= 0; i--) {
                    tlb[i+1][0] = tlb[i][0];
                    tlb[i+1][1] = tlb[i][1];
                }
                
                tlb[0][0] = page_table_number;
                tlb[0][1] = page_table[page_table_number];
                

                physical_address = frame_number*page_size + offset;
                value = physical_memory[frame_number][offset];

                //for (int i = 0; i < tlb_length; i++) {
                    //cout << tlb[i][0] << endl;
                //}
                        
            }
            
            //found in page table
            else { 
                
                physical_address = frame_number*page_size + offset;
                value = physical_memory[frame_number][offset];

                // update tlb with fifo

                for (int i = 0; i < tlb_length-1; i++) {
                    tlb[i+1][0] = tlb[i][0];
                    tlb[i+1][1] = tlb[i][1];
                }
                
                tlb[0][0] = page_table_number;
                tlb[0][1] = page_table[page_table_number];
                 
            }
               
        }

        //cout << frame_number << endl;
        fprintf(out_ptr, "Virtual Address: %d - Physical Address: %d - Value: %d \n", virtual_address, physical_address, value);
    }

    fprintf(out_ptr, "Number of Translated Addresses = %d \n", test_number);

    pageFaultRate = (double) page_fault_number / test_number;
    // cout << "Page Faults = " << page_fault_number << endl;
    // cout << "Page Fault Rate = " << pageFaultRate << endl; 
    fprintf(out_ptr, "Page Faults = %d \n", page_fault_number);
    fprintf(out_ptr, "Page Fault Rate = %0.3lf \n", pageFaultRate);
    
    TLBHitRate = (double) tlb_hit_number / test_number;
    // cout << "TLB Hits = " << tlb_hit_number << endl;
    // cout << "TLB Hit Rate = " << TLBHitRate << endl; 
    fprintf(out_ptr, "TLB Hits = %d \n", tlb_hit_number);
    fprintf(out_ptr, "TLB Hit Rate = %0.3lf \n",TLBHitRate);

    //jaye cout ha byad fprintf bzarm vase neveshtn tu file

    exit(EXIT_SUCCESS);

}