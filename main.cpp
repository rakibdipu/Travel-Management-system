#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "include/core/Graph.h"
#include "include/core/Utils.h"
#include "include/core/Exceptions.h"
#include "include/models/User.h"
#include "include/models/Trip.h"
#include "include/models/Transport.h"
#include "include/models/Payment.h"
#include "include/models/Booking.h"
#include "include/services/FileManager.h"
#include "include/services/AuthService.h"
#include "include/services/BookingService.h"
#include "include/ui/ConsoleUI.h"

// Payment selection helper
std::shared_ptr<PaymentMethod> promptPaymentMethod(double amount, std::shared_ptr<Customer> customer) {
    std::cout << "\n" << Utils::Color::BOLD_CYAN << "[+] Choose Payment Strategy for " 
              << Utils::formatCurrency(amount) << ":" << Utils::Color::RESET << "\n";
    std::cout << "  1. bKash Mobile Wallet (Fast & Easy)\n";
    std::cout << "  2. Nagad Digital Financial Service\n";
    std::cout << "  3. Visa / MasterCard Credit/Debit Card\n";
    std::cout << "  4. Internal Travel Wallet Balance (Available: " << Utils::formatCurrency(customer->getWalletBalance()) << ")\n";
    std::cout << "  5. Cash on Departure / Pay at Counter\n";

    int choice = ConsoleUI::getIntInput("Select payment option (1-5): ", 1, 5);

    switch (choice) {
        case 1: {
            std::string phone = ConsoleUI::getStringInput("Enter 11-digit bKash Number (e.g. 017xxxxxxxx): ", false);
            std::string pin = Utils::getMaskedPassword("Enter bKash PIN: ");
            return std::make_shared<DigitalWalletPayment>("bKash", phone, pin);
        }
        case 2: {
            std::string phone = ConsoleUI::getStringInput("Enter 11-digit Nagad Number (e.g. 018xxxxxxxx): ", false);
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
                std::cout << Utils::Color::BOLD_RED << " [-] Wallet balance insufficient. Defaulting to Cash on Departure." 
                          << Utils::Color::RESET << "\n";
                return std::make_shared<CashPayment>();
            }
            customer->deductFunds(amount);
            return std::make_shared<CashPayment>(); // Treated as pre-deducted
        }
        case 5:
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
            case 1: { // Explore & Book Packages
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                std::cout << "  [0] Go back to menu\n";
                int pkgChoice = ConsoleUI::getIntInput("Select Package # to Book: ", 0, static_cast<int>(packages.size()));
                if (pkgChoice > 0) {
                    const TourPackage& selectedPkg = packages[pkgChoice - 1];
                    std::cout << "\nYou selected: " << Utils::Color::BOLD_YELLOW << selectedPkg.getPackageName() 
                              << " (" << Utils::formatCurrency(selectedPkg.getBaseCost()) << ")" << Utils::Color::RESET << "\n";

                    std::string coupon = ConsoleUI::getStringInput("Enter Coupon Code (or press Enter to skip): ", true);
                    double subtotal = selectedPkg.getBaseCost();
                    double disc = bookingService.validateCoupon(coupon, subtotal) + (subtotal * customer->getTierDiscountPercent());
                    double total = (subtotal - disc) * 1.05;

                    auto payment = promptPaymentMethod(total, customer);
                    Booking newBooking;
                    std::string error;
                    if (bookingService.createPackageBooking(customer, selectedPkg, coupon, payment, newBooking, error)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] Booking Confirmed Successfully!" << Utils::Color::RESET << "\n";
                        newBooking.printBoardingPass();
                        std::cout << Utils::Color::CYAN << "\n[✓] Printable Ticket exported to receipts/TICKET_" 
                                  << newBooking.getBookingId() << ".txt" << Utils::Color::RESET << "\n";
                    } else {
                        std::cout << Utils::Color::BOLD_RED << "\n[-] Booking Failed: " << error << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 2: { // Plan Custom Trip with Dijkstra
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
                    std::cout << Utils::Color::BOLD_RED << "\n[-] No viable highway connection found between " 
                              << srcCity << " and " << destCity << "!" << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                    break;
                }

                int dist = res.getDistance(destCity);
                std::vector<std::string> path = res.getPath(destCity);

                std::cout << "\n" << Utils::Color::BOLD_GREEN << "[+] Shortest Route Found!" << Utils::Color::RESET << "\n";
                std::cout << "  • Total Highway Distance: " << Utils::Color::BOLD_YELLOW << dist << " KM" << Utils::Color::RESET << "\n";
                std::cout << "  • Full Route Path: " << Utils::Color::BOLD_CYAN;
                for (size_t i = 0; i < path.size(); ++i) {
                    std::cout << path[i];
                    if (i + 1 < path.size()) std::cout << " ===> ";
                }
                std::cout << Utils::Color::RESET << "\n\n";

                std::cout << Utils::Color::BOLD << "Select Desired Vehicle & Comfort Level:" << Utils::Color::RESET << "\n";
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
                std::cout << "  0. Cancel Trip Planning\n";

                int tChoice = ConsoleUI::getIntInput("Choose Transport (0-4): ", 0, 4);
                if (tChoice > 0) {
                    std::shared_ptr<Transport> selectedTrans = Transport::createTransport(static_cast<TransportType>(tChoice));
                    std::string coupon = ConsoleUI::getStringInput("Enter Promo Coupon Code (or press Enter to skip): ", true);
                    
                    double subtotal = selectedTrans->calculateFare(dist);
                    double disc = bookingService.validateCoupon(coupon, subtotal) + (subtotal * customer->getTierDiscountPercent());
                    double total = (subtotal - disc) * 1.05;

                    auto payment = promptPaymentMethod(total, customer);
                    Booking newBooking;
                    std::string error;
                    if (bookingService.createCustomTripBooking(customer, srcCity, destCity, selectedTrans, coupon, payment, newBooking, error)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] Custom Trip Booked Successfully!" << Utils::Color::RESET << "\n";
                        newBooking.printBoardingPass();
                        std::cout << Utils::Color::CYAN << "\n[✓] Printable Ticket exported to receipts/TICKET_" 
                                  << newBooking.getBookingId() << ".txt" << Utils::Color::RESET << "\n";
                    } else {
                        std::cout << Utils::Color::BOLD_RED << "\n[-] Booking Failed: " << error << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 3: { // My Bookings
                Utils::clearScreen();
                auto myBookings = bookingService.getCustomerBookings(customer->getUsername());
                ConsoleUI::renderBookingsTable(myBookings, "MY TRIP BOOKINGS (" + customer->getUsername() + ")");
                if (!myBookings.empty()) {
                    std::string bId = ConsoleUI::getStringInput("Enter Booking Reference to view Boarding Pass (or '0' to return): ", false);
                    if (bId != "0") {
                        const Booking* found = bookingService.getBookingById(bId);
                        if (found && found->getUsername() == customer->getUsername()) {
                            found->printBoardingPass();
                        } else {
                            std::cout << Utils::Color::BOLD_RED << "[-] Booking ID not found." << Utils::Color::RESET << "\n";
                        }
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 4: { // Cancel Booking
                Utils::clearScreen();
                auto myBookings = bookingService.getCustomerBookings(customer->getUsername());
                ConsoleUI::renderBookingsTable(myBookings, "CANCEL TRIP RESERVATION");
                std::string bId = ConsoleUI::getStringInput("Enter Booking ID to Cancel (or '0' to abort): ", false);
                if (bId != "0") {
                    std::string msg;
                    if (bookingService.cancelBooking(bId, customer, msg)) {
                        std::cout << Utils::Color::BOLD_GREEN << "\n[+] " << msg << Utils::Color::RESET << "\n";
                    } else {
                        std::cout << Utils::Color::BOLD_RED << "\n[-] " << msg << Utils::Color::RESET << "\n";
                    }
                }
                Utils::pauseScreen();
                break;
            }
            case 5: { // Top up wallet
                Utils::clearScreen();
                Utils::printHeader("TOP-UP IN-APP TRAVEL WALLET");
                std::cout << "Current Balance: " << Utils::Color::BOLD_GREEN << Utils::formatCurrency(customer->getWalletBalance()) << Utils::Color::RESET << "\n\n";
                double addAmt = ConsoleUI::getDoubleInput("Enter Top-Up Amount (BDT): ", 100.0, 100000.0);
                std::cout << "Select Payment Channel: 1. bKash  2. Nagad  3. Card\n";
                int c = ConsoleUI::getIntInput("Choice (1-3): ", 1, 3);
                (void)c;
                customer->addFunds(addAmt);
                std::cout << Utils::Color::BOLD_GREEN << "\n[+] Top-Up Successful! New Balance: " 
                          << Utils::formatCurrency(customer->getWalletBalance()) << Utils::Color::RESET << "\n";
                Utils::pauseScreen();
                break;
            }
            case 6: { // Redeem Loyalty points
                Utils::clearScreen();
                Utils::printHeader("REDEEM LOYALTY REWARD POINTS");
                std::cout << "Available Points: " << Utils::Color::BOLD_YELLOW << customer->getLoyaltyPoints() << " pts" << Utils::Color::RESET 
                          << " (Value: 1 pt = 0.50 BDT)\n\n";
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
            case 7: { // View Profile
                Utils::clearScreen();
                customer->displayDashboard();
                Utils::pauseScreen();
                break;
            }
            case 8:
                std::cout << "\nLogging out of customer session...\n";
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
            case 1: { // Manage Highway Network
                Utils::clearScreen();
                Utils::printHeader("HIGHWAY NETWORK MANAGEMENT");
                graph.displayNetwork();
                std::cout << "\n  1. Add New Highway Connection (Road)\n";
                std::cout << "  2. Add New City / Node\n";
                std::cout << "  0. Back to Admin Menu\n";
                int sub = ConsoleUI::getIntInput("Choice: ", 0, 2);
                if (sub == 1) {
                    std::string from = ConsoleUI::getStringInput("Enter From City: ", true);
                    std::string to = ConsoleUI::getStringInput("Enter To City: ", true);
                    int dist = ConsoleUI::getIntInput("Enter Highway Road Distance (KM): ", 1, 2000);
                    graph.addConnection(from, to, dist);
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Road connection added between " << from << " and " << to << " (" << dist << "km)!" << Utils::Color::RESET << "\n";
                } else if (sub == 2) {
                    std::string city = ConsoleUI::getStringInput("Enter New City Name: ", true);
                    graph.addLocation(city);
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] City " << city << " added to network!" << Utils::Color::RESET << "\n";
                }
                Utils::pauseScreen();
                break;
            }
            case 2: { // Manage Tour Packages
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                std::cout << "\n  1. Add New Tour Package\n";
                std::cout << "  2. Remove Tour Package\n";
                std::cout << "  0. Back to Admin Menu\n";
                int sub = ConsoleUI::getIntInput("Choice: ", 0, 2);
                if (sub == 1) {
                    std::string id = "PKG-" + std::to_string(100 + packages.size() + 1);
                    std::string name = ConsoleUI::getStringInput("Enter Tour Name: ", true);
                    std::string dest = ConsoleUI::getStringInput("Enter Destination: ", true);
                    double cost = ConsoleUI::getDoubleInput("Enter Base Price (BDT): ", 500.0, 500000.0);
                    int days = ConsoleUI::getIntInput("Enter Duration Days: ", 1, 30);
                    int nights = ConsoleUI::getIntInput("Enter Duration Nights: ", 0, 30);
                    std::string hotel = ConsoleUI::getStringInput("Enter Hotel Class (e.g. 5-Star Luxury Resort): ", true);
                    std::string inc = ConsoleUI::getStringInput("Enter Inclusions (e.g. Meals, AC Coach, Guide): ", true);
                    std::string desc = ConsoleUI::getStringInput("Enter Package Description: ", true);

                    packages.push_back(TourPackage(id, name, dest, cost, days, nights, hotel, inc, desc));
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Package " << id << " published successfully!" << Utils::Color::RESET << "\n";
                } else if (sub == 2) {
                    int pIdx = ConsoleUI::getIntInput("Enter Package # to remove (1-" + std::to_string(packages.size()) + "): ", 1, static_cast<int>(packages.size()));
                    packages.erase(packages.begin() + (pIdx - 1));
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Package removed!" << Utils::Color::RESET << "\n";
                }
                Utils::pauseScreen();
                break;
            }
            case 3: { // View Registered Customers
                Utils::clearScreen();
                ConsoleUI::renderCustomersTable(users);
                Utils::pauseScreen();
                break;
            }
            case 4: { // Master Bookings
                Utils::clearScreen();
                ConsoleUI::renderBookingsTable(bookings, "CENTRAL MASTER BOOKING REGISTRY");
                Utils::pauseScreen();
                break;
            }
            case 5: { // Financial Analytics
                Utils::clearScreen();
                ConsoleUI::renderRevenueAnalytics(bookings);
                Utils::pauseScreen();
                break;
            }
            case 6: { // Coupons
                Utils::clearScreen();
                Utils::printHeader("PROMOTIONAL COUPONS & OFFERS");
                std::cout << Utils::Color::BOLD << std::left << std::setw(18) << "Coupon Code" << "Discount Offer" << Utils::Color::RESET << "\n";
                Utils::printDivider(74);
                for (const auto& pair : coupons) {
                    std::cout << std::left << std::setw(18) << pair.first;
                    if (pair.second < 1.0) {
                        std::cout << static_cast<int>(pair.second * 100) << "% Percentage Discount\n";
                    } else {
                        std::cout << "Flat " << Utils::formatCurrency(pair.second) << " Off\n";
                    }
                }
                Utils::printDivider(74);
                std::cout << "\n  1. Add New Coupon Code\n  0. Back\n";
                int cChoice = ConsoleUI::getIntInput("Choice: ", 0, 1);
                if (cChoice == 1) {
                    std::string code = Utils::toUpper(ConsoleUI::getStringInput("Enter Code (e.g. WINTER50): ", false));
                    std::cout << "Coupon Type: 1. Flat Amount (e.g. 500 BDT)  2. Percentage (e.g. 15%)\n";
                    int t = ConsoleUI::getIntInput("Choice (1-2): ", 1, 2);
                    if (t == 1) {
                        double amt = ConsoleUI::getDoubleInput("Enter Flat Discount (BDT): ", 50.0, 10000.0);
                        coupons[code] = amt;
                    } else {
                        double pct = ConsoleUI::getDoubleInput("Enter Percentage (1-50%): ", 1.0, 50.0);
                        coupons[code] = pct / 100.0;
                    }
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Coupon " << code << " activated!" << Utils::Color::RESET << "\n";
                }
                Utils::pauseScreen();
                break;
            }
            case 7:
                std::cout << "\nLogging out of admin session...\n";
                break;
        }
    } while (choice != 7);
}

int main() {
    // 1. Initialize Domain State
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
            case 1: { // Customer Login
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
                    std::cout << Utils::Color::BOLD_YELLOW << "\n[!] This is an Administrator account. Please use Admin Portal (Option 3)." << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                } else {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Login Failed: " << error << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                }
                break;
            }
            case 2: { // Customer Registration
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
                    std::cout << Utils::Color::BOLD_GREEN << "\n[+] Registration Successful! Welcome 500 BDT bonus & 50 loyalty points credited!" 
                              << Utils::Color::RESET << "\n";
                    fileManager.saveUsers(users);
                } else {
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Registration Error: " << error << Utils::Color::RESET << "\n";
                }
                Utils::pauseScreen();
                break;
            }
            case 3: { // Admin Login
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
                    std::cout << Utils::Color::BOLD_RED << "\n[-] Authentication Failed! Access Restricted to Authorized Admins." 
                              << Utils::Color::RESET << "\n";
                    Utils::pauseScreen();
                }
                break;
            }
            case 4: { // Browse Packages
                Utils::clearScreen();
                ConsoleUI::renderPackagesCatalog(packages);
                Utils::pauseScreen();
                break;
            }
            case 5: { // View Highway Map
                Utils::clearScreen();
                graph.displayNetwork();
                Utils::pauseScreen();
                break;
            }
            case 6: { // Exit
                std::cout << Utils::Color::BOLD_CYAN << "\nSaving all system data and configurations..." << Utils::Color::RESET << "\n";
                fileManager.saveLocations(graph);
                fileManager.savePackages(packages);
                fileManager.saveUsers(users);
                fileManager.saveBookings(bookings);
                fileManager.saveCoupons(coupons);
                std::cout << Utils::Color::BOLD_GREEN << "Thank you for using Bangladesh Tourism & Travel Management System. Have a safe journey!" << Utils::Color::RESET << "\n";
                break;
            }
        }

        // Auto-save state after operations
        fileManager.saveLocations(graph);
        fileManager.savePackages(packages);
        fileManager.saveUsers(users);
        fileManager.saveBookings(bookings);
        fileManager.saveCoupons(coupons);

    } while (mainChoice != 6);

    return 0;
}
