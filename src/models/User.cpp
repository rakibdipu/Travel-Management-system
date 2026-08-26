#include "../../include/models/User.h"
#include <iomanip>
#include <sstream>

// ==================== User Base Class ====================

User::User(const std::string& uname, size_t passHash, const std::string& name,
           const std::string& ph, const std::string& em, UserRole r)
    : username(uname), passwordHash(passHash), fullName(name), phone(ph), email(em), role(r) {}

bool User::verifyPassword(const std::string& plainPassword) const {
    return Utils::simpleHash(plainPassword) == passwordHash;
}

// ==================== Customer Class ====================

Customer::Customer(const std::string& uname, size_t passHash, const std::string& name,
                   const std::string& ph, const std::string& em, const std::string& addr,
                   double balance, int points)
    : User(uname, passHash, name, ph, em, UserRole::CUSTOMER),
      address(addr), walletBalance(balance), loyaltyPoints(points) {}

CustomerTier Customer::getTier() const {
    if (loyaltyPoints >= 500) return CustomerTier::PLATINUM;
    if (loyaltyPoints >= 200) return CustomerTier::GOLD;
    return CustomerTier::SILVER;
}

std::string Customer::getTierString() const {
    switch (getTier()) {
        case CustomerTier::PLATINUM: return "Platinum (10% Off)";
        case CustomerTier::GOLD:     return "Gold (5% Off)";
        case CustomerTier::SILVER:   return "Silver (Standard)";
    }
    return "Silver";
}

double Customer::getTierDiscountPercent() const {
    switch (getTier()) {
        case CustomerTier::PLATINUM: return 0.10;
        case CustomerTier::GOLD:     return 0.05;
        case CustomerTier::SILVER:   return 0.00;
    }
    return 0.0;
}

void Customer::addFunds(double amount) {
    if (amount > 0) {
        walletBalance += amount;
    }
}

bool Customer::deductFunds(double amount) {
    if (amount > 0 && walletBalance >= amount) {
        walletBalance -= amount;
        return true;
    }
    return false;
}

void Customer::addLoyaltyPoints(int points) {
    if (points > 0) {
        loyaltyPoints += points;
    }
}

bool Customer::redeemLoyaltyPoints(int points, double& cashEquivalent) {
    if (points > 0 && loyaltyPoints >= points) {
        loyaltyPoints -= points;
        cashEquivalent = points * 0.5; // 1 point = 0.50 BDT
        walletBalance += cashEquivalent;
        return true;
    }
    return false;
}

void Customer::addBookingId(const std::string& id) {
    bookingHistory.push_back(id);
}

void Customer::displayDashboard() const {
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

std::string Customer::serialize() const {
    // Format: CUSTOMER|username|passHash|fullName|phone|email|address|walletBalance|loyaltyPoints|b1,b2,b3
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

// ==================== Admin Class ====================

Admin::Admin(const std::string& uname, size_t passHash, const std::string& name,
             const std::string& ph, const std::string& em, const std::string& dept,
             int level)
    : User(uname, passHash, name, ph, em, UserRole::ADMIN),
      department(dept), accessLevel(level) {}

void Admin::displayDashboard() const {
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

std::string Admin::serialize() const {
    // Format: ADMIN|username|passHash|fullName|phone|email|department|accessLevel
    std::ostringstream oss;
    oss << "ADMIN|" << username << "|" << passwordHash << "|" << fullName << "|"
        << phone << "|" << email << "|" << department << "|" << accessLevel;
    return oss.str();
}
