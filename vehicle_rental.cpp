#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CUSTOMERS 2
#define MAX_VEHICLES 2
#define MAX_RENTALS 10

struct Customer
{
    int id;
    char name[50];
    char mobile[15];
};

struct Vehicle
{
    int id;
    char number[20];
    char name[30];
    char type[20];
    float rate;
    int available;
};

struct Rental
{
    int id;
    int customerId;
    int vehicleId;
    int days;
    int priority;
    float total;
    char status[20];
};

struct Customer customers[MAX_CUSTOMERS];
struct Vehicle vehicles[MAX_VEHICLES];
struct Rental rentals[MAX_RENTALS];

int customerCount = 0;
int vehicleCount = 0;
int rentalCount = 0;


/* FIND CUSTOMER */
int findCustomer(int id)
{
    int i;

    for(i = 0; i < customerCount; i++)
    {
        if(customers[i].id == id)
            return i;
    }

    return -1;
}


/* FIND VEHICLE */
int findVehicle(int id)
{
    int i;

    for(i = 0; i < vehicleCount; i++)
    {
        if(vehicles[i].id == id)
            return i;
    }

    return -1;
}


/* FIND RENTAL */
int findRental(int id)
{
    int i;

    for(i = 0; i < rentalCount; i++)
    {
        if(rentals[i].id == id)
            return i;
    }

    return -1;
}


/* ADD CUSTOMER */
void addCustomer()
{
    struct Customer c;

    if(customerCount >= MAX_CUSTOMERS)
    {
        printf("\nOnly 2 customers can be added.\n");
        return;
    }

    c.id = customerCount + 1;

    printf("\nEnter Customer Name: ");
    scanf(" %[^\n]", c.name);

    printf("Enter Mobile Number: ");
    scanf("%14s", c.mobile);

    customers[customerCount] = c;
    customerCount++;

    printf("\nCustomer added successfully!");
    printf("\nCustomer ID: %d\n", c.id);
}


/* ADD VEHICLE */
void addVehicle()
{
    struct Vehicle v;

    if(vehicleCount >= MAX_VEHICLES)
    {
        printf("\nOnly 2 vehicles can be added.\n");
        return;
    }

    v.id = vehicleCount + 1;

    printf("\nEnter Vehicle Number: ");
    scanf("%19s", v.number);

    printf("Enter Vehicle Name: ");
    scanf(" %[^\n]", v.name);

    printf("Enter Vehicle Type: ");
    scanf(" %[^\n]", v.type);

    printf("Enter Rental Rate per Day: ");
    scanf("%f", &v.rate);

    v.available = 1;

    vehicles[vehicleCount] = v;
    vehicleCount++;

    printf("\nVehicle added successfully!");
    printf("\nVehicle ID: %d\n", v.id);
}


/* CREATE RENTAL */
void createRental()
{
    struct Rental r;
    int ci, vi;

    if(rentalCount >= MAX_RENTALS)
    {
        printf("\nRental limit reached.\n");
        return;
    }

    if(customerCount == 0 || vehicleCount == 0)
    {
        printf("\nPlease add customers and vehicles first.\n");
        return;
    }

    printf("\nEnter Customer ID: ");
    scanf("%d", &r.customerId);

    ci = findCustomer(r.customerId);

    if(ci == -1)
    {
        printf("\nCustomer not found!\n");
        return;
    }

    printf("Enter Vehicle ID: ");
    scanf("%d", &r.vehicleId);

    vi = findVehicle(r.vehicleId);

    if(vi == -1)
    {
        printf("\nVehicle not found!\n");
        return;
    }

    if(vehicles[vi].available == 0)
    {
        printf("\nVehicle is already rented!\n");
        return;
    }

    printf("Enter Number of Days: ");
    scanf("%d", &r.days);

    printf("Enter Priority (1-5): ");
    scanf("%d", &r.priority);

    if(r.priority < 1 || r.priority > 5)
        r.priority = 3;

    r.id = rentalCount + 1;

    r.total = vehicles[vi].rate * r.days;

    strcpy(r.status, "Active");

    rentals[rentalCount] = r;
    rentalCount++;

    vehicles[vi].available = 0;

    printf("\n=================================");
    printf("\n       RENTAL CREATED");
    printf("\n=================================");
    printf("\nRental ID     : %d", r.id);
    printf("\nCustomer      : %s", customers[ci].name);
    printf("\nMobile        : %s", customers[ci].mobile);
    printf("\nVehicle       : %s", vehicles[vi].name);
    printf("\nVehicle No    : %s", vehicles[vi].number);
    printf("\nDays          : %d", r.days);
    printf("\nPriority      : %d", r.priority);
    printf("\nTotal Bill    : %.2f", r.total);
    printf("\nStatus        : %s", r.status);
    printf("\n=================================\n");
}


/* SEARCH RENTAL */
void searchRental()
{
    int id, i, ci, vi;

    printf("\nEnter Rental ID: ");
    scanf("%d", &id);

    i = findRental(id);

    if(i == -1)
    {
        printf("\nRental not found!\n");
        return;
    }

    ci = findCustomer(rentals[i].customerId);
    vi = findVehicle(rentals[i].vehicleId);

    printf("\n=================================");
    printf("\n        RENTAL DETAILS");
    printf("\n=================================");

    printf("\nRental ID : %d", rentals[i].id);

    if(ci != -1)
    {
        printf("\nCustomer  : %s", customers[ci].name);
        printf("\nMobile    : %s", customers[ci].mobile);
    }

    if(vi != -1)
    {
        printf("\nVehicle   : %s", vehicles[vi].name);
        printf("\nNumber    : %s", vehicles[vi].number);
    }

    printf("\nDays      : %d", rentals[i].days);
    printf("\nPriority  : %d", rentals[i].priority);
    printf("\nTotal     : %.2f", rentals[i].total);
    printf("\nStatus    : %s", rentals[i].status);

    printf("\n=================================\n");
}


/* SEARCH VEHICLE */
void searchVehicle()
{
    char number[20];
    int i, found = -1;

    if(vehicleCount == 0)
    {
        printf("\nNo vehicles available.\n");
        return;
    }

    printf("\nEnter Vehicle Number: ");
    scanf("%19s", number);

    /* Linear Search */
    for(i = 0; i < vehicleCount; i++)
    {
        if(strcmp(vehicles[i].number, number) == 0)
        {
            found = i;
            break;
        }
    }

    if(found != -1)
    {
        printf("\nVehicle Found!");
        printf("\nVehicle Name : %s", vehicles[found].name);
        printf("\nType         : %s", vehicles[found].type);
        printf("\nRate/Day     : %.2f", vehicles[found].rate);
        printf("\nStatus       : %s\n",
               vehicles[found].available ? "Available" : "Rented");
    }
    else
    {
        printf("\nVehicle not found!\n");
    }
}


/* UPDATE RENTAL STATUS */
void updateRentalStatus()
{
    int id, choice, i, vi;

    printf("\nEnter Rental ID: ");
    scanf("%d", &id);

    i = findRental(id);

    if(i == -1)
    {
        printf("\nRental not found!\n");
        return;
    }

    printf("\n1. Active");
    printf("\n2. Completed");
    printf("\n3. Cancelled");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    vi = findVehicle(rentals[i].vehicleId);

    if(choice == 1)
    {
        strcpy(rentals[i].status, "Active");

        if(vi != -1)
            vehicles[vi].available = 0;
    }
    else if(choice == 2)
    {
        strcpy(rentals[i].status, "Completed");

        if(vi != -1)
            vehicles[vi].available = 1;
    }
    else if(choice == 3)
    {
        strcpy(rentals[i].status, "Cancelled");

        if(vi != -1)
            vehicles[vi].available = 1;
    }
    else
    {
        printf("\nInvalid choice!\n");
        return;
    }

    printf("\nRental status updated successfully!\n");
}


/* CANCEL RENTAL */
void cancelRental()
{
    int id, i, vi;

    printf("\nEnter Rental ID: ");
    scanf("%d", &id);

    i = findRental(id);

    if(i == -1)
    {
        printf("\nRental not found!\n");
        return;
    }

    strcpy(rentals[i].status, "Cancelled");

    vi = findVehicle(rentals[i].vehicleId);

    if(vi != -1)
        vehicles[vi].available = 1;

    printf("\nRental cancelled successfully!\n");
}


/* SORT RENTALS BY PRIORITY */
void sortRentals()
{
    int i, j;
    struct Rental temp;

    for(i = 0; i < rentalCount - 1; i++)
    {
        for(j = i + 1; j < rentalCount; j++)
        {
            if(rentals[i].priority > rentals[j].priority)
            {
                temp = rentals[i];
                rentals[i] = rentals[j];
                rentals[j] = temp;
            }
        }
    }
}


/* DISPLAY RENTALS BY PRIORITY */
void displayPriority()
{
    int i;

    if(rentalCount == 0)
    {
        printf("\nNo rental records.\n");
        return;
    }

    sortRentals();

    printf("\n=================================");
    printf("\n    RENTALS BY PRIORITY");
    printf("\n=================================");

    for(i = 0; i < rentalCount; i++)
    {
        printf("\n\nRental ID : %d", rentals[i].id);
        printf("\nPriority  : %d", rentals[i].priority);
        printf("\nStatus    : %s", rentals[i].status);
        printf("\nTotal     : %.2f", rentals[i].total);
    }

    printf("\n=================================\n");
}


/* DISPLAY BILL */
void displayBill()
{
    int id, i, ci, vi;

    printf("\nEnter Rental ID: ");
    scanf("%d", &id);

    i = findRental(id);

    if(i == -1)
    {
        printf("\nRental not found!\n");
        return;
    }

    ci = findCustomer(rentals[i].customerId);
    vi = findVehicle(rentals[i].vehicleId);

    printf("\n=================================");
    printf("\n       VEHICLE RENTAL BILL");
    printf("\n=================================");

    printf("\nRental ID : %d", rentals[i].id);

    if(ci != -1)
    {
        printf("\nCustomer  : %s", customers[ci].name);
        printf("\nMobile    : %s", customers[ci].mobile);
    }

    if(vi != -1)
    {
        printf("\nVehicle   : %s", vehicles[vi].name);
        printf("\nNumber    : %s", vehicles[vi].number);
        printf("\nRate/Day  : %.2f", vehicles[vi].rate);
    }

    printf("\nDays      : %d", rentals[i].days);
    printf("\n---------------------------------");
    printf("\nTOTAL     : %.2f", rentals[i].total);
    printf("\nStatus    : %s", rentals[i].status);

    printf("\n=================================\n");
}


/* DASHBOARD */
void dashboard()
{
    int i;
    int available = 0;
    int rented = 0;
    int active = 0;
    int completed = 0;
    int cancelled = 0;

    for(i = 0; i < vehicleCount; i++)
    {
        if(vehicles[i].available)
            available++;
        else
            rented++;
    }

    for(i = 0; i < rentalCount; i++)
    {
        if(strcmp(rentals[i].status, "Active") == 0)
            active++;

        else if(strcmp(rentals[i].status, "Completed") == 0)
            completed++;

        else if(strcmp(rentals[i].status, "Cancelled") == 0)
            cancelled++;
    }

    printf("\n=================================");
    printf("\n       FLEET DASHBOARD");
    printf("\n=================================");

    printf("\nTotal Customers : %d", customerCount);
    printf("\nTotal Vehicles  : %d", vehicleCount);
    printf("\nAvailable       : %d", available);
    printf("\nRented          : %d", rented);

    printf("\n\nTotal Rentals   : %d", rentalCount);
    printf("\nActive          : %d", active);
    printf("\nCompleted       : %d", completed);
    printf("\nCancelled       : %d", cancelled);

    printf("\n=================================\n");
}


/* DISPLAY ALL RECORDS */
void displayAll()
{
    int i;

    printf("\n\n========== CUSTOMERS ==========\n");

    for(i = 0; i < customerCount; i++)
    {
        printf("\nID     : %d", customers[i].id);
        printf("\nName   : %s", customers[i].name);
        printf("\nMobile : %s\n", customers[i].mobile);
    }

    printf("\n========== VEHICLES ==========\n");

    for(i = 0; i < vehicleCount; i++)
    {
        printf("\nID     : %d", vehicles[i].id);
        printf("\nNumber : %s", vehicles[i].number);
        printf("\nName   : %s", vehicles[i].name);
        printf("\nType   : %s", vehicles[i].type);
        printf("\nRate   : %.2f", vehicles[i].rate);
        printf("\nStatus : %s\n",
               vehicles[i].available ? "Available" : "Rented");
    }

    printf("\n========== RENTALS ==========\n");

    for(i = 0; i < rentalCount; i++)
    {
        printf("\nRental ID  : %d", rentals[i].id);
        printf("\nCustomer ID: %d", rentals[i].customerId);
        printf("\nVehicle ID : %d", rentals[i].vehicleId);
        printf("\nDays       : %d", rentals[i].days);
        printf("\nPriority   : %d", rentals[i].priority);
        printf("\nTotal      : %.2f", rentals[i].total);
        printf("\nStatus     : %s\n", rentals[i].status);
    }
}


/* SAVE RECORDS */
void saveRecords()
{
    FILE *fp;

    fp = fopen("vehicle_rental.dat", "wb");

    if(fp == NULL)
    {
        printf("\nUnable to create file!\n");
        return;
    }

    fwrite(&customerCount, sizeof(int), 1, fp);
    fwrite(customers, sizeof(struct Customer),
           customerCount, fp);

    fwrite(&vehicleCount, sizeof(int), 1, fp);
    fwrite(vehicles, sizeof(struct Vehicle),
           vehicleCount, fp);

    fwrite(&rentalCount, sizeof(int), 1, fp);
    fwrite(rentals, sizeof(struct Rental),
           rentalCount, fp);

    fclose(fp);

    printf("\nRecords saved successfully!");
    printf("\nFile name: vehicle_rental.dat\n");
}


/* LOAD RECORDS */
void loadRecords()
{
    FILE *fp;

    fp = fopen("vehicle_rental.dat", "rb");

    if(fp == NULL)
    {
        printf("\nNo previous data found.");
        printf("\nNew database will be created.\n");
        return;
    }

    fread(&customerCount, sizeof(int), 1, fp);
    fread(customers, sizeof(struct Customer),
          customerCount, fp);

    fread(&vehicleCount, sizeof(int), 1, fp);
    fread(vehicles, sizeof(struct Vehicle),
          vehicleCount, fp);

    fread(&rentalCount, sizeof(int), 1, fp);
    fread(rentals, sizeof(struct Rental),
          rentalCount, fp);

    fclose(fp);

    printf("\nPrevious records loaded successfully!\n");
}


/* MAIN */
int main()
{
    int choice;

    printf("\n============================================");
    printf("\n VEHICLE RENTAL AND FLEET MANAGEMENT SYSTEM");
    printf("\n============================================");

    /* Automatically load previous data */
    loadRecords();

    do
    {
        printf("\n\n---------------- MAIN MENU ----------------");
        printf("\n1. Add Customer");
        printf("\n2. Add Vehicle");
        printf("\n3. Create Rental Request");
        printf("\n4. Search Rental by ID");
        printf("\n5. Search Vehicle by Number");
        printf("\n6. Update Rental Status");
        printf("\n7. Cancel Rental");
        printf("\n8. Sort & Display Rentals by Priority");
        printf("\n9. Calculate / Display Bill");
        printf("\n10. Display Fleet Dashboard");
        printf("\n11. Display All Records");
        printf("\n12. Save Records to File");
        printf("\n13. Load Records from File");
        printf("\n0. Exit");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                addCustomer();
                break;

            case 2:
                addVehicle();
                break;

            case 3:
                createRental();
                break;

            case 4:
                searchRental();
                break;

            case 5:
                searchVehicle();
                break;

            case 6:
                updateRentalStatus();
                break;

            case 7:
                cancelRental();
                break;

            case 8:
                displayPriority();
                break;

            case 9:
                displayBill();
                break;

            case 10:
                dashboard();
                break;

            case 11:
                displayAll();
                break;

            case 12:
                saveRecords();
                break;

            case 13:
                loadRecords();
                break;

            case 0:
                saveRecords();
                printf("\nThank you for using the system!\n");
                break;

            default:
                printf("\nInvalid choice! Please enter 0-13.\n");
        }

    } while(choice != 0);

    return 0;
}