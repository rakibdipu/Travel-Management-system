#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../core/Utils.h"

enum class TransportType {
    NON_AC_BUS = 1,
    AC_BUS,
    EXPRESS_TRAIN,
    DOMESTIC_FLIGHT
};

class Transport {
protected:
    std::string name;
    double speedKmh;
    double farePerKm;
    TransportType type;

public:
    Transport(const std::string& n, double speed, double fare, TransportType t)
        : name(n), speedKmh(speed), farePerKm(fare), type(t) {}
    virtual ~Transport() = default;

    std::string getName() const { return name; }
    double getSpeed() const { return speedKmh; }
    double getFarePerKm() const { return farePerKm; }
    TransportType getType() const { return type; }

    virtual double calculateFare(double distanceKm) const {
        return distanceKm * farePerKm;
    }

    virtual double calculateDurationHours(double distanceKm) const {
        return distanceKm / speedKmh;
    }

    virtual std::string formatDuration(double distanceKm) const {
        double hours = calculateDurationHours(distanceKm);
        int h = static_cast<int>(hours);
        int m = static_cast<int>((hours - h) * 60);
        std::ostringstream oss;
        if (h > 0) oss << h << "h ";
        oss << m << "m";
        return oss.str();
    }

    virtual void displaySpecs() const {
        std::cout << "• " << std::left << std::setw(18) << name
                  << " | Speed: " << std::setw(8) << (std::to_string(static_cast<int>(speedKmh)) + " km/h")
                  << " | Rate: " << std::fixed << std::setprecision(2) << farePerKm << " BDT/km\n";
    }

    static std::shared_ptr<Transport> createTransport(TransportType t);
};

class NonACBus : public Transport {
public:
    NonACBus() : Transport("Economy Non-AC Bus", 45.0, 1.80, TransportType::NON_AC_BUS) {}
};

class ACBus : public Transport {
public:
    ACBus() : Transport("Executive AC Bus", 55.0, 2.80, TransportType::AC_BUS) {}
};

class ExpressTrain : public Transport {
public:
    ExpressTrain() : Transport("Intercity Express Train", 60.0, 1.50, TransportType::EXPRESS_TRAIN) {}
};

class DomesticFlight : public Transport {
public:
    DomesticFlight() : Transport("Domestic Air Flight", 450.0, 12.00, TransportType::DOMESTIC_FLIGHT) {}
    
    double calculateFare(double distanceKm) const override {
        // Base airport surcharge + per km
        return 2500.0 + (distanceKm * farePerKm * 0.75);
    }
};

inline std::shared_ptr<Transport> Transport::createTransport(TransportType t) {
    switch (t) {
        case TransportType::NON_AC_BUS:     return std::make_shared<NonACBus>();
        case TransportType::AC_BUS:         return std::make_shared<ACBus>();
        case TransportType::EXPRESS_TRAIN:  return std::make_shared<ExpressTrain>();
        case TransportType::DOMESTIC_FLIGHT:return std::make_shared<DomesticFlight>();
    }
    return std::make_shared<ACBus>();
}

#endif // TRANSPORT_H
