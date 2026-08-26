#include "../../include/services/AuthService.h"
#include "../../include/core/Utils.h"

bool AuthService::registerCustomer(const std::string& username, const std::string& plainPassword,
                                  const std::string& fullName, const std::string& phone,
                                  const std::string& email, const std::string& address,
                                  std::string& errorMsg) {
    std::string uname = Utils::trim(username);
    if (uname.length() < 3) {
        errorMsg = "Username must be at least 3 characters long.";
        return false;
    }
    if (plainPassword.length() < 4) {
        errorMsg = "Password must be at least 4 characters long.";
        return false;
    }
    if (userExists(uname)) {
        errorMsg = "Username '" + uname + "' is already registered! Please choose a different one.";
        return false;
    }

    size_t passHash = Utils::simpleHash(plainPassword);
    auto newCustomer = std::make_shared<Customer>(
        uname, passHash, fullName, phone, email, address,
        500.0, // Welcome bonus of 500 BDT
        50     // 50 starter loyalty points
    );

    users[uname] = newCustomer;
    return true;
}

std::shared_ptr<User> AuthService::login(const std::string& username, const std::string& plainPassword,
                                        std::string& errorMsg) {
    std::string uname = Utils::trim(username);
    auto it = users.find(uname);
    if (it == users.end()) {
        errorMsg = "Username not found in system.";
        return nullptr;
    }

    if (!it->second->verifyPassword(plainPassword)) {
        errorMsg = "Incorrect password. Access denied.";
        return nullptr;
    }

    return it->second;
}

bool AuthService::userExists(const std::string& username) const {
    return users.find(Utils::trim(username)) != users.end();
}
