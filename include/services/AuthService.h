#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <string>
#include <memory>
#include <unordered_map>
#include "../models/User.h"

class AuthService {
private:
    std::unordered_map<std::string, std::shared_ptr<User>>& users;

public:
    explicit AuthService(std::unordered_map<std::string, std::shared_ptr<User>>& userMap)
        : users(userMap) {}

    bool registerCustomer(const std::string& username, const std::string& plainPassword,
                          const std::string& fullName, const std::string& phone,
                          const std::string& email, const std::string& address,
                          std::string& errorMsg);

    std::shared_ptr<User> login(const std::string& username, const std::string& plainPassword,
                                std::string& errorMsg);

    bool userExists(const std::string& username) const;
};

#endif // AUTH_SERVICE_H
