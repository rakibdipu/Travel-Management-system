#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include "../core/Utils.h"
#include "Transport.h"

class Trip {
protected:
    std::string destination;
    double baseCost;

public:
    Trip(const std::string& dest, double cost) : destination(dest), baseCost(cost) {}
    virtual ~Trip() = default;

    virtual std::string getDestination() const { return destination; }
    virtual double getBaseCost() const { return baseCost; }

    virtual void displayDetails() const = 0;
    virtual std::string getTripType() const = 0;
    virtual std::string serialize() const = 0;
};

class TourPackage : public Trip {
private:
    std::string packageId;
    std::string packageName;
    int durationDays;
    int durationNights;
    std::string hotelRating; // e.g. "5-Star Luxury Resort"
    std::string inclusions;  // e.g. "Breakfast, AC Transport, Tour Guide, Entry Tickets"
    std::string description;

public:
    TourPackage(const std::string& id, const std::string& name, const std::string& dest,
                double cost, int days, int nights, const std::string& hotel,
                const std::string& inc, const std::string& desc);

    std::string getPackageId() const { return packageId; }
    std::string getPackageName() const { return packageName; }
    int getDurationDays() const { return durationDays; }
    int getDurationNights() const { return durationNights; }
    std::string getHotelRating() const { return hotelRating; }
    std::string getInclusions() const { return inclusions; }
    std::string getDescription() const { return description; }

    void displayDetails() const override;
    std::string getTripType() const override { return "Tour Package"; }
    std::string serialize() const override;

    // Operator overloading for sorting by cost
    bool operator<(const TourPackage& other) const {
        return baseCost < other.baseCost;
    }
};

class CustomTrip : public Trip {
private:
    std::string source;
    std::vector<std::string> route;
    int distanceKm;
    std::shared_ptr<Transport> transportMode;

public:
    CustomTrip(const std::string& src, const std::string& dest,
               const std::vector<std::string>& path, int dist,
               std::shared_ptr<Transport> transport);

    std::string getSource() const { return source; }
    const std::vector<std::string>& getRoute() const { return route; }
    int getDistanceKm() const { return distanceKm; }
    std::shared_ptr<Transport> getTransport() const { return transportMode; }

    std::string getRouteString() const;
    void displayDetails() const override;
    std::string getTripType() const override { return "Custom Route"; }
    std::string serialize() const override;
};

#endif // TRIP_H
