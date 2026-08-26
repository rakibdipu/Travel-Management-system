#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "../core/Utils.h"

enum class BookingStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Booking {
private:
    std::string bookingId;
    std::string username;
    std::string customerName;
    std::string customerPhone;
    std::string tripType;       // "Tour Package" or "Custom Route"
    std::string tripTitle;      // e.g. "Sundarbans Eco Adventure" or "Dhaka to Cox's Bazar"
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
    Booking();
    Booking(const std::string& id, const std::string& uname, const std::string& name,
            const std::string& phone, const std::string& type, const std::string& title,
            const std::string& src, const std::string& dest, const std::string& path,
            const std::string& transport, int dist, const std::string& date,
            double sub, double disc, const std::string& coupon, double vat,
            double total, BookingStatus stat, const std::string& payMethod,
            const std::string& txId);

    // Getters
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
    std::string getStatusString() const;
    std::string getPaymentMethod() const { return paymentMethod; }
    std::string getTransactionId() const { return transactionId; }

    void cancelBooking();
    void displaySummary() const;
    void printBoardingPass() const;
    bool exportTicketToFile(const std::string& directory = "receipts") const;
    std::string serialize() const;
    static Booking deserialize(const std::string& line);

    // Stream operator overloading
    friend std::ostream& operator<<(std::ostream& os, const Booking& b);
};

#endif // BOOKING_H
