/**
 * ============================================================================
 * Project: Bangladesh Tourism & Travel Management System (OOP Project)
 * Language: C++ (C++14/C++17 Compatible)
 * Features:
 *   - Object-Oriented Design (Inheritance, Polymorphism, Abstraction, Encapsulation)
 *   - Graph Theory & Dijkstra's Algorithm (Full Path Reconstruction)
 *   - Strategy Pattern (Payment Methods & Discount Policies)
 *   - Dynamic Transport Fleet (AC/Non-AC Bus, Express Train, Domestic Flight)
 *   - Full File Persistence (Locations, Packages, Users, Bookings, Coupons)
 *   - Digital Boarding Pass & Invoice Generation
 *   - ANSI Stylized Console UI with Masked Password Security
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <sys/stat.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#define CREATE_DIR(dir) _mkdir(dir)
#else
#include <termios.h>
#include <unistd.h>
#define CREATE_DIR(dir) mkdir(dir, 0777)
#endif

// ============================================================================
// 1. UTILITIES & CONSOLE FORMATTING
// ============================================================================
namespace Utils {
    namespace Color {
        const std::string RESET   = "\033[0m";
        const std::string BOLD    = "\033[1m";
        const std::string DIM     = "\033[2m";
        const std::string RED     = "\033[31m";
        const std::string GREEN   = "\033[32m";
        const std::string YELLOW  = "\033[33m";
        const std::string BLUE    = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN    = "\033[36m";
        const std::string WHITE   = "\033[37m";
        const std::string BOLD_CYAN   = "\033[1;36m";
        const std::string BOLD_GREEN  = "\033[1;32m";
        const std::string BOLD_YELLOW = "\033[1;33m";
        const std::string BOLD_RED    = "\033[1;31m";
    }

    inline void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[1;1H";
#endif
    }

    inline void pauseScreen() {
        std::cout << "\n" << Color::DIM << "Press Enter to continue..." << Color::RESET;
#ifdef _WIN32
        if (!_isatty(0)) {
            return;
        }
#endif
        std::cin.ignore(10000, '\n');
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    inline std::string toUpper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        return str;
    }

    inline std::string repeatString(const std::string& s, int count) {
        std::string res = "";
        for (int i = 0; i < count; ++i) res += s;
        return res;
    }

    inline std::string getMaskedPassword(const std::string& prompt = "Enter Password: ") {
        std::cout << prompt;
        std::string password = "";
#ifdef _WIN32
        if (!_isatty(0)) {
            std::getline(std::cin, password);
            return trim(password);
        }
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch == 3) {
                exit(0);
            } else if (ch >= 32 && ch <= 126) {
                password.push_back(ch);
                std::cout << '*';
            }
        }
        std::cout << "\n";
#else
        if (!isatty(STDIN_FILENO)) {
            std::getline(std::cin, password);
            return trim(password);
        }
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::getline(std::cin, password);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        std::cout << "\n";
#endif
        return trim(password);
    }

    inline std::string getCurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    inline std::string generateBookingId() {
        static int counter = 1000;
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::ostringstream oss;
        oss << "BK-" << (1900 + t->tm_year) << "-" << (++counter);
        return oss.str();
    }

    inline std::string formatCurrency(double amount) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << amount << " BDT";
        return oss.str();
    }

    inline size_t simpleHash(const std::string& str) {
        const size_t p = 31;
        const size_t m = 1e9 + 9;
        size_t hash_val = 0;
        size_t p_pow = 1;
        std::string salted = "TMS_SALT_2026_" + str;
        for (char c : salted) {
            hash_val = (hash_val + (static_cast<unsigned char>(c) + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        return hash_val;
    }

    inline void printHeader(const std::string& title) {
        int width = 72;
        int padding = (width - static_cast<int>(title.length())) / 2;
        if (padding < 0) padding = 0;

        std::cout << "\n" << Color::CYAN << "╔" << repeatString("═", width) << "╗" << Color::RESET << "\n";
        std::cout << Color::CYAN << "║" << Color::BOLD_YELLOW 
                  << std::string(padding, ' ') << title 
                  << std::string(std::max(0, width - padding - static_cast<int>(title.length())), ' ') 
                  << Color::CYAN << "║" << Color::RESET << "\n";
        std::cout << Color::CYAN << "╚" << repeatString("═", width) << "╝" << Color::RESET << "\n\n";
    }

    inline void printDivider(int width = 74) {
        std::cout << Color::DIM << std::string(width, '-') << Color::RESET << "\n";
    }
}

// ============================================================================
// 2. GRAPH & DIJKSTRA ALGORITHM
// ============================================================================
struct Edge {
    std::string to;
    int distance; // in KM
};

struct DijkstraResult {
    std::string startCity;
    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, std::string> parent;

    bool hasPath(const std::string& destination) const {
        auto it = distances.find(destination);
        return it != distances.end() && it->second < std::numeric_limits<int>::max();
    }

    int getDistance(const std::string& destination) const {
        auto it = distances.find(destination);
        if (it != distances.end()) return it->second;
        return -1;
    }

    std::vector<std::string> getPath(const std::string& destination) const {
        std::vector<std::string> path;
        if (!hasPath(destination)) return path;

        std::string curr = destination;
        while (!curr.empty()) {
            path.push_back(curr);
            if (curr == startCity) break;
            auto it = parent.find(curr);
            if (it != parent.end()) {
                curr = it->second;
            } else {
                break;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

class Graph {
private:
    std::vector<std::string> cityList;
    std::unordered_map<std::string, std::vector<Edge>> adjList;

public:
    void addLocation(const std::string& name) {
        std::string trimmed = Utils::trim(name);
        if (trimmed.empty()) return;
        if (!hasLocation(trimmed)) {
            cityList.push_back(trimmed);
            adjList[trimmed] = {};
        }
    }

    bool hasLocation(const std::string& name) const {
        return adjList.find(name) != adjList.end();
    }

    void addConnection(const std::string& from, const std::string& to, int distance) {
        if (from.empty() || to.empty() || distance <= 0) return;
        addLocation(from);
        addLocation(to);

        for (auto& edge : adjList[from]) {
            if (edge.to == to) {
                edge.distance = distance;
                for (auto& backEdge : adjList[to]) {
                    if (backEdge.to == from) {
                        backEdge.distance = distance;
                        return;
                    }
                }
                return;
            }
        }

        adjList[from].push_back({to, distance});
        adjList[to].push_back({from, distance});
    }

    const std::vector<std::string>& getLocations() const {
        return cityList;
    }

    const std::vector<Edge>& getNeighbors(const std::string& city) const {
        static const std::vector<Edge> empty;
        auto it = adjList.find(city);
        if (it != adjList.end()) return it->second;
        return empty;
    }

    const std::unordered_map<std::string, std::vector<Edge>>& getAllConnections() const {
        return adjList;
    }

    DijkstraResult findShortestPath(const std::string& start) const {
        DijkstraResult result;
        result.startCity = start;

        for (const auto& city : cityList) {
            result.distances[city] = std::numeric_limits<int>::max();
        }
        result.distances[start] = 0;

        typedef std::pair<int, std::string> DistPair;
        std::priority_queue<DistPair, std::vector<DistPair>, std::greater<DistPair>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            int d = pq.top().first;
            std::string u = pq.top().second;
            pq.pop();

            if (d > result.distances[u]) continue;

            auto it = adjList.find(u);
            if (it == adjList.end()) continue;

            for (const auto& edge : it->second) {
                const std::string& v = edge.to;
                int weight = edge.distance;

                if (result.distances[u] + weight < result.distances[v]) {
                    result.distances[v] = result.distances[u] + weight;
                    result.parent[v] = u;
                    pq.push({result.distances[v], v});
                }
            }
        }

        return result;
    }

    void displayNetwork() const {
        std::cout << Utils::Color::BOLD_CYAN << "\n[+] Current Highway & City Route Network:" << Utils::Color::RESET << "\n";
        Utils::printDivider(74);
        std::cout << std::left << std::setw(6) << "No." 
                  << std::setw(20) << "City / Node" 
                  << "Connected Direct Neighbors (Distance in KM)\n";
        Utils::printDivider(74);

        for (size_t i = 0; i < cityList.size(); ++i) {
            const std::string& city = cityList[i];
            std::cout << std::left << std::setw(6) << (i + 1)
                      << std::setw(20) << city;
            
            const auto& edges = getNeighbors(city);
            if (edges.empty()) {
                std::cout << Utils::Color::DIM << "(No direct roads)" << Utils::Color::RESET;
            } else {
                for (size_t j = 0; j < edges.size(); ++j) {
                    std::cout << edges[j].to << " (" << edges[j].distance << "km)";
                    if (j + 1 < edges.size()) std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
        Utils::printDivider(74);
    }

    void clear() {
        cityList.clear();
        adjList.clear();
    }
};

// ============================================================================
// 3. TRANSPORT HIERARCHY
// ============================================================================
enum class TransportType {
    NON_AC_BUS = 1,
    AC_BUS,
    EXPRESS_TRAIN,
    DOMESTIC_FLIGHT
};

class Transport {
protected:
    std::string name;
    double speedKmh;
    double farePerKm;
    TransportType type;

public:
    Transport(const std::string& n, double speed, double fare, TransportType t)
        : name(n), speedKmh(speed), farePerKm(fare), type(t) {}
    virtual ~Transport() = default;

    std::string getName() const { return name; }
    double getSpeed() const { return speedKmh; }
    double getFarePerKm() const { return farePerKm; }
    TransportType getType() const { return type; }

    virtual double calculateFare(double distanceKm) const {
        return distanceKm * farePerKm;
    }

    virtual double calculateDurationHours(double distanceKm) const {
        return distanceKm / speedKmh;
    }

    virtual std::string formatDuration(double distanceKm) const {
        double hours = calculateDurationHours(distanceKm);
        int h = static_cast<int>(hours);
        int m = static_cast<int>((hours - h) * 60);
        std::ostringstream oss;
        if (h > 0) oss << h << "h ";
        oss << m << "m";
        return oss.str();
    }

    static std::shared_ptr<Transport> createTransport(TransportType t);
};

class NonACBus : public Transport {
public:
    NonACBus() : Transport("Economy Non-AC Bus", 45.0, 1.80, TransportType::NON_AC_BUS) {}
};

class ACBus : public Transport {
public:
    ACBus() : Transport("Executive AC Bus", 55.0, 2.80, TransportType::AC_BUS) {}
};

class ExpressTrain : public Transport {
public:
    ExpressTrain() : Transport("Intercity Express Train", 60.0, 1.50, TransportType::EXPRESS_TRAIN) {}
};

class DomesticFlight : public Transport {
public:
    DomesticFlight() : Transport("Domestic Air Flight", 450.0, 12.00, TransportType::DOMESTIC_FLIGHT) {}
    
    double calculateFare(double distanceKm) const override {
        return 2500.0 + (distanceKm * farePerKm * 0.75);
    }
};

inline std::shared_ptr<Transport> Transport::createTransport(TransportType t) {
    switch (t) {
        case TransportType::NON_AC_BUS:     return std::make_shared<NonACBus>();
        case TransportType::AC_BUS:         return std::make_shared<ACBus>();
        case TransportType::EXPRESS_TRAIN:  return std::make_shared<ExpressTrain>();
        case TransportType::DOMESTIC_FLIGHT:return std::make_shared<DomesticFlight>();
    }
    return std::make_shared<ACBus>();
}

// ============================================================================
// 4. USER & INHERITANCE HIERARCHY
// ============================================================================
enum class UserRole { CUSTOMER, ADMIN };
enum class CustomerTier { SILVER, GOLD, PLATINUM };

class User {
protected:
    std::string username;
    size_t passwordHash;
    std::string fullName;
    std::string phone;
    std::string email;
    UserRole role;

public:
    User(const std::string& uname, size_t passHash, const std::string& name,
         const std::string& ph, const std::string& em, UserRole r)
        : username(uname), passwordHash(passHash), fullName(name), phone(ph), email(em), role(r) {}
    virtual ~User() = default;

    std::string getUsername() const { return username; }
    size_t getPasswordHash() const { return passwordHash; }
    std::string getFullName() const { return fullName; }
    std::string getPhone() const { return phone; }
    std::string getEmail() const { return email; }
    UserRole getRole() const { return role; }

    bool verifyPassword(const std::string& plainPassword) const {
        return Utils::simpleHash(plainPassword) == passwordHash;
    }

    virtual void displayDashboard() const = 0;
    virtual std::string getRoleString() const = 0;
    virtual std::string serialize() const = 0;
};

class Customer : public User {
private:
    std::string address;
    double walletBalance;
    int loyaltyPoints;
    std::vector<std::string> bookingHistory;

public:
    Customer(const std::string& uname, size_t passHash, const std::string& name,
             const std::string& ph, const std::string& em, const std::string& addr,
             double balance = 500.0, int points = 50)
        : User(uname, passHash, name, ph, em, UserRole::CUSTOMER),
          address(addr), walletBalance(balance), loyaltyPoints(points) {}

    std::string getAddress() const { return address; }
    double getWalletBalance() const { return walletBalance; }
    int getLoyaltyPoints() const { return loyaltyPoints; }
    const std::vector<std::string>& getBookingHistory() const { return bookingHistory; }

    CustomerTier getTier() const {
        if (loyaltyPoints >= 500) return CustomerTier::PLATINUM;
        if (loyaltyPoints >= 200) return CustomerTier::GOLD;
        return CustomerTier::SILVER;
    }

    std::string getTierString() const {
        switch (getTier()) {
            case CustomerTier::PLATINUM: return "Platinum (10% Off)";
            case CustomerTier::GOLD:     return "Gold (5% Off)";
            case CustomerTier::SILVER:   return "Silver (Standard)";
        }
        return "Silver";
    }

    double getTierDiscountPercent() const {
        switch (getTier()) {
            case CustomerTier::PLATINUM: return 0.10;
            case CustomerTier::GOLD:     return 0.05;
            case CustomerTier::SILVER:   return 0.00;
        }
        return 0.0;
    }

    void addFunds(double amount) {
        if (amount > 0) walletBalance += amount;
    }

    bool deductFunds(double amount) {
        if (amount > 0 && walletBalance >= amount) {
            walletBalance -= amount;
            return true;
        }
        return false;
    }

    void addLoyaltyPoints(int points) {
        if (points > 0) loyaltyPoints += points;
    }

    bool redeemLoyaltyPoints(int points, double& cashEquivalent) {
        if (points > 0 && loyaltyPoints >= points) {
            loyaltyPoints -= points;
            cashEquivalent = points * 0.5;
            walletBalance += cashEquivalent;
            return true;
        }
        return false;
    }

    void addBookingId(const std::string& id) {
        bookingHistory.push_back(id);
    }

    void displayDashboard() const override {
        Utils::printHeader("CUSTOMER DASHBOARD: " + fullName);
        std::cout << Utils::Color::BOLD_CYAN << " Account Details:" << Utils::Color::RESET << "\n";
        std::cout << "  * Username     : " << username << "\n";
        std::cout << "  * Full Name    : " << fullName << "\n";
        std::cout << "  * Phone        : " << phone << "\n";
        std::cout << "  * Email        : " << email << "\n";
        std::cout << "  * Address      : " << address << "\n";
        std::cout << "  * Member Tier  : " << Utils::Color::BOLD_YELLOW << getTierString() << Utils::Color::RESET << "\n";
        std::cout << "  * Loyalty Pts  : " << Utils::Color::BOLD_GREEN << loyaltyPoints << " pts" << Utils::Color::RESET << "\n";
        std::cout << "  * Wallet Funds : " << Utils::Color::BOLD_GREEN << Utils::formatCurrency(walletBalance) << Utils::Color::RESET << "\n";
        std::cout << "  * Total Trips  : " << bookingHistory.size() << " booking(s)\n";
        Utils::printDivider(74);
    }

    std::string getRoleString() const override { return "Customer"; }

    std::string serialize() const override {
        std::ostringstream oss;
        oss << "CUSTOMER|" << username << "|" << passwordHash << "|" << fullName << "|"
            << phone << "|" << email << "|" << address << "|" << walletBalance << "|"
            << loyaltyPoints << "|";
        for (size_t i = 0; i < bookingHistory.size(); ++i) {
            oss << bookingHistory[i];
            if (i + 1 < bookingHistory.size()) oss << ",";
        }
        return oss.str();
    }
};

class Admin : public User {
private:
    std::string department;
    int accessLevel;

public:
    Admin(const std::string& uname, size_t passHash, const std::string& name,
          const std::string& ph, const std::string& em, const std::string& dept = "Operations",
          int level = 3)
        : User(uname, passHash, name, ph, em, UserRole::ADMIN),
          department(dept), accessLevel(level) {}

    void displayDashboard() const override {
        Utils::printHeader("ADMIN CONTROL DASHBOARD: " + fullName);
        std::cout << Utils::Color::BOLD_CYAN << " Admin Profile:" << Utils::Color::RESET << "\n";
        std::cout << "  * Username     : " << username << "\n";
        std::cout << "  * Full Name    : " << fullName << "\n";
        std::cout << "  * Department   : " << department << "\n";
        std::cout << "  * Access Level : Level " << accessLevel << " (Full System Privileges)\n";
        std::cout << "  * Official Tel : " << phone << "\n";
        std::cout << "  * Email        : " << email << "\n";
        Utils::printDivider(74);
    }

    std::string getRoleString() const override { return "Administrator"; }

    std::string serialize() const override {
        std::ostringstream oss;
        oss << "ADMIN|" << username << "|" << passwordHash << "|" << fullName << "|"
            << phone << "|" << email << "|" << department << "|" << accessLevel;
        return oss.str();
    }
};

// ============================================================================
// 5. TRIP & TOUR PACKAGES
// ============================================================================
class Trip {
protected:
    std::string destination;
    double baseCost;

public:
    Trip(const std::string& dest, double cost) : destination(dest), baseCost(cost) {}
    virtual ~Trip() = default;

    virtual std::string getDestination() const { return destination; }
    virtual double getBaseCost() const { return baseCost; }

    virtual void displayDetails() const = 0;
    virtual std::string getTripType() const = 0;
    virtual std::string serialize() const = 0;
};

class TourPackage : public Trip {
private:
    std::string packageId;
    std::string packageName;
    int durationDays;
    int durationNights;
    std::string hotelRating;
    std::string inclusions;
    std::string description;

public:
    TourPackage(const std::string& id, const std::string& name, const std::string& dest,
                double cost, int days, int nights, const std::string& hotel,
                const std::string& inc, const std::string& desc)
        : Trip(dest, cost), packageId(id), packageName(name),
          durationDays(days), durationNights(nights), hotelRating(hotel),
          inclusions(inc), description(desc) {}

    std::string getPackageId() const { return packageId; }
    std::string getPackageName() const { return packageName; }
    int getDurationDays() const { return durationDays; }
    int getDurationNights() const { return durationNights; }
    std::string getHotelRating() const { return hotelRating; }
    std::string getInclusions() const { return inclusions; }
    std::string getDescription() const { return description; }

    void displayDetails() const override {
        std::cout << Utils::Color::BOLD_CYAN << "┌────────────────────────────────────────────────────────────────────────┐" << Utils::Color::RESET << "\n";
        std::cout << "│ " << Utils::Color::BOLD_YELLOW << std::left << std::setw(6) << packageId 
                  << std::setw(38) << packageName 
                  << "Price: " << std::setw(18) << Utils::formatCurrency(baseCost) 
                  << Utils::Color::RESET << "│\n";
        std::cout << "│ " << std::left << "Destination : " << std::setw(20) << destination 
                  << "Duration   : " << durationDays << " Days, " << durationNights << " Nights" 
                  << std::string(17, ' ') << "│\n";
        std::cout << "│ " << std::left << "Hotel Class : " << std::setw(56) << hotelRating << "│\n";
        std::cout << "│ " << std::left << "Inclusions  : " << std::setw(56) << inclusions << "│\n";
        std::cout << "│ " << Utils::Color::DIM << std::left << "Info        : " << std::setw(56) << description << Utils::Color::RESET << "│\n";
        std::cout << Utils::Color::BOLD_CYAN << "└────────────────────────────────────────────────────────────────────────┘" << Utils::Color::RESET << "\n";
    }

    std::string getTripType() const override { return "Tour Package"; }

    std::string serialize() const override {
        std::ostringstream oss;
        oss << "PKG|" << packageId << "|" << packageName << "|" << destination << "|"
            << baseCost << "|" << durationDays << "|" << durationNights << "|"
            << hotelRating << "|" << inclusions << "|" << description;
        return oss.str();
    }
};

class CustomTrip : public Trip {
private:
    std::string source;
    std::vector<std::string> route;
    int distanceKm;
    std::shared_ptr<Transport> transportMode;

public:
    CustomTrip(const std::string& src, const std::string& dest,
               const std::vector<std::string>& path, int dist,
               std::shared_ptr<Transport> transport)
        : Trip(dest, transport ? transport->calculateFare(dist) : static_cast<double>(dist)),
          source(src), route(path), distanceKm(dist), transportMode(transport) {}

    std::string getRouteString() const {
        std::string result = "";
        for (size_t i = 0; i < route.size(); ++i) {
            result += route[i];
            if (i + 1 < route.size()) result += " -> ";
        }
        return result;
    }

    void displayDetails() const override {
        std::cout << Utils::Color::BOLD_CYAN << "┌────────────────────────────────────────────────────────────────────────┐" << Utils::Color::RESET << "\n";
        std::cout << "│ " << Utils::Color::BOLD_YELLOW << "CUSTOM ROUTE TRIP: " << source << " to " << destination << Utils::Color::RESET 
                  << std::string(std::max(0, 70 - (21 + static_cast<int>(source.length() + destination.length()))), ' ') << "│\n";
        std::cout << "│ " << "Shortest Distance : " << std::left << std::setw(15) << (std::to_string(distanceKm) + " km")
                  << "Estimated Time : " << std::setw(20) << (transportMode ? transportMode->formatDuration(distanceKm) : "N/A") << "│\n";
        std::cout << "│ " << "Transport Mode    : " << std::left << std::setw(50) << (transportMode ? transportMode->getName() : "Standard") << "│\n";
        std::cout << "│ " << "Calculated Cost   : " << Utils::Color::BOLD_GREEN << std::left << std::setw(50) << Utils::formatCurrency(baseCost) << Utils::Color::RESET << "│\n";
        std::cout << "│ " << Utils::Color::CYAN << "Full Route Path   : " << std::left << std::setw(50) << getRouteString() << Utils::Color::RESET << "│\n";
        std::cout << Utils::Color::BOLD_CYAN << "└────────────────────────────────────────────────────────────────────────┘" << Utils::Color::RESET << "\n";
    }

    std::string getTripType() const override { return "Custom Route"; }

    std::string serialize() const override {
        std::ostringstream oss;
        int tType = transportMode ? static_cast<int>(transportMode->getType()) : 1;
        oss << "CUSTOM|" << source << "|" << destination << "|" << distanceKm << "|"
            << tType << "|" << baseCost << "|";
        for (size_t i = 0; i < route.size(); ++i) {
            oss << route[i];
            if (i + 1 < route.size()) oss << ",";
        }
        return oss.str();
    }
};

// ============================================================================
// 6. PAYMENT STRATEGIES
// ============================================================================
enum class PaymentType { CASH, BKASH, NAGAD, CREDIT_CARD, WALLET };

class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual bool process(double amount, std::string& transactionId, std::string& errorMsg) = 0;
    virtual std::string getMethodName() const = 0;
    virtual PaymentType getType() const = 0;
};

class CashPayment : public PaymentMethod {
public:
    bool process(double amount, std::string& transactionId, std::string& errorMsg) override {
        (void)amount;
        (void)errorMsg;
        transactionId = "CASH-PAID-ON-DEPARTURE";
        return true;
    }
    std::string getMethodName() const override { return "Cash on Departure"; }
    PaymentType getType() const override { return PaymentType::CASH; }
};

class DigitalWalletPayment : public PaymentMethod {
private:
    std::string provider;
    std::string accountPhone;
    std::string pin;

public:
    DigitalWalletPayment(const std::string& prov, const std::string& phone, const std::string& p)
        : provider(prov), accountPhone(phone), pin(p) {}

    bool process(double amount, std::string& transactionId, std::string& errorMsg) override {
        if (accountPhone.length() < 11) {
            errorMsg = "Invalid " + provider + " phone number (must be 11 digits)!";
            return false;
        }
        if (pin.length() < 4) {
            errorMsg = "Invalid security PIN length!";
            return false;
        }
        static int txCounter = 83921;
        std::ostringstream oss;
        oss << provider[0] << "TXN" << (++txCounter) << "BDT" << static_cast<int>(amount);
        transactionId = oss.str();
        return true;
    }

    std::string getMethodName() const override { return provider + " Digital Wallet (" + accountPhone + ")"; }
    PaymentType getType() const override {
        return (provider == "bKash") ? PaymentType::BKASH : PaymentType::NAGAD;
    }
};

class CardPayment : public PaymentMethod {
private:
    std::string cardNumber;
    std::string cardHolder;
    std::string expiry;

public:
    CardPayment(const std::string& cardNo, const std::string& holder, const std::string& exp)
        : cardNumber(cardNo), cardHolder(holder), expiry(exp) {}

    bool process(double amount, std::string& transactionId, std::string& errorMsg) override {
        if (cardNumber.length() < 15) {
            errorMsg = "Invalid credit/debit card number!";
            return false;
        }
        static int cardTxCounter = 54109;
        std::ostringstream oss;
        oss << "VISA-AUTH-" << (++cardTxCounter);
        transactionId = oss.str();
        (void)amount;
        return true;
    }

    std::string getMethodName() const override {
        std::string masked = "Card ending in " + (cardNumber.length() >= 4 ? cardNumber.substr(cardNumber.length() - 4) : "****");
        return masked;
    }
    PaymentType getType() const override { return PaymentType::CREDIT_CARD; }
};

// ============================================================================
// 7. BOOKING & BOARDING PASS
// ============================================================================
enum class BookingStatus { CONFIRMED, CANCELLED, COMPLETED };

class Booking {
private:
    std::string bookingId;
    std::string username;
    std::string customerName;
    std::string customerPhone;
    std::string tripType;
    std::string tripTitle;
    std::string source;
    std::string destination;
    std::string routePath;
    std::string transportName;
    int distanceKm;
    std::string bookingDate;
    double subtotal;
    double discountAmount;
    std::string couponCode;
    double vatAmount;
    double finalCost;
    BookingStatus status;
    std::string paymentMethod;
    std::string transactionId;

public:
    Booking()
        : distanceKm(0), subtotal(0), discountAmount(0), vatAmount(0),
          finalCost(0), status(BookingStatus::CONFIRMED) {}

    Booking(const std::string& id, const std::string& uname, const std::string& name,
            const std::string& phone, const std::string& type, const std::string& title,
            const std::string& src, const std::string& dest, const std::string& path,
            const std::string& transport, int dist, const std::string& date,
            double sub, double disc, const std::string& coupon, double vat,
            double total, BookingStatus stat, const std::string& payMethod,
            const std::string& txId)
        : bookingId(id), username(uname), customerName(name), customerPhone(phone),
          tripType(type), tripTitle(title), source(src), destination(dest),
          routePath(path), transportName(transport), distanceKm(dist),
          bookingDate(date), subtotal(sub), discountAmount(disc), couponCode(coupon),
          vatAmount(vat), finalCost(total), status(stat), paymentMethod(payMethod),
          transactionId(txId) {}

    std::string getBookingId() const { return bookingId; }
    std::string getUsername() const { return username; }
    std::string getCustomerName() const { return customerName; }
    std::string getCustomerPhone() const { return customerPhone; }
    std::string getTripType() const { return tripType; }
    std::string getTripTitle() const { return tripTitle; }
    std::string getSource() const { return source; }
    std::string getDestination() const { return destination; }
    std::string getRoutePath() const { return routePath; }
    std::string getTransportName() const { return transportName; }
    int getDistanceKm() const { return distanceKm; }
    std::string getBookingDate() const { return bookingDate; }
    double getSubtotal() const { return subtotal; }
    double getDiscountAmount() const { return discountAmount; }
    std::string getCouponCode() const { return couponCode; }
    double getVatAmount() const { return vatAmount; }
    double getFinalCost() const { return finalCost; }
    BookingStatus getStatus() const { return status; }
    std::string getPaymentMethod() const { return paymentMethod; }
    std::string getTransactionId() const { return transactionId; }

    std::string getStatusString() const {
        switch (status) {
            case BookingStatus::CONFIRMED: return "CONFIRMED";
            case BookingStatus::CANCELLED: return "CANCELLED";
            case BookingStatus::COMPLETED: return "COMPLETED";
        }
        return "UNKNOWN";
    }

    void cancelBooking() {
        status = BookingStatus::CANCELLED;
    }

    void displaySummary() const {
        std::string statColor = (status == BookingStatus::CONFIRMED) ? Utils::Color::BOLD_GREEN :
                                (status == BookingStatus::CANCELLED ? Utils::Color::BOLD_RED : Utils::Color::CYAN);

        std::cout << "• " << Utils::Color::BOLD << std::left << std::setw(15) << bookingId << Utils::Color::RESET
                  << " | " << std::left << std::setw(12) << username
                  << " | " << std::left << std::setw(22) << (tripTitle.length() > 20 ? tripTitle.substr(0, 18) + ".." : tripTitle)
                  << " | " << std::right << std::setw(12) << Utils::formatCurrency(finalCost)
                  << " | " << statColor << std::setw(11) << getStatusString() << Utils::Color::RESET
                  << " | Date: " << bookingDate << "\n";
    }

    void printBoardingPass() const {
        std::cout << Utils::Color::BOLD_CYAN
                  << "╔══════════════════════════════════════════════════════════════════════════╗\n"
                  << "║                       BANGLADESH TOURISM & TRAVEL                        ║\n"
                  << "║                         OFFICIAL BOARDING TICKET                         ║\n"
                  << "╠══════════════════════════════════════════════════════════════════════════╣"
                  << Utils::Color::RESET << "\n";
        
        std::cout << "║ " << std::left << std::setw(20) << "Booking Reference" << ": " 
                  << Utils::Color::BOLD_YELLOW << std::setw(48) << bookingId << Utils::Color::RESET << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Passenger Name" << ": " 
                  << std::setw(48) << customerName << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Contact Phone" << ": " 
                  << std::setw(48) << customerPhone << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Trip Category" << ": " 
                  << std::setw(48) << tripType << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Journey Title" << ": " 
                  << std::setw(48) << tripTitle << "║\n";
        if (!source.empty() && !destination.empty()) {
            std::cout << "║ " << std::left << std::setw(20) << "Route Origin-Dest" << ": " 
                      << std::setw(48) << (source + " ===> " + destination) << "║\n";
        }
        if (!routePath.empty()) {
            std::cout << "║ " << std::left << std::setw(20) << "Itinerary Nodes" << ": " 
                      << std::setw(48) << (routePath.length() > 48 ? routePath.substr(0, 45) + "..." : routePath) << "║\n";
        }
        std::cout << "║ " << std::left << std::setw(20) << "Transport Vehicle" << ": " 
                  << std::setw(48) << transportName << "║\n";
        if (distanceKm > 0) {
            std::cout << "║ " << std::left << std::setw(20) << "Total Distance" << ": " 
                      << std::setw(48) << (std::to_string(distanceKm) + " KM") << "║\n";
        }
        std::cout << "║ " << std::left << std::setw(20) << "Issued Timestamp" << ": " 
                  << std::setw(48) << bookingDate << "║\n";

        std::cout << Utils::Color::BOLD_CYAN 
                  << "╠══════════════════════════════════════════════════════════════════════════╣" 
                  << Utils::Color::RESET << "\n";

        std::cout << "║ " << std::left << std::setw(20) << "Base Subtotal" << ": " 
                  << std::setw(48) << Utils::formatCurrency(subtotal) << "║\n";
        if (discountAmount > 0) {
            std::string discInfo = "-" + Utils::formatCurrency(discountAmount) + " (" + (couponCode.empty() ? "Tier Discount" : couponCode) + ")";
            std::cout << "║ " << std::left << std::setw(20) << "Promo / Loyalty Disc" << ": " 
                      << Utils::Color::BOLD_GREEN << std::setw(48) << discInfo << Utils::Color::RESET << "║\n";
        }
        std::cout << "║ " << std::left << std::setw(20) << "Govt VAT (5%)" << ": " 
                  << std::setw(48) << Utils::formatCurrency(vatAmount) << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "TOTAL CHARGE" << ": " 
                  << Utils::Color::BOLD_GREEN << std::setw(48) << Utils::formatCurrency(finalCost) << Utils::Color::RESET << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Payment Strategy" << ": " 
                  << std::setw(48) << paymentMethod << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Txn Identifier" << ": " 
                  << std::setw(48) << transactionId << "║\n";
        std::cout << "║ " << std::left << std::setw(20) << "Ticket Status" << ": " 
                  << Utils::Color::BOLD << std::setw(48) << getStatusString() << Utils::Color::RESET << "║\n";

        std::cout << Utils::Color::BOLD_CYAN
                  << "╚══════════════════════════════════════════════════════════════════════════╝\n"
                  << Utils::Color::RESET;
    }

    bool exportTicketToFile(const std::string& directory = "receipts") const {
        CREATE_DIR(directory.c_str());
        std::string filePath = directory + "/TICKET_" + bookingId + ".txt";
        std::ofstream out(filePath);
        if (!out) return false;

        out << "========================================================================\n"
            << "                       BANGLADESH TOURISM & TRAVEL                      \n"
            << "                         OFFICIAL BOARDING PASS                         \n"
            << "========================================================================\n"
            << "Booking Reference : " << bookingId << "\n"
            << "Passenger Name    : " << customerName << "\n"
            << "Customer Phone    : " << customerPhone << "\n"
            << "Trip Category     : " << tripType << "\n"
            << "Tour Title        : " << tripTitle << "\n";
        if (!source.empty() && !destination.empty()) {
            out << "Route             : " << source << " to " << destination << "\n";
        }
        if (!routePath.empty()) {
            out << "Route Itinerary   : " << routePath << "\n";
        }
        out << "Transport Vehicle : " << transportName << "\n";
        if (distanceKm > 0) {
            out << "Total Distance    : " << distanceKm << " KM\n";
        }
        out << "Issue Timestamp   : " << bookingDate << "\n"
            << "------------------------------------------------------------------------\n"
            << "Base Subtotal     : " << Utils::formatCurrency(subtotal) << "\n"
            << "Discount Applied  : -" << Utils::formatCurrency(discountAmount) << " (" << (couponCode.empty() ? "Tier Discount" : couponCode) << ")\n"
            << "Govt VAT (5%)     : " << Utils::formatCurrency(vatAmount) << "\n"
            << "FINAL PAID AMOUNT : " << Utils::formatCurrency(finalCost) << "\n"
            << "Payment Method    : " << paymentMethod << "\n"
            << "Transaction ID    : " << transactionId << "\n"
            << "Booking Status    : " << getStatusString() << "\n"
            << "========================================================================\n"
            << "            Thank you for choosing Bangladesh Travel Management!        \n"
            << "              For 24/7 Helpline Support, call: +880 1700-000000        \n"
            << "========================================================================\n";
        out.close();
        return true;
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << bookingId << "|" << username << "|" << customerName << "|" << customerPhone << "|"
            << tripType << "|" << tripTitle << "|" << source << "|" << destination << "|"
            << routePath << "|" << transportName << "|" << distanceKm << "|" << bookingDate << "|"
            << subtotal << "|" << discountAmount << "|" << couponCode << "|" << vatAmount << "|"
            << finalCost << "|" << static_cast<int>(status) << "|" << paymentMethod << "|"
            << transactionId;
        return oss.str();
    }

    static Booking deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 20) return Booking();

        Booking b;
        b.bookingId = tokens[0];
        b.username = tokens[1];
        b.customerName = tokens[2];
        b.customerPhone = tokens[3];
        b.tripType = tokens[4];
        b.tripTitle = tokens[5];
        b.source = tokens[6];
        b.destination = tokens[7];
        b.routePath = tokens[8];
        b.transportName = tokens[9];
        b.distanceKm = std::stoi(tokens[10]);
        b.bookingDate = tokens[11];
        b.subtotal = std::stod(tokens[12]);
        b.discountAmount = std::stod(tokens[13]);
        b.couponCode = tokens[14];
        b.vatAmount = std::stod(tokens[15]);
        b.finalCost = std::stod(tokens[16]);
        b.status = static_cast<BookingStatus>(std::stoi(tokens[17]));
        b.paymentMethod = tokens[18];
        b.transactionId = tokens[19];
        return b;
    }
};

// ============================================================================
// 8. FILE PERSISTENCE SERVICE
// ============================================================================
class FileManager {
private:
    std::string dataDir;
    std::string locationsFile;
    std::string packagesFile;
    std::string usersFile;
    std::string bookingsFile;
    std::string couponsFile;

    void ensureDirectoryExists() const {
        CREATE_DIR(dataDir.c_str());
        CREATE_DIR("receipts");
    }

    void seedDefaultData(Graph& graph,
                         std::vector<TourPackage>& packages,
                         std::unordered_map<std::string, std::shared_ptr<User>>& users,
                         std::unordered_map<std::string, double>& coupons) const {
        std::vector<std::string> cities = {
            "Dhaka", "Chittagong", "Cox's Bazar", "Sylhet", "Khulna",
            "Rajshahi", "Rangpur", "Barisal", "Comilla", "Mymensingh",
            "Jessore", "Bogra", "Tangail", "Dinajpur", "Pabna",
            "Kushtia", "Saidpur", "Jamalpur", "Narayanganj", "Gazipur", "Narsingdi"
        };

        for (const auto& city : cities) {
            graph.addLocation(city);
        }

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

        users.clear();
        users["admin"] = std::make_shared<Admin>(
            "admin", Utils::simpleHash("admin123"), "System Administrator",
            "+880 1700-112233", "admin@travelbd.gov.bd", "Central Operations", 3
        );

        users["rahman"] = std::make_shared<Customer>(
            "rahman", Utils::simpleHash("pass123"), "Anisur Rahman",
            "+880 1712-345678", "rahman@gmail.com", "Dhanmondi 27, Dhaka", 25000.0, 250
        );

        coupons.clear();
        coupons["OOP100"]     = 1000.0;
        coupons["HOLIDAY20"]  = 0.20;
        coupons["BANGLADESH"] = 0.15;
        coupons["WELCOME50"]  = 500.0;
    }

public:
    FileManager(const std::string& dir = "data")
        : dataDir(dir),
          locationsFile(dir + "/locations.txt"),
          packagesFile(dir + "/packages.txt"),
          usersFile(dir + "/users.txt"),
          bookingsFile(dir + "/bookings.txt"),
          couponsFile(dir + "/coupons.txt") {}

    void initialize(Graph& graph,
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

        if (!locOk || !pkgOk || !usrOk || !cpnOk || graph.getLocations().empty()) {
            seedDefaultData(graph, packages, users, coupons);
            saveLocations(graph);
            savePackages(packages);
            saveUsers(users);
            saveCoupons(coupons);
        }
    }

    bool loadLocations(Graph& graph) const {
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
                graph.addLocation(line);
            }
        }
        return true;
    }

    bool saveLocations(const Graph& graph) const {
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
        return true;
    }

    bool loadPackages(std::vector<TourPackage>& packages) const {
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
            while (std::getline(ss, token, '|')) tokens.push_back(token);
            if (tokens.size() >= 10 && tokens[0] == "PKG") {
                try {
                    packages.push_back(TourPackage(tokens[1], tokens[2], tokens[3],
                                                   std::stod(tokens[4]), std::stoi(tokens[5]),
                                                   std::stoi(tokens[6]), tokens[7], tokens[8], tokens[9]));
                } catch (...) {}
            }
        }
        return true;
    }

    bool savePackages(const std::vector<TourPackage>& packages) const {
        std::ofstream file(packagesFile);
        if (!file) return false;
        file << "# Tour Packages Database\n";
        for (const auto& pkg : packages) file << pkg.serialize() << "\n";
        return true;
    }

    bool loadUsers(std::unordered_map<std::string, std::shared_ptr<User>>& users) const {
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
            while (std::getline(ss, token, '|')) tokens.push_back(token);

            if (tokens.size() >= 8 && tokens[0] == "ADMIN") {
                try {
                    users[tokens[1]] = std::make_shared<Admin>(
                        tokens[1], std::stoull(tokens[2]), tokens[3],
                        tokens[4], tokens[5], tokens[6], std::stoi(tokens[7])
                    );
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
                        while (std::getline(bss, bId, ',')) cust->addBookingId(Utils::trim(bId));
                    }
                    users[tokens[1]] = cust;
                } catch (...) {}
            }
        }
        return true;
    }

    bool saveUsers(const std::unordered_map<std::string, std::shared_ptr<User>>& users) const {
        std::ofstream file(usersFile);
        if (!file) return false;
        file << "# User Accounts Database\n";
        for (const auto& pair : users) {
            if (pair.second) file << pair.second->serialize() << "\n";
        }
        return true;
    }

    bool loadBookings(std::vector<Booking>& bookings) const {
        std::ifstream file(bookingsFile);
        if (!file) return false;
        bookings.clear();
        std::string line;
        while (std::getline(file, line)) {
            line = Utils::trim(line);
            if (line.empty() || line[0] == '#') continue;
            Booking b = Booking::deserialize(line);
            if (!b.getBookingId().empty()) bookings.push_back(b);
        }
        return true;
    }

    bool saveBookings(const std::vector<Booking>& bookings) const {
        std::ofstream file(bookingsFile);
        if (!file) return false;
        file << "# Bookings Registry\n";
        for (const auto& b : bookings) file << b.serialize() << "\n";
        return true;
    }

    bool loadCoupons(std::unordered_map<std::string, double>& coupons) const {
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
                try { coupons[Utils::trim(code)] = std::stod(Utils::trim(valStr)); } catch (...) {}
            }
        }
        return true;
    }

    bool saveCoupons(const std::unordered_map<std::string, double>& coupons) const {
        std::ofstream file(couponsFile);
        if (!file) return false;
        file << "# Coupons\n";
        for (const auto& pair : coupons) file << pair.first << "|" << pair.second << "\n";
        return true;
    }
};

// ============================================================================
// 9. UI & SERVICES
// ============================================================================
class ConsoleUI {
public:
    static void showBanner() {
        std::cout << Utils::Color::BOLD_CYAN << R"(
  ████████╗██████╗  █████╗ ██╗   ██╗███████╗██╗     ██████╗ ██████╗ 
  ╚══██╔══╝██╔══██╗██╔══██╗██║   ██║██╔════╝██║     ██╔══██╗██╔══██╗
     ██║   ██████╔╝███████║██║   ██║█████╗  ██║     ██████╔╝██║  ██║
     ██║   ██╔══██╗██╔══██║╚██╗ ██╔╝██╔══╝  ██║     ██╔══██╗██║  ██║
     ██║   ██║  ██║██║  ██║ ╚████╔╝ ███████╗███████╗██████╔╝██████╔╝
     ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚══════╝╚═════╝ ╚═════╝ 
       🇧🇩  SMART TOURISM & TRAVEL MANAGEMENT SYSTEM - BANGLADESH  🇧🇩
)" << Utils::Color::RESET << "\n";
    }

    static int getIntInput(const std::string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            std::cout << Utils::Color::BOLD << prompt << Utils::Color::RESET;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << Utils::Color::BOLD_RED << " [-] Invalid range (" << minVal << " - " << maxVal << ")!" << Utils::Color::RESET << "\n";
                }
            } else {
                std::cout << Utils::Color::BOLD_RED << " [-] Invalid integer!" << Utils::Color::RESET << "\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    static double getDoubleInput(const std::string& prompt, double minVal = 0.0, double maxVal = 1000000.0) {
        double value;
        while (true) {
            std::cout << Utils::Color::BOLD << prompt << Utils::Color::RESET;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << Utils::Color::BOLD_RED << " [-] Out of range (" << minVal << " - " << maxVal << ")!" << Utils::Color::RESET << "\n";
                }
            } else {
                std::cout << Utils::Color::BOLD_RED << " [-] Invalid numeric value!" << Utils::Color::RESET << "\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    static std::string getStringInput(const std::string& prompt, bool allowSpaces = true) {
        std::string value;
        while (true) {
            std::cout << Utils::Color::BOLD << prompt << Utils::Color::RESET;
            if (allowSpaces) {
                std::getline(std::cin, value);
            } else {
                std::cin >> value;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            value = Utils::trim(value);
            if (!value.empty()) return value;
            std::cout << Utils::Color::BOLD_RED << " [-] Input cannot be empty!" << Utils::Color::RESET << "\n";
        }
    }

    static void showMainMenu() {
        Utils::printHeader("BANGLADESH TRAVEL MANAGEMENT SYSTEM");
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Customer Portal Login\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Customer Registration (New Account)\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " Administrator Portal Login\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " Browse Available Tour Packages\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " View Highway Route Network & Map\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Exit Application\n";
        Utils::printDivider(74);
    }

    static void showCustomerMenu(const std::string& name, double wallet, const std::string& tier) {
        Utils::printHeader("CUSTOMER DASHBOARD: " + name);
        std::cout << "  " << Utils::Color::BOLD_GREEN << "Wallet Balance: " << Utils::formatCurrency(wallet) 
                  << " | Tier: " << tier << Utils::Color::RESET << "\n";
        Utils::printDivider(74);
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Explore & Book Curated Tour Packages\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Plan Custom Trip (Shortest Route & Vehicle Selection)\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " My Booking History & Download Tickets\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " Cancel a Booking (Refund to Wallet)\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " Top-Up Travel Wallet Funds\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Redeem Loyalty Points for Cash\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[7]" << Utils::Color::RESET << " View My Account Profile\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[8]" << Utils::Color::RESET << " Logout to Main Menu\n";
        Utils::printDivider(74);
    }

    static void showAdminMenu(const std::string& adminName) {
        Utils::printHeader("ADMINISTRATOR CONTROL PANEL (" + adminName + ")");
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Manage Highway Routes (Add City / Road Connection)\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Manage Tour Packages (Add / Remove Package)\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " View All Registered Customers\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " View Master Booking Registry\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " System Financial & Booking Analytics\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Manage Promotional Discount Coupons\n";
        std::cout << "  " << Utils::Color::BOLD_CYAN << "[7]" << Utils::Color::RESET << " Logout to Main Menu\n";
        Utils::printDivider(74);
    }

    static void renderCityGrid(const std::vector<std::string>& cities) {
        std::cout << Utils::Color::BOLD_CYAN << "\n[+] Available Highway Network Destinations (" << cities.size() << " Cities):" << Utils::Color::RESET << "\n";
        Utils::printDivider(74);
        for (size_t i = 0; i < cities.size(); ++i) {
            std::cout << std::right << std::setw(3) << (i + 1) << ". " 
                      << std::left << std::setw(18) << cities[i];
            if ((i + 1) % 3 == 0 || i + 1 == cities.size()) std::cout << "\n";
        }
        Utils::printDivider(74);
    }

    static void renderPackagesCatalog(const std::vector<TourPackage>& packages) {
        Utils::printHeader("FEATURED BANGLADESH TOUR PACKAGES");
        if (packages.empty()) {
            std::cout << "  No packages currently published.\n";
            return;
        }
        for (size_t i = 0; i < packages.size(); ++i) {
            std::cout << Utils::Color::BOLD_YELLOW << " [" << (i + 1) << "] " << Utils::Color::RESET;
            packages[i].displayDetails();
            std::cout << "\n";
        }
    }

    static void renderBookingsTable(const std::vector<Booking>& bookings, const std::string& title = "ALL BOOKINGS") {
        Utils::printHeader(title);
        if (bookings.empty()) {
            std::cout << "  No booking records found.\n";
            Utils::printDivider(74);
            return;
        }

        std::cout << Utils::Color::BOLD << std::left 
                  << std::setw(16) << "Booking ID"
                  << std::setw(14) << "Customer"
                  << std::setw(24) << "Tour / Route"
                  << std::setw(14) << "Amount"
                  << std::setw(12) << "Status"
                  << Utils::Color::RESET << "\n";
        Utils::printDivider(74);

        for (const auto& b : bookings) {
            std::string statColor = (b.getStatus() == BookingStatus::CONFIRMED) ? Utils::Color::BOLD_GREEN :
                                    (b.getStatus() == BookingStatus::CANCELLED ? Utils::Color::BOLD_RED : Utils::Color::CYAN);
            std::cout << std::left 
                      << std::setw(16) << b.getBookingId()
                      << std::setw(14) << b.getUsername()
                      << std::setw(24) << (b.getTripTitle().length() > 22 ? b.getTripTitle().substr(0, 20) + ".." : b.getTripTitle())
                      << std::setw(14) << Utils::formatCurrency(b.getFinalCost())
                      << statColor << std::setw(12) << b.getStatusString() << Utils::Color::RESET << "\n";
        }
        Utils::printDivider(74);
    }

    static void renderCustomersTable(const std::unordered_map<std::string, std::shared_ptr<User>>& users) {
        Utils::printHeader("REGISTERED CUSTOMER DIRECTORY");
        std::cout << Utils::Color::BOLD << std::left 
                  << std::setw(14) << "Username"
                  << std::setw(22) << "Full Name"
                  << std::setw(16) << "Phone Number"
                  << std::setw(12) << "Tier"
                  << std::setw(14) << "Wallet Bal."
                  << Utils::Color::RESET << "\n";
        Utils::printDivider(74);

        int count = 0;
        for (const auto& pair : users) {
            if (pair.second && pair.second->getRole() == UserRole::CUSTOMER) {
                auto cust = std::dynamic_pointer_cast<Customer>(pair.second);
                if (cust) {
                    count++;
                    std::cout << std::left 
                              << std::setw(14) << cust->getUsername()
                              << std::setw(22) << (cust->getFullName().length() > 20 ? cust->getFullName().substr(0, 18) + ".." : cust->getFullName())
                              << std::setw(16) << cust->getPhone()
                              << std::setw(12) << (cust->getTier() == CustomerTier::PLATINUM ? "Platinum" : (cust->getTier() == CustomerTier::GOLD ? "Gold" : "Silver"))
                              << std::setw(14) << Utils::formatCurrency(cust->getWalletBalance())
                              << "\n";
                }
            }
        }
        if (count == 0) std::cout << "  No registered customers found.\n";
        Utils::printDivider(74);
    }

    static void renderRevenueAnalytics(const std::vector<Booking>& bookings) {
        Utils::printHeader("FINANCIAL & BOOKING ANALYTICS");
        double totalRevenue = 0.0, totalDiscounts = 0.0, totalVat = 0.0;
        int confirmedCount = 0, cancelledCount = 0;
        std::unordered_map<std::string, int> destinationPopularity;

        for (const auto& b : bookings) {
            if (b.getStatus() == BookingStatus::CONFIRMED) {
                totalRevenue += b.getFinalCost();
                totalDiscounts += b.getDiscountAmount();
                totalVat += b.getVatAmount();
                confirmedCount++;
                if (!b.getDestination().empty()) destinationPopularity[b.getDestination()]++;
            } else if (b.getStatus() == BookingStatus::CANCELLED) {
                cancelledCount++;
            }
        }

        std::cout << "  " << Utils::Color::BOLD_GREEN << "• Gross Confirmed Revenue : " << Utils::formatCurrency(totalRevenue) << Utils::Color::RESET << "\n";
        std::cout << "  • Total Discounts Applied : " << Utils::formatCurrency(totalDiscounts) << "\n";
        std::cout << "  • Total VAT Collected     : " << Utils::formatCurrency(totalVat) << "\n";
        std::cout << "  • Confirmed Trips         : " << confirmedCount << "\n";
        std::cout << "  • Cancelled Reservations  : " << cancelledCount << "\n";
        Utils::printDivider(74);
        std::cout << Utils::Color::BOLD_CYAN << "  Top Traveled Destinations:" << Utils::Color::RESET << "\n";
        for (const auto& pair : destinationPopularity) {
            std::cout << "    * " << std::left << std::setw(20) << pair.first << ": " << pair.second << " visits\n";
        }
        Utils::printDivider(74);
    }
};

// ============================================================================
// 10. AUTH & BOOKING SERVICES
// ============================================================================
class AuthService {
private:
    std::unordered_map<std::string, std::shared_ptr<User>>& users;

public:
    explicit AuthService(std::unordered_map<std::string, std::shared_ptr<User>>& userMap)
        : users(userMap) {}

    bool registerCustomer(const std::string& username, const std::string& plainPassword,
                          const std::string& fullName, const std::string& phone,
                          const std::string& email, const std::string& address,
                          std::string& errorMsg) {
        std::string uname = Utils::trim(username);
        if (uname.length() < 3) {
            errorMsg = "Username must be at least 3 characters.";
            return false;
        }
        if (plainPassword.length() < 4) {
            errorMsg = "Password must be at least 4 characters.";
            return false;
        }
        if (users.find(uname) != users.end()) {
            errorMsg = "Username '" + uname + "' already exists.";
            return false;
        }

        users[uname] = std::make_shared<Customer>(
            uname, Utils::simpleHash(plainPassword), fullName, phone, email, address, 500.0, 50
        );
        return true;
    }

    std::shared_ptr<User> login(const std::string& username, const std::string& plainPassword, std::string& errorMsg) {
        std::string uname = Utils::trim(username);
        auto it = users.find(uname);
        if (it == users.end()) {
            errorMsg = "Username not found.";
            return nullptr;
        }
        if (!it->second->verifyPassword(plainPassword)) {
            errorMsg = "Incorrect password.";
            return nullptr;
        }
        return it->second;
    }
};

class BookingService {
private:
    std::vector<Booking>& bookings;
    std::unordered_map<std::string, double>& coupons;
    Graph& graph;

public:
    BookingService(std::vector<Booking>& bookList,
                   std::unordered_map<std::string, double>& couponMap,
                   Graph& g)
        : bookings(bookList), coupons(couponMap), graph(g) {}

    double validateCoupon(const std::string& code, double subtotal) const {
        std::string cleanCode = Utils::toUpper(Utils::trim(code));
        auto it = coupons.find(cleanCode);
        if (it == coupons.end()) return 0.0;
        double val = it->second;
        if (val > 0 && val < 1.0) return subtotal * val;
        else if (val >= 1.0) return std::min(val, subtotal * 0.8);
        return 0.0;
    }

    bool createPackageBooking(std::shared_ptr<Customer> customer,
                              const TourPackage& package,
                              const std::string& couponCode,
                              std::shared_ptr<PaymentMethod> payment,
                              Booking& outBooking,
                              std::string& errorMsg) {
        if (!customer) { errorMsg = "No active session."; return false; }
        double subtotal = package.getBaseCost();
        double tierDiscount = subtotal * customer->getTierDiscountPercent();
        double couponDiscount = validateCoupon(couponCode, subtotal);
        double totalDiscount = tierDiscount + couponDiscount;
        double discountedSubtotal = std::max(0.0, subtotal - totalDiscount);
        double vat = discountedSubtotal * 0.05;
        double totalCost = discountedSubtotal + vat;

        std::string txId, payError;
        if (!payment->process(totalCost, txId, payError)) {
            errorMsg = payError;
            return false;
        }

        std::string bookingId = Utils::generateBookingId();
        std::string now = Utils::getCurrentTimestamp();

        Booking b(bookingId, customer->getUsername(), customer->getFullName(),
                  customer->getPhone(), "Tour Package", package.getPackageName(),
                  "Dhaka (HQ)", package.getDestination(), "Direct Tour Itinerary",
                  "Luxury AC Coach", 0, now, subtotal, totalDiscount,
                  couponCode, vat, totalCost, BookingStatus::CONFIRMED,
                  payment->getMethodName(), txId);

        bookings.push_back(b);
        customer->addBookingId(bookingId);
        customer->addLoyaltyPoints(static_cast<int>(totalCost / 100));
        b.exportTicketToFile("receipts");
        outBooking = b;
        return true;
    }

    bool createCustomTripBooking(std::shared_ptr<Customer> customer,
                                 const std::string& source,
                                 const std::string& destination,
                                 std::shared_ptr<Transport> transport,
                                 const std::string& couponCode,
                                 std::shared_ptr<PaymentMethod> payment,
                                 Booking& outBooking,
                                 std::string& errorMsg) {
        if (!customer || !transport) { errorMsg = "Invalid booking arguments."; return false; }
        DijkstraResult res = graph.findShortestPath(source);
        if (!res.hasPath(destination)) {
            errorMsg = "No route found between " + source + " and " + destination;
            return false;
        }

        int distance = res.getDistance(destination);
        std::vector<std::string> path = res.getPath(destination);
        CustomTrip trip(source, destination, path, distance, transport);

        double subtotal = trip.getBaseCost();
        double tierDiscount = subtotal * customer->getTierDiscountPercent();
        double couponDiscount = validateCoupon(couponCode, subtotal);
        double totalDiscount = tierDiscount + couponDiscount;
        double discountedSubtotal = std::max(0.0, subtotal - totalDiscount);
        double vat = discountedSubtotal * 0.05;
        double totalCost = discountedSubtotal + vat;

        std::string txId, payError;
        if (!payment->process(totalCost, txId, payError)) {
            errorMsg = payError;
            return false;
        }

        std::string bookingId = Utils::generateBookingId();
        std::string now = Utils::getCurrentTimestamp();

        Booking b(bookingId, customer->getUsername(), customer->getFullName(),
                  customer->getPhone(), "Custom Route", source + " to " + destination,
                  source, destination, trip.getRouteString(),
                  transport->getName(), distance, now, subtotal, totalDiscount,
                  couponCode, vat, totalCost, BookingStatus::CONFIRMED,
                  payment->getMethodName(), txId);

        bookings.push_back(b);
        customer->addBookingId(bookingId);
        customer->addLoyaltyPoints(static_cast<int>(totalCost / 100));
        b.exportTicketToFile("receipts");
        outBooking = b;
        return true;
    }

    bool cancelBooking(const std::string& bookingId, std::shared_ptr<Customer> customer, std::string& statusMsg) {
        for (auto& b : bookings) {
            if (b.getBookingId() == bookingId) {
                if (b.getUsername() != customer->getUsername()) {
                    statusMsg = "Access Denied: Not your booking!";
                    return false;
                }
                if (b.getStatus() == BookingStatus::CANCELLED) {
                    statusMsg = "Booking is already cancelled.";
                    return false;
                }
                b.cancelBooking();
                double refund = b.getFinalCost() * 0.90;
                customer->addFunds(refund);
                statusMsg = "Booking cancelled! 90% refund (" + Utils::formatCurrency(refund) + ") credited to wallet.";
                return true;
            }
        }
        statusMsg = "Booking ID not found.";
        return false;
    }

    std::vector<Booking> getCustomerBookings(const std::string& username) const {
        std::vector<Booking> res;
        for (const auto& b : bookings) if (b.getUsername() == username) res.push_back(b);
        return res;
    }

    const Booking* getBookingById(const std::string& bookingId) const {
        for (const auto& b : bookings) if (b.getBookingId() == bookingId) return &b;
        return nullptr;
    }
};

// ============================================================================
// 11. PAYMENT SELECTION & SESSION HANDLERS
// ============================================================================
std::shared_ptr<PaymentMethod> promptPaymentMethod(double amount, std::shared_ptr<Customer> customer) {
    std::cout << "\n" << Utils::Color::BOLD_CYAN << "[+] Choose Payment Strategy for " 
              << Utils::formatCurrency(amount) << ":" << Utils::Color::RESET << "\n";
    std::cout << "  1. bKash Mobile Wallet\n";
    std::cout << "  2. Nagad Digital Financial Service\n";
    std::cout << "  3. Visa / MasterCard Credit/Debit Card\n";
    std::cout << "  4. Internal Travel Wallet Balance (Available: " << Utils::formatCurrency(customer->getWalletBalance()) << ")\n";
    std::cout << "  5. Cash on Departure\n";

    int choice = ConsoleUI::getIntInput("Select payment option (1-5): ", 1, 5);

    switch (choice) {
        case 1: {
            std::string phone = ConsoleUI::getStringInput("Enter 11-digit bKash Number: ", false);
            std::string pin = Utils::getMaskedPassword("Enter bKash PIN: ");
            return std::make_shared<DigitalWalletPayment>("bKash", phone, pin);
        }
        case 2: {
            std::string phone = ConsoleUI::getStringInput("Enter 11-digit Nagad Number: ", false);
            std::string pin = Utils::getMaskedPassword("Enter Nagad PIN: ");
            return std::make_shared<DigitalWalletPayment>("Nagad", phone, pin);
        }
        case 3: {
            std::string cardNo = ConsoleUI::getStringInput("Enter 16-digit Card Number: ", false);
            std::string holder = ConsoleUI::getStringInput("Enter Cardholder Name: ", true);
            std::string exp = ConsoleUI::getStringInput("Enter Expiry (MM/YY): ", false);
            return std::make_shared<CardPayment>(cardNo, holder, exp);
        }
        case 4: {
            if (customer->getWalletBalance() < amount) {
                std::cout << Utils::Color::BOLD_RED << " [-] Insufficient wallet funds. Defaulting to Cash on Departure.\n" << Utils::Color::RESET;
                return std::make_shared<CashPayment>();
            }
            customer->deductFunds(amount);
            return std::make_shared<CashPayment>();
        }
        default:
            return std::make_shared<CashPayment>();
    }
}

void handleCustomerSession(std::shared_ptr<Customer> customer,
                           Graph& graph,
                           std::vector<TourPackage>& packages,
                           BookingService& bookingService) {
    int choice;
    do {
        Utils::clearScreen();
        ConsoleUI::showCustomerMenu(customer->getFullName(), customer->getWalletBalance(), customer->getTierString());
        choice = ConsoleUI::getIntInput("Choose action (1-8): ", 1, 8);

        switch (choice) {
            case 1: {
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                std::cout << "  [0] Back to Menu\n";
                int pkgChoice = ConsoleUI::getIntInput("Select Package # to Book: ", 0, static_cast<int>(packages.size()));
                if (pkgChoice > 0) {
                    const TourPackage& selectedPkg = packages[pkgChoice - 1];
                    std::cout << "\nYou selected: " << Utils::Color::BOLD_YELLOW << selectedPkg.getPackageName() 
                              << " (" << Utils::formatCurrency(selectedPkg.getBaseCost()) << ")" << Utils::Color::RESET << "\n";

                    std::string coupon = ConsoleUI::getStringInput("Enter Coupon Code (or Enter to skip): ", true);
                    double subtotal = selectedPkg.getBaseCost();
                    double disc = bookingService.validateCoupon(coupon, subtotal) + (subtotal * customer->getTierDiscountPercent());
                    double total = (subtotal - disc) * 1.05;

                    auto payment = promptPaymentMethod(total, customer);
                    Booking newBooking;
                    std::string error;
                    if (bookingService.createPackageBooking(customer, selectedPkg, coupon, payment, newBooking, error)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] Package Booked Successfully!" << Utils::Color::RESET << "\n";
                        newBooking.printBoardingPass();
                    } else {
                        std::cout << Utils::Color::BOLD_RED << "\n[-] Booking Failed: " << error << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 2: {
                Utils::clearScreen();
                Utils::printHeader("PLAN CUSTOM TRIP VIA HIGHWAY NETWORK");
                const auto& cities = graph.getLocations();
                ConsoleUI::renderCityGrid(cities);

                int srcIdx = ConsoleUI::getIntInput("Enter Origin City Number: ", 1, static_cast<int>(cities.size()));
                int destIdx = ConsoleUI::getIntInput("Enter Destination City Number: ", 1, static_cast<int>(cities.size()));

                if (srcIdx == destIdx) {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Origin and Destination cannot be the same city!" << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                    break;
                }

                std::string srcCity = cities[srcIdx - 1];
                std::string destCity = cities[destIdx - 1];

                DijkstraResult res = graph.findShortestPath(srcCity);
                if (!res.hasPath(destCity)) {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] No viable highway connection found!" << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                    break;
                }

                int dist = res.getDistance(destCity);
                std::vector<std::string> path = res.getPath(destCity);

                std::cout << "\n" << Utils::Color::BOLD_GREEN << "[+] Shortest Route Found!" << Utils::Color::RESET << "\n";
                std::cout << "  • Total Highway Distance: " << Utils::Color::BOLD_YELLOW << dist << " KM" << Utils::Color::RESET << "\n";
                std::cout << "  • Route: " << Utils::Color::BOLD_CYAN;
                for (size_t i = 0; i < path.size(); ++i) {
                    std::cout << path[i];
                    if (i + 1 < path.size()) std::cout << " ===> ";
                }
                std::cout << Utils::Color::RESET << "\n\n";

                auto busNonAc = Transport::createTransport(TransportType::NON_AC_BUS);
                auto busAc = Transport::createTransport(TransportType::AC_BUS);
                auto train = Transport::createTransport(TransportType::EXPRESS_TRAIN);
                auto flight = Transport::createTransport(TransportType::DOMESTIC_FLIGHT);

                std::cout << "  1. " << std::left << std::setw(26) << busNonAc->getName() 
                          << " | Est. Time: " << std::setw(8) << busNonAc->formatDuration(dist) 
                          << " | Fare: " << Utils::formatCurrency(busNonAc->calculateFare(dist)) << "\n";
                std::cout << "  2. " << std::left << std::setw(26) << busAc->getName() 
                          << " | Est. Time: " << std::setw(8) << busAc->formatDuration(dist) 
                          << " | Fare: " << Utils::formatCurrency(busAc->calculateFare(dist)) << "\n";
                std::cout << "  3. " << std::left << std::setw(26) << train->getName() 
                          << " | Est. Time: " << std::setw(8) << train->formatDuration(dist) 
                          << " | Fare: " << Utils::formatCurrency(train->calculateFare(dist)) << "\n";
                std::cout << "  4. " << std::left << std::setw(26) << flight->getName() 
                          << " | Est. Time: " << std::setw(8) << flight->formatDuration(dist) 
                          << " | Fare: " << Utils::formatCurrency(flight->calculateFare(dist)) << "\n";
                std::cout << "  0. Cancel\n";

                int tChoice = ConsoleUI::getIntInput("Choose Transport (0-4): ", 0, 4);
                if (tChoice > 0) {
                    std::shared_ptr<Transport> selectedTrans = Transport::createTransport(static_cast<TransportType>(tChoice));
                    std::string coupon = ConsoleUI::getStringInput("Enter Promo Coupon (or Enter to skip): ", true);
                    double subtotal = selectedTrans->calculateFare(dist);
                    double disc = bookingService.validateCoupon(coupon, subtotal) + (subtotal * customer->getTierDiscountPercent());
                    double total = (subtotal - disc) * 1.05;

                    auto payment = promptPaymentMethod(total, customer);
                    Booking newBooking;
                    std::string error;
                    if (bookingService.createCustomTripBooking(customer, srcCity, destCity, selectedTrans, coupon, payment, newBooking, error)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] Trip Booked Successfully!" << Utils::Color::RESET << "\n";
                        newBooking.printBoardingPass();
                    } else {
                        std::cout << Utils::Color::BOLD_RED << "\n[-] Booking Failed: " << error << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 3: {
                Utils::clearScreen();
                auto myBookings = bookingService.getCustomerBookings(customer->getUsername());
                ConsoleUI::renderBookingsTable(myBookings, "MY TRIP BOOKINGS (" + customer->getUsername() + ")");
                if (!myBookings.empty()) {
                    std::string bId = ConsoleUI::getStringInput("Enter Booking Reference to view Boarding Pass (or '0' to return): ", false);
                    if (bId != "0") {
                        const Booking* found = bookingService.getBookingById(bId);
                        if (found && found->getUsername() == customer->getUsername()) found->printBoardingPass();
                        else std::cout << Utils::Color::BOLD_RED << "[-] Booking ID not found." << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 4: {
                Utils::clearScreen();
                auto myBookings = bookingService.getCustomerBookings(customer->getUsername());
                ConsoleUI::renderBookingsTable(myBookings, "CANCEL TRIP RESERVATION");
                std::string bId = ConsoleUI::getStringInput("Enter Booking ID to Cancel (or '0' to abort): ", false);
                if (bId != "0") {
                    std::string msg;
                    if (bookingService.cancelBooking(bId, customer, msg)) std::cout << Utils::Color::BOLD_GREEN << "\n[+] " << msg << Utils::Color::RESET << "\n";
                    else std::cout << Utils::Color::BOLD_RED << "\n[-] " << msg << Utils::Color::RESET << "\n";
                }
                Utils::pauseScreen();
                break;
            }
            case 5: {
                Utils::clearScreen();
                Utils::printHeader("TOP-UP IN-APP TRAVEL WALLET");
                std::cout << "Current Balance: " << Utils::Color::BOLD_GREEN << Utils::formatCurrency(customer->getWalletBalance()) << Utils::Color::RESET << "\n\n";
                double addAmt = ConsoleUI::getDoubleInput("Enter Top-Up Amount (BDT): ", 100.0, 100000.0);
                customer->addFunds(addAmt);
                std::cout << Utils::Color::BOLD_GREEN << "\n[+] Top-Up Successful! New Balance: " 
                          << Utils::formatCurrency(customer->getWalletBalance()) << Utils::Color::RESET << "\n";
                Utils::pauseScreen();
                break;
            }
            case 6: {
                Utils::clearScreen();
                Utils::printHeader("REDEEM LOYALTY REWARD POINTS");
                std::cout << "Available Points: " << Utils::Color::BOLD_YELLOW << customer->getLoyaltyPoints() << " pts" << Utils::Color::RESET << "\n\n";
                if (customer->getLoyaltyPoints() < 50) {
                    std::cout << Utils::Color::BOLD_RED << "[-] Minimum 50 loyalty points required for redemption." << Utils::Color::RESET << "\n";
                } else {
                    int pts = ConsoleUI::getIntInput("Enter points to redeem: ", 50, customer->getLoyaltyPoints());
                    double cash = 0.0;
                    if (customer->redeemLoyaltyPoints(pts, cash)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] Redeemed " << pts << " pts for " 
                                  << Utils::formatCurrency(cash) << " added directly to your Travel Wallet!" << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 7: {
                Utils::clearScreen();
                customer->displayDashboard();
                Utils::pauseScreen();
                break;
            }
            case 8:
                break;
        }
    } while (choice != 8);
}

void handleAdminSession(std::shared_ptr<Admin> admin,
                         Graph& graph,
                         std::vector<TourPackage>& packages,
                         std::unordered_map<std::string, std::shared_ptr<User>>& users,
                         std::vector<Booking>& bookings,
                         std::unordered_map<std::string, double>& coupons) {
    int choice;
    do {
        Utils::clearScreen();
        ConsoleUI::showAdminMenu(admin->getFullName());
        choice = ConsoleUI::getIntInput("Choose admin action (1-7): ", 1, 7);

        switch (choice) {
            case 1: {
                Utils::clearScreen();
                Utils::printHeader("HIGHWAY NETWORK MANAGEMENT");
                graph.displayNetwork();
                std::cout << "\n  1. Add Highway Road Connection\n  2. Add New City Node\n  0. Back\n";
                int sub = ConsoleUI::getIntInput("Choice: ", 0, 2);
                if (sub == 1) {
                    std::string from = ConsoleUI::getStringInput("Enter From City: ", true);
                    std::string to = ConsoleUI::getStringInput("Enter To City: ", true);
                    int dist = ConsoleUI::getIntInput("Enter Highway Road Distance (KM): ", 1, 2000);
                    graph.addConnection(from, to, dist);
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Road added between " << from << " and " << to << " (" << dist << "km)!\n" << Utils::Color::RESET;
                } else if (sub == 2) {
                    std::string city = ConsoleUI::getStringInput("Enter New City Name: ", true);
                    graph.addLocation(city);
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] City " << city << " added!\n" << Utils::Color::RESET;
                }
                Utils::pauseScreen();
                break;
            }
            case 2: {
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                std::cout << "\n  1. Add New Package\n  2. Remove Package\n  0. Back\n";
                int sub = ConsoleUI::getIntInput("Choice: ", 0, 2);
                if (sub == 1) {
                    std::string id = "PKG-" + std::to_string(100 + packages.size() + 1);
                    std::string name = ConsoleUI::getStringInput("Enter Tour Name: ", true);
                    std::string dest = ConsoleUI::getStringInput("Enter Destination: ", true);
                    double cost = ConsoleUI::getDoubleInput("Enter Base Price (BDT): ", 500.0, 500000.0);
                    int days = ConsoleUI::getIntInput("Enter Duration Days: ", 1, 30);
                    int nights = ConsoleUI::getIntInput("Enter Duration Nights: ", 0, 30);
                    std::string hotel = ConsoleUI::getStringInput("Enter Hotel Class: ", true);
                    std::string inc = ConsoleUI::getStringInput("Enter Inclusions: ", true);
                    std::string desc = ConsoleUI::getStringInput("Enter Description: ", true);

                    packages.push_back(TourPackage(id, name, dest, cost, days, nights, hotel, inc, desc));
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Package " << id << " published successfully!\n" << Utils::Color::RESET;
                } else if (sub == 2) {
                    int pIdx = ConsoleUI::getIntInput("Enter Package # to remove: ", 1, static_cast<int>(packages.size()));
                    packages.erase(packages.begin() + (pIdx - 1));
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Package removed!\n" << Utils::Color::RESET;
                }
                Utils::pauseScreen();
                break;
            }
            case 3: {
                Utils::clearScreen();
                ConsoleUI::renderCustomersTable(users);
                Utils::pauseScreen();
                break;
            }
            case 4: {
                Utils::clearScreen();
                ConsoleUI::renderBookingsTable(bookings, "CENTRAL MASTER BOOKING REGISTRY");
                Utils::pauseScreen();
                break;
            }
            case 5: {
                Utils::clearScreen();
                ConsoleUI::renderRevenueAnalytics(bookings);
                Utils::pauseScreen();
                break;
            }
            case 6: {
                Utils::clearScreen();
                Utils::printHeader("PROMOTIONAL COUPONS");
                for (const auto& pair : coupons) {
                    std::cout << "  * " << std::left << std::setw(18) << pair.first;
                    if (pair.second < 1.0) std::cout << static_cast<int>(pair.second * 100) << "% Off\n";
                    else std::cout << "Flat " << Utils::formatCurrency(pair.second) << " Off\n";
                }
                Utils::printDivider(74);
                std::cout << "\n  1. Add Coupon  0. Back\n";
                int cChoice = ConsoleUI::getIntInput("Choice: ", 0, 1);
                if (cChoice == 1) {
                    std::string code = Utils::toUpper(ConsoleUI::getStringInput("Enter Code: ", false));
                    std::cout << "Coupon Type: 1. Flat Amount  2. Percentage\n";
                    int t = ConsoleUI::getIntInput("Choice (1-2): ", 1, 2);
                    if (t == 1) {
                        double amt = ConsoleUI::getDoubleInput("Enter Flat Discount (BDT): ", 50.0, 10000.0);
                        coupons[code] = amt;
                    } else {
                        double pct = ConsoleUI::getDoubleInput("Enter Percentage (1-50%): ", 1.0, 50.0);
                        coupons[code] = pct / 100.0;
                    }
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Coupon activated!\n" << Utils::Color::RESET;
                }
                Utils::pauseScreen();
                break;
            }
            case 7:
                break;
        }
    } while (choice != 7);
}

// ============================================================================
// 12. ENTRY POINT
// ============================================================================
int main() {
    Graph graph;
    std::vector<TourPackage> packages;
    std::unordered_map<std::string, std::shared_ptr<User>> users;
    std::vector<Booking> bookings;
    std::unordered_map<std::string, double> coupons;

    FileManager fileManager("data");
    fileManager.initialize(graph, packages, users, bookings, coupons);

    AuthService authService(users);
    BookingService bookingService(bookings, coupons, graph);

    int mainChoice;
    do {
        Utils::clearScreen();
        ConsoleUI::showBanner();
        ConsoleUI::showMainMenu();
        mainChoice = ConsoleUI::getIntInput("Please select an option (1-6): ", 1, 6);

        switch (mainChoice) {
            case 1: {
                Utils::clearScreen();
                Utils::printHeader("CUSTOMER ACCOUNT LOGIN");
                std::string username = ConsoleUI::getStringInput("Enter Username: ", false);
                std::string password = Utils::getMaskedPassword("Enter Password: ");

                std::string error;
                auto user = authService.login(username, password, error);
                if (user && user->getRole() == UserRole::CUSTOMER) {
                    auto customer = std::dynamic_pointer_cast<Customer>(user);
                    handleCustomerSession(customer, graph, packages, bookingService);
                } else if (user && user->getRole() == UserRole::ADMIN) {
                    std::cout << Utils::Color::BOLD_YELLOW << "\n[!] This is an Administrator account. Please use Admin Portal (Option 3).\n" << Utils::Color::RESET;
                    Utils::pauseScreen();
                } else {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Login Failed: " << error << "\n" << Utils::Color::RESET;
                    Utils::pauseScreen();
                }
                break;
            }
            case 2: {
                Utils::clearScreen();
                Utils::printHeader("NEW CUSTOMER REGISTRATION");
                std::string username = ConsoleUI::getStringInput("Choose Username: ", false);
                std::string password = Utils::getMaskedPassword("Choose Password: ");
                std::string fullName = ConsoleUI::getStringInput("Enter Full Name: ", true);
                std::string phone    = ConsoleUI::getStringInput("Enter Contact Phone: ", false);
                std::string email    = ConsoleUI::getStringInput("Enter Email Address: ", false);
                std::string address  = ConsoleUI::getStringInput("Enter Residential Address: ", true);

                std::string error;
                if (authService.registerCustomer(username, password, fullName, phone, email, address, error)) {
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Registration Successful! Welcome 500 BDT bonus & 50 loyalty points credited!\n" 
                              << Utils::Color::RESET;
                    fileManager.saveUsers(users);
                } else {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Registration Error: " << error << "\n" << Utils::Color::RESET;
                }
                Utils::pauseScreen();
                break;
            }
            case 3: {
                Utils::clearScreen();
                Utils::printHeader("SECURE ADMINISTRATOR AUTHENTICATION");
                std::string username = ConsoleUI::getStringInput("Enter Admin Username: ", false);
                std::string password = Utils::getMaskedPassword("Enter Admin Password: ");

                std::string error;
                auto user = authService.login(username, password, error);
                if (user && user->getRole() == UserRole::ADMIN) {
                    auto admin = std::dynamic_pointer_cast<Admin>(user);
                    handleAdminSession(admin, graph, packages, users, bookings, coupons);
                } else {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Authentication Failed! Access Restricted to Authorized Admins.\n" 
                              << Utils::Color::RESET;
                    Utils::pauseScreen();
                }
                break;
            }
            case 4: {
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                Utils::pauseScreen();
                break;
            }
            case 5: {
                Utils::clearScreen();
                graph.displayNetwork();
                Utils::pauseScreen();
                break;
            }
            case 6: {
                std::cout << Utils::Color::BOLD_CYAN << "\nSaving all system data and configurations...\n" << Utils::Color::RESET;
                fileManager.saveLocations(graph);
                fileManager.savePackages(packages);
                fileManager.saveUsers(users);
                fileManager.saveBookings(bookings);
                fileManager.saveCoupons(coupons);
                std::cout << Utils::Color::BOLD_GREEN << "Thank you for using Bangladesh Tourism & Travel Management System. Have a safe journey!\n" << Utils::Color::RESET;
                break;
            }
        }

        fileManager.saveLocations(graph);
        fileManager.savePackages(packages);
        fileManager.saveUsers(users);
        fileManager.saveBookings(bookings);
        fileManager.saveCoupons(coupons);

    } while (mainChoice != 6);

    return 0;
}
