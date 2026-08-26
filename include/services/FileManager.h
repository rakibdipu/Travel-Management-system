#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../core/Graph.h"
#include "../models/User.h"
#include "../models/Trip.h"
#include "../models/Booking.h"

class FileManager {
private:
    std::string dataDir;
    std::string locationsFile;
    std::string packagesFile;
    std::string usersFile;
    std::string bookingsFile;
    std::string couponsFile;

    void ensureDirectoryExists() const;
    void seedDefaultData(Graph& graph,
                         std::vector<TourPackage>& packages,
                         std::unordered_map<std::string, std::shared_ptr<User>>& users,
                         std::unordered_map<std::string, double>& coupons) const;

public:
    FileManager(const std::string& dir = "data");

    void initialize(Graph& graph,
                    std::vector<TourPackage>& packages,
                    std::unordered_map<std::string, std::shared_ptr<User>>& users,
                    std::vector<Booking>& bookings,
                    std::unordered_map<std::string, double>& coupons);

    bool loadLocations(Graph& graph) const;
    bool saveLocations(const Graph& graph) const;

    bool loadPackages(std::vector<TourPackage>& packages) const;
    bool savePackages(const std::vector<TourPackage>& packages) const;

    bool loadUsers(std::unordered_map<std::string, std::shared_ptr<User>>& users) const;
    bool saveUsers(const std::unordered_map<std::string, std::shared_ptr<User>>& users) const;

    bool loadBookings(std::vector<Booking>& bookings) const;
    bool saveBookings(const std::vector<Booking>& bookings) const;

    bool loadCoupons(std::unordered_map<std::string, double>& coupons) const;
    bool saveCoupons(const std::unordered_map<std::string, double>& coupons) const;
};

#endif // FILE_MANAGER_H
