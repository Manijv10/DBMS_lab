#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[])
{
    Disk disk_run;

    RecBuffer relCatBuffer(RELCAT_BLOCK);
    RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

    HeadInfo relCatHeader;
    HeadInfo attrCatHeader;

    relCatBuffer.getHeader(&relCatHeader);
    attrCatBuffer.getHeader(&attrCatHeader);

    for (int i = 0; i < relCatHeader.numEntries; i++)
    {

        Attribute relCatRecord[RELCAT_NO_ATTRS];
        relCatBuffer.getRecord(relCatRecord, i);
        

        printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

        for (int j = 0; j < attrCatHeader.numEntries; j++)
        {
            Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
            attrCatBuffer.getRecord(attrCatRecord, j);

            if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0) { //DO NOT WRITE [ATTRCAT_ATTR_NAME_INDEX]
      		if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,"Student")==0){
      			if(strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class")==0){
      				strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Batch");
      			}
      		}
      		if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,"Events")==0){
      			if(strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"location")==0){
      				strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"CT");
      			}
      		}
      		

      		
                const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
                printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
                if (j == (attrCatHeader.numSlots) - 1 && attrCatHeader.rblock != -1)
                { // that is it crossed one block
                    attrCatBuffer = RecBuffer(attrCatHeader.rblock);
                    attrCatBuffer.getHeader(&attrCatHeader);
 		            j=-1;
                }
          
           }
        }
        
        printf("\n");
   }
    

    return 0;
}
