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

void seatsMenu(void)
{
    int choice, seatNumber;

    do {
        printf("\n1. Display all seats\n");
        printf("2. Book a seat\n");
        printf("3. Show available seat count\n");
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
                        case 3: printf("Booking module not added yet.\n"); break;
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

