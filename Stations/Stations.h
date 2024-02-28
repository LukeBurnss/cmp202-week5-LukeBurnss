
#include <vector>

#include <mutex>
#include <thread>

class RailwaySystem {
public:
    RailwaySystem();
    void startSimulation();

private:
    void trainA();
    void trainA2();
    void trainB();
    void trainB2();
    void enterSharedTrack(const std::string& trainName);
    void onSharedTrack(const std::string& trainName);
    void leaveSharedTrack(const std::string& trainName);
    void displayTracks();

    std::mutex sharedTrackMutex;
    int positionA, positionB;
    int sharedSectionStart, sharedSectionEnd;
};
#pragma once
