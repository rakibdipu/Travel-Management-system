cmake_minimum_required(VERSION 3.10)
project(TravelManagementSystem VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Modular Target
add_executable(TravelManagementSystem_modular
    main.cpp
    src/core/Graph.cpp
    src/models/User.cpp
    src/models/Trip.cpp
    src/models/Booking.cpp
    src/services/FileManager.cpp
    src/services/AuthService.cpp
    src/services/BookingService.cpp
    src/ui/ConsoleUI.cpp
)

# Standalone Target
add_executable(TravelManagementSystem
    TravelManagementSystem_standalone.cpp
)
