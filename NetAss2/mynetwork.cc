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
    int totalServers;  // Nombre total de serveurs
    int receivedResponses;  // Compteur pour les réponses reçues
    std::vector<int> maxValues;  // Pour stocker les valeurs maximales reçues des serveurs
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
    int n = 100;
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

    totalServers = n; // number of servers
    receivedResponses = 0;  // Initialize the counter for the number of responses received

    std::vector<int> shuffledNumbers = generateAndShuffle(n-1);

    // am I a client ?
    if (std::string(getName()).find("client") != std::string::npos)
    {
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

        std::vector<int> list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};  // Exemple de liste
        int partSize = list.size() / 5; // cut the list into 5 parts
        std::vector<int> partToSend(list.begin(), list.begin() + partSize);

        for (int i = 0; i < totalServers; i++) {
            std::string serverName = "server" + std::to_string(i + 1);
            cModule *serverModule = getParentModule()->getSubmodule(serverName.c_str());
            cGate *gate = gate("gate$o", i);

            // create a message to send to the server
            cMessage *msg = new cMessage("listPart");
            msg->setContextPointer(new std::vector<int>(partToSend));
            send(msg, gate);
        }
    }
    else {
        // i am a server
    }


}

void Node::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives at the module.

    // am I a server ?
    if (std::string(getName()).find("server") != std::string::npos) {

        // Seed the random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // define the distribution for real numbers between 0 and 1
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        // generate a random number between 0 and 1
        double random_number = dist(gen);

        // true if malicious
        bool malicious = random_number < 0.25;
        cMessage *answer;
        if (malicious){
            answer = new cMessage("Je suis un farfadet rempli de malices");
        }
        else
        {
            answer = new cMessage("Je suis un farfadet honnête✅✅✅✅✅");
        }
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
    else {
        std::string senderName = msg->getSenderModule()->getName();
        int receivedValue = std::stoi(msg->getName());

        // Stocker la valeur reçue dans une structure qui relie le serveur à ses valeurs
        serverValues[senderName].push_back(receivedValue);

        receivedResponses++;

        // Vérifier si toutes les réponses ont été reçues
        if (receivedResponses == totalServers * expectedResponsesPerServer) {
            std::cout << "All responses have been received" << std::endl;

            // Évaluer le fonctionnement de chaque serveur
            for (const auto &server: serverValues) {
                int correctCount = std::count_if(server.second.begin(), server.second.end(),
                                                 [expectedValue](int value) { return value == expectedValue; });

                // Supposons que si la majorité des réponses est correcte, alors le serveur fonctionne bien
                if (correctCount > server.second.size() / 2) {
                    std::cout << server.first << " fonctionne bien" << std::endl;
                    serverStatus[server.first] = true;
                } else {
                    std::cout << server.first << " ne fonctionne pas correctement" << std::endl;
                    serverStatus[server.first] = false;
                }
            }

            // notify the clients
            for (int i = 0; i < getParentModule()->getNumSubmodules(); i++) {
                cModule *submodule = getParentModule()->getSubmoduleAt(i);
                if (std::string(submodule->getName()).find("client") != std::string::npos && submodule != this) {
                    // Send a notification to each client
                    std::string statusMessage = "Server status: ...";  // Construct the status message
                    cMessage *notifMsg = new cMessage(statusMessage.c_str());
                    send(notifMsg, "gate$o");
                }
            }
        }
    }
}
