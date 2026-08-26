#include "../../include/services/FileManager.h"
#include "../../include/core/Utils.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MK_DIR(d) _mkdir(d)
#else
#define MK_DIR(d) mkdir(d, 0777)
#endif

FileManager::FileManager(const std::string& dir)
    : dataDir(dir),
      locationsFile(dir + "/locations.txt"),
      packagesFile(dir + "/packages.txt"),
      usersFile(dir + "/users.txt"),
      bookingsFile(dir + "/bookings.txt"),
      couponsFile(dir + "/coupons.txt") {}

void FileManager::ensureDirectoryExists() const {
    MK_DIR(dataDir.c_str());
    MK_DIR("receipts");
}

void FileManager::seedDefaultData(Graph& graph,
                                 std::vector<TourPackage>& packages,
                                 std::unordered_map<std::string, std::shared_ptr<User>>& users,
                                 std::unordered_map<std::string, double>& coupons) const {
    // 1. Seed Highway Network of Bangladesh (21 Major Districts)
    std::vector<std::string> cities = {
        "Dhaka", "Chittagong", "Cox's Bazar", "Sylhet", "Khulna",
        "Rajshahi", "Rangpur", "Barisal", "Comilla", "Mymensingh",
        "Jessore", "Bogra", "Tangail", "Dinajpur", "Pabna",
        "Kushtia", "Saidpur", "Jamalpur", "Narayanganj", "Gazipur", "Narsingdi"
    };

    for (const auto& city : cities) {
        graph.addLocation(city);
    }

    // Realistic road distances in KM
    graph.addConnection("Dhaka", "Chittagong", 243);
    graph.addConnection("Dhaka", "Khulna", 299);
    graph.addConnection("Dhaka", "Rajshahi", 249);
    graph.addConnection("Dhaka", "Comilla", 114);
    graph.addConnection("Dhaka", "Sylhet", 240);
    graph.addConnection("Dhaka", "Barisal", 193);
    graph.addConnection("Dhaka", "Rangpur", 285);
    graph.addConnection("Dhaka", "Narayanganj", 17);
    graph.addConnection("Dhaka", "Gazipur", 38);
    graph.addConnection("Dhaka", "Mymensingh", 115);
    graph.addConnection("Dhaka", "Jessore", 169);
    graph.addConnection("Dhaka", "Tangail", 68);
    graph.addConnection("Dhaka", "Bogra", 193);
    graph.addConnection("Dhaka", "Cox's Bazar", 389);
    graph.addConnection("Dhaka", "Narsingdi", 53);
    graph.addConnection("Dhaka", "Dinajpur", 352);
    graph.addConnection("Dhaka", "Pabna", 192);
    graph.addConnection("Dhaka", "Kushtia", 246);
    graph.addConnection("Dhaka", "Saidpur", 295);
    graph.addConnection("Dhaka", "Jamalpur", 192);

    graph.addConnection("Chittagong", "Cox's Bazar", 152);
    graph.addConnection("Chittagong", "Comilla", 151);
    graph.addConnection("Chittagong", "Sylhet", 308);
    graph.addConnection("Chittagong", "Barisal", 311);
    graph.addConnection("Chittagong", "Gazipur", 258);
    graph.addConnection("Chittagong", "Narayanganj", 225);

    graph.addConnection("Khulna", "Jessore", 65);
    graph.addConnection("Khulna", "Barisal", 56);
    graph.addConnection("Khulna", "Kushtia", 92);
    graph.addConnection("Khulna", "Rajshahi", 229);

    graph.addConnection("Rajshahi", "Pabna", 73);
    graph.addConnection("Rajshahi", "Bogra", 79);
    graph.addConnection("Rajshahi", "Kushtia", 145);
    graph.addConnection("Rajshahi", "Dinajpur", 174);
    graph.addConnection("Rajshahi", "Rangpur", 195);

    graph.addConnection("Rangpur", "Saidpur", 121);
    graph.addConnection("Rangpur", "Dinajpur", 107);
    graph.addConnection("Rangpur", "Bogra", 88);
    graph.addConnection("Rangpur", "Jamalpur", 92);

    graph.addConnection("Sylhet", "Mymensingh", 365);
    graph.addConnection("Sylhet", "Comilla", 243);

    graph.addConnection("Tangail", "Bogra", 213);
    graph.addConnection("Tangail", "Gazipur", 152);
    graph.addConnection("Tangail", "Mymensingh", 225);

    graph.addConnection("Bogra", "Dinajpur", 49);
    graph.addConnection("Bogra", "Saidpur", 98);
    graph.addConnection("Bogra", "Jamalpur", 19);

    // 2. Seed Tour Packages
    packages.clear();
    packages.push_back(TourPackage(
        "PKG-101", "Cox's Bazar Beach & Marine Drive Holiday", "Cox's Bazar",
        7500.0, 3, 2, "5-Star Sea Crown Resort",
        "Complimentary Breakfast, Sunset Cruise, AC Scania Transport, Beach BBQ",
        "Experience world's longest natural sea beach with luxury ocean view accommodation."
    ));
    packages.push_back(TourPackage(
        "PKG-102", "Sylhet Ratargul & Tea Valley Expedition", "Sylhet",
        5800.0, 3, 2, "4-Star Grand Sultan Tea Resort",
        "All Meals, Swamp Forest Boat Ride, Jaflong Tour Guide, AC Hiace",
        "Explore lush green rolling tea gardens, Ratargul swamp forest, and crystal waters of Jaflong."
    ));
    packages.push_back(TourPackage(
        "PKG-103", "Sundarbans Wild Mangrove Safari", "Khulna",
        12000.0, 4, 3, "Premium Cruise Ship Cabin",
        "Buffet Dining, Armed Forest Guard, Watch Tower Trekking, Boat Safari",
        "Venture into UNESCO World Heritage Mangrove Forest, spot Royal Bengal Tigers & spotted deer."
    ));
    packages.push_back(TourPackage(
        "PKG-104", "Heritage of Barisal & Floating Guava Market", "Barisal",
        4500.0, 2, 1, "3-Star River View Heritage Hotel",
        "Traditional Meals, Country Boat Tour, Sandhya River Cruise",
        "Witness traditional floating markets, rivers, backwaters, and rural beauty of southern Bangladesh."
    ));
    packages.push_back(TourPackage(
        "PKG-105", "North Bengal Archaeological Silk Trail", "Rajshahi",
        5200.0, 3, 2, "4-Star Royal Rajshahi Palace",
        "Breakfast & Dinner, Somapura Mahavihara & Puthia Palace Entry, AC Transport",
        "Explore ancient temples, historic silk factories, and panoramic sunset over Padma River."
    ));

    // 3. Seed Users (Default Admin & Demo Customer)
    users.clear();
    // Default Admin (username: admin, pass: admin123)
    users["admin"] = std::make_shared<Admin>(
        "admin", Utils::simpleHash("admin123"), "System Administrator",
        "+880 1700-112233", "admin@travelbd.gov.bd", "Central Operations", 3
    );

    // Demo Customer (username: rahman, pass: pass123)
    auto demoCust = std::make_shared<Customer>(
        "rahman", Utils::simpleHash("pass123"), "Anisur Rahman",
        "+880 1712-345678", "rahman@gmail.com", "Dhanmondi 27, Dhaka", 25000.0, 250
    );
    users["rahman"] = demoCust;

    // 4. Seed Promotional Coupons
    coupons.clear();
    coupons["OOP100"]     = 1000.0; // Flat 1000 BDT
    coupons["HOLIDAY20"]  = 0.20;   // 20% discount (ratio < 1 is percentage)
    coupons["BANGLADESH"] = 0.15;   // 15% discount
    coupons["WELCOME50"]  = 500.0;  // Flat 500 BDT
}

void FileManager::initialize(Graph& graph,
                            std::vector<TourPackage>& packages,
                            std::unordered_map<std::string, std::shared_ptr<User>>& users,
                            std::vector<Booking>& bookings,
                            std::unordered_map<std::string, double>& coupons) {
    ensureDirectoryExists();

    bool locOk = loadLocations(graph);
    bool pkgOk = loadPackages(packages);
    bool usrOk = loadUsers(users);
    bool cpnOk = loadCoupons(coupons);
    loadBookings(bookings);

    // If initial load failed because files didn't exist, seed and persist default data
    if (!locOk || !pkgOk || !usrOk || !cpnOk || graph.getLocations().empty()) {
        seedDefaultData(graph, packages, users, coupons);
        saveLocations(graph);
        savePackages(packages);
        saveUsers(users);
        saveCoupons(coupons);
    }
}

bool FileManager::loadLocations(Graph& graph) const {
    std::ifstream file(locationsFile);
    if (!file) return false;

    graph.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string from, to, distStr;
        if (std::getline(ss, from, '|') && std::getline(ss, to, '|') && std::getline(ss, distStr, '|')) {
            try {
                int dist = std::stoi(Utils::trim(distStr));
                graph.addConnection(Utils::trim(from), Utils::trim(to), dist);
            } catch (...) {}
        } else {
            // Single city without connections
            graph.addLocation(line);
        }
    }
    file.close();
    return true;
}

bool FileManager::saveLocations(const Graph& graph) const {
    std::ofstream file(locationsFile);
    if (!file) return false;

    file << "# Bangladesh Highway Network Locations (From|To|DistanceInKM)\n";
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> written;

    for (const auto& pair : graph.getAllConnections()) {
        const std::string& from = pair.first;
        for (const auto& edge : pair.second) {
            const std::string& to = edge.to;
            if (!written[from][to] && !written[to][from]) {
                file << from << "|" << to << "|" << edge.distance << "\n";
                written[from][to] = true;
                written[to][from] = true;
            }
        }
    }
    file.close();
    return true;
}

bool FileManager::loadPackages(std::vector<TourPackage>& packages) const {
    std::ifstream file(packagesFile);
    if (!file) return false;

    packages.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 10 && tokens[0] == "PKG") {
            try {
                TourPackage pkg(tokens[1], tokens[2], tokens[3],
                                std::stod(tokens[4]), std::stoi(tokens[5]),
                                std::stoi(tokens[6]), tokens[7], tokens[8], tokens[9]);
                packages.push_back(pkg);
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool FileManager::savePackages(const std::vector<TourPackage>& packages) const {
    std::ofstream file(packagesFile);
    if (!file) return false;

    file << "# Tour Packages Database (PKG|ID|Name|Destination|Cost|Days|Nights|Hotel|Inclusions|Description)\n";
    for (const auto& pkg : packages) {
        file << pkg.serialize() << "\n";
    }
    file.close();
    return true;
}

bool FileManager::loadUsers(std::unordered_map<std::string, std::shared_ptr<User>>& users) const {
    std::ifstream file(usersFile);
    if (!file) return false;

    users.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 8 && tokens[0] == "ADMIN") {
            try {
                auto admin = std::make_shared<Admin>(
                    tokens[1], std::stoull(tokens[2]), tokens[3],
                    tokens[4], tokens[5], tokens[6], std::stoi(tokens[7])
                );
                users[tokens[1]] = admin;
            } catch (...) {}
        } else if (tokens.size() >= 9 && tokens[0] == "CUSTOMER") {
            try {
                auto cust = std::make_shared<Customer>(
                    tokens[1], std::stoull(tokens[2]), tokens[3],
                    tokens[4], tokens[5], tokens[6],
                    std::stod(tokens[7]), std::stoi(tokens[8])
                );
                if (tokens.size() >= 10 && !tokens[9].empty()) {
                    std::stringstream bss(tokens[9]);
                    std::string bId;
                    while (std::getline(bss, bId, ',')) {
                        cust->addBookingId(Utils::trim(bId));
                    }
                }
                users[tokens[1]] = cust;
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool FileManager::saveUsers(const std::unordered_map<std::string, std::shared_ptr<User>>& users) const {
    std::ofstream file(usersFile);
    if (!file) return false;

    file << "# User Accounts Database (ADMIN|username|passHash|... or CUSTOMER|username|passHash|...)\n";
    for (const auto& pair : users) {
        if (pair.second) {
            file << pair.second->serialize() << "\n";
        }
    }
    file.close();
    return true;
}

bool FileManager::loadBookings(std::vector<Booking>& bookings) const {
    std::ifstream file(bookingsFile);
    if (!file) return false;

    bookings.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        Booking b = Booking::deserialize(line);
        if (!b.getBookingId().empty()) {
            bookings.push_back(b);
        }
    }
    file.close();
    return true;
}

bool FileManager::saveBookings(const std::vector<Booking>& bookings) const {
    std::ofstream file(bookingsFile);
    if (!file) return false;

    file << "# Bookings Registry\n";
    for (const auto& b : bookings) {
        file << b.serialize() << "\n";
    }
    file.close();
    return true;
}

bool FileManager::loadCoupons(std::unordered_map<std::string, double>& coupons) const {
    std::ifstream file(couponsFile);
    if (!file) return false;

    coupons.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string code, valStr;
        if (std::getline(ss, code, '|') && std::getline(ss, valStr, '|')) {
            try {
                coupons[Utils::trim(code)] = std::stod(Utils::trim(valStr));
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool FileManager::saveCoupons(const std::unordered_map<std::string, double>& coupons) const {
    std::ofstream file(couponsFile);
    if (!file) return false;

    file << "# Coupon Codes (Code|DiscountValue)\n";
    for (const auto& pair : coupons) {
        file << pair.first << "|" << pair.second << "\n";
    }
    file.close();
    return true;
}
