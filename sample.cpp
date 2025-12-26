#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;

// ========== CONSTANTS ==========
const int MAX_PASSENGERS = 100;
const int MAX_FLIGHTS = 50;
const int MAX_BOOKINGS = 200;
const int MAX_ADMINS = 5;

// ========== UTILITY FUNCTIONS ==========
string intToString(int num) {
    stringstream ss;
    ss << num;
    return ss.str();
}

string floatToString(float num, int precision = 2) {
    stringstream ss;
    ss << fixed << setprecision(precision) << num;
    return ss.str();
}

// ========== STRUCTURES ==========
struct Date {
    int day;
    int month;
    int year;
};

struct Time {
    int hour;
    int minute;
};

struct Passenger {
    int id;
    char name[50];
    char password[30];
    char email[50];
    char phone[15];
    int totalBookings;
    float totalSpent;
};

struct Flight {
    int flightNo;
    char destination[50];
    char origin[50];
    Date departureDate;
    Time departureTime;
    Date arrivalDate;
    Time arrivalTime;
    int totalSeats;
    int availableSeats;
    float baseFare;
    float distance; // in km
    char status[20]; // Available, Full, Delayed, Canceled
    int timesBooked;
    float totalRevenue;
};

struct Booking {
    int bookingId;
    int passengerId;
    int flightNo;
    Date bookingDate;
    Date travelDate;
    int seatsBooked;
    char classType[20]; // Economy, Business, First
    float farePaid;
    char status[20]; // Confirmed, Cancelled, Completed
};

struct Admin {
    int id;
    char username[30];
    char password[30];
    char email[50];
};

// ========== GLOBAL VARIABLES ==========
Passenger passengers[MAX_PASSENGERS];
Flight flights[MAX_FLIGHTS];
Booking bookings[MAX_BOOKINGS];
Admin admins[MAX_ADMINS];

int passengerCount = 0;
int flightCount = 0;
int bookingCount = 0;
int adminCount = 0;
int currentPassengerId = -1;
int currentAdminId = -1;

// ========== UTILITY FUNCTIONS ==========
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void printHeader(const string& title) {
    cout << "\n========================================\n";
    cout << "    " << title << "\n";
    cout << "========================================\n";
}

void printSeparator() {
    cout << "----------------------------------------\n";
}

int getValidInteger(const string& prompt, int min = 0, int max = INT_MAX) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore();
            return value;
        }
        cout << "Invalid input! Please enter a number between " << min << " and " << max << ".\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

float getValidFloat(const string& prompt, float min = 0) {
    float value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min) {
            cin.ignore();
            return value;
        }
        cout << "Invalid input! Please enter a valid number.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

void getStringInput(const string& prompt, char* buffer, int size) {
    cout << prompt;
    cin.getline(buffer, size);
    while (strlen(buffer) == 0) {
        cout << "Input cannot be empty! " << prompt;
        cin.getline(buffer, size);
    }
}

void getDateInput(Date& date, const string& prompt) {
    cout << prompt << " (DD MM YYYY): ";
    cin >> date.day >> date.month >> date.year;
    cin.ignore();
}

void getTimeInput(Time& time, const string& prompt) {
    cout << prompt << " (HH MM, 24-hour format): ";
    cin >> time.hour >> time.minute;
    cin.ignore();
}

bool isValidDate(const Date& date) {
    if (date.year < 2023 || date.year > 2030) return false;
    if (date.month < 1 || date.month > 12) return false;
    
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    if (date.month == 2) {
        bool isLeap = (date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0);
        if (isLeap) daysInMonth[1] = 29;
    }
    
    return date.day >= 1 && date.day <= daysInMonth[date.month - 1];
}

bool isFutureDate(const Date& date) {
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    
    int currentYear = currentTime->tm_year + 1900;
    int currentMonth = currentTime->tm_mon + 1;
    int currentDay = currentTime->tm_mday;
    
    if (date.year > currentYear) return true;
    if (date.year == currentYear && date.month > currentMonth) return true;
    if (date.year == currentYear && date.month == currentMonth && date.day > currentDay) return true;
    
    return false;
}

int generateBookingId() {
    static int lastId = 1000;
    return ++lastId;
}

float calculateFare(const Flight& flight, int seats, const string& classType) {
    float multiplier;
    
    if (classType == "Economy") multiplier = 1.0;
    else if (classType == "Business") multiplier = 2.0;
    else if (classType == "First") multiplier = 3.5;
    else multiplier = 1.0;
    
    // Base fare + distance factor
    float fare = flight.baseFare * seats * multiplier;
    
    // Add distance charge (0.5 per km)
    fare += flight.distance * 0.5 * seats;
    
    return fare;
}

float calculateRefund(const Booking& booking, const Date& currentDate) {
    int daysBefore = (booking.travelDate.year - currentDate.year) * 365 +
                    (booking.travelDate.month - currentDate.month) * 30 +
                    (booking.travelDate.day - currentDate.day);
    
    if (daysBefore >= 7) return booking.farePaid * 0.9; // 90% refund
    else if (daysBefore >= 3) return booking.farePaid * 0.5; // 50% refund
    else if (daysBefore >= 1) return booking.farePaid * 0.2; // 20% refund
    else return 0; // No refund
}

// ========== INITIALIZATION ==========
void initializeSystem() {
    // Initialize some default admins
    adminCount = 2;
    admins[0] = {1, "admin1", "password123", "admin1@airline.com"};
    admins[1] = {2, "admin2", "admin123", "admin2@airline.com"};
    
    // Initialize some default passengers
    passengerCount = 3;
    strcpy(passengers[0].name, "John Doe");
    strcpy(passengers[0].password, "john123");
    strcpy(passengers[0].email, "john@email.com");
    strcpy(passengers[0].phone, "1234567890");
    passengers[0].id = 1001;
    passengers[0].totalBookings = 0;
    passengers[0].totalSpent = 0;
    
    strcpy(passengers[1].name, "Jane Smith");
    strcpy(passengers[1].password, "jane456");
    strcpy(passengers[1].email, "jane@email.com");
    strcpy(passengers[1].phone, "9876543210");
    passengers[1].id = 1002;
    passengers[1].totalBookings = 0;
    passengers[1].totalSpent = 0;
    
    strcpy(passengers[2].name, "Bob Wilson");
    strcpy(passengers[2].password, "bob789");
    strcpy(passengers[2].email, "bob@email.com");
    strcpy(passengers[2].phone, "5551234567");
    passengers[2].id = 1003;
    passengers[2].totalBookings = 0;
    passengers[2].totalSpent = 0;
    
    // Initialize some default flights
    flightCount = 5;
    
    // Flight 1
    flights[0].flightNo = 101;
    strcpy(flights[0].destination, "New York");
    strcpy(flights[0].origin, "Los Angeles");
    flights[0].departureDate = {15, 12, 2023};
    flights[0].departureTime = {8, 0};
    flights[0].arrivalDate = {15, 12, 2023};
    flights[0].arrivalTime = {16, 30};
    flights[0].totalSeats = 200;
    flights[0].availableSeats = 180;
    flights[0].baseFare = 300.0;
    flights[0].distance = 4000.0;
    strcpy(flights[0].status, "Available");
    flights[0].timesBooked = 0;
    flights[0].totalRevenue = 0;
    
    // Flight 2
    flights[1].flightNo = 102;
    strcpy(flights[1].destination, "London");
    strcpy(flights[1].origin, "Paris");
    flights[1].departureDate = {16, 12, 2023};
    flights[1].departureTime = {10, 30};
    flights[1].arrivalDate = {16, 12, 2023};
    flights[1].arrivalTime = {12, 0};
    flights[1].totalSeats = 150;
    flights[1].availableSeats = 150;
    flights[1].baseFare = 250.0;
    flights[1].distance = 350.0;
    strcpy(flights[1].status, "Available");
    flights[1].timesBooked = 0;
    flights[1].totalRevenue = 0;
    
    // Flight 3
    flights[2].flightNo = 103;
    strcpy(flights[2].destination, "Tokyo");
    strcpy(flights[2].origin, "Sydney");
    flights[2].departureDate = {17, 12, 2023};
    flights[2].departureTime = {14, 0};
    flights[2].arrivalDate = {17, 12, 2023};
    flights[2].arrivalTime = {22, 0};
    flights[2].totalSeats = 180;
    flights[2].availableSeats = 120;
    flights[2].baseFare = 500.0;
    flights[2].distance = 7800.0;
    strcpy(flights[2].status, "Available");
    flights[2].timesBooked = 0;
    flights[2].totalRevenue = 0;
    
    // Flight 4
    flights[3].flightNo = 104;
    strcpy(flights[3].destination, "Dubai");
    strcpy(flights[3].origin, "Singapore");
    flights[3].departureDate = {18, 12, 2023};
    flights[3].departureTime = {9, 15};
    flights[3].arrivalDate = {18, 12, 2023};
    flights[3].arrivalTime = {15, 45};
    flights[3].totalSeats = 220;
    flights[3].availableSeats = 220;
    flights[3].baseFare = 400.0;
    flights[3].distance = 3800.0;
    strcpy(flights[3].status, "Available");
    flights[3].timesBooked = 0;
    flights[3].totalRevenue = 0;
    
    // Flight 5
    flights[4].flightNo = 105;
    strcpy(flights[4].destination, "Frankfurt");
    strcpy(flights[4].origin, "Beijing");
    flights[4].departureDate = {19, 12, 2023};
    flights[4].departureTime = {11, 30};
    flights[4].arrivalDate = {19, 12, 2023};
    flights[4].arrivalTime = {4, 0};
    flights[4].totalSeats = 170;
    flights[4].availableSeats = 100;
    flights[4].baseFare = 600.0;
    flights[4].distance = 8200.0;
    strcpy(flights[4].status, "Available");
    flights[4].timesBooked = 0;
    flights[4].totalRevenue = 0;
}

// ========== AUTHENTICATION ==========
bool passengerLogin() {
    clearScreen();
    printHeader("PASSENGER LOGIN");
    
    int id;
    char password[30];
    
    id = getValidInteger("Enter Passenger ID: ", 1000, 9999);
    getStringInput("Enter Password: ", password, 30);
    
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == id && strcmp(passengers[i].password, password) == 0) {
            currentPassengerId = id;
            cout << "\n✓ Login successful! Welcome " << passengers[i].name << "!\n";
            pauseScreen();
            return true;
        }
    }
    
    cout << "\n✗ Invalid credentials! Please try again.\n";
    pauseScreen();
    return false;
}

bool adminLogin() {
    clearScreen();
    printHeader("ADMIN LOGIN");
    
    char username[30];
    char password[30];
    
    getStringInput("Enter Username: ", username, 30);
    getStringInput("Enter Password: ", password, 30);
    
    for (int i = 0; i < adminCount; i++) {
        if (strcmp(admins[i].username, username) == 0 && 
            strcmp(admins[i].password, password) == 0) {
            currentAdminId = admins[i].id;
            cout << "\n✓ Login successful! Welcome Admin " << admins[i].username << "!\n";
            pauseScreen();
            return true;
        }
    }
    
    cout << "\n✗ Invalid credentials! Please try again.\n";
    pauseScreen();
    return false;
}

void changeAdminPassword() {
    clearScreen();
    printHeader("CHANGE ADMIN PASSWORD");
    
    char currentPass[30], newPass[30], confirmPass[30];
    
    getStringInput("Enter current password: ", currentPass, 30);
    
    // Find current admin
    for (int i = 0; i < adminCount; i++) {
        if (admins[i].id == currentAdminId) {
            if (strcmp(admins[i].password, currentPass) != 0) {
                cout << "\n✗ Current password is incorrect!\n";
                pauseScreen();
                return;
            }
            
            getStringInput("Enter new password: ", newPass, 30);
            getStringInput("Confirm new password: ", confirmPass, 30);
            
            if (strcmp(newPass, confirmPass) != 0) {
                cout << "\n✗ Passwords do not match!\n";
                pauseScreen();
                return;
            }
            
            strcpy(admins[i].password, newPass);
            cout << "\n✓ Password changed successfully!\n";
            pauseScreen();
            return;
        }
    }
}

// ========== PASSENGER FUNCTIONS ==========
void registerNewPassenger() {
    clearScreen();
    printHeader("NEW PASSENGER REGISTRATION");
    
    if (passengerCount >= MAX_PASSENGERS) {
        cout << "✗ Maximum passenger limit reached!\n";
        pauseScreen();
        return;
    }
    
    Passenger newPassenger;
    
    newPassenger.id = 1000 + passengerCount + 1;
    cout << "Your Passenger ID: " << newPassenger.id << " (Remember this for login)\n";
    
    getStringInput("Enter Name: ", newPassenger.name, 50);
    getStringInput("Enter Password: ", newPassenger.password, 30);
    getStringInput("Enter Email: ", newPassenger.email, 50);
    getStringInput("Enter Phone: ", newPassenger.phone, 15);
    newPassenger.totalBookings = 0;
    newPassenger.totalSpent = 0;
    
    passengers[passengerCount++] = newPassenger;
    
    cout << "\n✓ Registration successful! You can now login with ID: " << newPassenger.id << "\n";
    pauseScreen();
}

void viewAvailableFlights() {
    clearScreen();
    printHeader("AVAILABLE FLIGHTS");
    
    bool found = false;
    
    cout << left << setw(10) << "Flight #" 
         << setw(15) << "From" 
         << setw(15) << "To" 
         << setw(15) << "Departure" 
         << setw(15) << "Arrival" 
         << setw(10) << "Seats" 
         << setw(10) << "Fare" 
         << setw(12) << "Status" << "\n";
    printSeparator();
    
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].status, "Available") == 0 && flights[i].availableSeats > 0) {
            found = true;
            
            // Format departure date/time
            string depTime;
            {
                stringstream ss;
                ss << flights[i].departureTime.hour << ":" 
                   << (flights[i].departureTime.minute < 10 ? "0" : "") 
                   << flights[i].departureTime.minute;
                depTime = ss.str();
            }
            
            string depDate;
            {
                stringstream ss;
                ss << flights[i].departureDate.day << "/"
                   << flights[i].departureDate.month << "/"
                   << flights[i].departureDate.year;
                depDate = ss.str();
            }
            
            // Format arrival date/time
            string arrTime;
            {
                stringstream ss;
                ss << flights[i].arrivalTime.hour << ":" 
                   << (flights[i].arrivalTime.minute < 10 ? "0" : "") 
                   << flights[i].arrivalTime.minute;
                arrTime = ss.str();
            }
            
            string arrDate;
            {
                stringstream ss;
                ss << flights[i].arrivalDate.day << "/"
                   << flights[i].arrivalDate.month << "/"
                   << flights[i].arrivalDate.year;
                arrDate = ss.str();
            }
            
            string fareStr = "$" + intToString((int)flights[i].baseFare);
            
            cout << left << setw(10) << flights[i].flightNo
                 << setw(15) << flights[i].origin
                 << setw(15) << flights[i].destination
                 << setw(15) << (depDate + " " + depTime)
                 << setw(15) << (arrDate + " " + arrTime)
                 << setw(10) << flights[i].availableSeats
                 << setw(10) << fareStr
                 << setw(12) << flights[i].status << "\n";
        }
    }
    
    if (!found) {
        cout << "No available flights at the moment.\n";
    }
    
    pauseScreen();
}

void bookFlight() {
    clearScreen();
    printHeader("BOOK A FLIGHT");
    
    // Show available flights
    viewAvailableFlights();
    
    int flightChoice = getValidInteger("\nEnter Flight Number to book (0 to cancel): ", 0, 999);
    if (flightChoice == 0) return;
    
    // Find the flight
    Flight* selectedFlight = nullptr;
    int flightIndex = -1;
    
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightNo == flightChoice && 
            strcmp(flights[i].status, "Available") == 0 &&
            flights[i].availableSeats > 0) {
            selectedFlight = &flights[i];
            flightIndex = i;
            break;
        }
    }
    
    if (!selectedFlight) {
        cout << "\n✗ Invalid flight selection or flight not available!\n";
        pauseScreen();
        return;
    }
    
    // Get booking details
    int seats = getValidInteger("Number of seats to book: ", 1, selectedFlight->availableSeats);
    
    cout << "\nSelect Class Type:\n";
    cout << "1. Economy (1.0x)\n";
    cout << "2. Business (2.0x)\n";
    cout << "3. First Class (3.5x)\n";
    int classChoice = getValidInteger("Enter choice (1-3): ", 1, 3);
    
    string classType;
    switch(classChoice) {
        case 1: classType = "Economy"; break;
        case 2: classType = "Business"; break;
        case 3: classType = "First"; break;
    }
    
    Date travelDate;
    getDateInput(travelDate, "Enter travel date");
    
    // Validate travel date
    if (!isValidDate(travelDate)) {
        cout << "\n✗ Invalid date!\n";
        pauseScreen();
        return;
    }
    
    if (!isFutureDate(travelDate)) {
        cout << "\n✗ Travel date must be in the future!\n";
        pauseScreen();
        return;
    }
    
    // Calculate fare
    float fare = calculateFare(*selectedFlight, seats, classType);
    
    // Confirm booking
    cout << "\n=== BOOKING SUMMARY ===\n";
    cout << "Flight: " << selectedFlight->origin << " to " << selectedFlight->destination << "\n";
    {
        stringstream ss;
        ss << travelDate.day << "/" << travelDate.month << "/" << travelDate.year;
        cout << "Date: " << ss.str() << "\n";
    }
    cout << "Seats: " << seats << " (" << classType << " class)\n";
    cout << "Total Fare: $" << fixed << setprecision(2) << fare << "\n";
    
    char confirm;
    cout << "\nConfirm booking? (Y/N): ";
    cin >> confirm;
    cin.ignore();
    
    if (toupper(confirm) != 'Y') {
        cout << "\nBooking cancelled.\n";
        pauseScreen();
        return;
    }
    
    // Create booking record
    Booking newBooking;
    newBooking.bookingId = generateBookingId();
    newBooking.passengerId = currentPassengerId;
    newBooking.flightNo = flightChoice;
    
    // Set current date as booking date
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    newBooking.bookingDate.day = currentTime->tm_mday;
    newBooking.bookingDate.month = currentTime->tm_mon + 1;
    newBooking.bookingDate.year = currentTime->tm_year + 1900;
    
    newBooking.travelDate = travelDate;
    newBooking.seatsBooked = seats;
    strcpy(newBooking.classType, classType.c_str());
    newBooking.farePaid = fare;
    strcpy(newBooking.status, "Confirmed");
    
    // Add booking to array
    bookings[bookingCount++] = newBooking;
    
    // Update flight
    flights[flightIndex].availableSeats -= seats;
    flights[flightIndex].timesBooked++;
    flights[flightIndex].totalRevenue += fare;
    
    if (flights[flightIndex].availableSeats == 0) {
        strcpy(flights[flightIndex].status, "Full");
    }
    
    // Update passenger
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == currentPassengerId) {
            passengers[i].totalBookings++;
            passengers[i].totalSpent += fare;
            break;
        }
    }
    
    cout << "\n✓ Booking confirmed! Booking ID: " << newBooking.bookingId << "\n";
    cout << "✓ Receipt generated successfully!\n";
    
    pauseScreen();
}

void cancelBooking() {
    clearScreen();
    printHeader("CANCEL BOOKING");
    
    // Find passenger's bookings
    bool hasBookings = false;
    
    cout << "Your Bookings:\n";
    cout << left << setw(12) << "Booking ID" 
         << setw(10) << "Flight #" 
         << setw(12) << "Travel Date" 
         << setw(8) << "Seats" 
         << setw(10) << "Class" 
         << setw(12) << "Fare Paid" 
         << setw(12) << "Status" << "\n";
    printSeparator();
    
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].passengerId == currentPassengerId && 
            strcmp(bookings[i].status, "Confirmed") == 0) {
            hasBookings = true;
            
            string travelDate;
            {
                stringstream ss;
                ss << bookings[i].travelDate.day << "/"
                   << bookings[i].travelDate.month << "/"
                   << bookings[i].travelDate.year;
                travelDate = ss.str();
            }
            
            string fareStr = "$" + intToString((int)bookings[i].farePaid);
            
            cout << left << setw(12) << bookings[i].bookingId
                 << setw(10) << bookings[i].flightNo
                 << setw(12) << travelDate
                 << setw(8) << bookings[i].seatsBooked
                 << setw(10) << bookings[i].classType
                 << setw(12) << fareStr
                 << setw(12) << bookings[i].status << "\n";
        }
    }
    
    if (!hasBookings) {
        cout << "No active bookings found.\n";
        pauseScreen();
        return;
    }
    
    int bookingId = getValidInteger("\nEnter Booking ID to cancel (0 to go back): ", 0, 9999);
    if (bookingId == 0) return;
    
    // Find the booking
    int bookingIndex = -1;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].bookingId == bookingId && 
            bookings[i].passengerId == currentPassengerId &&
            strcmp(bookings[i].status, "Confirmed") == 0) {
            bookingIndex = i;
            break;
        }
    }
    
    if (bookingIndex == -1) {
        cout << "\n✗ Invalid Booking ID or booking cannot be cancelled!\n";
        pauseScreen();
        return;
    }
    
    Booking& booking = bookings[bookingIndex];
    
    // Calculate refund
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    Date currentDate = {currentTime->tm_mday, currentTime->tm_mon + 1, currentTime->tm_year + 1900};
    
    float refundAmount = calculateRefund(booking, currentDate);
    
    cout << "\n=== CANCELLATION DETAILS ===\n";
    cout << "Booking ID: " << booking.bookingId << "\n";
    cout << "Flight: " << booking.flightNo << "\n";
    cout << "Original Fare: $" << booking.farePaid << "\n";
    cout << "Refund Amount: $" << refundAmount << "\n";
    
    char confirm;
    cout << "\nAre you sure you want to cancel? (Y/N): ";
    cin >> confirm;
    cin.ignore();
    
    if (toupper(confirm) != 'Y') {
        cout << "\nCancellation aborted.\n";
        pauseScreen();
        return;
    }
    
    // Update booking status
    strcpy(booking.status, "Cancelled");
    
    // Update flight seats
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightNo == booking.flightNo) {
            flights[i].availableSeats += booking.seatsBooked;
            if (strcmp(flights[i].status, "Full") == 0) {
                strcpy(flights[i].status, "Available");
            }
            break;
        }
    }
    
    // Update passenger total spent
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == currentPassengerId) {
            passengers[i].totalSpent -= refundAmount;
            break;
        }
    }
    
    cout << "\n✓ Booking cancelled successfully!\n";
    cout << "✓ Refund of $" << refundAmount << " processed.\n";
    
    pauseScreen();
}

void generatePersonalReport() {
    clearScreen();
    printHeader("PERSONAL BOOKING REPORT");
    
    // Find passenger
    Passenger* currentPassenger = nullptr;
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == currentPassengerId) {
            currentPassenger = &passengers[i];
            break;
        }
    }
    
    if (!currentPassenger) {
        cout << "Passenger not found!\n";
        pauseScreen();
        return;
    }
    
    cout << "Passenger: " << currentPassenger->name << "\n";
    cout << "ID: " << currentPassenger->id << "\n";
    cout << "Email: " << currentPassenger->email << "\n";
    cout << "Phone: " << currentPassenger->phone << "\n";
    printSeparator();
    
    cout << "\n=== BOOKING HISTORY ===\n";
    cout << left << setw(12) << "Booking ID" 
         << setw(10) << "Flight #" 
         << setw(12) << "Booking Date" 
         << setw(12) << "Travel Date" 
         << setw(8) << "Seats" 
         << setw(10) << "Class" 
         << setw(12) << "Fare" 
         << setw(12) << "Status" << "\n";
    printSeparator();
    
    float totalSpent = 0;
    int totalBookings = 0;
    
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].passengerId == currentPassengerId) {
            totalBookings++;
            totalSpent += bookings[i].farePaid;
            
            string bookingDate;
            {
                stringstream ss;
                ss << bookings[i].bookingDate.day << "/"
                   << bookings[i].bookingDate.month << "/"
                   << bookings[i].bookingDate.year;
                bookingDate = ss.str();
            }
            
            string travelDate;
            {
                stringstream ss;
                ss << bookings[i].travelDate.day << "/"
                   << bookings[i].travelDate.month << "/"
                   << bookings[i].travelDate.year;
                travelDate = ss.str();
            }
            
            string fareStr = "$" + intToString((int)bookings[i].farePaid);
            
            cout << left << setw(12) << bookings[i].bookingId
                 << setw(10) << bookings[i].flightNo
                 << setw(12) << bookingDate
                 << setw(12) << travelDate
                 << setw(8) << bookings[i].seatsBooked
                 << setw(10) << bookings[i].classType
                 << setw(12) << fareStr
                 << setw(12) << bookings[i].status << "\n";
        }
    }
    
    cout << "\n=== SUMMARY ===\n";
    cout << "Total Bookings: " << totalBookings << "\n";
    cout << "Total Amount Spent: $" << fixed << setprecision(2) << totalSpent << "\n";
    
    pauseScreen();
}

void updateProfile() {
    clearScreen();
    printHeader("UPDATE PROFILE");
    
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == currentPassengerId) {
            cout << "Current Information:\n";
            cout << "1. Name: " << passengers[i].name << "\n";
            cout << "2. Email: " << passengers[i].email << "\n";
            cout << "3. Phone: " << passengers[i].phone << "\n";
            cout << "4. Password: ********\n";
            printSeparator();
            
            int choice = getValidInteger("\nSelect field to update (1-4, 0 to cancel): ", 0, 4);
            
            if (choice == 0) return;
            
            switch(choice) {
                case 1:
                    getStringInput("Enter new name: ", passengers[i].name, 50);
                    break;
                case 2:
                    getStringInput("Enter new email: ", passengers[i].email, 50);
                    break;
                case 3:
                    getStringInput("Enter new phone: ", passengers[i].phone, 15);
                    break;
                case 4:
                    getStringInput("Enter new password: ", passengers[i].password, 30);
                    break;
            }
            
            cout << "\n✓ Profile updated successfully!\n";
            pauseScreen();
            return;
        }
    }
}

// ========== ADMIN FUNCTIONS ==========
void viewAllPassengers() {
    clearScreen();
    printHeader("ALL PASSENGERS");
    
    cout << left << setw(10) << "ID" 
         << setw(20) << "Name" 
         << setw(25) << "Email" 
         << setw(15) << "Phone" 
         << setw(10) << "Bookings" 
         << setw(12) << "Total Spent" << "\n";
    printSeparator();
    
    for (int i = 0; i < passengerCount; i++) {
        string spentStr = "$" + intToString((int)passengers[i].totalSpent);
        
        cout << left << setw(10) << passengers[i].id
             << setw(20) << passengers[i].name
             << setw(25) << passengers[i].email
             << setw(15) << passengers[i].phone
             << setw(10) << passengers[i].totalBookings
             << setw(12) << spentStr << "\n";
    }
    
    cout << "\nTotal Passengers: " << passengerCount << "\n";
    pauseScreen();
}

void addNewPassenger() {
    clearScreen();
    printHeader("ADD NEW PASSENGER");
    
    if (passengerCount >= MAX_PASSENGERS) {
        cout << "✗ Maximum passenger limit reached!\n";
        pauseScreen();
        return;
    }
    
    Passenger newPassenger;
    
    newPassenger.id = getValidInteger("Enter Passenger ID (1000-9999): ", 1000, 9999);
    
    // Check if ID already exists
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == newPassenger.id) {
            cout << "✗ Passenger ID already exists!\n";
            pauseScreen();
            return;
        }
    }
    
    getStringInput("Enter Name: ", newPassenger.name, 50);
    getStringInput("Enter Password: ", newPassenger.password, 30);
    getStringInput("Enter Email: ", newPassenger.email, 50);
    getStringInput("Enter Phone: ", newPassenger.phone, 15);
    newPassenger.totalBookings = 0;
    newPassenger.totalSpent = 0;
    
    passengers[passengerCount++] = newPassenger;
    
    cout << "\n✓ New passenger added successfully!\n";
    pauseScreen();
}

void removePassenger() {
    clearScreen();
    printHeader("REMOVE PASSENGER");
    
    viewAllPassengers();
    
    int id = getValidInteger("\nEnter Passenger ID to remove (0 to cancel): ", 0, 9999);
    if (id == 0) return;
    
    // Find passenger
    int index = -1;
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "\n✗ Passenger not found!\n";
        pauseScreen();
        return;
    }
    
    // Check if passenger has active bookings
    bool hasActiveBookings = false;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].passengerId == id && strcmp(bookings[i].status, "Confirmed") == 0) {
            hasActiveBookings = true;
            break;
        }
    }
    
    if (hasActiveBookings) {
        cout << "\n✗ Cannot remove passenger with active bookings!\n";
        pauseScreen();
        return;
    }
    
    char confirm;
    cout << "\nAre you sure you want to remove " << passengers[index].name << "? (Y/N): ";
    cin >> confirm;
    cin.ignore();
    
    if (toupper(confirm) != 'Y') {
        cout << "\nRemoval cancelled.\n";
        pauseScreen();
        return;
    }
    
    // Shift array to remove passenger
    for (int i = index; i < passengerCount - 1; i++) {
        passengers[i] = passengers[i + 1];
    }
    passengerCount--;
    
    cout << "\n✓ Passenger removed successfully!\n";
    pauseScreen();
}

void updatePassengerData() {
    clearScreen();
    printHeader("UPDATE PASSENGER DATA");
    
    viewAllPassengers();
    
    int id = getValidInteger("\nEnter Passenger ID to update (0 to cancel): ", 0, 9999);
    if (id == 0) return;
    
    // Find passenger
    int index = -1;
    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "\n✗ Passenger not found!\n";
        pauseScreen();
        return;
    }
    
    cout << "\nCurrent Information:\n";
    cout << "1. Name: " << passengers[index].name << "\n";
    cout << "2. Email: " << passengers[index].email << "\n";
    cout << "3. Phone: " << passengers[index].phone << "\n";
    cout << "4. Password: ********\n";
    printSeparator();
    
    int choice = getValidInteger("\nSelect field to update (1-4, 0 to cancel): ", 0, 4);
    
    if (choice == 0) return;
    
    switch(choice) {
        case 1:
            getStringInput("Enter new name: ", passengers[index].name, 50);
            break;
        case 2:
            getStringInput("Enter new email: ", passengers[index].email, 50);
            break;
        case 3:
            getStringInput("Enter new phone: ", passengers[index].phone, 15);
            break;
        case 4:
            getStringInput("Enter new password: ", passengers[index].password, 30);
            break;
    }
    
    cout << "\n✓ Passenger data updated successfully!\n";
    pauseScreen();
}

void viewAllFlightsAdmin() {
    clearScreen();
    printHeader("ALL FLIGHTS - ADMIN VIEW");
    
    cout << left << setw(10) << "Flight #" 
         << setw(15) << "From" 
         << setw(15) << "To" 
         << setw(20) << "Departure" 
         << setw(20) << "Arrival" 
         << setw(10) << "Total" 
         << setw(10) << "Available" 
         << setw(10) << "Fare" 
         << setw(12) << "Status" 
         << setw(10) << "Bookings" 
         << setw(12) << "Revenue" << "\n";
    printSeparator();
    
    for (int i = 0; i < flightCount; i++) {
        string depTime;
        {
            stringstream ss;
            ss << flights[i].departureTime.hour << ":" 
               << (flights[i].departureTime.minute < 10 ? "0" : "") 
               << flights[i].departureTime.minute;
            depTime = ss.str();
        }
        
        string depDate;
        {
            stringstream ss;
            ss << flights[i].departureDate.day << "/"
               << flights[i].departureDate.month << "/"
               << flights[i].departureDate.year;
            depDate = ss.str();
        }
        
        string arrTime;
        {
            stringstream ss;
            ss << flights[i].arrivalTime.hour << ":" 
               << (flights[i].arrivalTime.minute < 10 ? "0" : "") 
               << flights[i].arrivalTime.minute;
            arrTime = ss.str();
        }
        
        string arrDate;
        {
            stringstream ss;
            ss << flights[i].arrivalDate.day << "/"
               << flights[i].arrivalDate.month << "/"
               << flights[i].arrivalDate.year;
            arrDate = ss.str();
        }
        
        string fareStr = "$" + intToString((int)flights[i].baseFare);
        string revenueStr = "$" + intToString((int)flights[i].totalRevenue);
        
        cout << left << setw(10) << flights[i].flightNo
             << setw(15) << flights[i].origin
             << setw(15) << flights[i].destination
             << setw(20) << (depDate + " " + depTime)
             << setw(20) << (arrDate + " " + arrTime)
             << setw(10) << flights[i].totalSeats
             << setw(10) << flights[i].availableSeats
             << setw(10) << fareStr
             << setw(12) << flights[i].status
             << setw(10) << flights[i].timesBooked
             << setw(12) << revenueStr << "\n";
    }
    
    cout << "\nTotal Flights: " << flightCount << "\n";
    pauseScreen();
}

void addNewFlight() {
    clearScreen();
    printHeader("ADD NEW FLIGHT");
    
    if (flightCount >= MAX_FLIGHTS) {
        cout << "✗ Maximum flight limit reached!\n";
        pauseScreen();
        return;
    }
    
    Flight newFlight;
    
    newFlight.flightNo = getValidInteger("Enter Flight Number (100-999): ", 100, 999);
    
    // Check if flight number exists
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightNo == newFlight.flightNo) {
            cout << "✗ Flight number already exists!\n";
            pauseScreen();
            return;
        }
    }
    
    getStringInput("Enter Origin: ", newFlight.origin, 50);
    getStringInput("Enter Destination: ", newFlight.destination, 50);
    
    cout << "\n=== DEPARTURE DETAILS ===\n";
    getDateInput(newFlight.departureDate, "Enter departure date");
    getTimeInput(newFlight.departureTime, "Enter departure time");
    
    cout << "\n=== ARRIVAL DETAILS ===\n";
    getDateInput(newFlight.arrivalDate, "Enter arrival date");
    getTimeInput(newFlight.arrivalTime, "Enter arrival time");
    
    newFlight.totalSeats = getValidInteger("Enter total seats: ", 1, 500);
    newFlight.availableSeats = newFlight.totalSeats;
    newFlight.baseFare = getValidFloat("Enter base fare: $", 0);
    newFlight.distance = getValidFloat("Enter distance (km): ", 0);
    strcpy(newFlight.status, "Available");
    newFlight.timesBooked = 0;
    newFlight.totalRevenue = 0;
    
    // Validate dates
    if (!isValidDate(newFlight.departureDate) || !isValidDate(newFlight.arrivalDate)) {
        cout << "\n✗ Invalid date(s)!\n";
        pauseScreen();
        return;
    }
    
    // Validate departure before arrival
    if (newFlight.departureDate.year > newFlight.arrivalDate.year ||
       (newFlight.departureDate.year == newFlight.arrivalDate.year && 
        newFlight.departureDate.month > newFlight.arrivalDate.month) ||
       (newFlight.departureDate.year == newFlight.arrivalDate.year && 
        newFlight.departureDate.month == newFlight.arrivalDate.month && 
        newFlight.departureDate.day > newFlight.arrivalDate.day)) {
        cout << "\n✗ Departure must be before arrival!\n";
        pauseScreen();
        return;
    }
    
    flights[flightCount++] = newFlight;
    
    cout << "\n✓ New flight added successfully!\n";
    pauseScreen();
}

void removeFlight() {
    clearScreen();
    printHeader("REMOVE FLIGHT");
    
    viewAllFlightsAdmin();
    
    int flightNo = getValidInteger("\nEnter Flight Number to remove (0 to cancel): ", 0, 999);
    if (flightNo == 0) return;
    
    // Find flight
    int index = -1;
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightNo == flightNo) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "\n✗ Flight not found!\n";
        pauseScreen();
        return;
    }
    
    // Check if flight has active bookings
    bool hasActiveBookings = false;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].flightNo == flightNo && 
            (strcmp(bookings[i].status, "Confirmed") == 0 || 
             strcmp(bookings[i].status, "Completed") == 0)) {
            hasActiveBookings = true;
            break;
        }
    }
    
    if (hasActiveBookings) {
        cout << "\n✗ Cannot remove flight with active bookings!\n";
        pauseScreen();
        return;
    }
    
    char confirm;
    cout << "\nAre you sure you want to remove flight " << flightNo << "? (Y/N): ";
    cin >> confirm;
    cin.ignore();
    
    if (toupper(confirm) != 'Y') {
        cout << "\nRemoval cancelled.\n";
        pauseScreen();
        return;
    }
    
    // Shift array to remove flight
    for (int i = index; i < flightCount - 1; i++) {
        flights[i] = flights[i + 1];
    }
    flightCount--;
    
    cout << "\n✓ Flight removed successfully!\n";
    pauseScreen();
}

void generatePassengerReportAdmin() {
    clearScreen();
    printHeader("PASSENGER REPORT");
    
    cout << left << setw(10) << "ID" 
         << setw(20) << "Name" 
         << setw(10) << "Bookings" 
         << setw(12) << "Total Spent" << "\n";
    printSeparator();
    
    float totalRevenue = 0;
    int totalBookings = 0;
    
    for (int i = 0; i < passengerCount; i++) {
        string spentStr = "$" + intToString((int)passengers[i].totalSpent);
        
        cout << left << setw(10) << passengers[i].id
             << setw(20) << passengers[i].name
             << setw(10) << passengers[i].totalBookings
             << setw(12) << spentStr << "\n";
        
        totalRevenue += passengers[i].totalSpent;
        totalBookings += passengers[i].totalBookings;
    }
    
    cout << "\n=== SUMMARY ===\n";
    cout << "Total Passengers: " << passengerCount << "\n";
    cout << "Total Bookings: " << totalBookings << "\n";
    cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << "\n";
    
    // Top spender
    if (passengerCount > 0) {
        int maxIndex = 0;
        for (int i = 1; i < passengerCount; i++) {
            if (passengers[i].totalSpent > passengers[maxIndex].totalSpent) {
                maxIndex = i;
            }
        }
        cout << "Top Spender: " << passengers[maxIndex].name 
             << " ($" << passengers[maxIndex].totalSpent << ")\n";
    }
    
    pauseScreen();
}

void generateFlightReport() {
    clearScreen();
    printHeader("FLIGHT INVENTORY REPORT");
    
    cout << left << setw(10) << "Flight #" 
         << setw(15) << "From-To" 
         << setw(10) << "Bookings" 
         << setw(10) << "Capacity" 
         << setw(10) << "Occupied" 
         << setw(12) << "Utilization" 
         << setw(12) << "Revenue" << "\n";
    printSeparator();
    
    float totalRevenue = 0;
    int totalPassengers = 0;
    
    for (int i = 0; i < flightCount; i++) {
        string route = string(flights[i].origin) + "-" + flights[i].destination;
        int occupied = flights[i].totalSeats - flights[i].availableSeats;
        float utilization = (flights[i].totalSeats > 0) ? 
                           (occupied * 100.0 / flights[i].totalSeats) : 0;
        
        string revenueStr = "$" + intToString((int)flights[i].totalRevenue);
        
        cout << left << setw(10) << flights[i].flightNo
             << setw(15) << route
             << setw(10) << flights[i].timesBooked
             << setw(10) << flights[i].totalSeats
             << setw(10) << occupied
             << setw(12) << fixed << setprecision(1) << utilization << "%"
             << setw(12) << revenueStr << "\n";
        
        totalRevenue += flights[i].totalRevenue;
        totalPassengers += occupied;
    }
    
    cout << "\n=== SUMMARY ===\n";
    cout << "Total Flights: " << flightCount << "\n";
    cout << "Total Passengers Carried: " << totalPassengers << "\n";
    cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << "\n";
    
    // Most booked flight
    if (flightCount > 0) {
        int maxIndex = 0;
        for (int i = 1; i < flightCount; i++) {
            if (flights[i].timesBooked > flights[maxIndex].timesBooked) {
                maxIndex = i;
            }
        }
        cout << "Most Booked Flight: " << flights[maxIndex].flightNo 
             << " (" << flights[maxIndex].timesBooked << " bookings)\n";
    }
    
    pauseScreen();
}

void generateUtilizationReport() {
    clearScreen();
    printHeader("UTILIZATION REPORT");
    
    cout << "Utilization by Flight:\n";
    cout << left << setw(10) << "Flight #" 
         << setw(20) << "Route" 
         << setw(15) << "Utilization" 
         << setw(15) << "Status" << "\n";
    printSeparator();
    
    for (int i = 0; i < flightCount; i++) {
        string route = string(flights[i].origin) + " to " + flights[i].destination;
        int occupied = flights[i].totalSeats - flights[i].availableSeats;
        float utilization = (flights[i].totalSeats > 0) ? 
                           (occupied * 100.0 / flights[i].totalSeats) : 0;
        
        string status;
        if (utilization >= 80) status = "High Demand";
        else if (utilization >= 50) status = "Moderate";
        else if (utilization >= 20) status = "Low";
        else status = "Very Low";
        
        cout << left << setw(10) << flights[i].flightNo
             << setw(20) << route
             << setw(15) << fixed << setprecision(1) << utilization << "%"
             << setw(15) << status << "\n";
    }
    
    // Overall statistics
    int totalSeats = 0;
    int occupiedSeats = 0;
    
    for (int i = 0; i < flightCount; i++) {
        totalSeats += flights[i].totalSeats;
        occupiedSeats += (flights[i].totalSeats - flights[i].availableSeats);
    }
    
    float overallUtilization = (totalSeats > 0) ? 
                              (occupiedSeats * 100.0 / totalSeats) : 0;
    
    cout << "\n=== OVERALL STATISTICS ===\n";
    cout << "Total Seats Available: " << totalSeats << "\n";
    cout << "Total Seats Occupied: " << occupiedSeats << "\n";
    cout << "Overall Utilization: " << fixed << setprecision(1) << overallUtilization << "%\n";
    
    pauseScreen();
}

void updateFlightStatus() {
    clearScreen();
    printHeader("UPDATE FLIGHT STATUS");
    
    viewAllFlightsAdmin();
    
    int flightNo = getValidInteger("\nEnter Flight Number to update (0 to cancel): ", 0, 999);
    if (flightNo == 0) return;
    
    // Find flight
    int index = -1;
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightNo == flightNo) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "\n✗ Flight not found!\n";
        pauseScreen();
        return;
    }
    
    cout << "\nCurrent Status: " << flights[index].status << "\n";
    cout << "\nSelect new status:\n";
    cout << "1. Available\n";
    cout << "2. Full\n";
    cout << "3. Delayed\n";
    cout << "4. Maintenance\n";
    cout << "5. Canceled\n";
    
    int choice = getValidInteger("Enter choice (1-5): ", 1, 5);
    
    switch(choice) {
        case 1: strcpy(flights[index].status, "Available"); break;
        case 2: strcpy(flights[index].status, "Full"); break;
        case 3: strcpy(flights[index].status, "Delayed"); break;
        case 4: strcpy(flights[index].status, "Maintenance"); break;
        case 5: strcpy(flights[index].status, "Canceled"); break;
    }
    
    cout << "\n✓ Flight status updated successfully!\n";
    
    // If flight is canceled, free all seats
    if (choice == 5) {
        flights[index].availableSeats = flights[index].totalSeats;
        
        // Cancel all bookings for this flight
        for (int i = 0; i < bookingCount; i++) {
            if (bookings[i].flightNo == flightNo && 
                strcmp(bookings[i].status, "Confirmed") == 0) {
                strcpy(bookings[i].status, "Cancelled");
            }
        }
        
        cout << "✓ All bookings for this flight have been canceled.\n";
    }
    
    pauseScreen();
}

// ========== MENUS ==========
void showPassengerMenu() {
    int choice;
    bool loggedIn = true;
    
    while (loggedIn) {
        clearScreen();
        printHeader("PASSENGER MENU");
        
        cout << "1. View Available Flights\n";
        cout << "2. Book a Flight\n";
        cout << "3. Cancel Booking\n";
        cout << "4. Generate Personal Report\n";
        cout << "5. Update Profile\n";
        cout << "6. Logout\n";
        cout << "\nEnter your choice (1-6): ";
        
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1: viewAvailableFlights(); break;
            case 2: bookFlight(); break;
            case 3: cancelBooking(); break;
            case 4: generatePersonalReport(); break;
            case 5: updateProfile(); break;
            case 6: 
                cout << "\n✓ Logged out successfully!\n";
                loggedIn = false;
                pauseScreen();
                break;
            default:
                cout << "\n✗ Invalid choice! Please try again.\n";
                pauseScreen();
        }
    }
}

void showAdminMenu() {
    int choice;
    bool loggedIn = true;
    
    while (loggedIn) {
        clearScreen();
        printHeader("ADMIN MENU");
        
        cout << "=== PASSENGER MANAGEMENT ===\n";
        cout << "1. View All Passengers\n";
        cout << "2. Add New Passenger\n";
        cout << "3. Remove Passenger\n";
        cout << "4. Update Passenger Data\n";
        
        cout << "\n=== FLIGHT MANAGEMENT ===\n";
        cout << "5. View All Flights\n";
        cout << "6. Add New Flight\n";
        cout << "7. Remove Flight\n";
        cout << "8. Update Flight Status\n";
        
        cout << "\n=== REPORTS ===\n";
        cout << "9. Generate Passenger Report\n";
        cout << "10. Generate Flight Report\n";
        cout << "11. Generate Utilization Report\n";
        
        cout << "\n=== SYSTEM ===\n";
        cout << "12. Change Password\n";
        cout << "13. Logout\n";
        
        cout << "\nEnter your choice (1-13): ";
        
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1: viewAllPassengers(); break;
            case 2: addNewPassenger(); break;
            case 3: removePassenger(); break;
            case 4: updatePassengerData(); break;
            case 5: viewAllFlightsAdmin(); break;
            case 6: addNewFlight(); break;
            case 7: removeFlight(); break;
            case 8: updateFlightStatus(); break;
            case 9: generatePassengerReportAdmin(); break;
            case 10: generateFlightReport(); break;
            case 11: generateUtilizationReport(); break;
            case 12: changeAdminPassword(); break;
            case 13: 
                cout << "\n✓ Logged out successfully!\n";
                loggedIn = false;
                pauseScreen();
                break;
            default:
                cout << "\n✗ Invalid choice! Please try again.\n";
                pauseScreen();
        }
    }
}

void showMainMenu() {
    int choice;
    bool exitSystem = false;
    
    while (!exitSystem) {
        clearScreen();
        printHeader("AIRLINE RESERVATION MANAGEMENT SYSTEM");
        
        cout << "1. Passenger Login\n";
        cout << "2. Passenger Registration\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit System\n";
        cout << "\nEnter your choice (1-4): ";
        
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1:
                if (passengerLogin()) {
                    showPassengerMenu();
                }
                break;
            case 2:
                registerNewPassenger();
                break;
            case 3:
                if (adminLogin()) {
                    showAdminMenu();
                }
                break;
            case 4:
                cout << "\nThank you for using the Airline Reservation System!\n";
                exitSystem = true;
                break;
            default:
                cout << "\n✗ Invalid choice! Please try again.\n";
                pauseScreen();
        }
    }
}

// ========== MAIN FUNCTION ==========
int main() {
    initializeSystem();
    showMainMenu();
    return 0;
}