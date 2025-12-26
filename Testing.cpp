#include<iostream>
#include<string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <fstream>
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



// Save passengers to file
void savePassengersToFile() {
    ofstream file("passengers.txt");
    if (!file) {
        cout << "Error: Cannot open passengers.txt for writing!\n";
        return;
    }
    
    file << passengerCount << "\n";
    for (int i = 0; i < passengerCount; i++) {
        file << passengers[i].id << "\n";
        file << passengers[i].name << "\n";
        file << passengers[i].password << "\n";
        file << passengers[i].email << "\n";
        file << passengers[i].phone << "\n";
        file << passengers[i].totalBookings << "\n";
        file << passengers[i].totalSpent << "\n";
    }
    file.close();
    //cout << "Passengers saved to file.\n";
}

// Load passengers from file
void loadPassengersFromFile() {
    ifstream file("passengers.txt");
    if (!file) {
        cout << "No passenger data found. Starting fresh.\n";
        return;
    }
    
    file >> passengerCount;
    file.ignore(); // Ignore newline
    
    for (int i = 0; i < passengerCount; i++) {
        file >> passengers[i].id;
        file.ignore();
        file.getline(passengers[i].name, 50);
        file.getline(passengers[i].password, 30);
        file.getline(passengers[i].email, 50);
        file.getline(passengers[i].phone, 15);
        file >> passengers[i].totalBookings;
        file >> passengers[i].totalSpent;
        file.ignore();
    }
    file.close();
    //cout << "Passengers loaded from file.\n";
}

// Save flights to file
void saveFlightsToFile() {
    ofstream file("flights.txt");
    if (!file) {
        cout << "Error: Cannot open flights.txt for writing!\n";
        return;
    }
    
    file << flightCount << "\n";
    for (int i = 0; i < flightCount; i++) {
        file << flights[i].flightNo << "\n";
        file << flights[i].destination << "\n";
        file << flights[i].origin << "\n";
        file << flights[i].departureDate.day << " " << flights[i].departureDate.month << " " << flights[i].departureDate.year << "\n";
        file << flights[i].departureTime.hour << " " << flights[i].departureTime.minute << "\n";
        file << flights[i].arrivalDate.day << " " << flights[i].arrivalDate.month << " " << flights[i].arrivalDate.year << "\n";
        file << flights[i].arrivalTime.hour << " " << flights[i].arrivalTime.minute << "\n";
        file << flights[i].totalSeats << "\n";
        file << flights[i].availableSeats << "\n";
        file << flights[i].baseFare << "\n";
        file << flights[i].distance << "\n";
        file << flights[i].status << "\n";
        file << flights[i].timesBooked << "\n";
        file << flights[i].totalRevenue << "\n";
    }
    file.close();
    //cout << "Flights saved to file.\n";
}

// Load flights from file
void loadFlightsFromFile() {
    ifstream file("flights.txt");
    if (!file) {
        cout << "No flight data found. Starting fresh.\n";
        return;
    }
    
    file >> flightCount;
    file.ignore();
    
    for (int i = 0; i < flightCount; i++) {
        file >> flights[i].flightNo;
        file.ignore();
        file.getline(flights[i].destination, 50);
        file.getline(flights[i].origin, 50);
        file >> flights[i].departureDate.day >> flights[i].departureDate.month >> flights[i].departureDate.year;
        file >> flights[i].departureTime.hour >> flights[i].departureTime.minute;
        file >> flights[i].arrivalDate.day >> flights[i].arrivalDate.month >> flights[i].arrivalDate.year;
        file >> flights[i].arrivalTime.hour >> flights[i].arrivalTime.minute;
        file >> flights[i].totalSeats;
        file >> flights[i].availableSeats;
        file >> flights[i].baseFare;
        file >> flights[i].distance;
        file.ignore();
        file.getline(flights[i].status, 20);
        file >> flights[i].timesBooked;
        file >> flights[i].totalRevenue;
        file.ignore();
    }
    file.close();
    //cout << "Flights loaded from file.\n";
}

// Save bookings to file
void saveBookingsToFile() {
    ofstream file("bookings.txt");
    if (!file) {
        cout << "Error: Cannot open bookings.txt for writing!\n";
        return;
    }
    
    file << bookingCount << "\n";
    for (int i = 0; i < bookingCount; i++) {
        file << bookings[i].bookingId << "\n";
        file << bookings[i].passengerId << "\n";
        file << bookings[i].flightNo << "\n";
        file << bookings[i].bookingDate.day << " " << bookings[i].bookingDate.month << " " << bookings[i].bookingDate.year << "\n";
        file << bookings[i].travelDate.day << " " << bookings[i].travelDate.month << " " << bookings[i].travelDate.year << "\n";
        file << bookings[i].seatsBooked << "\n";
        file << bookings[i].classType << "\n";
        file << bookings[i].farePaid << "\n";
        file << bookings[i].status << "\n";
    }
    file.close();
    //cout << "Bookings saved to file.\n";
}

// Load bookings from file
void loadBookingsFromFile() {
    ifstream file("bookings.txt");
    if (!file) {
        cout << "No booking data found. Starting fresh.\n";
        return;
    }
    
    file >> bookingCount;
    file.ignore();
    
    for (int i = 0; i < bookingCount; i++) {
        file >> bookings[i].bookingId;
        file >> bookings[i].passengerId;
        file >> bookings[i].flightNo;
        file >> bookings[i].bookingDate.day >> bookings[i].bookingDate.month >> bookings[i].bookingDate.year;
        file >> bookings[i].travelDate.day >> bookings[i].travelDate.month >> bookings[i].travelDate.year;
        file >> bookings[i].seatsBooked;
        file.ignore();
        file.getline(bookings[i].classType, 20);
        file >> bookings[i].farePaid;
        file.ignore();
        file.getline(bookings[i].status, 20);
    }
    file.close();
    //cout << "Bookings loaded from file.\n";
}

// Save all data to files
void saveAllData() {
    savePassengersToFile();
    saveFlightsToFile();
    saveBookingsToFile();
    cout << "All data saved to files successfully!\n";
}

// Load all data from files
void loadAllData() {
    loadPassengersFromFile();
    loadFlightsFromFile();
    loadBookingsFromFile();
    cout << "All data loaded from files successfully!\n";
}

//Validate Date

bool isValidDate(const Date& date) {
    if (date.day < 1 || date.day > 31){
        return false;
    }
    if (date.month < 1 || date.month > 12){
        return false;
        }
    if (date.year < 2024){
        return false;
    } 
    return true;
}
//Validate Future Date
bool isFutureDate(const Date& date) {
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    
    int currentYear = currentTime->tm_year + 1900;
    int currentMonth = currentTime->tm_mon + 1;
    int currentDay = currentTime->tm_mday;
    
    if (date.year < currentYear){
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

int generateBookingId() {
    static int lastId = 1000;
    // Load last booking ID from file if exists
    ifstream file("last_booking_id.txt");
    if (file) {
        file >> lastId;
        file.close();
    }
    
    lastId++;
    
    // Save new last booking ID
    ofstream outFile("last_booking_id.txt");
    if (outFile) {
        outFile << lastId;
        outFile.close();
    }
    
    return lastId;
}
//Validate E-mail
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
        cout << "No flights available at the moment.\n";
        return;
    }
    
    cout << left << setw(10) << "Flight # " 
         << setw(15) << "From" 
         << setw(15) << "To" 
         << setw(10) << "Seats" 
         << setw(10) << "Fare($)" 
         << setw(12) << "Status" << "\n";
    cout << string(72, '-') << "\n";
    
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].status, "Available") == 0 && flights[i].availableSeats > 0) {
            cout << left << setw(10) << flights[i].flightNo
                 << setw(15) << flights[i].origin
                 << setw(15) << flights[i].destination
                 << setw(10) << flights[i].availableSeats
                 << setw(10) << flights[i].baseFare
                 << setw(12) << flights[i].status << "\n";
        }
    }
}

// ========== BOOK FLIGHT ==========

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
    
    if (flightChoice == 0) return;
    
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
    
    // Save all data to files
    saveAllData();
    
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
               {
                cout << "Generate Report feature coming soon!\n";
                break;
                }
            case 5: 
                {
                cout << "Update Profile feature coming soon!\n";
                break;
                 }
            case 6: 
                {
                    cout << "Logged out successfully!\n";
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
    
    // Save passengers to file
    savePassengersToFile();
    
    cout << "\nRegistration successful!\n";
    cout << "\n=== REGISTRATION DETAILS ===\n";
    cout << "Passenger ID: " << newPassenger.id << "\n";
    cout << "Name: " << newPassenger.name << "\n";
    cout << "Email: " << newPassenger.email << "\n";
    cout << "Phone: " << newPassenger.phone << "\n";
    cout << "\nIMPORTANT: Save your Passenger ID for login: " << newPassenger.id << "\n";
}

// ========== ADD SAMPLE DATA (Optional) ==========
void addSampleData() {
    if (passengerCount == 0) {
        //Add one sample passenger
        passengerCount = 1;
        passengers[0].id = 1001;
        strcpy(passengers[0].name, "Test User");
        strcpy(passengers[0].password, "test123");
        strcpy(passengers[0].email, "test@example.com");
        strcpy(passengers[0].phone, "1234567890");
        passengers[0].totalBookings = 0;
        passengers[0].totalSpent = 0.0;
        
        //Save to file
        savePassengersToFile();
    }
    
    if (flightCount == 0) {
        // Add one sample flight
        flightCount = 1;
        flights[0].flightNo = 101;
        strcpy(flights[0].destination, "New York");
        strcpy(flights[0].origin, "Los Angeles");
        flights[0].departureDate = {15, 12, 2024};
        flights[0].departureTime = {8, 0};
        flights[0].arrivalDate = {15, 12, 2024};
        flights[0].arrivalTime = {16, 30};
        flights[0].totalSeats = 200;
        flights[0].availableSeats = 180;
        flights[0].baseFare = 300.0;
        flights[0].distance = 4000.0;
        strcpy(flights[0].status, "Available");
        flights[0].timesBooked = 20;
        flights[0].totalRevenue = 6000.00;
        
        // Save to file
        saveFlightsToFile();
    }
}
// ========== Admin Menu ==========
void adminMenu()
{
    cout << "\n===== ADMIN PANEL =====\n";
    cout << "1. Add Flight\n";
    cout << "2. View Flights\n";
    cout << "3. Update Flight\n";
    cout << "4. Delete Flight\n";
    cout << "5. View Bookings\n";
    cout << "6. Logout\n";
}
// ========== Admin Login ==========

void adminLoginPanel() {
    cout << "\n=== ADMIN LOGIN ===\n";
    
    string username;
    string password;

    cout << "Enter Admin Username: ";
    cin >> username;

    cout << "Enter Admin Password: ";
    cin >> password;

    if (username == "admin" && password == "1234") 
    {
    cout << "\nLogin successful! Welcome Admin!\n";
        int choice;
        do {
            adminMenu(); 

            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                {
                    addFlight();
                }
                break;

                case 2:
                {
                    viewFlights();
                }
                break;

                case 3:
                {
                    updateFlight();
                }
                break;

                case 4:
                {
                    deleteFlight();
                }
                break;

                case 5:
                {
                    viewBookings();
                }
                break;

                case 6:
                {
                    cout << "Logging out...\n";
                }
                break;

                default:
                {
                    cout << "Invalid choice!\n";
                }
            }
        } while (choice != 6);
    }
    else 
    {
        cout << "Access Denied! Invalid credentials.\n";
    }

}

// ========== MAIN MENU ==========
void mainMenu() {
    int choice;
    
    do {
        cout << "\n=== AIRLINE RESERVATION SYSTEM ===\n";
        cout << "1. Passenger Registration\n";
        cout << "2. Passenger Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Add Sample Data\n";
        cout << "5. Save All Data\n";
        cout << "6. Exit System\n";
        cout << "\nEnter your choice (1-6): ";
        
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1:
               { 
                PassengerRegistration();
                break;
                }
            case 2:
                {
                passengerLogin();
                break;
                }
            case 3:
                {
                   adminLoginPanel();
                   break;
                }
            case 4:
                {
                addSampleData();
                cout << "Sample data added (if files were empty).\n";
                break;
                }
            case 5:
               { 
                saveAllData();
                break;
                }
            case 6:
               { 
                cout << "Thank you for using the system!\n";
                // Auto-save before exit
                saveAllData();
                break;
                }
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while(choice != 6);
}

// ========== MAIN FUNCTION ==========
int main() {
    cout << "=== AIRLINE RESERVATION SYSTEM WITH FILE HANDLING ===\n";
    
    // Load all data from files at startup
    // loadAllData();
    
    // // Check if data was loaded, if not add sample data
    // if (passengerCount == 0 && flightCount == 0) {
    //     cout << "\nNo existing data found. Would you like to add sample data? (Y/N): ";
    //     char choice;
    //     cin >> choice;
    //     cin.ignore();
        
    //     if (toupper(choice) == 'Y') {
    //         addSampleData();
    //         cout << "Sample data added.\n";
    //     }
    // }
    
    mainMenu();
    return 0;
}