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
};


struct Location
{
    string name;
    unordered_map<string, int> connections;
};

class Graph
{
private:
    unordered_map<string, Location> locations;

public:
    vector<string> getLocations() const
    {
        vector<string> cityList;
        for (const auto& loc : locations)
        {
            cityList.push_back(loc.first);
        }
        return cityList;
    }
    void addLocation(const string& name)
    {
        locations[name] = Location{name, {}};
    }

    void addConnection(const string& from, const string& to, int distance)
    {
        locations[from].connections[to] = distance;
        locations[to].connections[from] = distance;
    }

    unordered_map<string, int> shortestPath(const string& start)
    {
        unordered_map<string, int> distances;
        for (const auto& loc : locations)
        {
            distances[loc.first] = numeric_limits<int>::max();
        }
        distances[start] = 0;

        auto compare = [&](const string& a, const string& b)
        {
            return distances[a] > distances[b];
        };

        priority_queue<string, vector<string>, decltype(compare)> pq(compare);
        pq.push(start);

        while (!pq.empty())
        {
            string current = pq.top();
            pq.pop();

            for (const auto& neighbor : locations[current].connections)
            {
                int newDist = distances[current] + neighbor.second;
                if (newDist < distances[neighbor.first])
                {
                    distances[neighbor.first] = newDist;
                    pq.push(neighbor.first);
                }
            }
        }

        return distances;
    }

    void printLocations() const
    {
        for (const auto& loc : locations)
        {
            cout << loc.first << endl;
        }
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
};




class UserManagement
{
private:
    unordered_set<string> users;

public:
    bool addUser(const string& user)
    {
        return users.insert(user).second;
    }

    bool userExists(const string& user) const
    {
        return users.find(user) != users.end();
    }

    void viewAllUsers() const
    {
        if (users.empty())
        {
            cout << "No customers registered yet." << endl;
            return;
        }
        for (const auto& user : users)
        {
            cout << user << endl;
        }
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


void customerPortal(Graph& graph, UserManagement& userManagement, BookingSystem& bookingSystem, PackageBookingSystem& packageBookingSystem)
{

    string username;
    cout << "Enter username: ";
    cin >> username;

    if (!userManagement.userExists(username))
    {
        int choice;
        cout << "Username not found! Do you want to create a new account? (1: Yes, 0: No): ";
        cin >> choice;
        if (choice == 1)
        {
            string name, address, phone;
            cout << "Enter your name: ";
            cin.ignore(); // Clear the input buffer
            getline(cin, name);
            cout << "Enter your address: ";
            getline(cin, address);
            cout << "Enter your phone number: ";
            getline(cin, phone);

            if (userManagement.addUser(username))
            {
                cout << "Account created successfully!" << endl;
            }
            else
            {
                cout << "Failed to create account!" << endl;
                return;
            }
        }
        else
        {
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
                for (const auto& loc : graph.getLocations())
                {
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
    for (const auto& loc : graph.getLocations())
    {
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
        if (choice != 5) {
            cout << "Press Enter to continue...";
            cin.ignore(); // Clear the input buffer
            cin.get();    // Wait for Enter key press
            clearConsole();
        }
    } while (choice != 5);
}

int main() {
    Graph travelGraph;
    travelGraph.addLocation("Dhaka");
    travelGraph.addLocation("Chittagong");
    travelGraph.addLocation("Khulna");
    travelGraph.addLocation("Rajshahi");
    travelGraph.addLocation("Comilla");
    travelGraph.addLocation("Sylhet");
    travelGraph.addLocation("Barisal");
    travelGraph.addLocation("Rangpur");
    travelGraph.addLocation("Narayanganj");
    travelGraph.addLocation("Gazipur");
    travelGraph.addLocation("Mymensingh");
    travelGraph.addLocation("Jessore");
    travelGraph.addLocation("Tangail");
    travelGraph.addLocation("Bogra");
    travelGraph.addLocation("Cox's Bazar");
    travelGraph.addLocation("Narsingdi");
    travelGraph.addLocation("Dinajpur");
    travelGraph.addLocation("Pabna");
    travelGraph.addLocation("Kushtia");
    travelGraph.addLocation("Saidpur");
    travelGraph.addLocation("Jamalpur");

    // Adding connections between locations
    travelGraph.addConnection("Dhaka", "Chittagong", 243);
    travelGraph.addConnection("Dhaka", "Khulna", 299);
    travelGraph.addConnection("Dhaka", "Rajshahi", 249);
    travelGraph.addConnection("Dhaka", "Comilla", 114);
    travelGraph.addConnection("Dhaka", "Sylhet", 240);
    travelGraph.addConnection("Dhaka", "Barisal", 193);
    travelGraph.addConnection("Dhaka", "Rangpur", 285);
    travelGraph.addConnection("Dhaka", "Narayanganj", 17);
    travelGraph.addConnection("Dhaka", "Gazipur", 38);
    travelGraph.addConnection("Dhaka", "Mymensingh", 115);
    travelGraph.addConnection("Dhaka", "Jessore", 169);
    travelGraph.addConnection("Dhaka", "Tangail", 68);
    travelGraph.addConnection("Dhaka", "Bogra", 193);
    travelGraph.addConnection("Dhaka", "Cox's Bazar", 389);
    travelGraph.addConnection("Dhaka", "Narsingdi", 53);
    travelGraph.addConnection("Dhaka", "Dinajpur", 352);
    travelGraph.addConnection("Dhaka", "Pabna", 192);
    travelGraph.addConnection("Dhaka", "Kushtia", 246);
    travelGraph.addConnection("Dhaka", "Saidpur", 295);
    travelGraph.addConnection("Dhaka", "Jamalpur", 192);

    travelGraph.addConnection("Chittagong", "Khulna", 363);
    travelGraph.addConnection("Chittagong", "Rajshahi", 526);
    travelGraph.addConnection("Chittagong", "Comilla", 151);
    travelGraph.addConnection("Chittagong", "Sylhet", 308);
    travelGraph.addConnection("Chittagong", "Barisal", 311);
    travelGraph.addConnection("Chittagong", "Rangpur", 614);
    travelGraph.addConnection("Chittagong", "Narayanganj", 225);
    travelGraph.addConnection("Chittagong", "Gazipur", 258);
    travelGraph.addConnection("Chittagong", "Mymensingh", 325);
    travelGraph.addConnection("Chittagong", "Jessore", 506);
    travelGraph.addConnection("Chittagong", "Tangail", 395);
    travelGraph.addConnection("Chittagong", "Bogra", 519);
    travelGraph.addConnection("Chittagong", "Cox's Bazar", 152);
    travelGraph.addConnection("Chittagong", "Narsingdi", 237);
    travelGraph.addConnection("Chittagong", "Dinajpur", 540);
    travelGraph.addConnection("Chittagong", "Pabna", 462);
    travelGraph.addConnection("Chittagong", "Kushtia", 479);
    travelGraph.addConnection("Chittagong", "Saidpur", 546);
    travelGraph.addConnection("Chittagong", "Jamalpur", 474);

    travelGraph.addConnection("Khulna", "Rajshahi", 229);
    travelGraph.addConnection("Khulna", "Comilla", 306);
    travelGraph.addConnection("Khulna", "Sylhet", 515);
    travelGraph.addConnection("Khulna", "Barisal", 56);
    travelGraph.addConnection("Khulna", "Rangpur", 416);
    travelGraph.addConnection("Khulna", "Narayanganj", 308);
    travelGraph.addConnection("Khulna", "Gazipur", 326);
    travelGraph.addConnection("Khulna", "Mymensingh", 394);
    travelGraph.addConnection("Khulna", "Jessore", 65);
    travelGraph.addConnection("Khulna", "Tangail", 337);
    travelGraph.addConnection("Khulna", "Bogra", 390);
    travelGraph.addConnection("Khulna", "Cox's Bazar", 445);
    travelGraph.addConnection("Khulna", "Narsingdi", 330);
    travelGraph.addConnection("Khulna", "Dinajpur", 426);
    travelGraph.addConnection("Khulna", "Pabna", 322);
    travelGraph.addConnection("Khulna", "Kushtia", 92);
    travelGraph.addConnection("Khulna", "Saidpur", 462);
    travelGraph.addConnection("Khulna", "Jamalpur", 340);

    travelGraph.addConnection("Rajshahi", "Comilla", 303);
    travelGraph.addConnection("Rajshahi", "Sylhet", 479);
    travelGraph.addConnection("Rajshahi", "Barisal", 286);
    travelGraph.addConnection("Rajshahi", "Rangpur", 195);
    travelGraph.addConnection("Rajshahi", "Narayanganj", 266);
    travelGraph.addConnection("Rajshahi", "Gazipur", 279);
    travelGraph.addConnection("Rajshahi", "Mymensingh", 283);
    travelGraph.addConnection("Rajshahi", "Jessore", 297);
    travelGraph.addConnection("Rajshahi", "Tangail", 157);
    travelGraph.addConnection("Rajshahi", "Bogra", 79);
    travelGraph.addConnection("Rajshahi", "Cox's Bazar", 548);
        travelGraph.addConnection("Rajshahi", "Narsingdi", 285);
    travelGraph.addConnection("Rajshahi", "Dinajpur", 174);
    travelGraph.addConnection("Rajshahi", "Pabna", 73);
    travelGraph.addConnection("Rajshahi", "Kushtia", 145);
    travelGraph.addConnection("Rajshahi", "Saidpur", 239);
    travelGraph.addConnection("Rajshahi", "Jamalpur", 156);

    travelGraph.addConnection("Comilla", "Sylhet", 243);
    travelGraph.addConnection("Comilla", "Barisal", 260);
    travelGraph.addConnection("Comilla", "Rangpur", 378);
    travelGraph.addConnection("Comilla", "Narayanganj", 100);
    travelGraph.addConnection("Comilla", "Gazipur", 114);
    travelGraph.addConnection("Comilla", "Mymensingh", 119);
    travelGraph.addConnection("Comilla", "Jessore", 202);
    travelGraph.addConnection("Comilla", "Tangail", 119);
    travelGraph.addConnection("Comilla", "Bogra", 186);
    travelGraph.addConnection("Comilla", "Cox's Bazar", 385);
    travelGraph.addConnection("Comilla", "Narsingdi", 80);
    travelGraph.addConnection("Comilla", "Dinajpur", 362);
    travelGraph.addConnection("Comilla", "Pabna", 188);
    travelGraph.addConnection("Comilla", "Kushtia", 242);
    travelGraph.addConnection("Comilla", "Saidpur", 291);
    travelGraph.addConnection("Comilla", "Jamalpur", 188);

    travelGraph.addConnection("Sylhet", "Barisal", 479);
    travelGraph.addConnection("Sylhet", "Rangpur", 682);
    travelGraph.addConnection("Sylhet", "Narayanganj", 302);
    travelGraph.addConnection("Sylhet", "Gazipur", 335);
    travelGraph.addConnection("Sylhet", "Mymensingh", 365);
    travelGraph.addConnection("Sylhet", "Jessore", 546);
    travelGraph.addConnection("Sylhet", "Tangail", 435);
    travelGraph.addConnection("Sylhet", "Bogra", 559);
    travelGraph.addConnection("Sylhet", "Cox's Bazar", 166);
    travelGraph.addConnection("Sylhet", "Narsingdi", 285);
    travelGraph.addConnection("Sylhet", "Dinajpur", 588);
    travelGraph.addConnection("Sylhet", "Pabna", 510);
    travelGraph.addConnection("Sylhet", "Kushtia", 527);
    travelGraph.addConnection("Sylhet", "Saidpur", 594);
    travelGraph.addConnection("Sylhet", "Jamalpur", 522);

    travelGraph.addConnection("Barisal", "Rangpur", 424);
    travelGraph.addConnection("Barisal", "Narayanganj", 336);
    travelGraph.addConnection("Barisal", "Gazipur", 374);
    travelGraph.addConnection("Barisal", "Mymensingh", 442);
    travelGraph.addConnection("Barisal", "Jessore", 56);
    travelGraph.addConnection("Barisal", "Tangail", 378);
    travelGraph.addConnection("Barisal", "Bogra", 431);
    travelGraph.addConnection("Barisal", "Cox's Bazar", 476);
    travelGraph.addConnection("Barisal", "Narsingdi", 361);
    travelGraph.addConnection("Barisal", "Dinajpur", 457);
    travelGraph.addConnection("Barisal", "Pabna", 354);
    travelGraph.addConnection("Barisal", "Kushtia", 124);
    travelGraph.addConnection("Barisal", "Saidpur", 504);
    travelGraph.addConnection("Barisal", "Jamalpur", 382);

    travelGraph.addConnection("Rangpur", "Narayanganj", 285);
    travelGraph.addConnection("Rangpur", "Gazipur", 299);
    travelGraph.addConnection("Rangpur", "Mymensingh", 244);
    travelGraph.addConnection("Rangpur", "Jessore", 389);
    travelGraph.addConnection("Rangpur", "Tangail", 280);
    travelGraph.addConnection("Rangpur", "Bogra", 88);
    travelGraph.addConnection("Rangpur", "Cox's Bazar", 547);
    travelGraph.addConnection("Rangpur", "Narsingdi", 366);
    travelGraph.addConnection("Rangpur", "Dinajpur", 107);
    travelGraph.addConnection("Rangpur", "Pabna", 89);
    travelGraph.addConnection("Rangpur", "Kushtia", 311);
    travelGraph.addConnection("Rangpur", "Saidpur", 121);
    travelGraph.addConnection("Rangpur", "Jamalpur", 92);

    travelGraph.addConnection("Narayanganj", "Gazipur", 21);
    travelGraph.addConnection("Narayanganj", "Mymensingh", 103);
    travelGraph.addConnection("Narayanganj", "Jessore", 284);
    travelGraph.addConnection("Narayanganj", "Tangail", 173);
    travelGraph.addConnection("Narayanganj", "Bogra", 297);
    travelGraph.addConnection("Narayanganj", "Cox's Bazar", 386);
    travelGraph.addConnection("Narayanganj", "Narsingdi", 55);
    travelGraph.addConnection("Narayanganj", "Dinajpur", 390);
    travelGraph.addConnection("Narayanganj", "Pabna", 219);
    travelGraph.addConnection("Narayanganj", "Kushtia", 302);
    travelGraph.addConnection("Narayanganj", "Saidpur", 309);
    travelGraph.addConnection("Narayanganj", "Jamalpur", 223);

    travelGraph.addConnection("Gazipur", "Mymensingh", 82);
    travelGraph.addConnection("Gazipur", "Jessore", 263);
    travelGraph.addConnection("Gazipur", "Tangail", 152);
    travelGraph.addConnection("Gazipur", "Bogra", 275);
    travelGraph.addConnection("Gazipur", "Cox's Bazar", 364);
    travelGraph.addConnection("Gazipur", "Narsingdi", 34);
        travelGraph.addConnection("Gazipur", "Dinajpur", 369);
    travelGraph.addConnection("Gazipur", "Pabna", 198);
    travelGraph.addConnection("Gazipur", "Kushtia", 281);
    travelGraph.addConnection("Gazipur", "Saidpur", 288);
    travelGraph.addConnection("Gazipur", "Jamalpur", 202);

    travelGraph.addConnection("Mymensingh", "Jessore", 336);
    travelGraph.addConnection("Mymensingh", "Tangail", 225);
    travelGraph.addConnection("Mymensingh", "Bogra", 135);
    travelGraph.addConnection("Mymensingh", "Cox's Bazar", 424);
    travelGraph.addConnection("Mymensingh", "Narsingdi", 89);
    travelGraph.addConnection("Mymensingh", "Dinajpur", 294);
    travelGraph.addConnection("Mymensingh", "Pabna", 123);
    travelGraph.addConnection("Mymensingh", "Kushtia", 206);
    travelGraph.addConnection("Mymensingh", "Saidpur", 213);
    travelGraph.addConnection("Mymensingh", "Jamalpur", 127);

    travelGraph.addConnection("Jessore", "Tangail", 278);
    travelGraph.addConnection("Jessore", "Bogra", 352);
    travelGraph.addConnection("Jessore", "Cox's Bazar", 399);
    travelGraph.addConnection("Jessore", "Narsingdi", 284);
    travelGraph.addConnection("Jessore", "Dinajpur", 388);
    travelGraph.addConnection("Jessore", "Pabna", 299);
    travelGraph.addConnection("Jessore", "Kushtia", 64);
    travelGraph.addConnection("Jessore", "Saidpur", 445);
    travelGraph.addConnection("Jessore", "Jamalpur", 323);

    travelGraph.addConnection("Tangail", "Bogra", 213);
    travelGraph.addConnection("Tangail", "Cox's Bazar", 302);
    travelGraph.addConnection("Tangail", "Narsingdi", 179);
    travelGraph.addConnection("Tangail", "Dinajpur", 283);
    travelGraph.addConnection("Tangail", "Pabna", 112);
    travelGraph.addConnection("Tangail", "Kushtia", 195);
    travelGraph.addConnection("Tangail", "Saidpur", 202);
    travelGraph.addConnection("Tangail", "Jamalpur", 116);

    travelGraph.addConnection("Bogra", "Cox's Bazar", 479);
    travelGraph.addConnection("Bogra", "Narsingdi", 364);
    travelGraph.addConnection("Bogra", "Dinajpur", 49);
    travelGraph.addConnection("Bogra", "Pabna", 97);
    travelGraph.addConnection("Bogra", "Kushtia", 319);
    travelGraph.addConnection("Bogra", "Saidpur", 98);
    travelGraph.addConnection("Bogra", "Jamalpur", 19);

    travelGraph.addConnection("Cox's Bazar", "Narsingdi", 445);
    travelGraph.addConnection("Cox's Bazar", "Dinajpur", 622);
    travelGraph.addConnection("Cox's Bazar", "Pabna", 523);
    travelGraph.addConnection("Cox's Bazar", "Kushtia", 595);
    travelGraph.addConnection("Cox's Bazar", "Saidpur", 648);
    travelGraph.addConnection("Cox's Bazar", "Jamalpur", 566);

    travelGraph.addConnection("Narsingdi", "Dinajpur", 409);
    travelGraph.addConnection("Narsingdi", "Pabna", 238);
    travelGraph.addConnection("Narsingdi", "Kushtia", 321);
    travelGraph.addConnection("Narsingdi", "Saidpur", 328);
    travelGraph.addConnection("Narsingdi", "Jamalpur", 242);

    travelGraph.addConnection("Dinajpur", "Pabna", 133);
    travelGraph.addConnection("Dinajpur", "Kushtia", 355);
    travelGraph.addConnection("Dinajpur", "Saidpur", 146);
    travelGraph.addConnection("Dinajpur", "Jamalpur", 68);

    travelGraph.addConnection("Pabna", "Kushtia", 222);
    travelGraph.addConnection("Pabna", "Saidpur", 229);
    travelGraph.addConnection("Pabna", "Jamalpur", 143);

    travelGraph.addConnection("Kushtia", "Saidpur", 322);
    travelGraph.addConnection("Kushtia", "Jamalpur", 240);

    travelGraph.addConnection("Saidpur", "Jamalpur", 118);


    UserManagement userManagement;
    BookingSystem bookingSystem;
    PackageBookingSystem packageBookingSystem;
    packageBookingSystem.addPackage({"Beach Retreat", "Cox's Bazar", 5000});
    packageBookingSystem.addPackage({"Hill Adventure", "Sylhet", 4000});
    packageBookingSystem.addPackage({"City Explorer", "Chittagong", 3000});
    cout<<""<<endl;
    cout<<"   Travel Management System"<<endl;
    cout<<""<<endl;

    int choice;
    do {
        cout << "1. Admin Portal\n2. Customer Portal\n3. Exit\nChoose option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                clearConsole();
                adminPortal(travelGraph, userManagement, bookingSystem);
                break;
            case 2:
                clearConsole();
                customerPortal(travelGraph, userManagement, bookingSystem, packageBookingSystem);
                break;
            case 3:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid option!" << endl;
                break;
        }
    } while (choice != 3);

    return 0;
}
