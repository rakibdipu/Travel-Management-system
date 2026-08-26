#include "../../include/services/BookingService.h"
#include "../../include/core/Utils.h"
#include <algorithm>

double BookingService::validateCoupon(const std::string& code, double subtotal) const {
    std::string cleanCode = Utils::toUpper(Utils::trim(code));
    auto it = coupons.find(cleanCode);
    if (it == coupons.end()) return 0.0;

    double val = it->second;
    if (val > 0 && val < 1.0) {
        // Percentage discount
        return subtotal * val;
    } else if (val >= 1.0) {
        // Flat discount
        return std::min(val, subtotal * 0.8); // Max 80% discount
    }
    return 0.0;
}

bool BookingService::createPackageBooking(std::shared_ptr<Customer> customer,
                                         const TourPackage& package,
                                         const std::string& couponCode,
                                         std::shared_ptr<PaymentMethod> payment,
                                         Booking& outBooking,
                                         std::string& errorMsg) {
    if (!customer) {
        errorMsg = "Invalid customer session!";
        return false;
    }

    double subtotal = package.getBaseCost();
    double tierDiscount = subtotal * customer->getTierDiscountPercent();
    double couponDiscount = validateCoupon(couponCode, subtotal);
    double totalDiscount = tierDiscount + couponDiscount;

    double discountedSubtotal = std::max(0.0, subtotal - totalDiscount);
    double vat = discountedSubtotal * 0.05; // 5% VAT
    double totalCost = discountedSubtotal + vat;

    std::string txId, payError;
    if (!payment->process(totalCost, txId, payError)) {
        errorMsg = "Payment Failed: " + payError;
        return false;
    }

    std::string bookingId = Utils::generateBookingId();
    std::string now = Utils::getCurrentTimestamp();

    Booking b(
        bookingId,
        customer->getUsername(),
        customer->getFullName(),
        customer->getPhone(),
        "Tour Package",
        package.getPackageName(),
        "Dhaka (HQ)",
        package.getDestination(),
        "Direct Tour Itinerary to " + package.getDestination(),
        "Luxury AC Tourist Coach",
        0, // Included in package
        now,
        subtotal,
        totalDiscount,
        couponCode,
        vat,
        totalCost,
        BookingStatus::CONFIRMED,
        payment->getMethodName(),
        txId
    );

    bookings.push_back(b);
    customer->addBookingId(bookingId);
    customer->addLoyaltyPoints(static_cast<int>(totalCost / 100)); // 1 point per 100 BDT
    b.exportTicketToFile("receipts");

    outBooking = b;
    return true;
}

bool BookingService::createCustomTripBooking(std::shared_ptr<Customer> customer,
                                            const std::string& source,
                                            const std::string& destination,
                                            std::shared_ptr<Transport> transport,
                                            const std::string& couponCode,
                                            std::shared_ptr<PaymentMethod> payment,
                                            Booking& outBooking,
                                            std::string& errorMsg) {
    if (!customer) {
        errorMsg = "Invalid customer session!";
        return false;
    }
    if (!transport) {
        errorMsg = "Transport mode not selected!";
        return false;
    }

    DijkstraResult res = graph.findShortestPath(source);
    if (!res.hasPath(destination)) {
        errorMsg = "No highway route found connecting " + source + " to " + destination;
        return false;
    }

    int distance = res.getDistance(destination);
    std::vector<std::string> path = res.getPath(destination);
    CustomTrip trip(source, destination, path, distance, transport);

    double subtotal = trip.getBaseCost();
    double tierDiscount = subtotal * customer->getTierDiscountPercent();
    double couponDiscount = validateCoupon(couponCode, subtotal);
    double totalDiscount = tierDiscount + couponDiscount;

    double discountedSubtotal = std::max(0.0, subtotal - totalDiscount);
    double vat = discountedSubtotal * 0.05; // 5% VAT
    double totalCost = discountedSubtotal + vat;

    std::string txId, payError;
    if (!payment->process(totalCost, txId, payError)) {
        errorMsg = "Payment Failed: " + payError;
        return false;
    }

    std::string bookingId = Utils::generateBookingId();
    std::string now = Utils::getCurrentTimestamp();

    Booking b(
        bookingId,
        customer->getUsername(),
        customer->getFullName(),
        customer->getPhone(),
        "Custom Route",
        source + " to " + destination + " (" + transport->getName() + ")",
        source,
        destination,
        trip.getRouteString(),
        transport->getName(),
        distance,
        now,
        subtotal,
        totalDiscount,
        couponCode,
        vat,
        totalCost,
        BookingStatus::CONFIRMED,
        payment->getMethodName(),
        txId
    );

    bookings.push_back(b);
    customer->addBookingId(bookingId);
    customer->addLoyaltyPoints(static_cast<int>(totalCost / 100));
    b.exportTicketToFile("receipts");

    outBooking = b;
    return true;
}

bool BookingService::cancelBooking(const std::string& bookingId,
                                  std::shared_ptr<Customer> customer,
                                  std::string& statusMsg) {
    for (auto& b : bookings) {
        if (b.getBookingId() == bookingId) {
            if (b.getUsername() != customer->getUsername()) {
                statusMsg = "Access Denied: You do not own this booking!";
                return false;
            }
            if (b.getStatus() == BookingStatus::CANCELLED) {
                statusMsg = "This booking is already cancelled.";
                return false;
            }

            b.cancelBooking();
            // Refund 90% of total cost back into customer's travel wallet (10% cancellation processing fee)
            double refund = b.getFinalCost() * 0.90;
            customer->addFunds(refund);
            statusMsg = "Booking " + bookingId + " successfully cancelled! 90% refund (" + 
                        Utils::formatCurrency(refund) + ") has been credited to your Travel Wallet.";
            return true;
        }
    }
    statusMsg = "Booking reference ID not found.";
    return false;
}

std::vector<Booking> BookingService::getCustomerBookings(const std::string& username) const {
    std::vector<Booking> result;
    for (const auto& b : bookings) {
        if (b.getUsername() == username) {
            result.push_back(b);
        }
    }
    return result;
}

const Booking* BookingService::getBookingById(const std::string& bookingId) const {
    for (const auto& b : bookings) {
        if (b.getBookingId() == bookingId) {
            return &b;
        }
    }
    return nullptr;
}
