#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../models/User.h"
#include "../models/Trip.h"
#include "../models/Booking.h"
#include "../models/Payment.h"
#include "../core/Graph.h"

class BookingService {
private:
    std::vector<Booking>& bookings;
    std::unordered_map<std::string, double>& coupons;
    Graph& graph;

public:
    BookingService(std::vector<Booking>& bookList,
                   std::unordered_map<std::string, double>& couponMap,
                   Graph& g)
        : bookings(bookList), coupons(couponMap), graph(g) {}

    double validateCoupon(const std::string& code, double subtotal) const;

    bool createPackageBooking(std::shared_ptr<Customer> customer,
                              const TourPackage& package,
                              const std::string& couponCode,
                              std::shared_ptr<PaymentMethod> payment,
                              Booking& outBooking,
                              std::string& errorMsg);

    bool createCustomTripBooking(std::shared_ptr<Customer> customer,
                                 const std::string& source,
                                 const std::string& destination,
                                 std::shared_ptr<Transport> transport,
                                 const std::string& couponCode,
                                 std::shared_ptr<PaymentMethod> payment,
                                 Booking& outBooking,
                                 std::string& errorMsg);

    bool cancelBooking(const std::string& bookingId,
                       std::shared_ptr<Customer> customer,
                       std::string& statusMsg);

    std::vector<Booking> getCustomerBookings(const std::string& username) const;
    const Booking* getBookingById(const std::string& bookingId) const;
};

#endif // BOOKING_SERVICE_H
