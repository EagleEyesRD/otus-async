#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

class PackManager {
    std::vector<std::string> statpack;
    std::vector<std::string> dynampack;
public:
    PackManager(size_t num_commands);
    void Process(std::istream& source_stream);
    void getData(const std::string& data);
    bool IsRunPacketFile(std::string command);
    void RunPacket(std::string command);
private:
    bool IsEndOfDymamicPacket(std::vector<std::string>& vpack);
    bool IsStartOfDynamicPacket(std::string command, std::vector<std::string>& pack);
    bool AreThereSubPackets(std::vector<std::string>& vpack);
    bool IsEndOfStaticPacket(std::string command);
    size_t limit = 0;
    std::mutex process_mutex;
};


