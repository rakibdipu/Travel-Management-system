#include "../../include/models/Trip.h"

// ==================== TourPackage Class ====================

TourPackage::TourPackage(const std::string& id, const std::string& name, const std::string& dest,
                         double cost, int days, int nights, const std::string& hotel,
                         const std::string& inc, const std::string& desc)
    : Trip(dest, cost), packageId(id), packageName(name),
      durationDays(days), durationNights(nights), hotelRating(hotel),
      inclusions(inc), description(desc) {}

void TourPackage::displayDetails() const {
    std::cout << Utils::Color::BOLD_CYAN << "┌────────────────────────────────────────────────────────────────────────┐" << Utils::Color::RESET << "\n";
    std::cout << "│ " << Utils::Color::BOLD_YELLOW << std::left << std::setw(6) << packageId 
              << std::setw(38) << packageName 
              << "Price: " << std::setw(18) << Utils::formatCurrency(baseCost) 
              << Utils::Color::RESET << "│\n";
    std::cout << "│ " << std::left << "Destination : " << std::setw(20) << destination 
              << "Duration   : " << durationDays << " Days, " << durationNights << " Nights" 
              << std::string(17, ' ') << "│\n";
    std::cout << "│ " << std::left << "Hotel Class : " << std::setw(56) << hotelRating << "│\n";
    std::cout << "│ " << std::left << "Inclusions  : " << std::setw(56) << inclusions << "│\n";
    std::cout << "│ " << Utils::Color::DIM << std::left << "Info        : " << std::setw(56) << description << Utils::Color::RESET << "│\n";
    std::cout << Utils::Color::BOLD_CYAN << "└────────────────────────────────────────────────────────────────────────┘" << Utils::Color::RESET << "\n";
}

std::string TourPackage::serialize() const {
    // Format: PKG|packageId|packageName|destination|baseCost|durationDays|durationNights|hotelRating|inclusions|description
    std::ostringstream oss;
    oss << "PKG|" << packageId << "|" << packageName << "|" << destination << "|"
        << baseCost << "|" << durationDays << "|" << durationNights << "|"
        << hotelRating << "|" << inclusions << "|" << description;
    return oss.str();
}

// ==================== CustomTrip Class ====================

CustomTrip::CustomTrip(const std::string& src, const std::string& dest,
                       const std::vector<std::string>& path, int dist,
                       std::shared_ptr<Transport> transport)
    : Trip(dest, transport ? transport->calculateFare(dist) : static_cast<double>(dist)),
      source(src), route(path), distanceKm(dist), transportMode(transport) {}

std::string CustomTrip::getRouteString() const {
    std::string result = "";
    for (size_t i = 0; i < route.size(); ++i) {
        result += route[i];
        if (i + 1 < route.size()) result += " -> ";
    }
    return result;
}

void CustomTrip::displayDetails() const {
    std::cout << Utils::Color::BOLD_CYAN << "┌────────────────────────────────────────────────────────────────────────┐" << Utils::Color::RESET << "\n";
    std::cout << "│ " << Utils::Color::BOLD_YELLOW << "CUSTOM ROUTE TRIP: " << source << " to " << destination << Utils::Color::RESET 
              << std::string(70 - (21 + source.length() + destination.length()), ' ') << "│\n";
    std::cout << "│ " << "Shortest Distance : " << std::left << std::setw(15) << (std::to_string(distanceKm) + " km")
              << "Estimated Time : " << std::setw(20) << (transportMode ? transportMode->formatDuration(distanceKm) : "N/A") << "│\n";
    std::cout << "│ " << "Transport Mode    : " << std::left << std::setw(50) << (transportMode ? transportMode->getName() : "Standard") << "│\n";
    std::cout << "│ " << "Calculated Cost   : " << Utils::Color::BOLD_GREEN << std::left << std::setw(50) << Utils::formatCurrency(baseCost) << Utils::Color::RESET << "│\n";
    std::cout << "│ " << Utils::Color::CYAN << "Full Route Path   : " << std::left << std::setw(50) << getRouteString() << Utils::Color::RESET << "│\n";
    std::cout << Utils::Color::BOLD_CYAN << "└────────────────────────────────────────────────────────────────────────┘" << Utils::Color::RESET << "\n";
}

std::string CustomTrip::serialize() const {
    // Format: CUSTOM|source|destination|distanceKm|transportType|cost|r1,r2,r3
    std::ostringstream oss;
    int tType = transportMode ? static_cast<int>(transportMode->getType()) : 1;
    oss << "CUSTOM|" << source << "|" << destination << "|" << distanceKm << "|"
        << tType << "|" << baseCost << "|";
    for (size_t i = 0; i < route.size(); ++i) {
        oss << route[i];
        if (i + 1 < route.size()) oss << ",";
    }
    return oss.str();
}
