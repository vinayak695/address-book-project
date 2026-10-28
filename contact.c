#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"
#include "file.h"


// colour codes 

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"



// name must have 3+ characters, only alphabets, and a single space between words
static int isValidName(const char *name)
{
    int k = 0;

    if(strlen(name) < 3)
    {
        printf("Name must contain atleast 4 characters\n");
        return 0;
    }

    while(name[k] != '\0')
    {
        if((name[k] >= 'A' && name[k] <= 'Z') ||
           (name[k] >= 'a' && name[k] <= 'z'))
        {
            k++;
        }
        else if(name[k] == ' ')
        {
            if(name[k + 1] == ' ')
            {
                printf("between two names only one space is allowed\n");
                return 0;
            }
            k++;
        }
        else
        {
            printf("Name should contain only alphabets\n");
            return 0;
        }
    }
    return 1;
}

// phone must be 10 digits and the first digit must be 6 to 9
static int isValidPhone(const char *phone)
{
    int k;

    if(strlen(phone) != 10)
    {
        printf("Phone number must contain exactly 10 digits\n");
        return 0;
    }

    if(phone[0] < '6' || phone[0] > '9')
    {
        printf("First digit must be between 6 and 9\n");
        return 0;
    }

    for(k = 0; phone[k] != '\0'; k++)
    {
        if(phone[k] < '0' || phone[k] > '9')
        {
            printf("Only digits are allowed\n");
            return 0;
        }
    }
    return 1;
}

// email: lowercase letters, digits, one @, one dot after @, ends with .com
static int isValidEmail(const char *email)
{
    int k;
    int atCount = 0;
    int dotCount = 0;
    int atPos = -1;
    int dotPos = -1;
    int len = strlen(email);

    for(k = 0; email[k] != '\0'; k++)
    {
        if((email[k] >= 'a' && email[k] <= 'z') ||
           (email[k] >= '0' && email[k] <= '9'))
        {
            continue;
        }
        else if(email[k] == '@')
        {
            atCount++;
            atPos = k;
        }
        else if(email[k] == '.')
        {
            dotCount++;
            dotPos = k;
        }
        else
        {
            printf("Invalid symbol\n");
            return 0;
        }
    }

    if(atCount == 0)
    {
        printf("Missing @\n");
        return 0;
    }

    if(atCount > 1)
    {
        printf("Multiple @ symbols are not allowed\n");
        return 0;
    }

    if(dotCount != 1)
    {
        printf("Exactly one dot is required\n");
        return 0;
    }

    if(dotPos < atPos)
    {
        printf("Dot must appear after @\n");
        return 0;
    }

    if(dotPos - atPos <= 1)
    {
        printf("At least one character required between @ and .\n");
        return 0;
    }

    if(len < 4 ||
       email[len-4] != '.' ||
       email[len-3] != 'c' ||
       email[len-2] != 'o' ||
       email[len-1] != 'm')
    {
        printf("Email must end with .com\n");
        return 0;
    }
    return 1;
}

// returns 1 if another contact (not at skipIndex) already has this phone
static int isPhoneTaken(AddressBook *addressBook, const char *phone, int skipIndex)
{
    int k;

    for(k = 0; k < addressBook->contactCount; k++)
    {
        if(k != skipIndex && strcmp(addressBook->contacts[k].phone, phone) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// returns 1 if another contact (not at skipIndex) already has this email
static int isEmailTaken(AddressBook *addressBook, const char *email, int skipIndex)
{
    int k;

    for(k = 0; k < addressBook->contactCount; k++)
    {
        if(k != skipIndex && strcmp(addressBook->contacts[k].email, email) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// finds contacts by name; if several have the same name, lets the user pick one

// returns index of the chosen contact, -1 if none found, -2 if selection was invalid
static int selectByName(AddressBook *addressBook, const char *name)
{
    int matches[100];
    int total = 0;
    int pick = 0;
    int k;

    for(k = 0; k < addressBook->contactCount && total < 100; k++)
    {
        if(strcmp(addressBook->contacts[k].name, name) == 0)
        {
            matches[total++] = k;
        }
    }

    if(total == 0)
    {
        return -1;
    }

    if(total == 1)
    {
        return matches[0];
    }

    printf("\nMultiple contacts found with this name\n");
    for(k = 0; k < total; k++)
    {
        printf("%d. %s %s %s\n",
               k + 1,
               addressBook->contacts[matches[k]].name,
               addressBook->contacts[matches[k]].phone,
               addressBook->contacts[matches[k]].email);
    }

    printf("Select Record : ");
    if(scanf("%d", &pick) != 1 || pick < 1 || pick > total)
    {
        printf("Invalid selection\n");
        while(getchar() != '\n');      // clear bad input
        return -2;
    }

    return matches[pick - 1];
}


//  To Display all contacts

void listContacts(AddressBook *addressBook, int sortCriteria) 
{
    printf(BOLD CYAN "\n========== CONTACT LIST ==========\n" RESET);

    for(int i = 0; i < addressBook->contactCount; i++)
    {
        printf(BOLD YELLOW "\nRecord %d\n" RESET, i + 1);
        printf(GREEN "Name  : " RESET "%s\n", addressBook->contacts[i].name);
        printf(GREEN "Phone : " RESET "%s\n", addressBook->contacts[i].phone);
        printf(GREEN "Email : " RESET "%s\n", addressBook->contacts[i].email);
    }
}


// to Initialize address book

void initialize(AddressBook *addressBook) {
    addressBook->contactCount = 0;
    // populateAddressBook(addressBook);
    
    // Load contacts from file during initialization (After files)
    loadContactsFromFile(addressBook);
}


// To save the contact and exit program

void saveAndExit(AddressBook *addressBook) {

    saveContactsToFile(addressBook); // Save contacts to file

    exit(EXIT_SUCCESS); // Exit the program

}

//To create a new contact with validation 

void createContact(AddressBook *addressBook)
{
	
     Contact newContact;
    
    printf("enter your name:");                                  
    scanf(" %[^\n]",newContact.name);

    printf("enter your phone number:");
    scanf(" %10s",newContact.phone);

    printf("Enter Email ID:");
    scanf(" %s", newContact.email);


// Name length validation atlest 3 char

    int store= strlen(newContact.name);
    if (store <=3)
    {
        printf("Name must contain atleast 4 character");
        return ;
    }


    // Name character validation

    int i=0; 

    while(newContact.name[i]!='\0')
    {
        if ((newContact.name[i]>='A' && newContact.name[i]<='Z') || 
        (newContact.name[i]>='a' && newContact.name[i]<='z'))

        {
           i++;  
        }
    
    // end 

    // space validation  

        else if(newContact.name[i] == ' ')
        {
            if (newContact.name[i+1] == ' ')
            {
                printf("between two names only one space is allowed");
                return ;
            }
            i++;
        }
        
        else
        {
            printf("Name should contain only alphabets");
            return ;
        } 

    }

     //  end


    // Phone number length validation

    int contactlength = strlen(newContact.phone);
    {
        if (contactlength != 10)
        {
            printf("Phone number must contain exactly 10 digits :");
            return ;
        }
    
    
    
    // First digit validation  is between 6 t 9 

    if(newContact.phone[0] <'6' || newContact.phone[0] > '9')
{
    printf("first digit must be between 6 and 9");
    return ;
}

// end

// to check if contact contain any character or symbol

    int j= 0;

    while(newContact.phone[j]!='\0')
    {
        if (newContact.phone[j]>='0' && 
            newContact.phone[j]<='9')

        {
            j++;
        }
        else 
        {
            printf("Other then Digit any other character not allowed in phone number");
            return ;
        }
        
        
    }

    // end


    // Email validation

    int m;
    int atCount = 0;
    int dotCount = 0;
    int atPos = -1;
    int dotPos = -1;

    // Check characters and count @ and .
    for(m = 0; newContact.email[m] != '\0'; m++)
    {
        if(newContact.email[m] >= 'a' && newContact.email[m] <= 'z')
        {
        }
        else if(newContact.email[m] >= '0' && newContact.email[m] <= '9')
        {
        }
        else if(newContact.email[m] == '@')
        {
            atCount++;
            atPos = m;
        }
        else if(newContact.email[m] == '.')
        {
            dotCount++;
            dotPos = m;
        }
        else
        {
            printf(" Invalid symbol");
            return ;
        }
    }

    // Check @

    if(atCount == 0)
    {
        printf(" Missing @ ");
        return ;
    }

    if(atCount > 1)
    {
        printf("Multiple @ symbols are not allowed");
        return ;
    }

    // Check .
    if(dotCount != 1)
    {
        printf("Exactly one dot is required");
        return ;
    }

    // Dot must be after @
    if(dotPos < atPos)
    {
        printf("Dot must appear after @");
        return ;
    }

    // At least one character between @ and .
    if(dotPos - atPos <= 1)
    {
        printf("At least one character required between @ and .\n");
        return ;
    }

    // Check .com at end
    int len = strlen(newContact.email);

    if(len < 4 ||
       newContact.email[len-4] != '.' ||
       newContact.email[len-3] != 'c' ||
       newContact.email[len-2] != 'o' ||
       newContact.email[len-1] != 'm')
    {
        printf("Email must end with .com\n");
        return ;
    }

    // Email unique validation


    for(int i = 0; i < addressBook->contactCount; i++)
    {
        if(strcmp(addressBook->contacts[i].email, newContact.email) == 0)
        {
            printf("Email ID already exists\n");
            return;
        }
    }



    // Phone number unique validation


    for(int i = 0; i < addressBook->contactCount; i++)
    {
        if(strcmp(addressBook->contacts[i].phone, newContact.phone) == 0)
        {
            printf("Phone Number already exists\n");
            return;
        }
    }


    //  to store contact in address book

    addressBook->contacts[addressBook->contactCount] = newContact;
    addressBook->contactCount++;

    }

    
}


// To search the contact using name,phone number or email

void searchContact(AddressBook *addressBook)
{
    int option;
    int match = 0;

    printf("\n========== SEARCH CONTACT ==========\n");
    printf("1. Search using Name\n");
    printf("2. Search using Phone Number\n");
    printf("3. Search using Email ID\n");
    printf("4. Exit\n");

    printf("Enter your choice : ");
    scanf("%d", &option);

    switch(option)
    {
        // Search by name

        case 1:
        {
            char name[50];

            printf("Enter Name : ");
            scanf(" %[^\n]", name);

            for(int i = 0; i < addressBook->contactCount; i++)
            {
                if(strcmp(name, addressBook->contacts[i].name) == 0)
                {
                    printf("\nRecord Found\n");
                    printf("Name  : %s\n", addressBook->contacts[i].name);
                    printf("Phone : %s\n", addressBook->contacts[i].phone);
                    printf("Email : %s\n", addressBook->contacts[i].email);

                    match = 1;
                }
            }
            break;
        }

        case 2:
        {
            // Search by phone number
            
            char phone[11];

            printf("Enter Phone Number : ");
            scanf("%10s", phone);

            for(int i = 0; i < addressBook->contactCount; i++)
            {
                if(strcmp(phone, addressBook->contacts[i].phone) == 0)
                {
                    printf("\nRecord Found\n");
                    printf("Name  : %s\n", addressBook->contacts[i].name);
                    printf("Phone : %s\n", addressBook->contacts[i].phone);
                    printf("Email : %s\n", addressBook->contacts[i].email);

                    match = 1;
                }
            }
            break;
        }

        case 3:

        {
            // Search by email ID

            char email[100];

            printf("Enter Email ID : ");
            scanf("%99s", email);

            for(int i = 0; i < addressBook->contactCount; i++)
            {
                if(strcmp(email, addressBook->contacts[i].email) == 0)
                {
                    printf("\nRecord Found\n");
                    printf("Name  : %s\n", addressBook->contacts[i].name);
                    printf("Phone : %s\n", addressBook->contacts[i].phone);
                    printf("Email : %s\n", addressBook->contacts[i].email);

                    match = 1;
                }
            }

            break;

        }

        case 4:
            return;

        default:
            printf("Invalid Option\n");
            return;
    }

    if(match == 0)

    {
        printf("No matching contact found\n");
    }
} 


// Edit existing contact details

void editContact(AddressBook *addressBook)

{
	 int searchChoice;
    int found = -1;

    printf("\n----- EDIT MENU -----\n");
    printf("1. Search by Name\n");
    printf("2. Search by Phone Number\n");
    printf("3. Search by Email ID\n");
    printf("4. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &searchChoice);

    if(searchChoice == 1)
    {
        // edit by name

        char name[50];

        printf("Enter Name: ");
        scanf(" %[^\n]", name);

        // handles duplicate names (asks which record to edit)
        found = selectByName(addressBook, name);

        if(found == -2)
        {
            return;
        }
    }

    else if(searchChoice == 2)
    {

        // edit by phone

        char phone[11];

        printf("Enter Phone Number: ");
        scanf("%10s", phone);

        for(int i = 0; i < addressBook->contactCount; i++)
        {
            if(strcmp(addressBook->contacts[i].phone, phone) == 0)

            {
                found = i;
                break;
            }
        }
    }

    else if(searchChoice == 3)

    {
         // edit by email

        char email[100];

        printf("Enter Email ID: ");
        scanf("%99s", email);

        for(int i = 0; i < addressBook->contactCount; i++)
        {
            if(strcmp(addressBook->contacts[i].email, email) == 0)
            {
                found = i;
                break;
            }
        }
    }

    else if(searchChoice == 4)

    {
        return;
    }

    else
    {
        printf("Invalid Choice\n");
        return;
    }

    if(found == -1)
    {
        printf("No matching record found\n");
        return;
    }

    printf("\nContact Found\n");
    printf("Name  : %s\n", addressBook->contacts[found].name);
    printf("Phone : %s\n", addressBook->contacts[found].phone);
    printf("Email : %s\n", addressBook->contacts[found].email);

    int editChoice;

    printf("\nWhat do you want to edit?\n");
    printf("1. Name\n");
    printf("2. Phone Number\n");
    printf("3. Email ID\n");

    printf("Enter choice: ");
    scanf("%d", &editChoice);

    if(editChoice == 1)
    {
        char newName[50];

        printf("Enter New Name: ");
        scanf(" %[^\n]", newName);

        // same validation as add contact
        if(!isValidName(newName))
        {
            return;
        }

        strcpy(addressBook->contacts[found].name, newName);

        printf("Name Updated Successfully\n");
    }
    else if(editChoice == 2)
    {
        char newPhone[11];

        printf("Enter New Phone Number: ");
        scanf("%10s", newPhone);

        // same validation as add contact
        if(!isValidPhone(newPhone))
        {
            return;
        }

        // phone must be unique
        if(isPhoneTaken(addressBook, newPhone, found))
        {
            printf("Phone Number already exists\n");
            return;
        }

        strcpy(addressBook->contacts[found].phone, newPhone);

        printf("Phone Number Updated Successfully\n");
    }

    else if(editChoice == 3)

    {
        char newEmail[100];

        printf("Enter New Email ID: ");
        scanf("%99s", newEmail);

        // same validation as add contact
        if(!isValidEmail(newEmail))
        {
            return;
        }

        // email must be unique
        if(isEmailTaken(addressBook, newEmail, found))
        {
            printf("Email ID already exists\n");
            return;
        }

        strcpy(addressBook->contacts[found].email, newEmail);

        printf("Email Updated Successfully\n");
    }

    else

    {
        printf("Invalid Choice\n");
    }
    
}


// Delete a contact from address book

void deleteContact(AddressBook *addressBook)
{
	int option;
    int index = -1;

    printf("\n========== DELETE CONTACT ==========\n");
    printf("1. Delete using Name\n");
    printf("2. Delete using Phone Number\n");
    printf("3. Delete using Email ID\n");
    printf("4. Exit\n");
    printf("Enter your choice : ");
    scanf("%d", &option);

    switch(option)
    {
        case 1:
        {
            char name[50];

            printf("Enter Name : ");
            scanf(" %[^\n]", name);

            // handles duplicate names (asks which record to delete)
            index = selectByName(addressBook, name);

            if(index == -2)
            {
                return;
            }
            break;
        }

        case 2:
        {
            char phone[11];

            printf("Enter Phone Number : ");
            scanf("%10s", phone);

            for(int i = 0; i < addressBook->contactCount; i++)
            {
                if(strcmp(addressBook->contacts[i].phone, phone) == 0)
                {
                    index = i;
                    break;
                }
            }
            break;
        }

        case 3:
        {
            char email[100];

            printf("Enter Email ID : ");
            scanf("%99s", email);

            for(int i = 0; i < addressBook->contactCount; i++)
            {
                if(strcmp(addressBook->contacts[i].email, email) == 0)
                {
                    index = i;
                    break;
                }
            }
            break;
        }

        case 4:
            return;

        default:
            printf("Invalid Option\n");
            return;
    }

    if(index == -1)
    {
        printf("No matching contact found\n");
        return;
    }

    printf("\nContact Details\n");
    printf("Name  : %s\n", addressBook->contacts[index].name);
    printf("Phone : %s\n", addressBook->contacts[index].phone);
    printf("Email : %s\n", addressBook->contacts[index].email);

    char answer;

    printf("\nConfirm Delete (Y/N) : ");
    scanf(" %c", &answer);

    if(answer == 'Y' || answer == 'y')
    {
        for(int i = index; i < addressBook->contactCount - 1; i++)
        {
            addressBook->contacts[i] =
            addressBook->contacts[i + 1];
        }

        addressBook->contactCount--;

        printf("Contact deleted successfully\n");
    }
    else
    {
        printf("Delete operation cancelled\n");
    }
   
}