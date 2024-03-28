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

private:
    int receivedresponse;
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
        // i am a client
        // put the received messages to 0
    }


}
std::vector<int> maxValues;

void Node::handleMessage(cMessage *msg)
{
    if (std::string(msg->getSenderModule()->getName()).find("server") != std::string::npos) {
        // i am a server
        std::string Message = msg->getName();

    }
    else {
        // i am a client
        std::cout << "Client received message: " << msg->getName() << std::endl;
        maxValues.push_back(std::stoi(msg->getName()));
        if (maxValues.size() == n)
        {
            SendScore(maxValues);
        }
    }
}


void SendScore(std::vector<bool> maxvector)
{
    //iterate through the maxValues vector and print the majority element
    int count = 1;
    int majority = maxValues[0];
    for (int i = 1; i < n; i++)
    {
        if (maxValues[i] == majority)
            count++;
        else
            count--;
        if (count == 0)
        {
            majority = maxValues[i];
            count = 1;
        }
    }
    //for each client send if the server i is reliable or not
    for (int i = 0; i < n; i++)
    {
        if (maxValues[i] == majority)
        {
            cMessage *msg = new cMessage("1");
            send(msg, "gate$o", i);
        }
        else
        {
            cMessage *msg = new cMessage("0");
            send(msg, "gate$o", i);
        }
    }
}
