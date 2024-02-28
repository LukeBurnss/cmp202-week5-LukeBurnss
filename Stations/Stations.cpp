// CMP202, a sample mini-project
// Mini-Project: Stations, an extension based on Trains Mini-Project 
// using a group of condition variables
// Javad Zarrin, j.zarrin@abertay.ac.uk
// 


#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include <fstream>
#include <sstream>
#include <atomic>
#include <windows.h>


// Function to hide the console cursor for cleaner simulation display
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Get the console handle
    CONSOLE_CURSOR_INFO info; // Console cursor information structure
    info.dwSize = 100; // The size of the cursor, from 1 to 100. The size is irrelevant when hiding the cursor
    info.bVisible = FALSE; // Set the cursor visibility to FALSE to hide it
    SetConsoleCursorInfo(consoleHandle, &info); // Apply the settings to the console
}

// ANSI color codes for terminal text coloring
const char* ANSI_RESET = "\033[0m";
const char* ANSI_RED = "\033[41m";
const char* ANSI_GREEN = "\033[42m";
const char* ANSI_BLUE = "\033[44m";
const char* ANSI_YELLOW = "\033[43m";

// RailwaySystem class definition
class RailwaySystem {
public:
    RailwaySystem(); // Constructor
    ~RailwaySystem(); // Destructor
    void startSimulation(); // Function to start the railway simulation

private:
    struct Segment { // Segment structure representing a piece of track
        std::mutex mutex; // Mutex for synchronization
        std::condition_variable cond; // Condition variable for segment occupancy
        bool occupied = false; // Flag indicating if the segment is occupied
    };

    void trainA(); // Function to simulate train A's movement
    void trainB(); // Function to simulate train B's movement

    void displayTracks(); // Function to display the current state of the tracks
    void logEvent(const std::string& event); // Function to log events to a file

    std::mutex displayMutex; // Mutex for synchronizing display output
    std::vector<std::unique_ptr<Segment>> segments; // Vector of track segments
    //const int totalLength = 60; // Total length of the track
    const int segmentLength = 10; // Length of a single track segment excluding the station
    const int shortStationLength = 2; // Length of a short station
    int positionA, positionB; // Positions of trains A and B
    std::ofstream logFile; // File stream for logging
    std::atomic<bool> simulationActive; // Atomic flag to control the simulation loop

    const int totalSegments = 5 + 2; // Original 5 segments + 2 new segments
    const int totalLength = 60 + 2 * (segmentLength + shortStationLength); // Adjusted total length

};

// Constructor initializes the simulation
RailwaySystem::RailwaySystem() : positionA(1), positionB(totalLength - 1), simulationActive(true) {
    logFile.open("RailwaySystemLog.txt", std::ofstream::out | std::ofstream::app); // Open log file
    int colors[] = { 31, 33, 32, 34, 36 }; // ANSI color codes for visualization

    // Create and add segments to the vector
    for (int i = 0; i < totalSegments; ++i) {
        auto segment = std::make_unique<Segment>();
        segments.push_back(std::move(segment));
    }
}

// Destructor closes the log file if it's open
RailwaySystem::~RailwaySystem() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

// startSimulation starts the simulation by creating threads for trains and display
void RailwaySystem::startSimulation() {
    // Thread for displaying the tracks
    std::thread displayThread([&]() {
        while (simulationActive) {
            displayTracks();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        });

    // Threads for trains A and B
    std::thread threadA(&RailwaySystem::trainA, this);
    std::thread threadB(&RailwaySystem::trainB, this);

    // Wait for train threads to complete
    threadA.join();
    threadB.join();

    // Stop the simulation and wait for the display thread to complete
    simulationActive = false;
    displayThread.join();
}

void RailwaySystem::trainA() {
    // Simulates the movement of train A around the track
    std::string trainName = "Train A"; 

    while (true) { // Infinite loop to simulate continuous movement

        // Calculate the current segment index based on train A's position
        int segmentIndex = positionA / (segmentLength + shortStationLength);

        // Each station has 2 spaces. This determines if the current position is the second space of a station based on modulo arithmetic
        // We reserve the first space of each station for trainB stopping time and the second space for trainA stopping.
        bool isStation = (positionA % (segmentLength + shortStationLength)) == 1;

        // If the train is at the start of a station, wait for the segment to be free and then occupy it
        if (isStation) {
            std::unique_lock<std::mutex> lock(segments[segmentIndex]->mutex); // Lock the current segment's mutex
            segments[segmentIndex]->cond.wait(lock, [this, segmentIndex] { return !segments[segmentIndex]->occupied; }); // Wait until the segment is not occupied
            segments[segmentIndex]->occupied = true; // Occupy the segment
        }

        // Move the train forward by one unit
        positionA = (positionA + 1) % totalLength;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate time taken to move

        // Check if the train has reached the end of a segment
        bool isSegmentEnd = (positionA % (segmentLength + shortStationLength)) == 0;
        segmentIndex = positionA / (segmentLength + shortStationLength);
        int prevIndex = segmentIndex - 1; // Calculate the index of the previous segment
        if (prevIndex < 0) prevIndex = segments.size() - 1; // Wrap around if necessary
        if (isSegmentEnd) {
            // Free the previous segment and notify other trains
            std::lock_guard<std::mutex> lock(segments[prevIndex]->mutex); // Lock the previous segment's mutex
            segments[prevIndex]->occupied = false; // Mark the previous segment as not occupied
            segments[prevIndex]->cond.notify_one(); // Notify other trains waiting for this segment
        }

        // Log the completion of one iteration for train A
        logEvent("Train A has finished one iteration of its journey.");
    }
}


void RailwaySystem::trainB() {
    // Simulates the movement of train B around the track
    std::string trainName = "Train B"; 

    while (true) { // Infinite loop to simulate continuous movement
        // Calculate the current segment index based on train B's position
        int segmentIndex = positionB / (segmentLength + shortStationLength);

        // Each station has 2 spaces. This determines if the current position is the first space of a station based on modulo arithmetic
        // We reserve the first space of each station for trainB stopping time and the second space for trainA stopping.
        bool isStation = (positionB % (segmentLength + shortStationLength)) == 0;

        // If the train is at a station, wait for the previous segment to be free and then occupy it
        if (isStation) {
            int prevIndex = segmentIndex - 1; // Calculate the index of the previous segment
            if (prevIndex < 0) prevIndex = segments.size() - 1; // Wrap around if necessary
            std::unique_lock<std::mutex> lock(segments[prevIndex]->mutex); // Lock the previous segment's mutex
            segments[prevIndex]->cond.wait(lock, [this, prevIndex] { return !segments[prevIndex]->occupied; }); // Wait until the segment is not occupied
            segments[prevIndex]->occupied = true; // Occupy the segment
        }

        // Move the train backward by one unit, handling wrap-around at the start of the track
        positionB = (positionB - 1) % totalLength;
        if (positionB < 0) positionB = totalLength - 1;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate time taken to move

        // Check if the train has reached the end of a segment
        bool isSegmentEnd = ((positionB) % (segmentLength + shortStationLength)) == 1;
        if (isSegmentEnd) {
            // Free the current segment and notify other trains
            std::lock_guard<std::mutex> lock(segments[segmentIndex]->mutex); // Lock the current segment's mutex
            segments[segmentIndex]->occupied = false; // Mark the current segment as not occupied
            segments[segmentIndex]->cond.notify_one(); // Notify other trains waiting for this segment
        }

        // Log the completion of the journey for train B (though this loop never exits as written)
        logEvent("Train B has finished its journey.");
    }
}




void RailwaySystem::displayTracks() {
    std::lock_guard<std::mutex> lock(displayMutex);
    std::cout << "\x1B[2J\x1B[H"; // Clears the screen
    int index = 0;
    for (auto& obj : segments) {
        if (obj->occupied) {
            std::cout << ANSI_RED << "index=" << index << " ocup=" << obj->occupied << " posA=" << positionA << " posB=" << positionB << " isA-Station=" << ((positionA % (segmentLength + shortStationLength)) < shortStationLength) << " isB-Station=" << ((positionB % (segmentLength + shortStationLength)) < shortStationLength) << "\n" << ANSI_RESET;
        }
        else {
            std::cout << ANSI_GREEN << "index=" << index << " ocup=" << obj->occupied << " posA=" << positionA << " posB=" << positionB << " isA-Station=" << ((positionA % (segmentLength + shortStationLength)) < shortStationLength) << " isB-Station=" << ((positionB % (segmentLength + shortStationLength)) < shortStationLength) << "\n" << ANSI_RESET;
        }
       
        index++;
    }

    // Starting station
    std::cout << ANSI_GREEN << " " << ANSI_RESET;

    for (int i = 0; i < totalLength ; ++i) {
        int segmentIndex = (i  / (segmentLength + shortStationLength)); // Determine segment index
        bool isStation = (i  % (segmentLength + shortStationLength)) < shortStationLength; // Check if position is within a station

        //std::cout <<"loc="<< i<< ", segIndex=" << segmentIndex << ", isStation=" << isStation << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(3));

        // Display train A or B if present, else display track or station
        if ((i == positionA)&&(!isStation)) {
            std::cout << "\033[1;37m" << 'A' << ANSI_RESET;
        }
        else if ((i == positionA) && (isStation)) {
            std::cout << ANSI_BLUE << "A" << ANSI_RESET;
        }
        else if ((i == positionB) && (!isStation)) {
            std::cout << "\033[1;37m" << 'B' << ANSI_RESET;
        }
        else if ((i == positionB) && (isStation)) {
            std::cout << ANSI_BLUE << "B" << ANSI_RESET;
        }
        else if (isStation) {
            std::cout << ANSI_BLUE << " " << ANSI_RESET; // Display station with color
        }
        else {
            std::cout << "-"; // Display track segment
        }

        if (i== totalLength-1)  std::cout << ANSI_GREEN << " " << ANSI_RESET << ANSI_BLUE << "  " << ANSI_RESET;
    }

    // Ending station
   

    std::cout << std::endl << std::flush; // Flush to ensure immediate output
}



void RailwaySystem::logEvent(const std::string& event) {
    if (logFile.is_open()) {
        logFile << event << std::endl;
    }
}

int main() {
    hideCursor();
    RailwaySystem railwaySystem;
    railwaySystem.startSimulation();

    return 0;
}
