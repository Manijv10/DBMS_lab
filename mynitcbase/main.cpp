#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>


int main(int argc, char *argv[]){
  Disk disk_run;
  StaticBuffer buffer;
  //update_names();
  OpenRelTable cache;

  for(int i=0;i<=2;i++){
    RelCatEntry relCatEntry;
    int getRelResult = RelCacheTable::getRelCatEntry(i,&relCatEntry);
   if(getRelResult == SUCCESS){
    printf("Relation: %s\n",relCatEntry.relName);
    for(int j=0;j<relCatEntry.numAttrs;j++){
        AttrCatEntry attrcatEntry;
        int getAttrResult = AttrCacheTable::getAttrCatEntry(i,j,&attrcatEntry);
        if(getAttrResult == SUCCESS){
            const char* attrType = attrcatEntry.attrType== NUMBER ? "NUM" : "STR";
            printf(" %s: %s\n", attrcatEntry.attrName, attrType); 
        }
    }
  }
  printf("\n");
}
return 0;
}
