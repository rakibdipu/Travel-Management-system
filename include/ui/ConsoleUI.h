#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../models/User.h"
#include "../models/Trip.h"
#include "../models/Booking.h"
#include "../core/Graph.h"

class ConsoleUI {
public:
    static void showBanner();
    static int getIntInput(const std::string& prompt, int minVal, int maxVal);
    static double getDoubleInput(const std::string& prompt, double minVal = 0.0, double maxVal = 1000000.0);
    static std::string getStringInput(const std::string& prompt, bool allowSpaces = true);

    static void showMainMenu();
    static void showCustomerMenu(const std::string& customerName, double wallet, const std::string& tier);
    static void showAdminMenu(const std::string& adminName);

    static void renderCityGrid(const std::vector<std::string>& cities);
    static void renderPackagesCatalog(const std::vector<TourPackage>& packages);
    static void renderBookingsTable(const std::vector<Booking>& bookings, const std::string& title = "ALL BOOKINGS");
    static void renderCustomersTable(const std::unordered_map<std::string, std::shared_ptr<User>>& users);
    static void renderRevenueAnalytics(const std::vector<Booking>& bookings);
};

#endif // CONSOLE_UI_H
