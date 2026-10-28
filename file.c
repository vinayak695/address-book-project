
#include <stdio.h>
#include "file.h"
 
// Save: write every contact as one line -> name,phone,email

void saveContactsToFile(AddressBook *addressBook)
{
    FILE *fp = fopen("contacts.csv", "w");
    if (fp == NULL)
    {
        printf("Error: Could not open the file for saving\n");
        return;
    }
 
    for (int i = 0; i < addressBook->contactCount; i++)
    {
        fprintf(fp, "%s,%s,%s\n",
                addressBook->contacts[i].name,
                addressBook->contacts[i].phone,
                addressBook->contacts[i].email);
    }
 
    fclose(fp);
}
 
// Load: read each line back into the structure
void loadContactsFromFile(AddressBook *addressBook)
{
    FILE *fp = fopen("contacts.csv", "r");
    if (fp == NULL)
        return;          // no file yet, start empty
 
    while (addressBook->contactCount < MAX_CONTACTS &&
           fscanf(fp, " %49[^,],%19[^,],%49[^\n]",
                  addressBook->contacts[addressBook->contactCount].name,
                  addressBook->contacts[addressBook->contactCount].phone,
                  addressBook->contacts[addressBook->contactCount].email) == 3)
    {
        addressBook->contactCount++;
    }
 
    fclose(fp);
}    

