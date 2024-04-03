#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream> // Include the header for file input/output operations
#include <unistd.h> // for sleep()

using namespace omnetpp;


struct TopoEntry {
    int value;
    std::string type;
};

class Node : public cSimpleModule
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // Store connections to hosts
    std::map<std::string, cModule*> hostConnections;
    std::vector<std::pair<int, std::string>> maxValues;
    int totalServers;
    int receivedResponses = 0;

    std::vector<int> shuffledNumbers;
    std::list<std::string> defectiveServers;
    std::list<int> myList;
    // store the number of server
    int n;
};

// The module class needs to be registered with OMNeT++
Define_Module(Node);

// Function to divide a list into m equal parts
std::vector<std::list<int>> divideList(const std::list<int>& myList, int m) {
    std::vector<std::list<int>> dividedParts(m); // Vector of m lists

    int totalSize = myList.size(); // Total size of the list
    int partSize = totalSize / m; // Size of each part

    auto it = myList.begin(); // Iterator for the original list

    for (int i = 0; i < m && it != myList.end(); ++i) {
        // Extract elements for each part
        for (int j = 0; j < partSize && it != myList.end(); ++j) {
            dividedParts[i].push_back(*it);
            ++it;
        }
    }

    // Distribute remaining elements evenly across the parts
    while (it != myList.end()) {
        for (int i = 0; i < m && it != myList.end(); ++i) {
            dividedParts[i].push_back(*it);
            ++it;
        }
    }

    return dividedParts;
}

std::vector<int> generateAndShuffle(int n, std::list<std::string> forbidden) {
    if (forbidden.size() != 0)
    {
        // Create a vector containing integers from 0 to n
        std::vector<int> numbers(n);
        std::iota(numbers.begin(), numbers.end(), 0);

        // Shuffle the vector
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(numbers.begin(), numbers.end(), g);
        for (const auto& str : forbidden)
        {
            if (str != "")
            {
                std::string numericPart;
                for (char c : str) {
                    if (isdigit(c)) {
                        numericPart += c;
                    }
                }

                int intValue = std::stoi(numericPart);
                numbers.erase(std::remove(numbers.begin(), numbers.end(), intValue), numbers.end());
            }

        }

        return numbers;
    }

    // Create a vector containing integers from 0 to n
    std::vector<int> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 0);

    // Shuffle the vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);



    return numbers;
}



void Node::initialize()
{
    // Initialize is called at the beginning of the simulation.
    //retrieve nb of server
    std::ifstream file("topo.txt");
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return ;
    }

    std::vector<TopoEntry> topoEntries;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        TopoEntry entry;
        iss >> entry.value;
        iss.ignore(); // ignore ':'
        iss >> entry.type;
        topoEntries.push_back(entry);
    }
    file.close();

    for (const auto& entry : topoEntries) {
        if (entry.type == "serverNode")
            n = entry.value;
    }

    shuffledNumbers = generateAndShuffle(n,defectiveServers);

    // Keep only the first n/2 + 1 elements
    shuffledNumbers.resize(n / 2 + 1);
    //TODO maybe change the value of the totalServers
    totalServers = n / 2 + 1;

    myList = {10, 20, 30, 40, 50};
    int m = n; // nb of server to begin
    int subSize = myList.size() / m;
    while (subSize < 2)
    {
        m--;
        subSize = myList.size() / m;
    }
    std::vector<std::list<int>> dividedParts = divideList(myList, m);

    // am I a client ?
    if (std::string(getName()).find("client") != std::string::npos) {
        // for every sublist
        for(int j = 0; j < m; j++)
        {
            std::list<int> intArray = dividedParts[j];
            std::string messageContent = "";

            for (const auto& elem : intArray)
            {
                messageContent += std::to_string(elem);
                messageContent += ",";
            }
            messageContent.erase(messageContent.size() - 1); // erase the last character
            // for every server
            for(int i = 0; i < n/2+1; i++)
            {
                std::string servName = "server" + std::to_string(shuffledNumbers[i]);

                hostConnections[servName.c_str()] = getParentModule()->getSubmodule(servName.c_str());
                if (hostConnections[servName.c_str()] == nullptr)
                {
                    servName = "serverMalicious" + std::to_string(shuffledNumbers[i]);
                    hostConnections[servName.c_str()] = getParentModule()->getSubmodule(servName.c_str());

                }
                // find the right gate and iterate through gates of the client module
                cModule *serverModule = hostConnections[servName.c_str()];
                cGate *serverGate = nullptr;

                // Iterate through gates of the client module
                for (int j = 0; j < gateSize("gate$o"); j++) {
                    cGate *gate = this->gate("gate$o", j);

                    if (gate->getType() == cGate::OUTPUT && gate->getPathEndGate()->getOwnerModule() == serverModule) {
                        serverGate = gate;
                        break;
                    }
                }


                // Send a message to the server

                cMessage *msg = new cMessage(messageContent.c_str());
                send(msg, serverGate);

            }
        }


    }
    else {
        // i am a server
    }

}

void Node::handleMessage(cMessage *msg)
{
    std::ofstream outputFile("output.txt", std::ios::app);
    // The handleMessage() method is called whenever a message arrives at the module.

    // am I a server ?
    if (std::string(getName()).find("server") != std::string::npos) {

        const char* content = msg->getName();
        std::vector<int> subList;

        std::stringstream ss(content); // stringstream from the content
        std::string token; // temporary string to hold each token

        // iterate through each token separated by ','
        while (getline(ss, token, ',')) {
            // convert the token string to an integer and add it to the list
            subList.push_back(std::stoi(token));
        }

        cMessage *answer;

        // am I malicious ?
        std::string str;
        if (std::string(getName()).find("Malicious") != std::string::npos){
            // Je suis un farfadet rempli de malices
            str = std::to_string(subList[0]);

        }
        else
        {
            str = std::to_string(*std::max_element(subList.begin(), subList.end() ));
        }
        answer = new cMessage(str.c_str());
        std::cout << "serverID : " << getId() << " - " << "Majority from subtask : " << str << std::endl;
        outputFile << "serverID : " << getId() << " - " << "Majority from subtask : " << str << std::endl;
        // get the pointer to the sender module
        cModule *senderModule = msg->getSenderModule();
        cGate *serverGate = nullptr;

        // iterate through gates of the client module
        for (int j = 0; j < gateSize("gate$o"); j++) {
            cGate *gate = this->gate("gate$o", j);

            if (gate->getType() == cGate::OUTPUT && gate->getPathEndGate()->getOwnerModule() == senderModule) {
                serverGate = gate;
                break;
            }
        }

        // Send the message back to the sender module
        send(answer, serverGate);

    }
    else // i am a client
    {
        std::string msgContent = msg->getName();
        if (msgContent.find(":") != std::string::npos) {
            // Le message est une liste de serveurs défectueux
            size_t lastColonPos = msgContent.find_last_of(':');
            msgContent = msgContent.substr(lastColonPos + 1);
            std::stringstream ss(msgContent);
            std::string serverInfo;

            while (getline(ss, serverInfo, ',')) {
                // Store defective Servers in the list only if it is not already in
                auto it = std::find(defectiveServers.begin(), defectiveServers.end(), serverInfo);
                if (it == defectiveServers.end())
                {
                    defectiveServers.push_back(serverInfo);
                }

            }

            // additional code, next round
            EV << "New malicious server list of " << this->gate("gate$i",0)->getPathEndGate()->getOwnerModule()->getName() << " : " << endl;
            for (const auto& str : defectiveServers) {
                    EV << str << endl;
            }

            shuffledNumbers = generateAndShuffle(n,defectiveServers);

            // Keep only the first n/2 + 1 elements
            shuffledNumbers.resize(n / 2 + 1);
            //TODO maybe change the value of the totalServers
            totalServers = n / 2 + 1;

            myList = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
            int m = n; // nb of server to begin
            int subSize = myList.size() / m;
            while (subSize < 2)
            {
                m--;
                subSize = myList.size() / m;
            }
            std::vector<std::list<int>> dividedParts = divideList(myList, m);

            // for every sublist
            for(int j = 0; j < m; j++)
            {
                std::list<int> intArray = dividedParts[j];
                std::string messageContent = "";

                for (const auto& elem : intArray)
                {
                    messageContent += std::to_string(elem);
                    messageContent += ",";
                }
                messageContent.erase(messageContent.size() - 1); // erase the last character
                // for every server
                for(int i = 0; i < n/2+1; i++)
                {
                    std::string servName = "server" + std::to_string(shuffledNumbers[i]);

                    hostConnections[servName.c_str()] = getParentModule()->getSubmodule(servName.c_str());
                    if (hostConnections[servName.c_str()] == nullptr)
                    {
                        servName = "serverMalicious" + std::to_string(shuffledNumbers[i]);
                        hostConnections[servName.c_str()] = getParentModule()->getSubmodule(servName.c_str());

                    }
                    // find the right gate and iterate through gates of the client module
                    cModule *serverModule = hostConnections[servName.c_str()];
                    cGate *serverGate = nullptr;

                    // Iterate through gates of the client module
                    for (int j = 0; j < gateSize("gate$o"); j++) {
                        cGate *gate = this->gate("gate$o", j);

                        if (gate->getType() == cGate::OUTPUT && gate->getPathEndGate()->getOwnerModule() == serverModule) {
                            serverGate = gate;
                            break;
                        }
                    }


                    // Send a message to the server

                    cMessage *msg = new cMessage(messageContent.c_str());
                    send(msg, serverGate);

                }
            }


        }
        else {
            int receivedMax = std::stoi(msg->getName());
            std::string senderName = msg->getSenderModule()->getName();
            maxValues.push_back(std::make_pair(receivedMax, senderName));

            receivedResponses++;

            // Verify if all the responses has been received
            if (receivedResponses == totalServers) {

                //TODO get the real Ip address
                //cModule * host = getContainingNode(this);
                //L3Address addr = L3AddressResolver().addressOf(host);


                // Determine the majority value among the max values
                std::map<int, int> counts;
                for (const auto &maxVal: maxValues) {
                    counts[maxVal.first]++;  // Increment the count for this max value
                }

                int majorityValue = maxValues[0].first;  // Default to the first value
                int maxCount = 0;
                for (const auto &count: counts) {
                    if (count.second > maxCount) {
                        majorityValue = count.first;
                        maxCount = count.second;
                    }
                }

                std::cout << "clientID : " << getId() << " - " << "Majority from subtask : " << majorityValue << std::endl;
                //outputFile << "clientID : " << getId() << " - " << "Majority from subtask : " << majorityValue << std::endl;
                std::stringstream s;
                for (const auto &maxVal: maxValues) {
                    if (maxVal.first != majorityValue) {
                        defectiveServers.push_back(maxVal.second);
                        s << maxVal.second << ",";
                    }
                }
                std::string nonMajorityServers = s.str();
                if (!nonMajorityServers.empty()) {
                    nonMajorityServers.pop_back(); // Suppress the last comma
                }

                std::stringstream ss;
                simtime_t currentTime = simTime();
                std::string timestamp = std::to_string(
                        currentTime.dbl());

                cModule *networkModule = getParentModule();
                std::string ipAddress = std::to_string(getId());

                ss << timestamp << ":" << ipAddress << ":" << nonMajorityServers << ",";
                std::string maliciousServersInfo = ss.str();
                if (!maliciousServersInfo.empty()) {
                    maliciousServersInfo.pop_back();
                }


                for (int j = 0; j < gateSize("gate$o"); j++) {
                    cGate *gate = this->gate("gate$o", j);

                    if (gate->getType() == cGate::OUTPUT &&
                        std::string(gate->getPathEndGate()->getOwnerModule()->getName()).find("client") !=
                        std::string::npos) {
                        cMessage *msg = new cMessage(maliciousServersInfo.c_str());
                        send(msg, gate);
                    }
                }



            }

        }
    }
    outputFile.close();
}
