// main.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <stdexcept>
#include <time.h>

// PROFILER
#include <tracy/Tracy.hpp>

#include "quadtree.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " <points.csv> [-q]\n";
        return 1;
    }

#ifdef TRACY_ENABLE
    std::cout << "WARNING: will not start until connected to a Tracy Profiler!\n"
              << std::flush;
    while (!tracy::GetProfiler().IsConnected())
        std::this_thread::yield();
#endif

    const QuadTree::QuadTree quadtree(argv[1]);

    std::cout << "\n";
    quadtree.analyze();
    std::cout << "\n";

    if (argc == 3 && strcmp(argv[2], "-q") == 0)
    {
        // Quit after buiding and analysing
        std::cout << "EXITING: early exit parameter found\n";

        // Flush Tracy before exiting
        FrameMark;
        TracyMessageL("Early exit");

        return 0;
    }

    std::string line;
    while (true)
    {
        std::cout << "\nEnter lat,lon,radius,limit (or 'quit'): ";

        if (!std::getline(std::cin, line))
            break;

        if (line == "quit" || line == "exit")
            break;

        try
        {
            std::stringstream ss(line);

            std::string latStr;
            std::string lonStr;
            std::string radiusStr;
            std::string limitStr;

            if (!std::getline(ss, latStr, ',') ||
                !std::getline(ss, lonStr, ',') ||
                !std::getline(ss, radiusStr, ',') ||
                !std::getline(ss, limitStr))
            {
                throw std::runtime_error("Invalid format");
            }

            const double latitude = std::stod(latStr);
            const double longitude = std::stod(lonStr);
            const double radiusKm = std::stod(radiusStr);
            const double limit = std::stod(limitStr);

            const auto result = quadtree.find(latitude, longitude, radiusKm, limit);

            std::cout << result << '\n';
        }
        catch (const std::exception &)
        {
            std::cerr << "Invalid input. Expected format: lat,lon,radius,limit\n";
        }
    }

    return 0;
}