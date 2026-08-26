#include "../../include/models/Booking.h"
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define CREATE_DIR(dir) _mkdir(dir)
#else
#define CREATE_DIR(dir) mkdir(dir, 0777)
#endif

Booking::Booking()
    : distanceKm(0), subtotal(0), discountAmount(0), vatAmount(0),
      finalCost(0), status(BookingStatus::CONFIRMED) {}

Booking::Booking(const std::string& id, const std::string& uname, const std::string& name,
                 const std::string& phone, const std::string& type, const std::string& title,
                 const std::string& src, const std::string& dest, const std::string& path,
                 const std::string& transport, int dist, const std::string& date,
                 double sub, double disc, const std::string& coupon, double vat,
                 double total, BookingStatus stat, const std::string& payMethod,
                 const std::string& txId)
    : bookingId(id), username(uname), customerName(name), customerPhone(phone),
      tripType(type), tripTitle(title), source(src), destination(dest),
      routePath(path), transportName(transport), distanceKm(dist),
      bookingDate(date), subtotal(sub), discountAmount(disc), couponCode(coupon),
      vatAmount(vat), finalCost(total), status(stat), paymentMethod(payMethod),
      transactionId(txId) {}

std::string Booking::getStatusString() const {
    switch (status) {
        case BookingStatus::CONFIRMED: return "CONFIRMED";
        case BookingStatus::CANCELLED: return "CANCELLED";
        case BookingStatus::COMPLETED: return "COMPLETED";
    }
    return "UNKNOWN";
}

void Booking::cancelBooking() {
    status = BookingStatus::CANCELLED;
}

void Booking::displaySummary() const {
    std::string statColor = (status == BookingStatus::CONFIRMED) ? Utils::Color::BOLD_GREEN :
                            (status == BookingStatus::CANCELLED ? Utils::Color::BOLD_RED : Utils::Color::CYAN);

    std::cout << "• " << Utils::Color::BOLD << std::left << std::setw(15) << bookingId << Utils::Color::RESET
              << " | " << std::left << std::setw(12) << username
              << " | " << std::left << std::setw(22) << tripTitle
              << " | " << std::right << std::setw(12) << Utils::formatCurrency(finalCost)
              << " | " << statColor << std::setw(11) << getStatusString() << Utils::Color::RESET
              << " | Date: " << bookingDate << "\n";
}

void Booking::printBoardingPass() const {
    std::cout << Utils::Color::BOLD_CYAN
              << "╔══════════════════════════════════════════════════════════════════════════╗\n"
              << "║                       BANGLADESH TOURISM & TRAVEL                        ║\n"
              << "║                         OFFICIAL BOARDING TICKET                         ║\n"
              << "╠══════════════════════════════════════════════════════════════════════════╣"
              << Utils::Color::RESET << "\n";
    
    std::cout << "║ " << std::left << std::setw(20) << "Booking Reference" << ": " 
              << Utils::Color::BOLD_YELLOW << std::setw(48) << bookingId << Utils::Color::RESET << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Passenger Name" << ": " 
              << std::setw(48) << customerName << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Contact Phone" << ": " 
              << std::setw(48) << customerPhone << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Trip Category" << ": " 
              << std::setw(48) << tripType << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Journey Title" << ": " 
              << std::setw(48) << tripTitle << "║\n";
    if (!source.empty() && !destination.empty()) {
        std::cout << "║ " << std::left << std::setw(20) << "Route Origin-Dest" << ": " 
                  << std::setw(48) << (source + " ===> " + destination) << "║\n";
    }
    if (!routePath.empty()) {
        std::cout << "║ " << std::left << std::setw(20) << "Itinerary Nodes" << ": " 
                  << std::setw(48) << (routePath.length() > 48 ? routePath.substr(0, 45) + "..." : routePath) << "║\n";
    }
    std::cout << "║ " << std::left << std::setw(20) << "Transport Vehicle" << ": " 
              << std::setw(48) << transportName << "║\n";
    if (distanceKm > 0) {
        std::cout << "║ " << std::left << std::setw(20) << "Total Distance" << ": " 
                  << std::setw(48) << (std::to_string(distanceKm) + " KM") << "║\n";
    }
    std::cout << "║ " << std::left << std::setw(20) << "Issued Timestamp" << ": " 
              << std::setw(48) << bookingDate << "║\n";

    std::cout << Utils::Color::BOLD_CYAN 
              << "╠══════════════════════════════════════════════════════════════════════════╣" 
              << Utils::Color::RESET << "\n";

    std::cout << "║ " << std::left << std::setw(20) << "Base Subtotal" << ": " 
              << std::setw(48) << Utils::formatCurrency(subtotal) << "║\n";
    if (discountAmount > 0) {
        std::string discInfo = "-" + Utils::formatCurrency(discountAmount) + " (" + (couponCode.empty() ? "Tier Discount" : couponCode) + ")";
        std::cout << "║ " << std::left << std::setw(20) << "Promo / Loyalty Disc" << ": " 
                  << Utils::Color::BOLD_GREEN << std::setw(48) << discInfo << Utils::Color::RESET << "║\n";
    }
    std::cout << "║ " << std::left << std::setw(20) << "Govt VAT (5%)" << ": " 
              << std::setw(48) << Utils::formatCurrency(vatAmount) << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "TOTAL CHARGE" << ": " 
              << Utils::Color::BOLD_GREEN << std::setw(48) << Utils::formatCurrency(finalCost) << Utils::Color::RESET << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Payment Strategy" << ": " 
              << std::setw(48) << paymentMethod << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Txn Identifier" << ": " 
              << std::setw(48) << transactionId << "║\n";
    std::cout << "║ " << std::left << std::setw(20) << "Ticket Status" << ": " 
              << Utils::Color::BOLD << std::setw(48) << getStatusString() << Utils::Color::RESET << "║\n";

    std::cout << Utils::Color::BOLD_CYAN
              << "╚══════════════════════════════════════════════════════════════════════════╝\n"
              << Utils::Color::RESET;
}

bool Booking::exportTicketToFile(const std::string& directory) const {
    CREATE_DIR(directory.c_str());
    std::string filePath = directory + "/TICKET_" + bookingId + ".txt";
    std::ofstream out(filePath);
    if (!out) return false;

    out << "========================================================================\n"
        << "                       BANGLADESH TOURISM & TRAVEL                      \n"
        << "                         OFFICIAL BOARDING PASS                         \n"
        << "========================================================================\n"
        << "Booking Reference : " << bookingId << "\n"
        << "Passenger Name    : " << customerName << "\n"
        << "Customer Phone    : " << customerPhone << "\n"
        << "Trip Category     : " << tripType << "\n"
        << "Tour Title        : " << tripTitle << "\n";
    if (!source.empty() && !destination.empty()) {
        out << "Route             : " << source << " to " << destination << "\n";
    }
    if (!routePath.empty()) {
        out << "Route Itinerary   : " << routePath << "\n";
    }
    out << "Transport Vehicle : " << transportName << "\n";
    if (distanceKm > 0) {
        out << "Total Distance    : " << distanceKm << " KM\n";
    }
    out << "Issue Timestamp   : " << bookingDate << "\n"
        << "------------------------------------------------------------------------\n"
        << "Base Subtotal     : " << Utils::formatCurrency(subtotal) << "\n"
        << "Discount Applied  : -" << Utils::formatCurrency(discountAmount) << " (" << (couponCode.empty() ? "Tier Discount" : couponCode) << ")\n"
        << "Govt VAT (5%)     : " << Utils::formatCurrency(vatAmount) << "\n"
        << "FINAL PAID AMOUNT : " << Utils::formatCurrency(finalCost) << "\n"
        << "Payment Method    : " << paymentMethod << "\n"
        << "Transaction ID    : " << transactionId << "\n"
        << "Booking Status    : " << getStatusString() << "\n"
        << "========================================================================\n"
        << "            Thank you for choosing Bangladesh Travel Management!        \n"
        << "              For 24/7 Helpline Support, call: +880 1700-000000        \n"
        << "========================================================================\n";
    out.close();
    return true;
}

std::string Booking::serialize() const {
    // Format: bookingId|username|customerName|customerPhone|tripType|tripTitle|source|destination|routePath|transportName|distanceKm|bookingDate|subtotal|discountAmount|couponCode|vatAmount|finalCost|statusInt|paymentMethod|transactionId
    std::ostringstream oss;
    oss << bookingId << "|" << username << "|" << customerName << "|" << customerPhone << "|"
        << tripType << "|" << tripTitle << "|" << source << "|" << destination << "|"
        << routePath << "|" << transportName << "|" << distanceKm << "|" << bookingDate << "|"
        << subtotal << "|" << discountAmount << "|" << couponCode << "|" << vatAmount << "|"
        << finalCost << "|" << static_cast<int>(status) << "|" << paymentMethod << "|"
        << transactionId;
    return oss.str();
}

Booking Booking::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    if (tokens.size() < 20) return Booking();

    Booking b;
    b.bookingId = tokens[0];
    b.username = tokens[1];
    b.customerName = tokens[2];
    b.customerPhone = tokens[3];
    b.tripType = tokens[4];
    b.tripTitle = tokens[5];
    b.source = tokens[6];
    b.destination = tokens[7];
    b.routePath = tokens[8];
    b.transportName = tokens[9];
    b.distanceKm = std::stoi(tokens[10]);
    b.bookingDate = tokens[11];
    b.subtotal = std::stod(tokens[12]);
    b.discountAmount = std::stod(tokens[13]);
    b.couponCode = tokens[14];
    b.vatAmount = std::stod(tokens[15]);
    b.finalCost = std::stod(tokens[16]);
    b.status = static_cast<BookingStatus>(std::stoi(tokens[17]));
    b.paymentMethod = tokens[18];
    b.transactionId = tokens[19];
    return b;
}

std::ostream& operator<<(std::ostream& os, const Booking& b) {
    b.displaySummary();
    return os;
}
