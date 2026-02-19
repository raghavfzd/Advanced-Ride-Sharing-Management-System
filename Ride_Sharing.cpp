#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <stdexcept>

using namespace std;

/* =========================
   ABSTRACT BASE CLASS
========================= */

class User {
protected:
    string id;
    string name;
    string phone;
    string password;

public:
    User() {}
    User(string id, string name, string phone, string password)
        : id(id), name(name), phone(phone), password(password) {}

    virtual void display() = 0;

    string getID() const { return id; }
    string getPassword() const { return password; }
    string getName() const { return name; }

    virtual ~User() {}
};

/* =========================
   DRIVER CLASS
========================= */

class Driver : public User {
private:
    bool available;
    double rating;
    int totalRides;

public:
    Driver() {}
    Driver(string id, string name, string phone, string password)
        : User(id, name, phone, password),
          available(true), rating(5.0), totalRides(0) {}

    bool isAvailable() const { return available; }
    void setAvailability(bool status) { available = status; }

    void updateRating(double newRating) {
        rating = (rating * totalRides + newRating) / (totalRides + 1);
        totalRides++;
    }

    double getRating() const { return rating; }

    void display() override {
        cout << "Driver ID: " << id
             << " | Name: " << name
             << " | Rating: " << fixed << setprecision(2) << rating
             << " | Available: " << (available ? "Yes" : "No") << endl;
    }
};

/* =========================
   CUSTOMER CLASS
========================= */

class Customer : public User {
public:
    Customer() {}
    Customer(string id, string name, string phone, string password)
        : User(id, name, phone, password) {}

    void display() override {
        cout << "Customer ID: " << id
             << " | Name: " << name << endl;
    }
};

/* =========================
   RIDE CLASS
========================= */

class Ride {
private:
    string rideID;
    string customerID;
    string driverID;
    double distance;
    double fare;
    string status;

public:
    Ride(string rID, string cID, string dID, double dist)
        : rideID(rID), customerID(cID),
          driverID(dID), distance(dist), status("Completed") {
        fare = calculateFare();
    }

    double calculateFare() {
        return 50 + (distance * 12); // base + per km
    }

    string getRideID() const { return rideID; }

    void display() {
        cout << "\nRide ID: " << rideID
             << "\nCustomer ID: " << customerID
             << "\nDriver ID: " << driverID
             << "\nDistance: " << distance << " km"
             << "\nFare: ₹" << fare
             << "\nStatus: " << status << endl;
    }

    void saveToFile() {
        ofstream file("rides.txt", ios::app);
        file << rideID << "," << customerID << ","
             << driverID << "," << distance
             << "," << fare << "," << status << endl;
        file.close();
    }
};

/* =========================
   MAIN SYSTEM CLASS
========================= */

class RideSharingSystem {
private:
    unordered_map<string, Driver*> drivers;
    unordered_map<string, Customer*> customers;
    vector<Ride*> rides;

    int rideCounter = 1;

public:
    ~RideSharingSystem() {
        for (auto& d : drivers) delete d.second;
        for (auto& c : customers) delete c.second;
        for (auto& r : rides) delete r;
    }

    /* ===== DRIVER ===== */

    void registerDriver() {
        string id, name, phone, password;
        cout << "\nEnter Driver ID: ";
        cin >> id;
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter Phone: ";
        cin >> phone;
        cout << "Enter Password: ";
        cin >> password;

        drivers[id] = new Driver(id, name, phone, password);
        cout << "Driver Registered Successfully!\n";
    }

    /* ===== CUSTOMER ===== */

    void registerCustomer() {
        string id, name, phone, password;
        cout << "\nEnter Customer ID: ";
        cin >> id;
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter Phone: ";
        cin >> phone;
        cout << "Enter Password: ";
        cin >> password;

        customers[id] = new Customer(id, name, phone, password);
        cout << "Customer Registered Successfully!\n";
    }

    /* ===== LOGIN ===== */

    Customer* customerLogin() {
        string id, password;
        cout << "\nEnter Customer ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;

        if (customers.count(id) && customers[id]->getPassword() == password) {
            cout << "Login Successful!\n";
            return customers[id];
        }

        cout << "Invalid Credentials!\n";
        return nullptr;
    }

    Driver* findAvailableDriver() {
        for (auto& d : drivers) {
            if (d.second->isAvailable())
                return d.second;
        }
        return nullptr;
    }

    /* ===== BOOK RIDE ===== */

    void bookRide(Customer* customer) {
        double distance;
        cout << "\nEnter Distance (km): ";
        cin >> distance;

        if (distance <= 0)
            throw invalid_argument("Distance must be positive!");

        Driver* driver = findAvailableDriver();

        if (!driver) {
            cout << "No Drivers Available!\n";
            return;
        }

        driver->setAvailability(false);

        string rideID = "R" + to_string(rideCounter++);
        Ride* ride = new Ride(rideID, customer->getID(),
                              driver->getID(), distance);

        rides.push_back(ride);
        ride->saveToFile();

        cout << "\nRide Booked Successfully!\n";
        ride->display();

        double rating;
        cout << "\nRate Driver (1-5): ";
        cin >> rating;

        if (rating >= 1 && rating <= 5)
            driver->updateRating(rating);

        driver->setAvailability(true);
    }

    /* ===== ADMIN ===== */

    void viewAllDrivers() {
        cout << "\n--- Driver List ---\n";
        for (auto& d : drivers)
            d.second->display();
    }

    void viewRideHistory() {
        cout << "\n--- Ride History ---\n";
        for (auto& r : rides)
            r->display();
    }

    /* ===== MAIN MENU ===== */

    void menu() {
        int choice;

        while (true) {
            cout << "\n========= RIDE SHARING SYSTEM =========\n";
            cout << "1. Register Driver\n";
            cout << "2. Register Customer\n";
            cout << "3. Customer Login & Book Ride\n";
            cout << "4. View All Drivers (Admin)\n";
            cout << "5. View Ride History (Admin)\n";
            cout << "6. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            try {
                switch (choice) {
                case 1:
                    registerDriver();
                    break;
                case 2:
                    registerCustomer();
                    break;
                case 3: {
                    Customer* c = customerLogin();
                    if (c) bookRide(c);
                    break;
                }
                case 4:
                    viewAllDrivers();
                    break;
                case 5:
                    viewRideHistory();
                    break;
                case 6:
                    cout << "Exiting System...\n";
                    return;
                default:
                    cout << "Invalid Choice!\n";
                }
            }
            catch (exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
    }
};

/* =========================
   MAIN FUNCTION
========================= */

int main() {
    RideSharingSystem system;
    system.menu();
    return 0;
}
