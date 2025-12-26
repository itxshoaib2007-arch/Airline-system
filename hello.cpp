#include<iostream>
#include<string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <ctime>
using namespace std;

const int MAX_PASSENGERS = 100;
const int MAX_FLIGHTS = 100;
const int MAX_BOOKINGS = 100;
const int MAX_ADMINS = 5;

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
    float distance; 
    char status[20]; 
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
    char classType[20];
    float farePaid;
    char status[20]; 
};

struct Admin {
    int id;
    char username[30];
    char password[30];
    char email[50];
};

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

// Function prototypes
void viewAvailableFlights();
void bookFlight();
void showPassengerMenu();
void passengerLogin();
bool isValidEmail(const char* email);
void PassengerRegistration();
void mainMenu();
bool isValidDate(const Date& date);
bool isFutureDate(const Date& date);
float calculateFare(const Flight& flight, int seats, const string& classType);
int generateBookingId();
void addSampleData();

// Validation of date

bool isValidDate(const Date& date) {
    if (date.day < 1 || date.day > 31){
        return false;
    }
     
    if (date.month < 1 || date.month > 12){
     return false;
    }
    if (date.year < 2024) {
         return false;
    }
   
    return true;
}
// Ensure date of booking is in future

bool isFutureDate(const Date& date) {
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    
    int currentYear = currentTime->tm_year + 1900;
    int currentMonth = currentTime->tm_mon + 1;
    int currentDay = currentTime->tm_mday;
    
    if (date.year < currentYear) {
        return false;
    }
    
    if (date.year == currentYear && date.month < currentMonth){
         return false;
    }
    
    if (date.year == currentYear && date.month == currentMonth && date.day <= currentDay){
        return false;
    } 
    
    return true;
}
// Calculate Total Fare

float calculateFare(const Flight& flight, int seats, const string& classType) {
    float multiplier;
    
    if (classType == "Economy"){
        multiplier = 1.0;
    } 
    else if (classType == "Business"){
        multiplier = 2.0;
    } 
    else if (classType == "First"){
        multiplier = 3.5;
    } 
    else{
        multiplier = 1.0;
    } 
    
    return flight.baseFare * seats * multiplier;
}
//Book Flight

int generateBookingId() {
    //Static ID 
    static int lastId = 1000;
    return ++lastId;
}
//Validation of Email

bool isValidEmail(const char* email) {
    int atCount = 0;
    int dotCount = 0;
    int len = strlen(email);
    
    for (int i = 0; i < len; i++) {
        if (email[i] == '@')
         atCount++;
        if (email[i] == '.')
         dotCount++;
    }
    
    return (atCount == 1 && dotCount >= 1 && len >= 5);
}

// ========== VIEW AVAILABLE FLIGHTS ==========

void viewAvailableFlights() {
    cout << "\n=== AVAILABLE FLIGHTS ===\n";
    
    if (flightCount == 0) {
        cout << "Sorry! No flights available at the moment.\n";
        return;
    }
    
    cout << left << setw(10) << "Flight #" 
         << setw(15) << "From" 
         << setw(15) << "To" 
         << setw(10) << "Seats" 
         << setw(10) << "Fare" 
         << setw(12) << "Status" << "\n";
    
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].status, "Available") == 0 && flights[i].availableSeats > 0) {
          
            cout << left << setw(10) << flights[i].flightNo
                 << setw(15) << flights[i].origin
                 << setw(15) << flights[i].destination
                 << setw(10) << flights[i].availableSeats
                 << setw(10) << "$" << flights[i].baseFare
                 << setw(12) << flights[i].status << "\n";
        }
    }
}

//  BOOK FLIGHT 
void bookFlight() {
    if (currentPassengerId == -1) {
        cout << "You must login first!\n";
        return;
    }
    
    cout << "\n=== BOOK A FLIGHT ===\n";
    viewAvailableFlights();
    
    if (flightCount == 0) {
        cout << "No flights available to book.\n";
        return;
    }
    
    int flightChoice;
    cout << "\nEnter Flight Number to book (0 to cancel): ";
    cin >> flightChoice;
    
    if (flightChoice == 0)
     return;
    
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
        cout << "Invalid flight selection or flight not available!\n";
        return;
    }
    
    // Get number of seats

    int seats;
    cout << "Number of seats to book (1-" << selectedFlight->availableSeats << "): ";
    cin >> seats;
    
    while (seats < 1 || seats > selectedFlight->availableSeats) {
        cout << "Invalid! Enter between 1 and " << selectedFlight->availableSeats << " seats: ";
        cin >> seats;
    }
    
    // Get class type
    cout << "\nSelect Class Type:\n";
    cout << "1. Economy (1.0x)\n";
    cout << "2. Business (2.0x)\n";
    cout << "3. First Class (3.5x)\n";
    
    int classChoice;
    cout << "Enter choice (1-3): ";
    cin >> classChoice;
    
    string classType;
    switch(classChoice) {
        case 1:
        {
            classType = "Economy"; 
            break;
        } 
        case 2:
        {
            classType = "Business";
             break;
        } 
        case 3:
        {
            classType = "First"; 
            break;
        } 
        default: 
            cout << "Invalid choice! Defaulting to Economy.\n";
            classType = "Economy";
            break;
    }
    
    // Get travel date
    
    Date travelDate;
    cout << "\nEnter travel date (DD MM YYYY): ";
    cin >> travelDate.day >> travelDate.month >> travelDate.year;
    
    // Validate date
    if (!isValidDate(travelDate)) {
        cout << "Invalid date!\n";
        return;
    }
    
    if (!isFutureDate(travelDate)) {
        cout << "Travel date must be in the future!\n";
        return;
    }
    
    // Calculate fare
    float fare = calculateFare(*selectedFlight, seats, classType);
    
    // Show booking summary
    cout << "\n=== BOOKING SUMMARY ===\n";
    cout << "Flight: " << selectedFlight->origin << " to " << selectedFlight->destination << "\n";
    cout << "Date: " << travelDate.day << "/" << travelDate.month << "/" << travelDate.year << "\n";
    cout << "Seats: " << seats << " (" << classType << " class)\n";
    cout << "Total Fare: $" << fixed << setprecision(2) << fare << "\n";
    
    char confirm;
    cout << "\nConfirm booking? (Y/N): ";
    cin >> confirm;
    
    // if (toupper(confirm) != 'Y') {
    //     cout << "Booking cancelled.\n";
    //     return;
    // }

     if (confirm != 'Y') {
        cout << "Booking cancelled.\n";
        return;
    }
    
    // Create booking

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
    flights[flightIndex].availableSeats -= seats;   // Minus seat
    flights[flightIndex].timesBooked++;
    flights[flightIndex].totalRevenue += fare;  //Calculate revnue
     //If flight is full
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
    
    cout << "\nBooking confirmed! Booking ID: " << newBooking.bookingId << "\n";
    cout << "Receipt generated successfully!\n";
}

// ========== PASSENGER MENU ==========
void showPassengerMenu() {
    int choice;
    bool loggedIn = true;
    
    while (loggedIn) {
        cout << "\n=== PASSENGER MENU ===\n";
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
            case 1:
            {
                viewAvailableFlights(); 
                break;
            }  
            case 2:
            { 
                bookFlight(); 
                break;
            }
            case 3:
            {
                cout << "Cancel Booking feature coming soon!\n";
                break;
            } 
            case 4: 
                {cout << "Generate Report feature coming soon!\n";
                break;
            }
            case 5: 
               { cout << "Update Profile feature coming soon!\n";
                break;
            }
            case 6: 
               { cout << "Logged out successfully!\n";
                loggedIn = false;
                currentPassengerId = -1;
                break;
            }
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
}

// ========== PASSENGER LOGIN ==========
void passengerLogin() {
    cout << "\n=== PASSENGER LOGIN ===\n";
    
    int id;
    char password[30];
    
    cout << "Enter Passenger ID: ";
    cin >> id;
    cin.ignore();
    
    cout << "Enter Password: ";
    cin.getline(password, 30);
cout<<endl;
    bool login = false;

    for (int i = 0; i < passengerCount; i++) {
        if (passengers[i].id == id && strcmp(passengers[i].password, password) == 0) {
            currentPassengerId = id;
            cout << "\nLogin successful! Welcome " << passengers[i].name << "!\n";
            login = true;
            showPassengerMenu();
            return;
        }
    }
    
    if (!login) {
        cout << "Invalid credentials! Please try again.\n";
    }
}

// ========== PASSENGER REGISTRATION ==========
void PassengerRegistration() {
    if (passengerCount >= MAX_PASSENGERS) {
        cout << "Maximum passenger limit reached! Cannot register more passengers.\n";
        return;
    }

    Passenger newPassenger;
    
    cout << "\n=== PASSENGER REGISTRATION ===\n";
    
    // Auto-generate ID
    newPassenger.id = 1000 + passengerCount + 1;
    cout << "Your Passenger ID: " << newPassenger.id << " (Remember this for login)\n";
    
    // Get name
    cout << "Enter your name: ";
    cin.ignore();
    cin.getline(newPassenger.name, 50);
    
    // Get password
    cout << "Enter your Password: ";
    cin.getline(newPassenger.password, 30);
    
    // Get email with validation
    char email[50];
    while (true) {
        cout << "Enter E-mail: ";
        cin.getline(email, 50);
        if (isValidEmail(email)) {
            strcpy(newPassenger.email, email);
            break;
        } else {
            cout << "Invalid email format! Please enter a valid email (e.g., user@domain.com)\n";
        }
    }
    
    // Get phone
    cout << "Enter Phone Number: ";
    cin.getline(newPassenger.phone, 15);
    
    // Initialize other fields
    newPassenger.totalBookings = 0;
    newPassenger.totalSpent = 0.0;
    
    // Add to array
    passengers[passengerCount] = newPassenger;
    passengerCount++;
    
    cout << "\nRegistration successful!\n";


    cout << "\n=== REGISTRATION DETAILS ===\n";
    cout << "Passenger ID: " << newPassenger.id << "\n";
    cout << "Name: " << newPassenger.name << "\n";
    cout << "Email: " << newPassenger.email << "\n";
    cout << "Phone: " << newPassenger.phone << "\n";
    cout << "\nIMPORTANT: Save your Passenger ID for login: " << newPassenger.id << "\n";
}

// ========== MAIN MENU ==========
void mainMenu() {
    int choice;
    
    do {
        cout << "\n=== AIRLINE RESERVATION SYSTEM ===\n";
        cout << "1. Passenger Registration\n";
        cout << "2. Passenger Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit System\n";
        cout << "\nEnter your choice (1-4): ";
        
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1:
            {
                PassengerRegistration();
                break;
            }
            case 2:
               { passengerLogin();
                break;
            }
            case 3:
                {
                cout << "Admin Login feature coming soon!\n";
                break;
                }
            case 4:
                {cout << "Thank you for using the system!\n";
                break;
                }
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while(choice != 4);
}

// ========== MAIN FUNCTION ==========

int main() {
    // Add one sample passenger for testing
    // passengerCount = 1;
    // passengers[0].id = 1001;
    // strcpy(passengers[0].name, "Test User");
    // strcpy(passengers[0].password, "test123");
    // strcpy(passengers[0].email, "test@example.com");
    // strcpy(passengers[0].phone, "1234567890");
    // passengers[0].totalBookings = 0;
    // passengers[0].totalSpent = 0.0;
    
    // Add one sample flight for testing
    // flightCount = 1;
    // flights[0].flightNo = 101;
    // strcpy(flights[0].destination, "Pakistan");
    // strcpy(flights[0].origin, "India");
    // flights[0].departureDate = {15, 12, 2024};
    // flights[0].departureTime = {8, 0};
    // flights[0].arrivalDate = {15, 12, 2024};
    // flights[0].arrivalTime = {16, 30};
    // flights[0].totalSeats = 200;
    // flights[0].availableSeats = 180;
    // flights[0].baseFare = 300.0;
    // flights[0].distance = 4000.0;
    // strcpy(flights[0].status, "Available");
    // flights[0].timesBooked = 20;
    // flights[0].totalRevenue = 6000.00;
    
    mainMenu();
    return 0;
}