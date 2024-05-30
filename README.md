if you want to add file 





#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct TravelPackage {
    string name;
    string destination;
    int cost;
};

class PackageBookingSystem {
private:
    vector<TravelPackage> packages;

public:
    void addPackage(const TravelPackage& travelPackage) {
        packages.push_back(travelPackage);
    }

    void viewPackages() const {
        if (packages.empty()) {
            cout << "No travel packages available." << endl;
            return;
        }
        int packageNumber = 1;
        for (const auto& pkg : packages) {
            cout << packageNumber << ". " << pkg.name << " to " << pkg.destination << ", Cost: " << pkg.cost << endl;
            packageNumber++;
        }
    }

    const TravelPackage* getPackage(int index) const {
        if (index >= 0 && index < packages.size()) {
            return &packages[index];
        }
        return nullptr;
    }

    void loadPackagesFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        packages.clear();
        TravelPackage pkg;
        while (file >> pkg.name >> pkg.destination >> pkg.cost) {
            packages.push_back(pkg);
        }
        file.close();
    }

    void savePackagesToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        for (const auto& pkg : packages) {
            file << pkg.name << " " << pkg.destination << " " << pkg.cost << endl;
        }
        file.close();
    }
};

struct Location {
    string name;
    unordered_map<string, int> connections;
};

class Graph {
private:
    unordered_map<string, Location> locations;

public:
    vector<string> getLocations() const {
        vector<string> cityList;
        for (const auto& loc : locations) {
            cityList.push_back(loc.first);
        }
        return cityList;
    }

    void addLocation(const string& name) {
        locations[name] = Location{name, {}};
    }

    void addConnection(const string& from, const string& to, int distance) {
        locations[from].connections[to] = distance;
        locations[to].connections[from] = distance;
    }

    unordered_map<string, int> shortestPath(const string& start) {
        unordered_map<string, int> distances;
        for (const auto& loc : locations) {
            distances[loc.first] = numeric_limits<int>::max();
        }
        distances[start] = 0;

        auto compare = [&](const string& a, const string& b) {
            return distances[a] > distances[b];
        };

        priority_queue<string, vector<string>, decltype(compare)> pq(compare);
        pq.push(start);

        while (!pq.empty()) {
            string current = pq.top();
            pq.pop();

            for (const auto& neighbor : locations[current].connections) {
                int newDist = distances[current] + neighbor.second;
                if (newDist < distances[neighbor.first]) {
                    distances[neighbor.first] = newDist;
                    pq.push(neighbor.first);
                }
            }
        }

        return distances;
    }

    void printLocations() const {
        for (const auto& loc : locations) {
            cout << loc.first << endl;
        }
    }

    void loadLocationsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        locations.clear();
        string name;
        while (file >> name) {
            addLocation(name);
        }
        file.close();
    }

    void saveLocationsToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        for (const auto& loc : locations) {
            file << loc.first << endl;
        }
        file.close();
    }
};

struct Booking {
    string userName;
    string phoneNumber;
    string destination;
    int cost;
};

class BookingSystem {
private:
    vector<Booking> bookings;

public:
    void addBooking(const Booking& booking) {
        bookings.push_back(booking);
    }

    void viewBookings(const string& user) const {
        bool found = false;
        for (const auto& booking : bookings) {
            if (booking.userName == user) {
                found = true;
                cout << "User: " << booking.userName << ", Phone: " << booking.phoneNumber
                     << ", Destination: " << booking.destination << ", Cost: " << booking.cost << endl;
            }
        }
        if (!found) {
            cout << "No bookings found for user " << user << endl;
        }
    }

    void viewAllBookings() const {
        if (bookings.empty()) {
            cout << "No bookings made yet." << endl;
            return;
        }
        for (const auto& booking : bookings) {
            cout << "User: " << booking.userName << ", Phone: " << booking.phoneNumber
                 << ", Destination: " << booking.destination << ", Cost: " << booking.cost << endl;
        }
    }

    void loadBookingsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        bookings.clear();
        Booking booking;
        while (file >> booking.userName >> booking.phoneNumber >> booking.destination >> booking.cost) {
            bookings.push_back(booking);
        }
        file.close();
    }

    void saveBookingsToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        for (const auto& booking : bookings) {
            file << booking.userName << " " << booking.phoneNumber << " " << booking.destination << " " << booking.cost << endl;
        }
        file.close();
    }
};

class UserManagement {
private:
    unordered_set<string> users;

public:
    bool addUser(const string& user) {
        return users.insert(user).second;
    }

    bool userExists(const string& user) const {
        return users.find(user) != users.end();
    }

    void viewAllUsers() const {
        if (users.empty()) {
            cout << "No customers registered yet." << endl;
            return;
        }
        for (const auto& user : users) {
            cout << user << endl;
        }
    }

    void loadUsersFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        users.clear();
        string user;
        while (file >> user) {
            users.insert(user);
        }
        file.close();
    }

    void saveUsersToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        for (const auto& user : users) {
            file << user << endl;
        }
        file.close();
    }
};

void clearConsole() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void adminPortal(Graph& graph, UserManagement& userManagement, BookingSystem& bookingSystem) {
    // Add your admin portal logic here

    // Secure Admin Login
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    // Perform authentication using hashing
    // You can implement your hashing mechanism here
    // For simplicity, let's assume successful login
    cout << "Login successful!" << endl;
    cout << "Admin portal accessed." << endl;

    int choice;
    do {
        cout << "\nAdmin Portal Menu:" << endl;
        cout << "1. Location Management" << endl;
        cout << "2. View all customers" << endl;
        cout << "3. View all bookings" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose option: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                clearConsole();
                // Location Management
                string location;
                cout << "Enter new travel location to add: ";
                cin >> location;
                // Add the new travel location to the graph
                graph.addLocation(location);
                cout << "New travel location added successfully." << endl;
                break;
            }
            case 2: {
                clearConsole();
                // View all customers
                cout << "All registered customers:" << endl;
                userManagement.viewAllUsers();
                break;
            }
            case 3: {
                clearConsole();
                // View all bookings
                cout << "All bookings:" << endl;
                bookingSystem.viewAllBookings();
                break;
            }
            case 4:
                cout << "Exiting Admin Portal..." << endl;
                break;
            default:
                cout << "Invalid option!" << endl;
                break;
        }
    } while (choice != 4);
}

void customerPortal(Graph& graph, UserManagement& userManagement, BookingSystem& bookingSystem, PackageBookingSystem& packageBookingSystem) {
    string username;
    cout << "Enter username: ";
    cin >> username;

    if (!userManagement.userExists(username)) {
        int choice;
        cout << "Username not found! Do you want to create a new account? (1: Yes, 0: No): ";
        cin >> choice;
        if (choice == 1) {
            string name, address, phone;
            cout << "Enter your name: ";
            cin.ignore(); // Clear the input buffer
            getline(cin, name);
            cout << "Enter your address: ";
            getline(cin, address);
            cout << "Enter your phone number: ";
            getline(cin, phone);

            if (userManagement.addUser(username)) {
                cout << "Account created successfully!" << endl;
            } else {
                cout << "Failed to create account!" << endl;
                return;
            }
        } else {
            return;
        }
    }

    int choice;
    do {
        cout << "1. View available locations\n2. Plan custom trip\n3. Book a package\n4. View booking history\n5. Logout\nChoose option: ";
        cin >> choice;
        switch (choice) {
            case 1: {
                clearConsole();
                // Display available locations with numbers
                int cityNumber = 1;
                cout << "Available locations:" << endl;
                for (const auto& loc : graph.getLocations()) {
                    cout << cityNumber << ". " << loc << endl;
                    cityNumber++;
                }
                break;
            }
            case 2: {
                clearConsole();
                // Display available locations with numbers
                int cityNumber = 1;
                cout << "Available locations:" << endl;
                for (const auto& loc : graph.getLocations()) {
                    cout << cityNumber << ". " << loc << endl;
                    cityNumber++;
                }

                // Input source city number
                cout << "Enter source city number: ";
                int sourceNumber;
                cin >> sourceNumber;
                string source = graph.getLocations()[sourceNumber - 1]; // Adjust for 0-based indexing

                // Input destination city number
                cout << "Enter destination city number: ";
                int destinationNumber;
                cin >> destinationNumber;
                string destination = graph.getLocations()[destinationNumber - 1]; // Adjust for 0-based indexing

                auto distances = graph.shortestPath(source);
                if (distances.find(destination) != distances.end()) {
                    cout << "Shortest distance from " << source << " to " << destination << " is " << distances[destination] << endl;

                    // Prompt user for booking
                    cout << "Do you want to book this trip? (1: Yes, 0: No): ";
                    int bookChoice;
                    cin >> bookChoice;
                    if (bookChoice == 1) {
                        int cost = distances[destination]; // Assuming cost is the distance for simplicity
                        // Collect user information
                        string phone;
                        cout << "Enter your phone number: ";
                        cin.ignore(); // Clear the input buffer
                        getline(cin, phone);

                        // Create a Booking object and assign values
                        Booking booking;
                        booking.userName = username;
                        booking.phoneNumber = phone; // Assign phone number collected from the user
                        booking.destination = destination;
                        booking.cost = cost;

                        // Add booking
                        bookingSystem.addBooking(booking);
                        cout << "Booking successful!" << endl;
                    }
                } else {
                    cout << "Destination not found in graph!" << endl;
                }
                break;
            }
            case 3: {
                clearConsole();
                // Display available packages
                packageBookingSystem.viewPackages();

                // Collect user information
                string name, phone;
                cout << "Enter your name: ";
                cin.ignore(); // Clear the input buffer
                getline(cin, name);
                cout << "Enter your phone number: ";
                getline(cin, phone);

                // Book a package
                cout << "Enter package number to book: ";
                int packageNumber;
                cin >> packageNumber;

                const TravelPackage* selectedPackage = packageBookingSystem.getPackage(packageNumber - 1); // Adjust for 0-based indexing
                if (selectedPackage) {
                    // Create a Booking object with the appropriate parameters
                    Booking booking;
                    booking.userName = name;
                    booking.phoneNumber = phone;
                    booking.destination = selectedPackage->destination;
                    booking.cost = selectedPackage->cost;

                    // Add booking with user information
                    bookingSystem.addBooking(booking);
                    cout << "Package booking successful!" << endl;
                } else {
                    cout << "Invalid package number!" << endl;
                }
                break;
            }
            case 4: {
                clearConsole();
                // View booking history
                bookingSystem.viewBookings(username);
                break;
            }
            case 5:
                cout << "Logging out..." << endl;
                break;
            default:
                cout << "Invalid option!" << endl;
                break;
        }
    } while (choice != 5);
}

int main() {
    Graph graph;
    UserManagement userManagement;
    BookingSystem bookingSystem;
    PackageBookingSystem packageBookingSystem;

    // Load data from files
    graph.loadLocationsFromFile("locations.txt");
    userManagement.loadUsersFromFile("users.txt");
    bookingSystem.loadBookingsFromFile("bookings.txt");
    packageBookingSystem.loadPackagesFromFile("packages.txt");

    int choice;
    do {
        cout << "1. Admin Portal\n2. Customer Portal\n3. Exit\nChoose option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                clearConsole();
                adminPortal(graph, userManagement, bookingSystem);
                break;
            case 2:
                clearConsole();
                customerPortal(graph, userManagement, bookingSystem, packageBookingSystem);
                break;
            case 3:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid option!" << endl;
                break;
        }
    } while (choice != 3);

    // Save data to files
    graph.saveLocationsToFile("locations.txt");
    userManagement.saveUsersToFile("users.txt");
    bookingSystem.saveBookingsToFile("bookings.txt");
    packageBookingSystem.savePackagesToFile("packages.txt");

    return 0;
}
