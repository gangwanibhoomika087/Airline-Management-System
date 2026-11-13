
 * AIRLINE MANAGEMENT SYSTEM  
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ------------------------- CONSTANTS ------------------------- */

#define TOTAL_SEATS      180
#define MAX_PASSENGERS   200
#define MAX_FLIGHTS      50
#define MAX_RESERVATIONS 1000

#define FLIGHT_FILE      "flights.txt"
#define PASSENGER_FILE   "passengers.txt"
#define RESERVATION_FILE "reservations.txt"

#define STAFF_DISCOUNT       0.50   /* 50% off ticket fare for staff   */
#define STAFF_EXTRA_BAGGAGE  10     /* extra free kg for staff         */
#define EXTRA_BAGGAGE_RATE   500.0  /* PKR per extra kg                */
#define MAX_BAGGAGE_KG       60
#define PET_BASE_CHARGE      5000.0 /* PKR flat charge for a pet       */
#define PET_RATE_PER_KG      300.0  /* PKR per kg of pet               */
#define MAX_PET_KG           32

enum { ST_SCHEDULED, ST_DELAYED, ST_BOARDING, ST_DEPARTED, ST_CANCELLED };

const char *statusNames[] = { "Scheduled", "Delayed", "Boarding",
                              "Departed", "Cancelled" };
const char *className[]   = { "", "Economy", "Premium Economy",
                              "Business", "First Class" };
const double classMult[]  = { 0, 1.0, 1.5, 2.5, 4.0 };
const int    freeBag[]    = { 0, 20, 25, 35, 45 };
const char *mealName[]    = { "No meal", "Vegetarian", "Non-Vegetarian", "Vegan" };
const double mealPrice[]  = { 0, 800, 1000, 900 };

/* ------------------------- STRUCTURES ------------------------ */

typedef struct {
    int  passengerID;
    char passengerName[50];
    char passengerPhone[20];
    char passportNumber[20];
    int  isStaff;
} Passenger;

typedef struct {
    int    flightID;
    char   origin[30];
    char   destination[30];
    char   date[11];      /* DD-MM-YYYY */
    char   depTime[6];    /* HH:MM      */
    int    status;
    double baseFare;      /* Economy fare */
} Flight;

typedef struct {
    int    bookingID;
    int    passengerID;
    int    flightID;
    int    seatNumber;
    int    meal;
    int    baggageKg;
    int    hasPet;
    int    petKg;
    int    status;        /* 1 = confirmed, 0 = cancelled */
    int    checkedIn;
    double ticketFare;
    double mealCharge;
    double baggageCharge;
    double petCharge;
} Reservation;

Passenger   passengers[MAX_PASSENGERS];
Flight      flights[MAX_FLIGHTS];
Reservation reservations[MAX_RESERVATIONS];

int passengerCount = 0, flightCount = 0, resCount = 0;
int nextPassengerID = 1001, nextFlightID = 101, nextBookingID = 5001;

/* ---------------------- INPUT HELPERS ------------------------ */

void trim(char *s)
{
    int start = 0, end = (int)strlen(s);

    while (end > 0 && isspace((unsigned char)s[end - 1]))
        end--;
    s[end] = '\0';

    while (s[start] && isspace((unsigned char)s[start]))
        start++;
    if (start > 0)
        memmove(s, s + start, strlen(s + start) + 1);
}

void readLine(const char *prompt, char *buf, int size)
{
    int len, ch;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, size, stdin) == NULL) {
        printf("\nInput closed. Exiting program.\n");
        exit(0);
    }

    len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    else
        while ((ch = getchar()) != '\n' && ch != EOF) { }

    trim(buf);
}

int readInt(const char *prompt, int min, int max)
{
    char buf[32], *end;
    long v;

    for (;;) {
        readLine(prompt, buf, sizeof(buf));
        if (buf[0] != '\0') {
            v = strtol(buf, &end, 10);
            if (*end == '\0' && v >= min && v <= max)
                return (int)v;
        }
        printf("Invalid input! Enter a number from %d to %d.\n", min, max);
    }
}

int readYesNo(const char *prompt)
{
    return readInt(prompt, 0, 1);
}

void removeBar(char *s)
{
    for (; *s; s++)
        if (*s == '|')
            *s = '-';
}

void readNonEmpty(const char *prompt, char *buf, int size)
{
    for (;;) {
        readLine(prompt, buf, size);
        removeBar(buf);
        if (buf[0] != '\0')
            return;
        printf("This field cannot be empty!\n");
    }
}

/* ------------------------- VALIDATORS ------------------------ */

int isValidPhone(const char *s)
{
    int i, len = (int)strlen(s);

    if (len < 7 || len > 15)
        return 0;
    for (i = 0; i < len; i++)
        if (!isdigit((unsigned char)s[i]) && s[i] != '+' && s[i] != '-')
            return 0;
    return 1;
}

int isValidPassport(const char *s)
{
    int i, len = (int)strlen(s);

    if (len < 6 || len > 19)
        return 0;
    for (i = 0; i < len; i++)
        if (!isalnum((unsigned char)s[i]))
            return 0;
    return 1;
}

int isValidDate(const char *d)
{
    int i, dd, mm, yy, dim;
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (strlen(d) != 10 || d[2] != '-' || d[5] != '-')
        return 0;
    for (i = 0; i < 10; i++)
        if (i != 2 && i != 5 && !isdigit((unsigned char)d[i]))
            return 0;

    sscanf(d, "%d-%d-%d", &dd, &mm, &yy);
    if (mm < 1 || mm > 12 || yy < 2026 || yy > 2100)
        return 0;

    dim = days[mm - 1];
    if (mm == 2 && ((yy % 4 == 0 && yy % 100 != 0) || yy % 400 == 0))
        dim = 29;

    return dd >= 1 && dd <= dim;
}

int equalsIgnoreCase(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

int containsIgnoreCase(const char *text, const char *key)
{
    int i, j, tl = (int)strlen(text), kl = (int)strlen(key);

    if (kl == 0)
        return 1;
    for (i = 0; i + kl <= tl; i++) {
        for (j = 0; j < kl; j++)
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)key[j]))
                break;
        if (j == kl)
            return 1;
    }
    return 0;
}

void readValidPhone(const char *prompt, char *buf, int size)
{
    for (;;) {
        readLine(prompt, buf, size);
        if (isValidPhone(buf))
            return;
        printf("Invalid phone! Use 7-15 digits (+ and - allowed).\n");
    }
}

void readValidPassport(const char *prompt, char *buf, int size)
{
    for (;;) {
        readLine(prompt, buf, size);
        if (isValidPassport(buf))
            return;
        printf("Invalid passport! Use 6-19 letters/digits, no spaces.\n");
    }
}

/* ------------------------- LOOKUPS --------------------------- */

int findPassengerIndex(int id)
{
    int i;
    for (i = 0; i < passengerCount; i++)
        if (passengers[i].passengerID == id)
            return i;
    return -1;
}

int findFlightIndex(int id)
{
    int i;
    for (i = 0; i < flightCount; i++)
        if (flights[i].flightID == id)
            return i;
    return -1;
}

int findReservationIndex(int id)
{
    int i;
    for (i = 0; i < resCount; i++)
        if (reservations[i].bookingID == id)
            return i;
    return -1;
}

int passportExists(const char *passport, int ignoreID)
{
    int i;
    for (i = 0; i < passengerCount; i++)
        if (passengers[i].passengerID != ignoreID &&
            equalsIgnoreCase(passengers[i].passportNumber, passport))
            return 1;
    return 0;
}

int passengerHasActiveBooking(int pid)
{
    int i;
    for (i = 0; i < resCount; i++)
        if (reservations[i].passengerID == pid && reservations[i].status == 1)
            return 1;
    return 0;
}

int passengerBookedFlight(int pid, int fid, int ignoreBookingID)
{
    int i;
    for (i = 0; i < resCount; i++)
        if (reservations[i].status == 1 &&
            reservations[i].passengerID == pid &&
            reservations[i].flightID == fid &&
            reservations[i].bookingID != ignoreBookingID)
            return 1;
    return 0;
}

int isBookable(int status)
{
    return status == ST_SCHEDULED || status == ST_DELAYED;
}

/* ------------------------- FILE HANDLING --------------------- */

int savePassengers(void)
{
    FILE *fp = fopen(PASSENGER_FILE, "w");
    int i;

    if (fp == NULL) {
        printf("Error opening passenger file!\n");
        return 0;
    }
    for (i = 0; i < passengerCount; i++)
        fprintf(fp, "%d|%s|%s|%s|%d\n",
                passengers[i].passengerID, passengers[i].passengerName,
                passengers[i].passengerPhone, passengers[i].passportNumber,
                passengers[i].isStaff);
    fclose(fp);
    return 1;
}

void loadPassengers(void)
{
    FILE *fp = fopen(PASSENGER_FILE, "r");
    char line[200];
    Passenger p;
    int maxID = 1000;

    if (fp == NULL)
        return;

    passengerCount = 0;
    while (passengerCount < MAX_PASSENGERS && fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%49[^|]|%19[^|]|%19[^|]|%d",
                   &p.passengerID, p.passengerName, p.passengerPhone,
                   p.passportNumber, &p.isStaff) == 5) {
            passengers[passengerCount++] = p;
            if (p.passengerID > maxID)
                maxID = p.passengerID;
        }
    }
    nextPassengerID = maxID + 1;
    fclose(fp);
}

int saveFlights(void)
{
    FILE *fp = fopen(FLIGHT_FILE, "w");
    int i;

    if (fp == NULL) {
        printf("Error opening flight file!\n");
        return 0;
    }
    for (i = 0; i < flightCount; i++)
        fprintf(fp, "%d|%s|%s|%s|%s|%d|%.2f\n",
                flights[i].flightID, flights[i].origin, flights[i].destination,
                flights[i].date, flights[i].depTime, flights[i].status,
                flights[i].baseFare);
    fclose(fp);
    return 1;
}

int loadFlights(void)
{
    FILE *fp = fopen(FLIGHT_FILE, "r");
    char line[200];
    Flight f;
    int maxID = 100;

    if (fp == NULL)
        return 0;

    flightCount = 0;
    while (flightCount < MAX_FLIGHTS && fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%29[^|]|%29[^|]|%10[^|]|%5[^|]|%d|%lf",
                   &f.flightID, f.origin, f.destination, f.date, f.depTime,
                   &f.status, &f.baseFare) == 7 &&
            f.status >= ST_SCHEDULED && f.status <= ST_CANCELLED) {
            flights[flightCount++] = f;
            if (f.flightID > maxID)
                maxID = f.flightID;
        }
    }
    nextFlightID = maxID + 1;
    fclose(fp);
    return 1;
}

int saveReservations(void)
{
    FILE *fp = fopen(RESERVATION_FILE, "w");
    int i;
    Reservation *r;

    if (fp == NULL) {
        printf("Error opening reservation file!\n");
        return 0;
    }
    for (i = 0; i < resCount; i++) {
        r = &reservations[i];
        fprintf(fp, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%.2f|%.2f|%.2f|%.2f\n",
                r->bookingID, r->passengerID, r->flightID, r->seatNumber,
                r->meal, r->baggageKg, r->hasPet, r->petKg, r->status,
                r->checkedIn, r->ticketFare, r->mealCharge,
                r->baggageCharge, r->petCharge);
    }
    fclose(fp);
    return 1;
}

void loadReservations(void)
{
    FILE *fp = fopen(RESERVATION_FILE, "r");
    char line[250];
    Reservation r;
    int maxID = 5000;

    if (fp == NULL)
        return;

    resCount = 0;
    while (resCount < MAX_RESERVATIONS && fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%lf|%lf|%lf|%lf",
                   &r.bookingID, &r.passengerID, &r.flightID, &r.seatNumber,
                   &r.meal, &r.baggageKg, &r.hasPet, &r.petKg, &r.status,
                   &r.checkedIn, &r.ticketFare, &r.mealCharge,
                   &r.baggageCharge, &r.petCharge) == 14 &&
            r.seatNumber >= 1 && r.seatNumber <= TOTAL_SEATS &&
            r.meal >= 0 && r.meal <= 3) {
            reservations[resCount++] = r;
            if (r.bookingID > maxID)
                maxID = r.bookingID;
        }
    }
    nextBookingID = maxID + 1;
    fclose(fp);
}

void addSampleFlights(void)
{
    const char *from[] = { "Karachi", "Karachi", "Karachi" };
    const char *to[]   = { "Lahore", "Islamabad", "Dubai" };
    const char *dt[]   = { "20-12-2026", "22-12-2026", "25-12-2026" };
    const char *tm[]   = { "09:30", "14:15", "21:45" };
    double fare[]      = { 18000, 22000, 45000 };
    int i;

    for (i = 0; i < 3; i++) {
        flights[i].flightID = nextFlightID++;
        strcpy(flights[i].origin, from[i]);
        strcpy(flights[i].destination, to[i]);
        strcpy(flights[i].date, dt[i]);
        strcpy(flights[i].depTime, tm[i]);
        flights[i].status = ST_SCHEDULED;
        flights[i].baseFare = fare[i];
    }
    flightCount = 3;
}

/* ------------------------- SEATS ----------------------------- */

int getSeatClass(int seat)
{
    if (seat <= 126) return 1;
    if (seat <= 150) return 2;
    if (seat <= 174) return 3;
    return 4;
}

void getClassRange(int cls, int *first, int *last)
{
    switch (cls) {
        case 1:  *first = 1;   *last = 126; break;
        case 2:  *first = 127; *last = 150; break;
        case 3:  *first = 151; *last = 174; break;
        default: *first = 175; *last = 180; break;
    }
}

int isSeatTaken(int flightID, int seat)
{
    int i;
    for (i = 0; i < resCount; i++)
        if (reservations[i].status == 1 &&
            reservations[i].flightID == flightID &&
            reservations[i].seatNumber == seat)
            return 1;
    return 0;
}

int countAvailable(int flightID, int cls)
{
    int first = 1, last = TOTAL_SEATS, s, count = 0;

    if (cls != 0)
        getClassRange(cls, &first, &last);
    for (s = first; s <= last; s++)
        if (!isSeatTaken(flightID, s))
            count++;
    return count;
}

void displaySeatMap(int flightID)
{
    int cls, first, last, s;

    printf("\n========== SEAT MAP (Flight %d) ==========\n", flightID);
    for (cls = 1; cls <= 4; cls++) {
        getClassRange(cls, &first, &last);
        printf("%-16s seats %3d-%3d : %d available\n", className[cls],
               first, last, countAvailable(flightID, cls));
    }
    printf("\n[ X ] = booked\n");
    for (s = 1; s <= TOTAL_SEATS; s++) {
        if (isSeatTaken(flightID, s))
            printf("[ X ]");
        else
            printf("[%3d]", s);
        if (s % 10 == 0)
            printf("\n");
    }
}

int chooseSeat(int flightID, int cls)
{
    int first, last, seat, s, n = 0;

    getClassRange(cls, &first, &last);
    printf("\nAvailable %s seats:\n", className[cls]);
    for (s = first; s <= last; s++)
        if (!isSeatTaken(flightID, s)) {
            printf("%4d", s);
            if (++n % 12 == 0)
                printf("\n");
        }
    printf("\n");

    for (;;) {
        seat = readInt("Enter seat number (0 to cancel): ", 0, TOTAL_SEATS);
        if (seat == 0)
            return 0;
        if (seat < first || seat > last)
            printf("Seat %d is not in %s (%d-%d)!\n", seat, className[cls],
                   first, last);
        else if (isSeatTaken(flightID, seat))
            printf("Seat already booked!\n");
        else
            return seat;
    }
}

void seatMapMenu(void)
{
    int fid = readInt("Enter Flight ID (0 to go back): ", 0, 999999);

    if (fid == 0)
        return;
    if (findFlightIndex(fid) < 0)
        printf("Flight not found!\n");
    else
        displaySeatMap(fid);
}

/* ------------------------- FARE ------------------------------ */

double totalFare(const Reservation *r)
{
    return r->ticketFare + r->mealCharge + r->baggageCharge + r->petCharge;
}

void calculateFare(Reservation *r, const Flight *f, int isStaff)
{
    int cls = getSeatClass(r->seatNumber);
    int allowance = freeBag[cls] + (isStaff ? STAFF_EXTRA_BAGGAGE : 0);
    int extra = r->baggageKg - allowance;

    r->ticketFare = f->baseFare * classMult[cls];
    if (isStaff)
        r->ticketFare *= (1.0 - STAFF_DISCOUNT);

    r->mealCharge = (cls >= 3) ? 0.0 : mealPrice[r->meal];
    r->baggageCharge = (extra > 0) ? extra * EXTRA_BAGGAGE_RATE : 0.0;
    r->petCharge = r->hasPet ? PET_BASE_CHARGE + r->petKg * PET_RATE_PER_KG : 0.0;
}

/* ------------------------- PASSENGERS ------------------------ */

void displayPassenger(const Passenger *p)
{
    printf("\nPassenger ID   : %d\n", p->passengerID);
    printf("Name           : %s\n", p->passengerName);
    printf("Phone          : %s\n", p->passengerPhone);
    printf("Passport Number: %s\n", p->passportNumber);
    printf("Staff Status   : %s\n",
           p->isStaff ? "Airline Staff Member" : "Ordinary Passenger");
}

/* returns new passenger ID or 0 on failure */
int addPassenger(int byAdmin)
{
    Passenger p;

    if (passengerCount >= MAX_PASSENGERS) {
        printf("Passenger list is full!\n");
        return 0;
    }

    p.passengerID = nextPassengerID;
    readNonEmpty("Enter passenger name: ", p.passengerName,
                 sizeof(p.passengerName));
    readValidPhone("Enter phone number: ", p.passengerPhone,
                   sizeof(p.passengerPhone));

    for (;;) {
        readValidPassport("Enter passport number: ", p.passportNumber,
                          sizeof(p.passportNumber));
        if (!passportExists(p.passportNumber, 0))
            break;
        printf("This passport number is already registered!\n");
    }

    p.isStaff = 0;
    if (byAdmin)
        p.isStaff = readYesNo("Is this passenger airline staff? (1 = Yes, 0 = No): ");

    passengers[passengerCount++] = p;
    if (savePassengers()) {
        nextPassengerID++;
        printf("Passenger added successfully! ID: %d\n", p.passengerID);
        return p.passengerID;
    }
    passengerCount--;
    return 0;
}

void updatePassenger(int id, int byAdmin)
{
    int idx = findPassengerIndex(id);
    Passenger old, *p;
    char buf[50];

    if (idx < 0) {
        printf("Passenger not found!\n");
        return;
    }
    p = &passengers[idx];
    old = *p;
    printf("(Press Enter to keep the current value)\n");

    readLine("New name: ", buf, sizeof(buf));
    removeBar(buf);
    if (buf[0] != '\0')
        strcpy(p->passengerName, buf);

    for (;;) {
        readLine("New phone: ", buf, sizeof(buf));
        if (buf[0] == '\0')
            break;
        if (isValidPhone(buf)) {
            strcpy(p->passengerPhone, buf);
            break;
        }
        printf("Invalid phone! Use 7-15 digits (+ and - allowed).\n");
    }

    for (;;) {
        readLine("New passport number: ", buf, sizeof(buf));
        if (buf[0] == '\0')
            break;
        if (!isValidPassport(buf))
            printf("Invalid passport! Use 6-19 letters/digits.\n");
        else if (passportExists(buf, id))
            printf("This passport number is already registered!\n");
        else {
            strcpy(p->passportNumber, buf);
            break;
        }
    }

    if (byAdmin)
        p->isStaff = readYesNo("Is passenger airline staff? (1 = Yes, 0 = No): ");

    if (savePassengers())
        printf("Passenger updated successfully!\n");
    else
        *p = old;
}

/* returns 1 if deleted */
int deletePassenger(int id)
{
    int idx = findPassengerIndex(id), j;
    Passenger deleted;

    if (idx < 0) {
        printf("Passenger not found!\n");
        return 0;
    }
    if (passengerHasActiveBooking(id)) {
        printf("Cannot delete: passenger has active bookings. Cancel them first.\n");
        return 0;
    }
    if (!readYesNo("Are you sure you want to delete? (1 = Yes, 0 = No): "))
        return 0;

    deleted = passengers[idx];
    for (j = idx; j < passengerCount - 1; j++)
        passengers[j] = passengers[j + 1];
    passengerCount--;

    if (savePassengers()) {
        printf("Passenger deleted successfully!\n");
        return 1;
    }
    for (j = passengerCount; j > idx; j--)
        passengers[j] = passengers[j - 1];
    passengers[idx] = deleted;
    passengerCount++;
    return 0;
}

/* ------------------------- FLIGHTS --------------------------- */

void printFlightHeader(void)
{
    printf("\n%-5s %-14s %-14s %-11s %-6s %-10s %11s %6s\n", "ID", "From",
           "To", "Date", "Time", "Status", "Econ. Fare", "Seats");
    printf("---------------------------------------------------------------"
           "------------\n");
}

void printFlightRow(const Flight *f)
{
    printf("%-5d %-14.14s %-14.14s %-11s %-6s %-10s %11.0f %6d\n",
           f->flightID, f->origin, f->destination, f->date, f->depTime,
           statusNames[f->status], f->baseFare,
           countAvailable(f->flightID, 0));
}

void viewFlights(void)
{
    int i;

    if (flightCount == 0) {
        printf("No flights available!\n");
        return;
    }
    printFlightHeader();
    for (i = 0; i < flightCount; i++)
        printFlightRow(&flights[i]);
}

/* reads a valid DD-MM-YYYY date; blank input = cancel (buf becomes "") */
void readTravelDate(const char *prompt, char *buf)
{
    for (;;) {
        readLine(prompt, buf, 12);
        if (buf[0] == '\0' || isValidDate(buf))
            return;
        printf("Invalid date! Use DD-MM-YYYY (year 2026 or later).\n");
    }
}

/* shows flights departing on the given date; returns how many were found */
int showFlightsOnDate(const char *date)
{
    int i, found = 0;

    for (i = 0; i < flightCount; i++)
        if (strcmp(flights[i].date, date) == 0) {
            if (!found)
                printFlightHeader();
            printFlightRow(&flights[i]);
            found++;
        }
    if (!found)
        printf("No flights available on %s! Try another date.\n", date);
    return found;
}

void searchFlights(void)
{
    char key[30], idText[12];
    int i, found = 0;

    readLine("Enter flight ID, city or date to search: ", key, sizeof(key));
    if (key[0] == '\0') {
        printf("Nothing entered!\n");
        return;
    }
    for (i = 0; i < flightCount; i++) {
        sprintf(idText, "%d", flights[i].flightID);
        if (strcmp(idText, key) == 0 ||
            containsIgnoreCase(flights[i].origin, key) ||
            containsIgnoreCase(flights[i].destination, key) ||
            containsIgnoreCase(flights[i].date, key)) {
            if (!found)
                printFlightHeader();
            printFlightRow(&flights[i]);
            found = 1;
        }
    }
    if (!found)
        printf("No matching flights found!\n");
}

/* ------------------------- BOOKINGS -------------------------- */

void displayReservation(const Reservation *r)
{
    int fi = findFlightIndex(r->flightID);
    int pi = findPassengerIndex(r->passengerID);
    int cls = getSeatClass(r->seatNumber);

    printf("\n==========================================\n");
    printf("             BOOKING DETAILS\n");
    printf("==========================================\n");
    printf("Booking ID     : %d\n", r->bookingID);
    printf("Passenger      : %s (ID %d)%s\n",
           pi >= 0 ? passengers[pi].passengerName : "Unknown", r->passengerID,
           (pi >= 0 && passengers[pi].isStaff) ? " [Staff]" : "");
    if (fi >= 0)
        printf("Flight         : %d  %s -> %s\n", r->flightID,
               flights[fi].origin, flights[fi].destination);
    else
        printf("Flight         : %d (no longer exists)\n", r->flightID);
    if (fi >= 0) {
        printf("Departure      : %s at %s\n", flights[fi].date,
               flights[fi].depTime);
        printf("Flight Status  : %s\n", statusNames[flights[fi].status]);
    }
    printf("Seat / Class   : %d / %s\n", r->seatNumber, className[cls]);
    printf("Meal           : %s\n", mealName[r->meal]);
    printf("Baggage        : %d kg\n", r->baggageKg);
    if (r->hasPet)
        printf("Pet Transport  : Yes (%d kg)\n", r->petKg);
    else
        printf("Pet Transport  : No\n");
    printf("Booking Status : %s\n", r->status ? "Confirmed" : "Cancelled");
    printf("Check-in       : %s\n", r->checkedIn ? "Done" : "Not done");
    printf("------------------------------------------\n");
    printf("Ticket fare    : PKR %10.2f\n", r->ticketFare);
    printf("Meal charge    : PKR %10.2f\n", r->mealCharge);
    printf("Baggage charge : PKR %10.2f\n", r->baggageCharge);
    printf("Pet charge     : PKR %10.2f\n", r->petCharge);
    printf("TOTAL FARE     : PKR %10.2f\n", totalFare(r));
    printf("==========================================\n");
}

int askBooking(int pid)
{
    int id = readInt("Enter Booking ID (0 to go back): ", 0, 999999), idx;

    if (id == 0)
        return -1;
    idx = findReservationIndex(id);
    if (idx < 0) {
        printf("Booking not found!\n");
        return -1;
    }
    if (pid != 0 && reservations[idx].passengerID != pid) {
        printf("This booking does not belong to your account!\n");
        return -1;
    }
    return idx;
}

void viewBookings(int pid)
{
    int i, fi, pi, found = 0;

    for (i = 0; i < resCount; i++) {
        if (pid != 0 && reservations[i].passengerID != pid)
            continue;
        if (!found) {
            printf("\n%-8s %-16s %-7s %-5s %-16s %12s %-10s\n", "Booking",
                   "Passenger", "Flight", "Seat", "Class", "Total (PKR)",
                   "Status");
            printf("----------------------------------------------------"
                   "------------------------\n");
        }
        found = 1;
        pi = findPassengerIndex(reservations[i].passengerID);
        fi = findFlightIndex(reservations[i].flightID);
        printf("%-8d %-16.16s %-7d %-5d %-16s %12.2f %-10s\n",
               reservations[i].bookingID,
               pi >= 0 ? passengers[pi].passengerName : "Unknown",
               reservations[i].flightID, reservations[i].seatNumber,
               className[getSeatClass(reservations[i].seatNumber)],
               totalFare(&reservations[i]),
               reservations[i].status ? "Confirmed" : "Cancelled");
        (void)fi;
    }
    if (!found)
        printf("No bookings found!\n");
}

void searchBooking(int pid)
{
    int idx = askBooking(pid);

    if (idx >= 0)
        displayReservation(&reservations[idx]);
}

int selectClass(int fIdx, int staff)
{
    int cls, left;
    double fare;

    printf("\nSelect ticket class:\n");
    for (cls = 1; cls <= 4; cls++) {
        fare = flights[fIdx].baseFare * classMult[cls];
        if (staff)
            fare *= (1.0 - STAFF_DISCOUNT);
        left = countAvailable(flights[fIdx].flightID, cls);
        printf("%d. %-16s PKR %10.0f  (%d seats left)\n", cls, className[cls],
               fare, left);
    }
    if (staff)
        printf("(Staff discount of %.0f%% already applied)\n",
               STAFF_DISCOUNT * 100);
    printf("0. Cancel\n");

    for (;;) {
        cls = readInt("Enter class: ", 0, 4);
        if (cls == 0)
            return 0;
        if (countAvailable(flights[fIdx].flightID, cls) == 0)
            printf("No seats left in %s! Choose another class.\n",
                   className[cls]);
        else
            return cls;
    }
}

void askExtras(Reservation *r, int cls, int staff)
{
    int i, allowance = freeBag[cls] + (staff ? STAFF_EXTRA_BAGGAGE : 0);

    printf("\nSelect meal preference:\n");
    for (i = 0; i <= 3; i++) {
        if (i == 0)
            printf("%d. %s\n", i, mealName[i]);
        else if (cls >= 3)
            printf("%d. %-15s (complimentary)\n", i, mealName[i]);
        else
            printf("%d. %-15s PKR %.0f\n", i, mealName[i], mealPrice[i]);
    }
    r->meal = readInt("Enter meal choice: ", 0, 3);

    printf("\nFree baggage allowance: %d kg. Extra baggage: PKR %.0f per kg.\n",
           allowance, EXTRA_BAGGAGE_RATE);
    r->baggageKg = readInt("Total baggage weight in kg: ", 0, MAX_BAGGAGE_KG);

    r->hasPet = readYesNo("\nTravelling with a pet? (1 = Yes, 0 = No): ");
    r->petKg = 0;
    if (r->hasPet) {
        printf("Pet charge: PKR %.0f + PKR %.0f per kg.\n", PET_BASE_CHARGE,
               PET_RATE_PER_KG);
        r->petKg = readInt("Pet weight in kg: ", 1, MAX_PET_KG);
    }
}

void bookTicket(int pid)
{
    Reservation r;
    int pIdx = findPassengerIndex(pid), fIdx, fid, cls, seat;
    char date[12];

    if (resCount >= MAX_RESERVATIONS) {
        printf("Reservation list is full!\n");
        return;
    }
    if (flightCount == 0) {
        printf("No flights available!\n");
        return;
    }

    readTravelDate("\nEnter travel date (DD-MM-YYYY, Enter to cancel): ", date);
    if (date[0] == '\0')
        return;
    if (!showFlightsOnDate(date))
        return;

    fid = readInt("\nEnter Flight ID to book (0 to cancel): ", 0, 999999);
    if (fid == 0)
        return;
    fIdx = findFlightIndex(fid);
    if (fIdx < 0) {
        printf("Flight not found!\n");
        return;
    }
    if (strcmp(flights[fIdx].date, date) != 0) {
        printf("That flight is not on %s! Choose a flight from the list.\n",
               date);
        return;
    }
    if (!isBookable(flights[fIdx].status)) {
        printf("This flight is %s and cannot be booked.\n",
               statusNames[flights[fIdx].status]);
        return;
    }
    if (passengerBookedFlight(pid, fid, 0)) {
        printf("You already have an active booking on this flight!\n");
        return;
    }
    if (countAvailable(fid, 0) == 0) {
        printf("Flight is fully booked!\n");
        return;
    }

    cls = selectClass(fIdx, passengers[pIdx].isStaff);
    if (cls == 0)
        return;
    seat = chooseSeat(fid, cls);
    if (seat == 0)
        return;

    memset(&r, 0, sizeof(r));
    r.bookingID = nextBookingID;
    r.passengerID = pid;
    r.flightID = fid;
    r.seatNumber = seat;
    r.status = 1;
    askExtras(&r, cls, passengers[pIdx].isStaff);
    calculateFare(&r, &flights[fIdx], passengers[pIdx].isStaff);

    displayReservation(&r);
    if (!readYesNo("Confirm booking? (1 = Yes, 0 = No): ")) {
        printf("Booking discarded.\n");
        return;
    }

    reservations[resCount++] = r;
    if (saveReservations()) {
        nextBookingID++;
        printf("Ticket booked successfully! Your Booking ID is %d\n",
               r.bookingID);
    } else
        resCount--;
}

void cancelTicket(int pid)
{
    int idx = askBooking(pid), fi;

    if (idx < 0)
        return;
    if (!reservations[idx].status) {
        printf("This booking is already cancelled!\n");
        return;
    }
    fi = findFlightIndex(reservations[idx].flightID);
    if (fi >= 0 && flights[fi].status == ST_DEPARTED) {
        printf("This flight has already departed!\n");
        return;
    }
    displayReservation(&reservations[idx]);
    if (!readYesNo("Cancel this booking? (1 = Yes, 0 = No): "))
        return;

    reservations[idx].status = 0;
    reservations[idx].checkedIn = 0;
    if (saveReservations())
        printf("Booking cancelled. The seat is available again.\n");
    else
        reservations[idx].status = 1;
}

void rescheduleTicket(int pid)
{
    int idx = askBooking(pid), oldFi, newFi, nf, cls, seat, pi, staff;
    Reservation old, r;
    char date[12];

    if (idx < 0)
        return;
    old = reservations[idx];
    if (!old.status) {
        printf("Cancelled bookings cannot be rescheduled!\n");
        return;
    }
    if (old.checkedIn) {
        printf("Checked-in bookings cannot be rescheduled!\n");
        return;
    }
    oldFi = findFlightIndex(old.flightID);
    if (oldFi >= 0 && !isBookable(flights[oldFi].status)) {
        printf("Current flight is %s; it cannot be rescheduled.\n",
               statusNames[flights[oldFi].status]);
        return;
    }

    readTravelDate("\nEnter NEW travel date (DD-MM-YYYY, Enter to cancel): ",
                   date);
    if (date[0] == '\0')
        return;
    if (!showFlightsOnDate(date))
        return;

    nf = readInt("\nEnter NEW Flight ID (0 to cancel): ", 0, 999999);
    if (nf == 0)
        return;
    newFi = findFlightIndex(nf);
    if (newFi < 0) {
        printf("Flight not found!\n");
        return;
    }
    if (strcmp(flights[newFi].date, date) != 0) {
        printf("That flight is not on %s! Choose a flight from the list.\n",
               date);
        return;
    }
    if (nf == old.flightID) {
        printf("That is your current flight!\n");
        return;
    }
    if (!isBookable(flights[newFi].status)) {
        printf("New flight is %s and cannot be booked.\n",
               statusNames[flights[newFi].status]);
        return;
    }
    if (passengerBookedFlight(old.passengerID, nf, old.bookingID)) {
        printf("The passenger already has a booking on that flight!\n");
        return;
    }

    cls = getSeatClass(old.seatNumber);
    if (countAvailable(nf, cls) == 0) {
        printf("No %s seats left on that flight!\n", className[cls]);
        return;
    }
    seat = chooseSeat(nf, cls);
    if (seat == 0)
        return;

    pi = findPassengerIndex(old.passengerID);
    staff = (pi >= 0 && passengers[pi].isStaff);
    r = old;
    r.flightID = nf;
    r.seatNumber = seat;
    calculateFare(&r, &flights[newFi], staff);

    displayReservation(&r);
    printf("Old total: PKR %.2f   New total: PKR %.2f\n", totalFare(&old),
           totalFare(&r));
    if (!readYesNo("Confirm reschedule? (1 = Yes, 0 = No): "))
        return;

    reservations[idx] = r;
    if (saveReservations())
        printf("Ticket rescheduled successfully!\n");
    else
        reservations[idx] = old;
}

/* ------------------------- CHECK-IN -------------------------- */

void passengerCheckin(int pid)
{
    int idx = askBooking(pid), fi;

    if (idx < 0)
        return;
    if (!reservations[idx].status) {
        printf("This booking is cancelled!\n");
        return;
    }
    if (reservations[idx].checkedIn) {
        printf("Passenger is already checked in!\n");
        return;
    }
    fi = findFlightIndex(reservations[idx].flightID);
    if (fi < 0 || flights[fi].status == ST_CANCELLED ||
        flights[fi].status == ST_DEPARTED) {
        printf("Check-in is not possible for this flight.\n");
        return;
    }

    reservations[idx].checkedIn = 1;
    if (saveReservations())
        printf("Check-in completed! You may now generate the boarding pass.\n");
    else
        reservations[idx].checkedIn = 0;
}

void generateBoardingPass(int pid)
{
    int idx = askBooking(pid), fi, pi;
    Reservation *r;

    if (idx < 0)
        return;
    r = &reservations[idx];
    if (!r->status) {
        printf("This booking is cancelled!\n");
        return;
    }
    if (!r->checkedIn) {
        printf("Passenger has not checked in yet!\n");
        return;
    }
    fi = findFlightIndex(r->flightID);
    pi = findPassengerIndex(r->passengerID);

    printf("\n====================================\n");
    printf("          BOARDING PASS\n");
    printf("====================================\n");
    printf("Passenger Name : %s\n", pi >= 0 ? passengers[pi].passengerName : "Unknown");
    printf("Booking ID     : %d\n", r->bookingID);
    printf("Flight Number  : %d\n", r->flightID);
    if (fi >= 0) {
        printf("Route          : %s -> %s\n", flights[fi].origin,
               flights[fi].destination);
        printf("Date / Time    : %s  %s\n", flights[fi].date, flights[fi].depTime);
    }
    printf("Seat           : %d (%s)\n", r->seatNumber,
           className[getSeatClass(r->seatNumber)]);
    printf("Status         : CHECKED IN\n");
    printf("====================================\n");
}

/* ------------------------- MENUS ----------------------------- */

void passengerMenu(int pid)
{
    int choice;

    do {
        printf("\n===== PASSENGER MENU =====\n");
        printf(" 1. View Flights\n 2. Search Flights\n 3. Book Ticket\n");
        printf(" 4. Cancel Ticket\n 5. Reschedule Ticket\n 6. My Bookings\n");
        printf(" 7. Search My Booking\n 8. View Seat Map\n 9. Check-in\n");
        printf("10. Generate Boarding Pass\n11. My Profile\n12. Update Profile\n");
        printf("13. Delete My Account\n 0. Logout\n");
        choice = readInt("Enter your choice: ", 0, 13);

        switch (choice) {
            case 1:  viewFlights(); break;
            case 2:  searchFlights(); break;
            case 3:  bookTicket(pid); break;
            case 4:  cancelTicket(pid); break;
            case 5:  rescheduleTicket(pid); break;
            case 6:  viewBookings(pid); break;
            case 7:  searchBooking(pid); break;
            case 8:  seatMapMenu(); break;
            case 9:  passengerCheckin(pid); break;
            case 10: generateBoardingPass(pid); break;
            case 11: displayPassenger(&passengers[findPassengerIndex(pid)]); break;
            case 12: updatePassenger(pid, 0); break;
            case 13:
                if (deletePassenger(pid))
                    choice = 0;
                break;
        }
    } while (choice != 0);
}

void passengerSession(void)
{
    int choice, id, idx;
    char passport[20];

    do {
        printf("\n===== PASSENGER =====\n");
        printf("1. Register new passenger\n2. Login\n0. Back\n");
        choice = readInt("Enter your choice: ", 0, 2);

        if (choice == 1) {
            id = addPassenger(0);
            if (id != 0)
                passengerMenu(id);
        } else if (choice == 2) {
            id = readInt("Enter your Passenger ID: ", 1, 999999);
            readLine("Enter your passport number: ", passport, sizeof(passport));
            idx = findPassengerIndex(id);
            if (idx >= 0 && equalsIgnoreCase(passengers[idx].passportNumber, passport))
                passengerMenu(id);
            else
                printf("Invalid ID or passport number!\n");
        }
    } while (choice != 0);
}

/* ------------------------- MAIN ------------------------------ */

int main(void)
{
    int role;

    loadPassengers();
    loadReservations();
    if (!loadFlights()) {
        addSampleFlights();
        saveFlights();
    }

    do {
        printf("\n==========================================\n");
        printf("      AIRLINE MANAGEMENT SYSTEM\n");
        printf("==========================================\n");
        printf("Welcome! Please select your role:\n\n");
        printf("1. Passenger\n2. Admin\n3. Staff\n4. Exit\n");
        printf("------------------------------------------\n");
        role = readInt("Enter your choice: ", 1, 4);

        switch (role) {
            case 1: passengerSession(); break;
            case 2: printf("\nAdmin portal: not added.\n"); break;
            case 3: printf("\nStaff portal: not added.\n"); break;
            case 4: printf("Exiting Airline Management System. Goodbye!\n"); break;
        }
    } while (role != 4);

    return 0;
}
