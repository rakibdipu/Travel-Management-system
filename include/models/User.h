#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <iostream>
#include "../core/Utils.h"

enum class UserRole {
    CUSTOMER,
    ADMIN
};

enum class CustomerTier {
    SILVER,
    GOLD,
    PLATINUM
};

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
         const std::string& ph, const std::string& em, UserRole r);
    virtual ~User() = default;

    // Getters
    std::string getUsername() const { return username; }
    size_t getPasswordHash() const { return passwordHash; }
    std::string getFullName() const { return fullName; }
    std::string getPhone() const { return phone; }
    std::string getEmail() const { return email; }
    UserRole getRole() const { return role; }

    // Setters
    void setFullName(const std::string& name) { fullName = name; }
    void setPhone(const std::string& ph) { phone = ph; }
    void setEmail(const std::string& em) { email = em; }
    void setPasswordHash(size_t hash) { passwordHash = hash; }

    bool verifyPassword(const std::string& plainPassword) const;

    // Pure Virtual Functions (Abstraction & Polymorphism)
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
             double balance = 500.0, int points = 50);

    std::string getAddress() const { return address; }
    double getWalletBalance() const { return walletBalance; }
    int getLoyaltyPoints() const { return loyaltyPoints; }
    const std::vector<std::string>& getBookingHistory() const { return bookingHistory; }

    CustomerTier getTier() const;
    std::string getTierString() const;
    double getTierDiscountPercent() const;

    void addFunds(double amount);
    bool deductFunds(double amount);
    void addLoyaltyPoints(int points);
    bool redeemLoyaltyPoints(int points, double& cashEquivalent);
    void addBookingId(const std::string& id);

    void displayDashboard() const override;
    std::string getRoleString() const override { return "Customer"; }
    std::string serialize() const override;
};

class Admin : public User {
private:
    std::string department;
    int accessLevel; // 1 = Moderator, 2 = Senior Admin, 3 = Super Admin

public:
    Admin(const std::string& uname, size_t passHash, const std::string& name,
          const std::string& ph, const std::string& em, const std::string& dept = "Operations",
          int level = 3);

    std::string getDepartment() const { return department; }
    int getAccessLevel() const { return accessLevel; }

    void displayDashboard() const override;
    std::string getRoleString() const override { return "Administrator"; }
    std::string serialize() const override;
};

#endif // USER_H
