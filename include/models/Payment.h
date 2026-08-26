#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <iomanip>
#include "../core/Utils.h"

enum class PaymentType {
    CASH,
    BKASH,
    NAGAD,
    CREDIT_CARD,
    WALLET
};

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
    std::string provider; // "bKash" or "Nagad"
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
        // Simulated gateway response
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

class WalletPayment : public PaymentMethod {
private:
    double& customerWallet;

public:
    explicit WalletPayment(double& walletRef) : customerWallet(walletRef) {}

    bool process(double amount, std::string& transactionId, std::string& errorMsg) override {
        if (customerWallet < amount) {
            errorMsg = "Insufficient funds in Travel Wallet! Current: " + Utils::formatCurrency(customerWallet);
            return false;
        }
        customerWallet -= amount;
        static int wCounter = 1204;
        std::ostringstream oss;
        oss << "WAL-DED-" << (++wCounter);
        transactionId = oss.str();
        return true;
    }

    std::string getMethodName() const override { return "Internal App Wallet Balance"; }
    PaymentType getType() const override { return PaymentType::WALLET; }
};

#endif // PAYMENT_H
