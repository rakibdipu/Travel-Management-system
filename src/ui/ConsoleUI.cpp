#include "../../include/ui/ConsoleUI.h"
#include "../../include/core/Utils.h"
#include <iostream>
#include <iomanip>
#include <limits>

void ConsoleUI::showBanner() {
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

int ConsoleUI::getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << Utils::Color::BOLD << prompt << Utils::Color::RESET;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << Utils::Color::BOLD_RED << " [-] Invalid range! Please enter a number between " 
                          << minVal << " and " << maxVal << "." << Utils::Color::RESET << "\n";
            }
        } else {
            std::cout << Utils::Color::BOLD_RED << " [-] Invalid input! Please enter a valid integer." 
                      << Utils::Color::RESET << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double ConsoleUI::getDoubleInput(const std::string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        std::cout << Utils::Color::BOLD << prompt << Utils::Color::RESET;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << Utils::Color::BOLD_RED << " [-] Please enter a value between " 
                          << minVal << " and " << maxVal << "." << Utils::Color::RESET << "\n";
            }
        } else {
            std::cout << Utils::Color::BOLD_RED << " [-] Invalid numeric input! Please try again." 
                      << Utils::Color::RESET << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string ConsoleUI::getStringInput(const std::string& prompt, bool allowSpaces) {
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
        if (!value.empty()) {
            return value;
        }
        std::cout << Utils::Color::BOLD_RED << " [-] Input cannot be empty! Please try again." 
                  << Utils::Color::RESET << "\n";
    }
}

void ConsoleUI::showMainMenu() {
    Utils::printHeader("WELCOME TO BANGLADESH TRAVEL MANAGEMENT SYSTEM");
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Customer Portal Login\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Customer Registration (New Account)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " Administrator Portal Login\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " Browse Available Tour Packages (Public View)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " View Highway Route Network & Map\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Exit Application\n";
    Utils::printDivider(74);
}

void ConsoleUI::showCustomerMenu(const std::string& customerName, double wallet, const std::string& tier) {
    Utils::printHeader("CUSTOMER DASHBOARD: " + customerName);
    std::cout << "  " << Utils::Color::BOLD_GREEN << "Wallet: " << Utils::formatCurrency(wallet) 
              << " | Tier: " << tier << Utils::Color::RESET << "\n";
    Utils::printDivider(74);
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Explore & Book Curated Tour Packages\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Plan Custom Trip (Shortest Route & Vehicle Selection)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " My Booking History & Download Tickets\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " Cancel a Booking (Refund to Wallet)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " Top-Up Travel Wallet Funds\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Redeem Loyalty Points for Wallet Cash\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[7]" << Utils::Color::RESET << " View My Account Profile\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[8]" << Utils::Color::RESET << " Logout to Main Menu\n";
    Utils::printDivider(74);
}

void ConsoleUI::showAdminMenu(const std::string& adminName) {
    Utils::printHeader("ADMINISTRATOR CONTROL PANEL (" + adminName + ")");
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[1]" << Utils::Color::RESET << " Manage Highway Routes (Add City Node / Add Road Edge)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[2]" << Utils::Color::RESET << " Manage Tour Packages (Add New Package / Edit / Remove)\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[3]" << Utils::Color::RESET << " View All Registered Customers\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[4]" << Utils::Color::RESET << " View Master Booking Registry\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[5]" << Utils::Color::RESET << " System Financial & Booking Analytics\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[6]" << Utils::Color::RESET << " Manage Promotional Discount Coupons\n";
    std::cout << "  " << Utils::Color::BOLD_CYAN << "[7]" << Utils::Color::RESET << " Logout to Main Menu\n";
    Utils::printDivider(74);
}

void ConsoleUI::renderCityGrid(const std::vector<std::string>& cities) {
    std::cout << Utils::Color::BOLD_CYAN << "\n[+] Available Highway Network Destinations (" << cities.size() << " Cities):" << Utils::Color::RESET << "\n";
    Utils::printDivider(74);
    for (size_t i = 0; i < cities.size(); ++i) {
        std::cout << std::right << std::setw(3) << (i + 1) << ". " 
                  << std::left << std::setw(18) << cities[i];
        if ((i + 1) % 3 == 0 || i + 1 == cities.size()) {
            std::cout << "\n";
        }
    }
    Utils::printDivider(74);
}

void ConsoleUI::renderPackagesCatalog(const std::vector<TourPackage>& packages) {
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

void ConsoleUI::renderBookingsTable(const std::vector<Booking>& bookings, const std::string& title) {
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

void ConsoleUI::renderCustomersTable(const std::unordered_map<std::string, std::shared_ptr<User>>& users) {
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
    if (count == 0) {
        std::cout << "  No registered customers found.\n";
    }
    Utils::printDivider(74);
}

void ConsoleUI::renderRevenueAnalytics(const std::vector<Booking>& bookings) {
    Utils::printHeader("TRAVEL SYSTEM FINANCIAL & BOOKING ANALYTICS");

    double totalRevenue = 0.0;
    double totalDiscounts = 0.0;
    double totalVat = 0.0;
    int confirmedCount = 0;
    int cancelledCount = 0;
    std::unordered_map<std::string, int> destinationPopularity;

    for (const auto& b : bookings) {
        if (b.getStatus() == BookingStatus::CONFIRMED) {
            totalRevenue += b.getFinalCost();
            totalDiscounts += b.getDiscountAmount();
            totalVat += b.getVatAmount();
            confirmedCount++;
            if (!b.getDestination().empty()) {
                destinationPopularity[b.getDestination()]++;
            }
        } else if (b.getStatus() == BookingStatus::CANCELLED) {
            cancelledCount++;
        }
    }

    std::cout << "  " << Utils::Color::BOLD_GREEN << "• Total Confirmed Gross Revenue : " 
              << Utils::formatCurrency(totalRevenue) << Utils::Color::RESET << "\n";
    std::cout << "  • Total Discounts Granted      : " << Utils::formatCurrency(totalDiscounts) << "\n";
    std::cout << "  • Total Govt VAT Collected     : " << Utils::formatCurrency(totalVat) << "\n";
    std::cout << "  • Total Confirmed Bookings     : " << confirmedCount << " bookings\n";
    std::cout << "  • Total Cancelled Bookings     : " << cancelledCount << " bookings\n";
    Utils::printDivider(74);

    std::cout << Utils::Color::BOLD_CYAN << "  Top Traveled Destinations:" << Utils::Color::RESET << "\n";
    if (destinationPopularity.empty()) {
        std::cout << "    (No trip data yet)\n";
    } else {
        for (const auto& pair : destinationPopularity) {
            std::cout << "    * " << std::left << std::setw(20) << pair.first << ": " << pair.second << " visits\n";
        }
    }
    Utils::printDivider(74);
}
