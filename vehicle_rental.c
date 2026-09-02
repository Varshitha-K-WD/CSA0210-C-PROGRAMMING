/* ============================================================================
   VEHICLE RENTAL AND FLEET MANAGEMENT SYSTEM
   Course: CSA0210 - C Programming
   Assignment: Vehicle Rental Centre
   ============================================================================
   Demonstrates: structures, nested structures, multidimensional arrays,
   functions & pointers, searching (linear & binary), sorting (insertion &
   quicksort/qsort comparison), file handling (binary persistence),
   static storage class, menu-driven design.
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CUSTOMERS   100
#define MAX_VEHICLES    100
#define MAX_RENTALS     200
#define NAME_LEN        50
#define VNO_LEN         15

#define TYPE_CAR   0
#define TYPE_BIKE  1
#define TYPE_VAN   2
#define NUM_TYPES  3

#define STATUS_AVAILABLE 0
#define STATUS_RENTED    1
#define NUM_STATUS       2

/* ---------------------------- STRUCTURES --------------------------------- */

typedef struct {
    int day, month, year;
} Date;

/* Nested structure #1: rental rate details nested inside Vehicle */
typedef struct {
    float dailyRate;
    float weeklyRate;
} RentalRate;

/* Nested structure #2: maintenance details nested inside Vehicle */
typedef struct {
    Date lastServiceDate;
    Date nextServiceDue;
    char status[20];   /* "OK", "DUE", "IN_SERVICE" */
} MaintenanceInfo;

typedef struct {
    char vehicleNo[VNO_LEN];
    char type[15];         /* Car / Bike / Van */
    char model[30];
    RentalRate rate;        /* nested */
    MaintenanceInfo maint;  /* nested */
    int isAvailable;        /* 1 = available, 0 = rented */
} Vehicle;

typedef struct {
    int customerId;
    char name[NAME_LEN];
    char phone[15];
    char licenseNo[20];
    char address[100];
} Customer;

/* Nested structure #3: payment details nested inside Rental */
typedef struct {
    float amount;
    int isPaid;
    char mode[15];  /* Cash / Card / UPI */
} Payment;

typedef struct {
    int rentalId;
    int customerId;
    char vehicleNo[VNO_LEN];
    Date startDate;
    Date endDate;
    int priority;        /* 1 = Urgent, 2 = Normal, 3 = Low */
    char status[15];     /* Active / Completed / Cancelled */
    Payment payment;     /* nested */
} Rental;

/* ---------------------------- GLOBAL DATA --------------------------------- */

Customer customers[MAX_CUSTOMERS];
Vehicle  vehicles[MAX_VEHICLES];
Rental   rentals[MAX_RENTALS];

int customerCount = 0;
int vehicleCount  = 0;
int rentalCount   = 0;

/* Multidimensional array: fleet dashboard matrix
   rows = vehicle type (Car/Bike/Van), cols = status (Available/Rented) */
int fleetMatrix[NUM_TYPES][NUM_STATUS];

const char *typeNames[NUM_TYPES]   = {"Car", "Bike", "Van"};
const char *statusNames[NUM_STATUS] = {"Available", "Rented"};

/* ---------------------------- FUNCTION PROTOTYPES ------------------------- */

int  nextRentalId(void);                       /* static counter demo */
void addCustomer(void);
void addVehicle(void);
Customer* getCustomerById(int id);
Vehicle*  getVehicleByNoLinear(const char *no, int *comparisons);
void buildVehicleIndex(int idx[]);
int  binarySearchVehicle(int idx[], int n, const char *key, int *comparisons);
void searchVehicleMenu(void);
void createRentalRequest(void);
Rental* searchRentalById(int id);
void searchRentalMenu(void);
void updateRentalStatusMenu(void);
void cancelRentalMenu(void);
long daysBetween(Date a, Date b);
float computeBill(const Rental *r, const Vehicle *v);
void billingMenu(void);
void insertionSortByPriority(Rental arr[], int n, long *comparisons);
int  cmpRentalPriority(const void *a, const void *b);
void sortDisplayMenu(void);
void sortingAlgorithmComparison(void);
void buildFleetMatrix(void);
void displayFleetMatrix(void);
void displayAllRecords(void);
void saveAllToFile(void);
void loadAllFromFile(void);
void printDate(Date d);
Date inputDate(const char *label);
void pause_enter(void);

/* ---------------------------- STATIC COUNTER ------------------------------
   'static' local variable retains its value across calls without being a
   global. If it were a normal (automatic) local variable, it would reset to
   its initial value on every function call, generating duplicate rental IDs
   (e.g., every rental would be assigned ID 1000). 'static' guarantees the
   counter persists for the lifetime of the program while still being
   encapsulated inside the function (unlike a global variable, it cannot be
   accidentally modified elsewhere in the program).
   ---------------------------------------------------------------------- */
int nextRentalId(void) {
    static int counter = 1000;   /* initialized only once, ever */
    counter++;
    return counter;
}

/* ---------------------------- UTILITIES ------------------------------ */

void printDate(Date d) { printf("%02d-%02d-%04d", d.day, d.month, d.year); }

Date inputDate(const char *label) {
    Date d;
    printf("Enter %s (dd mm yyyy): ", label);
    scanf("%d %d %d", &d.day, &d.month, &d.year);
    return d;
}

void pause_enter(void) {
    /* no-op in batch/test mode; kept for interactive UX clarity */
}

long daysBetween(Date a, Date b) {
    long da = a.day + a.month * 30L + a.year * 365L;
    long db = b.day + b.month * 30L + b.year * 365L;
    long diff = db - da;
    return diff < 0 ? -diff : diff;
}

/* ---------------------------- CUSTOMER MODULE ------------------------------ */

void addCustomer(void) {
    if (customerCount >= MAX_CUSTOMERS) { printf("Customer storage full!\n"); return; }
    Customer *c = &customers[customerCount];
    c->customerId = 1 + customerCount;
    printf("\n--- Add Customer ---\n");
    printf("Name: "); scanf(" %49[^\n]", c->name);
    printf("Phone: "); scanf(" %14s", c->phone);
    printf("License No: "); scanf(" %19s", c->licenseNo);
    printf("Address: "); scanf(" %99[^\n]", c->address);
    customerCount++;
    printf("Customer added with ID: %d\n", c->customerId);
}

Customer* getCustomerById(int id) {
    for (int i = 0; i < customerCount; i++)
        if (customers[i].customerId == id) return &customers[i];
    return NULL;
}

/* ---------------------------- VEHICLE MODULE ------------------------------ */

void addVehicle(void) {
    if (vehicleCount >= MAX_VEHICLES) { printf("Vehicle storage full!\n"); return; }
    Vehicle *v = &vehicles[vehicleCount];
    printf("\n--- Add Vehicle ---\n");
    printf("Vehicle No: "); scanf(" %14s", v->vehicleNo);
    printf("Type (Car/Bike/Van): "); scanf(" %14s", v->type);
    printf("Model: "); scanf(" %29[^\n]", v->model);
    printf("Daily Rate: "); scanf("%f", &v->rate.dailyRate);
    printf("Weekly Rate: "); scanf("%f", &v->rate.weeklyRate);
    v->maint.lastServiceDate = inputDate("Last Service Date");
    v->maint.nextServiceDue  = inputDate("Next Service Due Date");
    strcpy(v->maint.status, "OK");
    v->isAvailable = 1;
    vehicleCount++;
    printf("Vehicle %s added successfully.\n", v->vehicleNo);
}

/* Linear search - O(n) - baseline algorithm */
Vehicle* getVehicleByNoLinear(const char *no, int *comparisons) {
    int c = 0;
    for (int i = 0; i < vehicleCount; i++) {
        c++;
        if (strcmp(vehicles[i].vehicleNo, no) == 0) {
            if (comparisons) *comparisons = c;
            return &vehicles[i];
        }
    }
    if (comparisons) *comparisons = c;
    return NULL;
}

/* Build an index array of vehicle positions sorted by vehicleNo (insertion sort) */
void buildVehicleIndex(int idx[]) {
    for (int i = 0; i < vehicleCount; i++) idx[i] = i;
    for (int i = 1; i < vehicleCount; i++) {
        int key = idx[i];
        int j = i - 1;
        while (j >= 0 && strcmp(vehicles[idx[j]].vehicleNo, vehicles[key].vehicleNo) > 0) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
}

/* Binary search over the sorted index - O(log n) */
int binarySearchVehicle(int idx[], int n, const char *key, int *comparisons) {
    int lo = 0, hi = n - 1, c = 0;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        c++;
        int cmp = strcmp(vehicles[idx[mid]].vehicleNo, key);
        if (cmp == 0) { if (comparisons) *comparisons = c; return idx[mid]; }
        else if (cmp < 0) lo = mid + 1;
        else hi = mid - 1;
    }
    if (comparisons) *comparisons = c;
    return -1;
}

void searchVehicleMenu(void) {
    char no[VNO_LEN];
    printf("\n--- Search Vehicle by Number ---\n");
    printf("Vehicle No: "); scanf(" %14s", no);

    int cmpLin = 0;
    Vehicle *vLin = getVehicleByNoLinear(no, &cmpLin);

    int idx[MAX_VEHICLES];
    buildVehicleIndex(idx);
    int cmpBin = 0;
    int pos = binarySearchVehicle(idx, vehicleCount, no, &cmpBin);

    printf("\n[Linear Search]  found=%s  comparisons=%d\n", vLin ? "YES" : "NO", cmpLin);
    printf("[Binary Search]  found=%s  comparisons=%d\n", pos >= 0 ? "YES" : "NO", cmpBin);

    if (vLin) {
        printf("\nVehicle: %s | Type: %s | Model: %s | Daily Rate: %.2f | %s\n",
               vLin->vehicleNo, vLin->type, vLin->model, vLin->rate.dailyRate,
               vLin->isAvailable ? "AVAILABLE" : "RENTED");
    } else {
        printf("Vehicle not found.\n");
    }
}

/* ---------------------------- RENTAL MODULE ------------------------------ */

void createRentalRequest(void) {
    if (rentalCount >= MAX_RENTALS) { printf("Rental storage full!\n"); return; }
    int cid; char vno[VNO_LEN];
    printf("\n--- Create Rental Request ---\n");
    printf("Customer ID: "); scanf("%d", &cid);
    if (!getCustomerById(cid)) { printf("Invalid customer ID.\n"); return; }

    printf("Vehicle No: "); scanf(" %14s", vno);
    Vehicle *v = getVehicleByNoLinear(vno, NULL);
    if (!v) { printf("Vehicle not found.\n"); return; }
    if (!v->isAvailable) { printf("Vehicle is currently rented out.\n"); return; }

    Rental *r = &rentals[rentalCount];
    r->rentalId = nextRentalId();
    r->customerId = cid;
    strcpy(r->vehicleNo, vno);
    r->startDate = inputDate("Start Date");
    r->endDate   = inputDate("End Date");
    printf("Priority (1=Urgent 2=Normal 3=Low): "); scanf("%d", &r->priority);
    if (r->priority < 1 || r->priority > 3) r->priority = 2;
    strcpy(r->status, "Active");
    r->payment.amount = computeBill(r, v);
    r->payment.isPaid = 0;
    strcpy(r->payment.mode, "Pending");

    v->isAvailable = 0;
    rentalCount++;
    printf("Rental created. Rental ID: %d | Estimated Charge: %.2f\n", r->rentalId, r->payment.amount);
}

Rental* searchRentalById(int id) {
    for (int i = 0; i < rentalCount; i++)
        if (rentals[i].rentalId == id) return &rentals[i];
    return NULL;
}

void searchRentalMenu(void) {
    int id;
    printf("\n--- Search Rental by ID ---\n");
    printf("Rental ID: "); scanf("%d", &id);
    Rental *r = searchRentalById(id);
    if (!r) { printf("Rental not found.\n"); return; }
    Customer *c = getCustomerById(r->customerId);
    printf("Rental ID: %d | Customer: %s | Vehicle: %s | Priority: %d | Status: %s\n",
           r->rentalId, c ? c->name : "?", r->vehicleNo, r->priority, r->status);
    printf("Start: "); printDate(r->startDate);
    printf("  End: "); printDate(r->endDate);
    printf("\nAmount: %.2f | Paid: %s\n", r->payment.amount, r->payment.isPaid ? "Yes" : "No");
}

void updateRentalStatusMenu(void) {
    int id; char st[15];
    printf("\n--- Update Rental Status ---\n");
    printf("Rental ID: "); scanf("%d", &id);
    Rental *r = searchRentalById(id);
    if (!r) { printf("Rental not found.\n"); return; }
    printf("New Status (Active/Completed/Cancelled): "); scanf(" %14s", st);
    strcpy(r->status, st);
    if (strcmp(st, "Completed") == 0 || strcmp(st, "Cancelled") == 0) {
        Vehicle *v = getVehicleByNoLinear(r->vehicleNo, NULL);
        if (v) v->isAvailable = 1;
    }
    printf("Rental %d updated to %s.\n", id, st);
}

void cancelRentalMenu(void) {
    int id;
    printf("\n--- Cancel Rental ---\n");
    printf("Rental ID: "); scanf("%d", &id);
    Rental *r = searchRentalById(id);
    if (!r) { printf("Rental not found.\n"); return; }
    if (strcmp(r->status, "Cancelled") == 0) { printf("Already cancelled.\n"); return; }
    strcpy(r->status, "Cancelled");
    Vehicle *v = getVehicleByNoLinear(r->vehicleNo, NULL);
    if (v) v->isAvailable = 1;
    printf("Rental %d cancelled. Vehicle %s released.\n", id, r->vehicleNo);
}

/* ---------------------------- BILLING MODULE ------------------------------ */

float computeBill(const Rental *r, const Vehicle *v) {
    long days = daysBetween(r->startDate, r->endDate);
    if (days <= 0) days = 1;
    long weeks = days / 7;
    long remDays = days % 7;
    float amount = weeks * v->rate.weeklyRate + remDays * v->rate.dailyRate;
    return amount;
}

void billingMenu(void) {
    int id;
    printf("\n--- Calculate / Display Bill ---\n");
    printf("Rental ID: "); scanf("%d", &id);
    Rental *r = searchRentalById(id);
    if (!r) { printf("Rental not found.\n"); return; }
    Vehicle *v = getVehicleByNoLinear(r->vehicleNo, NULL);
    if (!v) { printf("Vehicle record missing.\n"); return; }
    r->payment.amount = computeBill(r, v);
    printf("Rental %d | Vehicle %s | Days billed at daily/weekly rate\n", r->rentalId, r->vehicleNo);
    printf("Total Amount: %.2f\n", r->payment.amount);
    printf("Mark as paid? (Cash/Card/UPI/None): "); scanf(" %14s", r->payment.mode);
    r->payment.isPaid = strcmp(r->payment.mode, "None") != 0;
}

/* ---------------------------- SORTING MODULE ------------------------------ */

/* Priority-based Insertion Sort: stable, sorts by priority (1 highest) then
   by start date (earlier first) as a tie-breaker. O(n^2) worst case, O(n)
   best case (nearly-sorted rental queues, common in real operations). */
void insertionSortByPriority(Rental arr[], int n, long *comparisons) {
    long cmp = 0;
    for (int i = 1; i < n; i++) {
        Rental key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            cmp++;
            long keyDays = key.startDate.year * 372L + key.startDate.month * 31L + key.startDate.day;
            long jDays = arr[j].startDate.year * 372L + arr[j].startDate.month * 31L + arr[j].startDate.day;
            int shouldShift = (arr[j].priority > key.priority) ||
                               (arr[j].priority == key.priority && jDays > keyDays);
            if (!shouldShift) break;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
    if (comparisons) *comparisons = cmp;
}

/* Comparator for qsort (Quicksort library implementation) - alternative algorithm */
int cmpRentalPriority(const void *a, const void *b) {
    const Rental *ra = (const Rental *)a;
    const Rental *rb = (const Rental *)b;
    if (ra->priority != rb->priority) return ra->priority - rb->priority;
    long da = ra->startDate.year * 372L + ra->startDate.month * 31L + ra->startDate.day;
    long db = rb->startDate.year * 372L + rb->startDate.month * 31L + rb->startDate.day;
    return (int)(da - db);
}

void sortDisplayMenu(void) {
    if (rentalCount == 0) { printf("No rentals to sort.\n"); return; }
    Rental temp[MAX_RENTALS];
    memcpy(temp, rentals, sizeof(Rental) * rentalCount);

    int choice;
    printf("\nChoose sorting algorithm:\n1. Insertion Sort (manual, priority queue style)\n2. Quick Sort (qsort library)\nChoice: ");
    scanf("%d", &choice);

    clock_t t0 = clock();
    if (choice == 2) {
        qsort(temp, rentalCount, sizeof(Rental), cmpRentalPriority);
    } else {
        long cmp;
        insertionSortByPriority(temp, rentalCount, &cmp);
    }
    clock_t t1 = clock();

    printf("\n%-4s %-6s %-10s %-10s %-8s %-10s %-10s\n",
           "ID", "CustID", "Vehicle", "Priority", "Status", "Start", "End");
    for (int i = 0; i < rentalCount; i++) {
        Rental *r = &temp[i];
        printf("%-4d %-6d %-10s %-10d %-8s ", r->rentalId, r->customerId, r->vehicleNo, r->priority, r->status);
        printDate(r->startDate); printf("  "); printDate(r->endDate); printf("\n");
    }
    printf("\nSort executed in %.6f seconds (algorithm: %s)\n",
           (double)(t1 - t0) / CLOCKS_PER_SEC, choice == 2 ? "QuickSort/qsort" : "Insertion Sort");
}

/* Generates N synthetic rentals in-memory (not saved) purely to benchmark
   both sorting algorithms at scale, satisfying the evaluation requirement
   to compare execution time / scalability. */
void sortingAlgorithmComparison(void) {
    int n;
    printf("\n--- Sorting Algorithm Benchmark ---\n");
    printf("Enter number of synthetic rental records to generate (e.g. 5000): ");
    scanf("%d", &n);
    if (n <= 0 || n > 200000) { printf("Invalid size.\n"); return; }

    Rental *a = malloc(sizeof(Rental) * n);
    Rental *b = malloc(sizeof(Rental) * n);
    if (!a || !b) { printf("Memory allocation failed.\n"); return; }

    srand(42); /* fixed seed for reproducible benchmark */
    for (int i = 0; i < n; i++) {
        a[i].rentalId = 1000 + i;
        a[i].priority = 1 + (rand() % 3);
        a[i].startDate.day = 1 + rand() % 28;
        a[i].startDate.month = 1 + rand() % 12;
        a[i].startDate.year = 2024 + rand() % 3;
        strcpy(a[i].status, "Active");
    }
    memcpy(b, a, sizeof(Rental) * n);

    clock_t t0 = clock();
    long comparisons;
    insertionSortByPriority(a, n, &comparisons);
    clock_t t1 = clock();
    double insertionTime = (double)(t1 - t0) / CLOCKS_PER_SEC;

    clock_t t2 = clock();
    qsort(b, n, sizeof(Rental), cmpRentalPriority);
    clock_t t3 = clock();
    double qsortTime = (double)(t3 - t2) / CLOCKS_PER_SEC;

    printf("\n===== Sorting Algorithm Comparison (n = %d) =====\n", n);
    printf("Insertion Sort : time = %.6f sec | comparisons = %ld | complexity ~ O(n^2)\n",
           insertionTime, comparisons);
    printf("Quick Sort     : time = %.6f sec | complexity ~ O(n log n) average\n", qsortTime);
    printf("==================================================\n");

    free(a); free(b);
}

/* ---------------------------- FLEET DASHBOARD (2D ARRAY) ------------------ */

void buildFleetMatrix(void) {
    for (int t = 0; t < NUM_TYPES; t++)
        for (int s = 0; s < NUM_STATUS; s++)
            fleetMatrix[t][s] = 0;

    for (int i = 0; i < vehicleCount; i++) {
        int t = -1;
        if (strcasecmp(vehicles[i].type, "Car") == 0) t = TYPE_CAR;
        else if (strcasecmp(vehicles[i].type, "Bike") == 0) t = TYPE_BIKE;
        else if (strcasecmp(vehicles[i].type, "Van") == 0) t = TYPE_VAN;
        if (t == -1) continue;
        int s = vehicles[i].isAvailable ? STATUS_AVAILABLE : STATUS_RENTED;
        fleetMatrix[t][s]++;
    }
}

void displayFleetMatrix(void) {
    buildFleetMatrix();
    printf("\n--- Fleet Dashboard Matrix (2D array: type x status) ---\n");
    printf("%-8s %-12s %-10s\n", "Type", "Available", "Rented");
    for (int t = 0; t < NUM_TYPES; t++) {
        printf("%-8s %-12d %-10d\n", typeNames[t], fleetMatrix[t][STATUS_AVAILABLE], fleetMatrix[t][STATUS_RENTED]);
    }
}

/* ---------------------------- DISPLAY ALL ------------------------------ */

void displayAllRecords(void) {
    printf("\n===== CUSTOMERS (%d) =====\n", customerCount);
    for (int i = 0; i < customerCount; i++)
        printf("ID:%d Name:%s Phone:%s License:%s\n",
               customers[i].customerId, customers[i].name, customers[i].phone, customers[i].licenseNo);

    printf("\n===== VEHICLES (%d) =====\n", vehicleCount);
    for (int i = 0; i < vehicleCount; i++)
        printf("No:%s Type:%s Model:%s DailyRate:%.2f Status:%s MaintDue:%02d-%02d-%04d\n",
               vehicles[i].vehicleNo, vehicles[i].type, vehicles[i].model, vehicles[i].rate.dailyRate,
               vehicles[i].isAvailable ? "Available" : "Rented",
               vehicles[i].maint.nextServiceDue.day, vehicles[i].maint.nextServiceDue.month, vehicles[i].maint.nextServiceDue.year);

    printf("\n===== RENTALS (%d) =====\n", rentalCount);
    for (int i = 0; i < rentalCount; i++) {
        printf("ID:%d Cust:%d Vehicle:%s Priority:%d Status:%s Amount:%.2f Paid:%s\n",
               rentals[i].rentalId, rentals[i].customerId, rentals[i].vehicleNo, rentals[i].priority,
               rentals[i].status, rentals[i].payment.amount, rentals[i].payment.isPaid ? "Yes" : "No");
    }
}

/* ---------------------------- FILE HANDLING ------------------------------ */

void saveAllToFile(void) {
    FILE *fc = fopen("customers.dat", "wb");
    FILE *fv = fopen("vehicles.dat", "wb");
    FILE *fr = fopen("rentals.dat", "wb");
    if (!fc || !fv || !fr) { printf("Error opening files for writing.\n"); return; }

    fwrite(&customerCount, sizeof(int), 1, fc);
    fwrite(customers, sizeof(Customer), customerCount, fc);

    fwrite(&vehicleCount, sizeof(int), 1, fv);
    fwrite(vehicles, sizeof(Vehicle), vehicleCount, fv);

    fwrite(&rentalCount, sizeof(int), 1, fr);
    fwrite(rentals, sizeof(Rental), rentalCount, fr);

    fclose(fc); fclose(fv); fclose(fr);
    printf("All records saved to customers.dat, vehicles.dat, rentals.dat\n");
}

void loadAllFromFile(void) {
    FILE *fc = fopen("customers.dat", "rb");
    FILE *fv = fopen("vehicles.dat", "rb");
    FILE *fr = fopen("rentals.dat", "rb");
    if (!fc || !fv || !fr) {
        printf("No saved records found on disk.\n");
        if (fc) fclose(fc);
        if (fv) fclose(fv);
        if (fr) fclose(fr);
        return;
    }
    fread(&customerCount, sizeof(int), 1, fc);
    fread(customers, sizeof(Customer), customerCount, fc);

    fread(&vehicleCount, sizeof(int), 1, fv);
    fread(vehicles, sizeof(Vehicle), vehicleCount, fv);

    fread(&rentalCount, sizeof(int), 1, fr);
    fread(rentals, sizeof(Rental), rentalCount, fr);

    fclose(fc); fclose(fv); fclose(fr);
    printf("Loaded %d customers, %d vehicles, %d rentals from disk.\n",
           customerCount, vehicleCount, rentalCount);
}

/* ---------------------------- MAIN MENU ------------------------------ */

int main(void) {
    int choice;
    printf("============================================\n");
    printf(" VEHICLE RENTAL AND FLEET MANAGEMENT SYSTEM\n");
    printf("============================================\n");

    do {
        printf("\n---------------- MAIN MENU ----------------\n");
        printf(" 1. Add Customer\n");
        printf(" 2. Add Vehicle\n");
        printf(" 3. Create Rental Request\n");
        printf(" 4. Search Rental by ID\n");
        printf(" 5. Search Vehicle by Number (Linear vs Binary)\n");
        printf(" 6. Update Rental Status\n");
        printf(" 7. Cancel Rental\n");
        printf(" 8. Sort & Display Rentals by Priority\n");
        printf(" 9. Calculate / Display Bill\n");
        printf("10. Display Fleet Dashboard Matrix\n");
        printf("11. Display All Records\n");
        printf("12. Save Records to File\n");
        printf("13. Load Records from File\n");
        printf("14. Run Sorting Algorithm Benchmark (Evaluation)\n");
        printf(" 0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: addCustomer(); break;
            case 2: addVehicle(); break;
            case 3: createRentalRequest(); break;
            case 4: searchRentalMenu(); break;
            case 5: searchVehicleMenu(); break;
            case 6: updateRentalStatusMenu(); break;
            case 7: cancelRentalMenu(); break;
            case 8: sortDisplayMenu(); break;
            case 9: billingMenu(); break;
            case 10: displayFleetMatrix(); break;
            case 11: displayAllRecords(); break;
            case 12: saveAllToFile(); break;
            case 13: loadAllFromFile(); break;
            case 14: sortingAlgorithmComparison(); break;
            case 0: printf("Exiting. Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
