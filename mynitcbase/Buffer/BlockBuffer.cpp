
#include "BlockBuffer.h"
#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum) {
  // initialise this.blockNum with the argument
  this->blockNum=blockNum;
}

// calls the parent class constructor using subclass recbuffer.Ensures the blocknum initialisation occurs only in the parent class
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum){}


// load the block header into the argument pointer
int BlockBuffer::getHeader(struct HeadInfo *head) {  //to get the header of the block
  unsigned char buffer[BLOCK_SIZE];
  Disk::readBlock(buffer,this->blockNum);

  memcpy(&head->numSlots, buffer + 24, 4);
  memcpy(&head->numEntries, buffer+16, 4);
  memcpy(&head->numAttrs,buffer+20,4);
  memcpy(&head->rblock,buffer+12, 4);
  memcpy(&head->lblock,buffer+8, 4);

  return SUCCESS;
}

// load the record at slotNum into the argument pointer
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  struct HeadInfo head;
  unsigned char buffer[BLOCK_SIZE];
  
  // get the header using this.getHeader() function
  BlockBuffer::getHeader(&head);

  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  // read the block at this.blockNum into a buffer
  Disk::readBlock(buffer,this->blockNum);
  int recordSize = attrCount * ATTR_SIZE;
  int slotMapSize = slotCount; //DON'T FORGET TO MENTION THIS
  unsigned char *slotPointer = buffer + (recordSize *slotNum)+ 32 +slotMapSize;

  // load the record into the rec data structure
  memcpy(rec, slotPointer, recordSize);

  return SUCCESS;
}

