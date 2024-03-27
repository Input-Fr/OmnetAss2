#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream> // Include the header for file input/output operations

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
};

// The module class needs to be registered with OMNeT++
Define_Module(Node);

std::vector<int> generateAndShuffle(int n) {
    // Create a vector containing integers from 1 to n
    std::vector<int> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 1);

    // Shuffle the vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);

    // Keep only the first n/2 + 1 elements
    numbers.resize(n / 2 + 1);

    return numbers;
}

void Node::initialize()
{
    // Initialize is called at the beginning of the simulation.
    //retrieve nb of server
    int n = 0;
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

    std::vector<int> shuffledNumbers = generateAndShuffle(n-1);

    // am I a client ?
    if (std::string(getName()).find("client") != std::string::npos) {
        // i am a client
        for(int i = 0; i < n/2+1; i++)
        {
            std::string servName = "server" + std::to_string(shuffledNumbers[i]);
            std::cout << "retrieved name : " << servName << std::endl;

            hostConnections[servName.c_str()] = getParentModule()->getSubmodule(servName.c_str());

            // find the right gate and iterate through gates of the client module
            cModule *serverModule = hostConnections[servName.c_str()];
            cGate *serverGate = nullptr;

            // Iterate through gates of the client module
            for (int j = 0; j < gateSize("gate$o"); j++) {
                cGate *gate = this->gate("gate$o", j);
                std::cout << "retrieved gate : " << gate->getPathEndGate()->getOwnerModule() << std::endl;
                std::cout << "wanted gate : " << serverModule << std::endl;

                if (gate->getType() == cGate::OUTPUT && gate->getPathEndGate()->getOwnerModule() == serverModule) {
                    serverGate = gate;
                    break;
                }
            }


            // Send a message to the server
            cMessage *msg = new cMessage("wassup bro");
            send(msg, serverGate);
        }
    }
    else {
        // i am a server
    }


}

void Node::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives at the module.
    //send(msg, "out");
}
