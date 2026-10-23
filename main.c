#include <stdio.h>
#include <string.h>

#define TOTAL_SEATS 180

/* ==========================================
   PASSENGER MANAGEMENT
   ========================================== */

typedef struct {
    int passengerID;
    char passengerName[50];
    char passengerPhone[20];
    char passportNumber[20];
    int isStaff;
} Passenger;

Passenger passengers[50];
int passengerCount = 0;
int nextPassengerID = 1001;

void clearInputLine(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int savePassengers(void)
{
    FILE *fp = fopen("passengers.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error opening passenger file!\n");
        return 0;
    }

    for (i = 0; i < passengerCount; i++) {
        fprintf(fp, "%d|%s|%s|%s|%d\n",
                passengers[i].passengerID,
                passengers[i].passengerName,
                passengers[i].passengerPhone,
                passengers[i].passportNumber,
                passengers[i].isStaff);
    }

    fclose(fp);
    return 1;
}

void loadPassengers(void)
{
    FILE *fp = fopen("passengers.txt", "r");
    char line[160];
    Passenger p;
    int maxID = 1000;

    if (fp == NULL)
        return;

    passengerCount = 0;

    while (passengerCount < 50 &&
           fgets(line, sizeof(line), fp) != NULL) {

        if (sscanf(line, "%d|%49[^|]|%19[^|]|%19[^|]|%d",
                   &p.passengerID,
                   p.passengerName,
                   p.passengerPhone,
                   p.passportNumber,
                   &p.isStaff) == 5) {

            passengers[passengerCount++] = p;

            if (p.passengerID > maxID)
                maxID = p.passengerID;
        }
    }

    nextPassengerID = maxID + 1;
    fclose(fp);
}

void displayPassenger(Passenger p)
{
    printf("\nPassenger ID: %d\n", p.passengerID);
    printf("Name: %s\n", p.passengerName);
    printf("Phone: %s\n", p.passengerPhone);
    printf("Passport Number: %s\n", p.passportNumber);
    printf("Staff Status: %s\n",
           p.isStaff ? "Staff Member" : "Ordinary Passenger");
}

void addPassenger(void)
{
    Passenger p;

    if (passengerCount >= 50) {
        printf("Passenger list is full!\n");
        return;
    }

    p.passengerID = nextPassengerID;

    printf("Enter passenger name: ");
    if (fgets(p.passengerName, sizeof(p.passengerName), stdin) == NULL)
        return;

    p.passengerName[strcspn(p.passengerName, "\n")] = '\0';

    printf("Enter phone number: ");
    scanf("%19s", p.passengerPhone);

    printf("Enter passport number: ");
    scanf("%19s", p.passportNumber);

    printf("Is passenger staff? (1 = Yes, 0 = No): ");
    if (scanf("%d", &p.isStaff) != 1) {
        clearInputLine();
        printf("Invalid input!\n");
        return;
    }

    clearInputLine();

    if (p.isStaff != 0 && p.isStaff != 1) {
        printf("Enter only 1 or 0!\n");
        return;
    }

    passengers[passengerCount++] = p;

    if (savePassengers()) {
        nextPassengerID++;
        printf("Passenger added successfully! ID: %d\n", p.passengerID);
    } else {
        passengerCount--;
    }
}

void viewPassengers(void)
{
    int i;

    if (passengerCount == 0) {
        printf("No passengers found!\n");
        return;
    }

    for (i = 0; i < passengerCount; i++)
        displayPassenger(passengers[i]);
}

void findPassenger(void)
{
    int id, i;

    printf("Enter passenger ID: ");
    if (scanf("%d", &id) != 1) {
        clearInputLine();
        printf("Invalid ID!\n");
        return;
    }
    clearInputLine();

    for (i = 0; i < passengerCount; i++) {
        if (passengers[i].passengerID == id) {
            displayPassenger(passengers[i]);
            return;
        }
    }

    printf("Passenger not found!\n");
}

void updatePassenger(void)
{
    int id, i;
    Passenger old;

    printf("Enter passenger ID to update: ");
    if (scanf("%d", &id) != 1) {
        clearInputLine();
        printf("Invalid ID!\n");
        return;
    }
    clearInputLine();

    for (i = 0; i < passengerCount; i++) {
        if (passengers[i].passengerID == id) {
            old = passengers[i];

            printf("Enter new name: ");
            if (fgets(passengers[i].passengerName,
                      sizeof(passengers[i].passengerName), stdin) == NULL)
                return;

            passengers[i].passengerName[
                strcspn(passengers[i].passengerName, "\n")] = '\0';

            printf("Enter new phone: ");
            scanf("%19s", passengers[i].passengerPhone);

            printf("Enter new passport number: ");
            scanf("%19s", passengers[i].passportNumber);

            printf("Is passenger staff? (1 = Yes, 0 = No): ");
            if (scanf("%d", &passengers[i].isStaff) != 1) {
                clearInputLine();
                passengers[i] = old;
                printf("Invalid input!\n");
                return;
            }
            clearInputLine();

            if ((passengers[i].isStaff != 0 &&
                 passengers[i].isStaff != 1) || 
                passengers[i].passengerName[0] == '\0') {
                passengers[i] = old;
                printf("Invalid details!\n");
                return;
            }

            if (savePassengers())
                printf("Passenger updated successfully!\n");
            else
                passengers[i] = old;

            return;
        }
    }

    printf("Passenger not found!\n");
}

void deletePassenger(void)
{
    int id, i, j;
    Passenger deleted;

    printf("Enter passenger ID to delete: ");
    if (scanf("%d", &id) != 1) {
        clearInputLine();
        printf("Invalid ID!\n");
        return;
    }
    clearInputLine();

    for (i = 0; i < passengerCount; i++) {
        if (passengers[i].passengerID == id) {
            deleted = passengers[i];

            for (j = i; j < passengerCount - 1; j++)
                passengers[j] = passengers[j + 1];

            passengerCount--;

            if (savePassengers()) {
                printf("Passenger deleted successfully!\n");
            } else {
                for (j = passengerCount; j > i; j--)
                    passengers[j] = passengers[j - 1];

                passengers[i] = deleted;
                passengerCount++;
            }
            return;
        }
    }

    printf("Passenger not found!\n");
}

/* ==========================================
   SEATS MODULE
   ========================================== */

typedef struct {
    int seatNumber;
    int seatClass;
    int isBooked;
} Seat;

Seat seats[TOTAL_SEATS];

void initializeSeats(Seat seats[])
{
    int i;

    for (i = 0; i < TOTAL_SEATS; i++) {
        seats[i].seatNumber = i + 1;
        seats[i].isBooked = 0;

        if (i < 126)
            seats[i].seatClass = 1;
        else if (i < 150)
            seats[i].seatClass = 2;
        else if (i < 174)
            seats[i].seatClass = 3;
        else
            seats[i].seatClass = 4;
    }
}

int saveSeats(Seat seats[])
{
    FILE *fp = fopen("seats.dat", "wb");

    if (fp == NULL) {
        printf("Error opening seat file!\n");
        return 0;
    }

    if (fwrite(seats, sizeof(Seat), TOTAL_SEATS, fp) != TOTAL_SEATS) {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int loadSeats(Seat seats[])
{
    FILE *fp = fopen("seats.dat", "rb");
    size_t count;

    if (fp == NULL)
        return 0;

    count = fread(seats, sizeof(Seat), TOTAL_SEATS, fp);
    fclose(fp);

    return count == TOTAL_SEATS;
}

void displaySeats(Seat seats[])
{
    int i;

    printf("\nSeat No.    Class                 Status\n");

    for (i = 0; i < TOTAL_SEATS; i++) {
        printf("%-12d", seats[i].seatNumber);

        switch (seats[i].seatClass) {
            case 1: printf("%-22s", "Economy"); break;
            case 2: printf("%-22s", "Premium Economy"); break;
            case 3: printf("%-22s", "Business"); break;
            case 4: printf("%-22s", "First Class"); break;
        }

        printf("%s\n",
               seats[i].isBooked ? "Booked" : "Available");
    }
}

int countAvailableSeats(Seat seats[])
{
    int i, count = 0;

    for (i = 0; i < TOTAL_SEATS; i++)
        if (!seats[i].isBooked)
            count++;

    return count;
}

/* ==========================================
   WEEK 2 - SEAT GRID MODULE  (Bhoomika)
   2D seat grid: 30 rows x 6 columns (A-F) = 180 seats
   Row 1-21  Economy
   Row 22-25 Premium Economy
   Row 26-29 Business
   Row 30    First Class
   The grid is rebuilt from seats[] every time, so the
   existing seats.dat file keeps working unchanged.
   ========================================== */

#define SEAT_ROWS 30
#define SEAT_COLS 6

int seatGrid[SEAT_ROWS][SEAT_COLS];   /* 0 = free, 1 = booked */

const char *seatClassName(int seatClass)
{
    switch (seatClass) {
        case 1: return "Economy";
        case 2: return "Premium Economy";
        case 3: return "Business";
        case 4: return "First Class";
        default: return "Unknown";
    }
}

void buildSeatGrid(void)
{
    int r, c;

    for (r = 0; r < SEAT_ROWS; r++)
        for (c = 0; c < SEAT_COLS; c++)
            seatGrid[r][c] = seats[r * SEAT_COLS + c].isBooked;
}

void displaySeatGrid(void)
{
    int r, c, cls, prevClass = 0;

    buildSeatGrid();

    printf("\n         A   B   C       D   E   F\n");

    for (r = 0; r < SEAT_ROWS; r++) {
        cls = seats[r * SEAT_COLS].seatClass;

        if (cls != prevClass) {
            printf("   ---- %s ----\n", seatClassName(cls));
            prevClass = cls;
        }

        printf("  %2d  ", r + 1);

        for (c = 0; c < SEAT_COLS; c++) {
            printf("%s ", seatGrid[r][c] ? "[X]" : "[ ]");

            if (c == 2)
                printf("    ");   /* aisle */
        }

        printf("\n");
    }

    printf("\n[ ] = Available    [X] = Booked\n");
}

/* Lets the user type a seat like 12C. Returns the seat number
   (1-180) of a free seat, or 0 if the user cancels. */
int pickSeat(void)
{
    char input[16];
    char col, extra;
    int row, r, c;

    while (1) {
        printf("Enter seat (e.g. 12C, or 0 to cancel): ");

        if (scanf("%15s", input) != 1) {
            clearInputLine();
            return 0;
        }
        clearInputLine();

        if (strcmp(input, "0") == 0)
            return 0;

        if (sscanf(input, "%d%c%c", &row, &col, &extra) != 2) {
            printf("Invalid format! Use row number then letter, e.g. 12C.\n");
            continue;
        }

        if (col >= 'a' && col <= 'f')
            col = col - 'a' + 'A';

        if (row < 1 || row > SEAT_ROWS ||
            col < 'A' || col >= 'A' + SEAT_COLS) {
            printf("Invalid seat! Rows are 1-%d, columns A-F.\n", SEAT_ROWS);
            continue;
        }

        r = row - 1;
        c = col - 'A';

        buildSeatGrid();

        if (seatGrid[r][c]) {
            printf("Seat %d%c is already booked! Pick another.\n", row, col);
            continue;
        }

        return r * SEAT_COLS + c + 1;
    }
}

/* Marks a seat as booked and saves; undoes the change if saving fails. */
int bookSeatNumber(int seatNumber)
{
    seats[seatNumber - 1].isBooked = 1;

    if (!saveSeats(seats)) {
        seats[seatNumber - 1].isBooked = 0;
        return 0;
    }

    return 1;
}

void seatGridMenu(void)
{
    int seatNumber;

    displaySeatGrid();

    seatNumber = pickSeat();

    if (seatNumber == 0) {
        printf("Seat selection cancelled.\n");
        return;
    }

    if (bookSeatNumber(seatNumber))
        printf("Seat %d%c booked successfully!\n",
               (seatNumber - 1) / SEAT_COLS + 1,
               'A' + (seatNumber - 1) % SEAT_COLS);
    else
        printf("Could not save seat booking!\n");
}

/* ==========================================
   WEEK 2 - FARE TABLE + BOOKING SUMMARY  (Sakshi)
   fareTable[class][passenger type]
   rows    : 0 Economy, 1 Premium Economy, 2 Business, 3 First Class
   columns : 0 Ordinary passenger, 1 Staff member
   ========================================== */

#define CLASS_COUNT 4
#define TAX_PERCENT 10
#define CURRENCY    "$"

int fareTable[CLASS_COUNT][2] = {
    {  120,   90 },   /* Economy         */
    {  200,  150 },   /* Premium Economy */
    {  450,  340 },   /* Business        */
    {  900,  700 }    /* First Class     */
};

int getFare(int seatClass, int isStaff)
{
    return fareTable[seatClass - 1][isStaff ? 1 : 0];
}

void displayFareTable(void)
{
    int i;

    printf("\n==========================================\n");
    printf("               FARE TABLE\n");
    printf("==========================================\n");
    printf("%-20s%-14s%-14s\n", "Class", "Ordinary", "Staff");
    printf("------------------------------------------\n");

    for (i = 0; i < CLASS_COUNT; i++)
        printf("%-20s%s%-13d%s%-13d\n",
               seatClassName(i + 1),
               CURRENCY, fareTable[i][0],
               CURRENCY, fareTable[i][1]);

    printf("------------------------------------------\n");
    printf("Tax: %d%% is added to every fare.\n", TAX_PERCENT);
}

void displayBookingSummary(Passenger p, int seatNumber)
{
    int seatClass = seats[seatNumber - 1].seatClass;
    int fare = getFare(seatClass, p.isStaff);
    int tax = fare * TAX_PERCENT / 100;

    printf("\n==========================================\n");
    printf("            BOOKING SUMMARY\n");
    printf("==========================================\n");
    printf("Passenger ID   : %d\n", p.passengerID);
    printf("Name           : %s\n", p.passengerName);
    printf("Passport No.   : %s\n", p.passportNumber);
    printf("Passenger Type : %s\n",
           p.isStaff ? "Staff Member" : "Ordinary Passenger");
    printf("------------------------------------------\n");
    printf("Seat           : %d%c (Seat No. %d)\n",
           (seatNumber - 1) / SEAT_COLS + 1,
           'A' + (seatNumber - 1) % SEAT_COLS,
           seatNumber);
    printf("Class          : %s\n", seatClassName(seatClass));
    printf("------------------------------------------\n");
    printf("Base Fare      : %s%d\n", CURRENCY, fare);
    printf("Tax (%d%%)      : %s%d\n", TAX_PERCENT, CURRENCY, tax);
    printf("TOTAL          : %s%d\n", CURRENCY, fare + tax);
    printf("==========================================\n");
}

void bookTicket(void)
{
    int id, i, found = -1, seatNumber;
    char confirm;

    printf("Enter passenger ID: ");
    if (scanf("%d", &id) != 1) {
        clearInputLine();
        printf("Invalid ID!\n");
        return;
    }
    clearInputLine();

    for (i = 0; i < passengerCount; i++) {
        if (passengers[i].passengerID == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Passenger not found! Add the passenger first.\n");
        return;
    }

    displayFareTable();
    displaySeatGrid();

    seatNumber = pickSeat();

    if (seatNumber == 0) {
        printf("Booking cancelled.\n");
        return;
    }

    displayBookingSummary(passengers[found], seatNumber);

    printf("Confirm booking? (Y/N): ");
    if (scanf(" %c", &confirm) != 1) {
        clearInputLine();
        return;
    }
    clearInputLine();

    if (confirm != 'Y' && confirm != 'y') {
        printf("Booking cancelled.\n");
        return;
    }

    if (bookSeatNumber(seatNumber))
        printf("Ticket booked successfully!\n");
    else
        printf("Could not save seat booking!\n");
}

/* ==========================================
   SEATS MENU
   ========================================== */

void seatsMenu(void)
{
    int choice, seatNumber;

    do {
        printf("\n1. Display all seats\n");
        printf("2. Book a seat\n");
        printf("3. Show available seat count\n");
        printf("4. Seat grid (view and pick a seat)\n");
        printf("5. View fare table\n");
        printf("0. Back\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputLine();
            printf("Invalid input!\n");
            continue;
        }
        clearInputLine();

        switch (choice) {
            case 1:
                displaySeats(seats);
                break;

            case 2:
                printf("Enter seat number (1-180): ");

                if (scanf("%d", &seatNumber) != 1) {
                    clearInputLine();
                    printf("Invalid seat number!\n");
                    break;
                }
                clearInputLine();

                if (seatNumber < 1 || seatNumber > TOTAL_SEATS) {
                    printf("Invalid seat number!\n");
                } else if (seats[seatNumber - 1].isBooked) {
                    printf("Seat already booked!\n");
                } else {
                    seats[seatNumber - 1].isBooked = 1;

                    if (saveSeats(seats))
                        printf("Seat booked successfully!\n");
                    else {
                        seats[seatNumber - 1].isBooked = 0;
                        printf("Could not save seat booking!\n");
                    }
                }
                break;

            case 3:
                printf("Available seats: %d\n",
                       countAvailableSeats(seats));
                break;

            case 4:
                seatGridMenu();
                break;

            case 5:
                displayFareTable();
                break;

            case 0:
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);
}

/* ==========================================
   CHECK-IN MODULE
   ========================================== */

void passengerCheckin(void)
{
    char Name[50];
    char BookingID[20];
    int flightnumber;
    FILE *fp;

    printf("Enter passenger name: ");
    if (fgets(Name, sizeof(Name), stdin) == NULL)
        return;

    Name[strcspn(Name, "\n")] = '\0';

    printf("Enter Booking ID: ");
    scanf("%19s", BookingID);

    printf("Enter Flight Number: ");
    if (scanf("%d", &flightnumber) != 1) {
        clearInputLine();
        printf("Invalid flight number!\n");
        return;
    }
    clearInputLine();

    printf("\nPassenger Name: %s\n", Name);
    printf("Booking ID: %s\n", BookingID);
    printf("Flight Number: %d\n", flightnumber);

    fp = fopen("checkin.txt", "a");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(fp, "%s,%s,%d\n", Name, BookingID, flightnumber);
    fclose(fp);

    printf("Check-in record saved successfully!\n");
}

void viewCheckinStatus(void)
{
    FILE *fp = fopen("checkin.txt", "r");
    char record[100];

    if (fp == NULL) {
        printf("No check-in records found!\n");
        return;
    }

    printf("\n===== CHECK-IN RECORDS =====\n");

    while (fgets(record, sizeof(record), fp) != NULL)
        printf("%s", record);

    fclose(fp);
}

void generateBoardingPass(void)
{
    FILE *fp;
    char Name[50], BookingID[20], searchID[20];
    int flightnumber, found = 0;

    printf("Enter Booking ID: ");
    scanf("%19s", searchID);
    clearInputLine();

    fp = fopen("checkin.txt", "r");

    if (fp == NULL) {
        printf("No check-in records found!\n");
        return;
    }

    while (fscanf(fp, " %49[^,],%19[^,],%d",
                  Name, BookingID, &flightnumber) == 3) {

        if (strcmp(searchID, BookingID) == 0) {
            printf("\n====================================\n");
            printf("          BOARDING PASS\n");
            printf("====================================\n");
            printf("Passenger Name : %s\n", Name);
            printf("Booking ID     : %s\n", BookingID);
            printf("Flight Number  : %d\n", flightnumber);
            printf("Status         : CHECK-IN RECORDED\n");
            printf("====================================\n");

            found = 1;
            break;
        }
    }

    if (!found)
        printf("Booking ID not found!\n");

    fclose(fp);
}

void checkinMenu(void)
{
    int choice;

    do {
        printf("\n1. Passenger Check-in\n");
        printf("2. View Check-in Status\n");
        printf("3. Generate Boarding Pass\n");
        printf("4. Back\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputLine();
            printf("Invalid input!\n");
            continue;
        }
        clearInputLine();

        switch (choice) {
            case 1: passengerCheckin(); break;
            case 2: viewCheckinStatus(); break;
            case 3: generateBoardingPass(); break;
            case 4: break;
            default: printf("Invalid choice!\n");
        }

    } while (choice != 4);
}

/* ==========================================
   MAIN PROGRAM
   ========================================== */

int main(void)
{
    int role, choice;

    loadPassengers();

    if (!loadSeats(seats)) {
        initializeSeats(seats);
        saveSeats(seats);
    }

    do {
        printf("\n==========================================\n");
        printf("      AIRLINE MANAGEMENT SYSTEM\n");
        printf("==========================================\n");
        printf("Welcome! Please select your role:\n\n");
        printf("1. Passenger\n");
        printf("2. Admin\n");
        printf("3. Staff\n");
        printf("4. Exit\n");
        printf("------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &role) != 1) {
            clearInputLine();
            printf("Invalid choice!\n");
            continue;
        }
        clearInputLine();

        switch (role) {
            case 1:
                do {
                    printf("\n===== PASSENGER MENU =====\n");
                    printf("1. Add Passenger\n");
                    printf("2. View Flights\n");
                    printf("3. Book Ticket\n");
                    printf("4. Cancel Ticket\n");
                    printf("5. Reschedule Ticket\n");
                    printf("6. View All Passengers\n");
                    printf("7. Search Passenger\n");
                    printf("8. Update Passenger\n");
                    printf("9. Delete Passenger\n");
                    printf("10. Seats\n");
                    printf("11. Check-in\n");
                    printf("12. Back\n");
                    printf("Enter your choice: ");

                    if (scanf("%d", &choice) != 1) {
                        clearInputLine();
                        printf("Invalid choice!\n");
                        continue;
                    }
                    clearInputLine();

                    switch (choice) {
                        case 1: addPassenger(); break;
                        case 2: printf("View Flights module not added yet.\n"); break;
                        case 3: bookTicket(); break;
                        case 4: printf("Cancellation module not added yet.\n"); break;
                        case 5: printf("Rescheduling module not added yet.\n"); break;
                        case 6: viewPassengers(); break;
                        case 7: findPassenger(); break;
                        case 8: updatePassenger(); break;
                        case 9: deletePassenger(); break;
                        case 10: seatsMenu(); break;
                        case 11: checkinMenu(); break;
                        case 12: break;
                        default: printf("Invalid choice!\n");
                    }

                } while (choice != 12);
                break;

            case 2:
                printf("Admin module has not been added yet.\n");
                break;

            case 3:
                do {
                    printf("\n===== STAFF MENU =====\n");
                    printf("1. Manage Seats\n");
                    printf("2. Passenger Check-in\n");
                    printf("3. View Check-in Records\n");
                    printf("4. Generate Boarding Pass\n");
                    printf("5. Back\n");
                    printf("Enter your choice: ");

                    if (scanf("%d", &choice) != 1) {
                        clearInputLine();
                        printf("Invalid choice!\n");
                        continue;
                    }
                    clearInputLine();

                    switch (choice) {
                        case 1: seatsMenu(); break;
                        case 2: passengerCheckin(); break;
                        case 3: viewCheckinStatus(); break;
                        case 4: generateBoardingPass(); break;
                        case 5: break;
                        default: printf("Invalid choice!\n");
                    }

                } while (choice != 5);
                break;

            case 4:
                printf("Exiting Airline Management System.\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (role != 4);

    return 0;
}
