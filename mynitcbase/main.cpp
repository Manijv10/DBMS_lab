//stage 1
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>

int main(int argc, char *argv[]) {

  Disk disk_run;
  unsigned char buffer[BLOCK_SIZE];
  char message1[]="hello";
  Disk ::readBlock(buffer,7000); //read from block && load in buffer
  memcpy(buffer+20,message1,5); //modifying the buffer from the position 20 with new words
  Disk :: writeBlock(buffer,7000);  //write modified buffer content into disk
  
  unsigned char buffer2[BLOCK_SIZE];
  char message2[6];
  Disk::readBlock(buffer2,7000); //read the same block and load into buffer2
  memcpy(message2,buffer2 + 20,6);//cpy from buffer2+20 onwards into an array message2
  std::cout <<message2;
  

  return 0;
}

