#include "utils.h"
void PackManager::Process(std::istream& source_stream) {
    std::lock_guard<std::mutex> guard{ process_mutex };
    for (std::string line; std::getline(source_stream, line);) {
        total_lines++;
	if (IsRunPacketFile(line))
            RunPacket(line);
    }
    
    total_blocks = 0;
}

void PackManager::getData(const std::string& line) {
    if (IsRunPacketFile(line))
        RunPacket(line);
}

PackManager::PackManager(size_t num_commands) {
    statpack = std::vector<std::string>();
    dynampack = std::vector<std::string>();
    limit = num_commands;
}
bool PackManager::IsEndOfDymamicPacket(std::vector<std::string>& vpack) {
    int cntOpenBrackets = 0;
    int cntCloseBrackets = 0;
    for (auto& xcomm : vpack) {
        if (xcomm == "{")
            cntOpenBrackets++;
        if (xcomm == "}")
            cntCloseBrackets++;
    }
    return (cntOpenBrackets == cntCloseBrackets && cntOpenBrackets > 0 && cntCloseBrackets > 0);
}

bool PackManager::IsStartOfDynamicPacket(std::string command, std::vector<std::string>& pack) {
    std::lock_guard<std::mutex> guard{data_mutex};
    int cntOpenBrackets = 0;
    int cntCommands = 0;
    pack.push_back(command);
    for (auto& xcomm : pack) {
        if (xcomm == "{") {
            cntOpenBrackets++;
        }
        else if (xcomm != "}")
            cntCommands++;
    }
    return cntOpenBrackets > 0 && cntCommands == 0;
};

bool PackManager::AreThereSubPackets(std::vector<std::string>& vpack) {
    int cntOpenBrackets = 0;
    int cntCommands = 0;
    for (auto& xcomm : vpack) {
        if (xcomm == "{")
            cntOpenBrackets++;
        else if (xcomm != "}")
            cntCommands++;
    }
    return cntOpenBrackets > 0 && cntCommands > 1;
};

bool PackManager::IsEndOfStaticPacket(std::string command) {
    std::lock_guard<std::mutex> guard{data_mutex};
    if (command == "EOF")
        return true;

    if (command == "{") {
        dynampack.push_back(command);
        if (statpack.size() > 0)
            return true;
        else
            return false;
    }
    else {
        statpack.push_back(command);
    }
    return statpack.size() == limit;
};

bool PackManager::IsRunPacketFile(std::string command) {
    if (dynampack.size() == 0) {
        if (IsEndOfStaticPacket(command))
            return true;

        return false;
    }
    else {
        if (command == "EOF")
            return true;

        if (!IsStartOfDynamicPacket(command, dynampack))
            if (IsEndOfDymamicPacket(dynampack))
                return true;

        return false;
    }
}

void PackManager::RunPacket(std::string command){
    if (statpack.size() > 0) {
        for (std::string& xcommand : statpack)
            if (xcommand != "{")
                if (xcommand != "}")
                    if (xcommand != "EOF")
                        //std::cout << xcommand << ", ";
        statpack.clear();
    }
    else {
        if (dynampack.size() > 0) {
            if (command != "EOF")
            {
                if (IsEndOfDymamicPacket(dynampack)) {
                    for (std::string& xcommand : dynampack)
                        if (xcommand != "{")
                            if (xcommand != "}")
                                //std::cout << xcommand << ", ";
                }
            }
            dynampack.clear();
        }
    }
}

void PackManager::print_statistics(std::ostream& output_stream) {
    std::lock_guard<std::mutex> guard{data_mutex};	
    std::lock_guard<std::mutex> guard2{process_mutex};
    output_stream << "main thread - "
                  << total_lines << " lines, "
                  << statpack.size() + dynampack.size() << " commands, " << std::endl;
}
