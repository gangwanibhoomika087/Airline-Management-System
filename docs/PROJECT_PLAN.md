# Airline Management System: Project Plan

**Team:** Bhoomika Gangwani (26K-0589) and Sakshi Kumari (26K-0596)
**Course:** Programming Fundamentals, Fall 2026

## 1. Goal

Build a console-based airline ticketing system in C. A user can manage flights and passengers, book a seat with extras (meal, baggage, pet), see the total fare, cancel or reschedule, and have everything saved between runs.

## 2. How We Will Build It

The project grows together with the course. Each week we use only the topics already taught. Early versions use simple arrays and menus. Later we replace them with structs, dynamic memory and files. If a topic has not been taught yet, that task moves to the following week.

Both of us write our own code, commit from our own GitHub account, and can explain every part we wrote.

## 3. Code Layout and Ownership

```
Airline-Management-System/
├── src/      all .c and .h files listed below
├── data/     saved .txt files
├── docs/     PROJECT_PLAN.md
├── README.md
└── .gitignore
```

| File | Owner | What it does |
|---|---|---|
| `main.c` | Bhoomika | Menu loop, program start and exit |
| `airline.h` | Bhoomika | Constants, structs, function prototypes |
| `flights.c` | Bhoomika | Add, edit, remove, search, list flights; schedule and status |
| `seats.c` | Bhoomika | Seat map, availability, reserve and free a seat |
| `checkin.c` | Bhoomika | Passenger check-in |
| `flightfiles.c` | Bhoomika | Save and load flights and seats |
| `input.c` | Sakshi | Safe input reading and validation |
| `passengers.c` | Sakshi | Passenger records, staff flag |
| `bookings.c` | Sakshi | Book, cancel, reschedule; reference numbers and search; blocks duplicate bookings |
| `pricing.c` | Sakshi | Class fare, meal, baggage, pet charge, staff benefit, total |
| `bookingfiles.c` | Sakshi | Save and load passengers and bookings |

### Workload balance

| | Bhoomika | Sakshi |
|---|---|---|
| Feature areas | Flights, seats, check-in | Passengers, bookings, pricing |
| Shared base code | Main menu, `airline.h` | Input validation |
| Data files | Flights and seats | Passengers and bookings |
| Harder parts | Dynamic seat map, recursive flight search, growing flight list | Fare rules, rescheduling, recursive reference check, growing passenger and booking lists |
| Own documents | Flowchart and test cases for her modules | Flowchart and test cases for her modules |
| Weekly routine | Take turns writing the status message and updating the README progress list | Same |

If one side turns out heavier than expected, we move a task across and note it here.

### Functions we agree to provide to each other

- Bhoomika provides `findFlight(flightNo)`, `seatIsFree(flightNo, seat)`, `takeSeat(flightNo, seat)` and `releaseSeat(flightNo, seat)`.
- Sakshi provides `computeFare(booking)`, `findBooking(reference)`, and the input functions for numbers, text and dates.
- Bookings call the seat functions. Check-in calls `findBooking`. Everyone uses the shared input functions.

## 4. Data Design

- **Date:** day, month, year.
- **Flight:** flight number, from, to, date, departure time, status, seats left, seat map.
- **Passenger:** id, name, phone, passport number, staff flag.
- **Booking:** reference, flight number, passenger id, seat, class, meal, baggage weight, pet flag, total fare, checked-in flag.

Flights, passengers and bookings are kept in lists that grow as records are added.

## 5. Course Topics and Where We Use Them

| Topic | Bhoomika | Sakshi |
|---|---|---|
| `switch` and nested menus | Main menu | Sub-menus for passengers and bookings |
| Loops and 1D arrays | Flight list | Passenger list |
| 2D arrays | Seat grid | Fare table by class |
| Functions, pass by reference | Reserving a seat | Calculating the fare |
| Pointers | Finding a flight record | Finding a passenger or booking record |
| Strings | Searching flights by city | Reference numbers, name checks |
| Recursion | Flight search by number | Reference number check |
| `static` and `const` | Fixed seat layout constants | Booking counter, fixed fare tables |
| Structs and nested structs | Flight, Date | Passenger, Booking |
| Dynamic memory | Flight list, seat map | Passenger and booking lists |
| File handling | Flights and seats | Passengers and bookings |

## 6. Weekly Plan

Week 1 is the week we begin. Tasks are tied to what the class has been taught, so the order may shift.

- [ ] **Week 1:** Bhoomika: main menu with `switch`, add and list flights using arrays. Sakshi: sub-menu stubs with nested `switch`, add and search passengers using arrays.
- [ ] **Week 2:** Bhoomika: seat grid as a 2D array, display it and pick a seat. Sakshi: fare table as a 2D array, booking summary screen.
- [ ] **Week 3:** Bhoomika: move flights and seats into their own files, reserve and free seats using pointers. Sakshi: `input.c` with validated input, fare function.
- [ ] **Week 4:** Bhoomika: search flights by city with string functions, recursive search by flight number. Sakshi: `static` booking counter, reference numbers, recursive reference check.
- [ ] **Week 5:** Bhoomika: Flight and Date structs, growing flight list, dynamic seat map. Sakshi: Passenger and Booking structs, growing lists, cancellation that frees the seat.
- [ ] **Week 6:** Bhoomika: save and load flights and seats, check-in, flight status. Sakshi: save and load passengers and bookings (restore the booking counter from the highest saved reference when the program starts), reschedule, staff benefit.
- [ ] **Week 7:** Both: test every feature, fix bugs, final README, practise explaining our own code.

## 7. Working Rules

- Each of us commits at least once a week from our own GitHub account.
- Pull before starting work and push when done.
- Each of us edits only our own files. `airline.h` is shared: add your structs there in a small separate commit and tell the other person before pushing.
- One feature per commit, with a clear message.
- The weekly status goes to the instructor with the repo link and the features done.
- If we fall behind, we drop optional extras first (pet transport, staff benefits) and keep the core: flights, passengers, booking, seats, fares and saving.
