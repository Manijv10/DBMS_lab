#include "OpenRelTable.h"
#include<iostream>
#include <cstring>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable:: OpenRelTable() {
    // initialize relCache and attrCache with nullptr
    for(int i = 0; i< MAX_OPEN; i++){
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
    }


    /******* Setting up Relation Cache entries *********/
    // we need to populate relation cache with entries for the relation and attribute catalog.

    // Setting up Relation Catalog relation in the Relation Cache Table
    RecBuffer relCatBlock(RELCAT_BLOCK);
    Attribute relCatRecord[RELCAT_NO_ATTRS];

    relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);
    struct RelCacheEntry relCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
    relCacheEntry.recId.block = RELCAT_BLOCK;
    relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT; // 0
    // allocate this on the heap beacuse we want it to persist outside this function
    RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;



    // Setting up Attribute Catalog relation in the Relation Cache Table
    relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);
    // set up the relation cache entry for the attribute catalog similarly
    // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
    RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
    relCacheEntry.recId.block = RELCAT_BLOCK; //block number 4 (relational catalog block)
    relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT; // 1
    RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[ATTRCAT_RELID]) = relCacheEntry;



    /******** Setting up Attribute Cache Entries *********/
    // Setting up Relation Catalog relation in the Attribute Cache Table
    RecBuffer attrCatBlock(ATTRCAT_BLOCK);
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

    // iterate through all the attributes of the relation catalog and create a linked list
    // NOTE: allocate each entry dynamically using malloc
    struct AttrCacheEntry *head, *last;
    for(int i = 0; i<6; i++){
        attrCatBlock.getRecord(attrCatRecord,i);
        struct AttrCacheEntry* attrCacheEntry = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);
        attrCacheEntry->recId.block = ATTRCAT_BLOCK;
        attrCacheEntry->recId.slot = i;
        if(i == 0){
            head = attrCacheEntry;
            last = attrCacheEntry;
        }
        else{
            last->next = attrCacheEntry;
            last = last->next;
        }
    }
    last->next = nullptr;
    AttrCacheTable::attrCache[RELCAT_RELID] = head;


    // Setting up Attribute Catalog relation in the Attribute Cache Table
    for(int i = 6; i<12; i++){
        attrCatBlock.getRecord(attrCatRecord,i);
        struct AttrCacheEntry* attrCacheEntry = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);
        attrCacheEntry->recId.block = ATTRCAT_BLOCK;
        attrCacheEntry->recId.slot = i;
        if(i == 6){
            head = attrCacheEntry;
            last = attrCacheEntry;
        }
        else{
            last->next = attrCacheEntry;
            last = attrCacheEntry;
        }
    }
    last->next = nullptr;
    AttrCacheTable::attrCache[ATTRCAT_RELID] = head;



for(int i=0;i<MAX_OPEN;i++){
    if(i==RELCAT_RELID){
        tableMetaInfo[i].free=false;
        strcpy(tableMetaInfo[i].relName, RELCAT_RELNAME);
    }
    else if(i == ATTRCAT_RELID){    // i == 1
        tableMetaInfo[i].free = false;
        strcpy(tableMetaInfo[i].relName, ATTRCAT_RELNAME);
    }
    else{
        tableMetaInfo[i].free = true;
    }
}

}

OpenRelTable::~OpenRelTable(){

    for (int i = 2; i < MAX_OPEN; ++i) {
        if (!tableMetaInfo[i].free) {
          OpenRelTable::closeRel(i); // we will implement this function later
        }
      }
    // free all the memories that is allocted in the constructor
    for(int i = 0; i<MAX_OPEN; i++){
        if(RelCacheTable::relCache[i] != nullptr){
            free(RelCacheTable::relCache[i]);
            RelCacheTable::relCache[i] = nullptr;
        }
        if(AttrCacheTable::attrCache[i] != nullptr){
            struct AttrCacheEntry* attrCacheEntry = AttrCacheTable::attrCache[i];
            while(attrCacheEntry != nullptr){
                struct AttrCacheEntry* tempCacheEntry = attrCacheEntry;
                attrCacheEntry = attrCacheEntry->next;
                free(tempCacheEntry);
            }
            AttrCacheTable::attrCache[i] = nullptr;
        }
    }
}


/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/

//getrelId function
int OpenRelTable::getRelId(char relName[ATTR_SIZE]){
    for(int i=0;i<MAX_OPEN;i++){
        if(strcmp(tableMetaInfo[i].relName, relName) == 0){
            std::cout << "The value of relid is: " << i<< std::endl;
  
            return i;
        }

    }
    return E_RELNOTOPEN;
}

//fetFreeOpenRelTableEntry
int OpenRelTable::getFreeOpenRelTableEntry(){
    for(int i=2;i<MAX_OPEN;i++){ //first two will never be free
        if(tableMetaInfo[i].free==true){
            return i;
        }
    }
    return E_CACHEFULL;
}

//openRel

int OpenRelTable::openRel(char relName[ATTR_SIZE]){
    int relId=getRelId(relName);
    if(relId !=E_RELNOTOPEN){
        return relId;
    }
    else{
        relId=getFreeOpenRelTableEntry();
        if(relId==E_CACHEFULL){
            return E_CACHEFULL;
        }
    }

    //setting up relation cache entry for the relation
    Attribute relationname;
    strcpy(relationname.sVal,relName);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    char relCatAttrRelName[ATTR_SIZE];
    strcpy(relCatAttrRelName, RELCAT_ATTR_RELNAME);
    RecId relCatRecId=BlockAccess::linearSearch(RELCAT_RELID,relCatAttrRelName ,relationname, EQ);
    if(relCatRecId.block==-1 && relCatRecId.slot==-1){
    return E_RELNOTEXIST;
    }
    else{
    RecBuffer relCatBlock(relCatRecId.block);
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    relCatBlock.getRecord(relCatRecord,relCatRecId.slot);
    struct RelCacheEntry relcacheentry;
    RelCacheTable::recordToRelCatEntry(relCatRecord,&relcacheentry.relCatEntry);
    relcacheentry.recId.block=relCatRecId.block;
    relcacheentry.recId.slot=relCatRecId.slot;
    RelCacheTable::relCache[relId]= (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[relId]) = relcacheentry;

   //setting up attribute cache entry for relation
   AttrCacheEntry *listHead,*now;
   RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
   for(int i=0;i<relcacheentry.relCatEntry.numAttrs;i++){
        RecId attrcatRecId=BlockAccess::linearSearch(ATTRCAT_RELID,relCatAttrRelName,relationname,EQ);
        if(attrcatRecId.block==-1 && attrcatRecId.slot==-1){
        return E_RELNOTEXIST;
        }

        RecBuffer attrCatBlock(attrcatRecId.block);
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        attrCatBlock.getRecord(attrCatRecord,attrcatRecId.slot);
        struct AttrCacheEntry* attrcacheentry = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&attrcacheentry->attrCatEntry);
        attrcacheentry->recId=attrcatRecId;
        if(i==0){
            listHead=attrcacheentry;
            now=attrcacheentry;
        }
        else{
            now->next=attrcacheentry;
            now=attrcacheentry;
        }
        }
        now->next=nullptr;
        AttrCacheTable::attrCache[relId]=listHead;

        ///////setting up the metadata in the open relation table for the relation/////
        OpenRelTable::tableMetaInfo[relId].free=false;
        strcpy(OpenRelTable::tableMetaInfo[relId].relName,relName);

   }

   return relId;

}

////close rel////

int OpenRelTable::closeRel(int relId) {
    if (relId==RELCAT_RELID || relId==ATTRCAT_RELID) {
      return E_NOTPERMITTED;
    }
  
    if (relId <0|| relId >= MAX_OPEN) {
      return E_OUTOFBOUND;
    }
  
    if (tableMetaInfo[relId].free==true) {
      return E_RELNOTOPEN;
    }
  
    // free the memory allocated in the relation and attribute caches which was
    // allocated in the OpenRelTable::openRel() function
    free(RelCacheTable::relCache[relId]);

    
    AttrCacheEntry *entry, *temp;
    entry=AttrCacheTable::attrCache[relId];
    while(entry !=nullptr){
        temp=entry;
        entry=entry->next;
        free(temp);
    }
    
    // update `tableMetaInfo` to set `relId` as a free slot
    // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
    tableMetaInfo[relId].free=true;
    RelCacheTable::relCache[relId] = nullptr;
    AttrCacheTable::attrCache[relId] = nullptr;

    return SUCCESS;
}
