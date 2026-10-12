# Airline-Management-System

Airline Ticket Booking and Fare Processing System, a Programming Fundamentals project in C.

> [!NOTE]
> This project is under development. Features are added and committed week by week, so some parts described below are still planned.

**Course:** Programming Fundamentals, Fall 2026, Semester 1
**University:** FAST National University of Computer and Emerging Sciences, Karachi
**Built with:** C, GCC, console interface, Windows
**Status:** Under development

---

## Background

Handling an airline's bookings by hand is error-prone. Seats get sold twice, cancellations are missed, fares are added up wrongly, and extras such as baggage, meals and pets are easy to forget. Flight details, passenger details and tickets also end up scattered, so one change can leave the others out of date.

## Our approach

This program keeps flights, passengers and bookings together in one system. When a ticket is sold, the program checks the seat, adds up the charges, and updates the flight at the same time, so the records always agree with each other. Everything is saved to files so nothing is lost when the program is closed.

## What it can do

**Flight handling**
- Add, update, delete, search and display flights
- Maintain schedules and flight status

**Passenger handling**
- Store and manage passenger records
- Basic check-in once a booking exists

**Ticketing**
- Book, cancel and reschedule tickets
- Seat selection with an availability check
- Booking reference numbers that can be searched later

**Extras and pricing**
- Four ticket classes: Economy, Premium Economy, Business and First
- Meal preference
- Baggage allowance with charges for extra baggage
- Pet transport with its own charge
- Airline staff benefits
- Total fare worked out and shown before the booking is saved

**Saving data**
- Flights, passengers and bookings stored and reloaded using text files

## How a booking flows through the program

1. The program starts and loads the saved files.
2. The main menu appears and the user picks a task.
3. For a new ticket: choose a flight, then enter or select the passenger.
4. Pick a ticket class and a free seat.
5. Add meal, baggage and pet options.
6. The program applies any staff benefit and calculates the total fare.
7. The booking is saved, the seat is marked taken, and a reference number is printed.
8. When the user exits, all records are written back to the files.

## Screen preview (planned)

The values below are only an example of the finished output.

```
+---------------------------------------------+
|        AIRLINE MANAGEMENT SYSTEM            |
+---------------------------------------------+
|  1. Flights          5. Check-in            |
|  2. Passengers       6. Find booking        |
|  3. Book ticket      7. Reports             |
|  4. Cancel/Change    0. Save and exit       |
+---------------------------------------------+
Your choice: _
```

```
+---------------------------------------------+
|              BOOKING CONFIRMED              |
+---------------------------------------------+
|  Reference : AMS-0001                       |
|  Passenger : Sample Name                    |
|  Flight    : PK-301  KHI -> LHE             |
|  Class     : Business         Seat : 4A     |
|  Meal      : Vegetarian                     |
|  Baggage   : 30 kg (10 kg extra)            |
|  Pet       : No                             |
|  Total     : PKR 00,000                     |
+---------------------------------------------+
```

## Programming topics used

- **Menus and decisions:** `switch` statements and nested menus drive every screen.
- **Loops and arrays:** lists of flights and passengers, and a seat grid built from a 2D array.
- **Functions and pointers:** reserving seats and calculating fares by passing values by reference.
- **Strings:** searching flights by city, checking names and building reference numbers.
- **Recursion:** searching for a flight and validating reference numbers.
- **`static` and `const`:** a booking counter and fixed fare tables.
- **Structures:** flights, passengers, bookings and dates are each a struct.
- **Dynamic memory:** lists that grow as records are added, and the seat map.
- **File handling:** saving and loading all records.

## Compile and run

This program is meant for Windows and needs a C compiler (GCC through MinGW, or the one that comes with Dev-C++).

1. On this page, click the green **Code** button and choose **Download ZIP**, then extract the folder.
2. Open a terminal inside the extracted folder.
3. Compile the program:
   ```
   gcc -Wall src/*.c -o airline
   ```
4. Start it:
   ```
   airline.exe
   ```

> [!TIP]
> Always start the program from the main project folder so it can find the `data/` folder.

> [!WARNING]
> Records are saved into `data/`. Do not delete or rename the files there while testing, or saved data will not load.

## Group members and roles
| Member | Roll No. | Main work |
|---|---|---|
| Bhoomika Gangwani | 26K-0589 | Main program, flights, seat map, check-in, saving and loading flight data |
| Sakshi Kumari | 26K-0596 | Input checks, passengers, bookings, fare calculation, saving and loading passenger and booking data |


## More details

- [Project plan and weekly schedule](docs/PROJECT_PLAN.md)

## Progress

Boxes are ticked when a feature is finished.

- [ ] Main menu
- [ ] Flights
- [ ] Passengers
- [ ] Seat selection
- [ ] Booking, cancellation, rescheduling
- [ ] Fare calculation
- [ ] File handling
- [ ] Check-in
