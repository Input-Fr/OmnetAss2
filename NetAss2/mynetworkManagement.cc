#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct TopoEntry {
    int value;
    std::string type;
};

int main() {
    int delay = 0;
    int nbOfClient = 0;
    int nbOfServer = 0;

    std::ifstream file("topo.txt");
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
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
        if (entry.type == "delay")
            delay = entry.value;
        else if (entry.type == "clientNode")
            nbOfClient = entry.value;
        else if (entry.type == "serverNode")
            nbOfServer = entry.value;
    }

    std::stringstream ss;
    ss << "simple Node\n"
       << "{\n"
       << "    parameters:\n"
       << "        @display(\"i=block/routing\");\n"
       << "    gates:\n"
       << "        inout gate[];  // declare two way connections \n"
       << "}\n\n"
       << "network Network\n"
       << "{\n"
       << "    @display(\"bgb=500,500\"); \n"
       << "    types:\n"
       << "        channel Channel extends ned.DelayChannel\n"
       << "        {\n"
       << "            delay = " << delay << "ms;\n"
       << "        }\n"
       << "        submodules:\n";

    for (int i = 0; i < nbOfServer; ++i) {
        ss << "        server" << i << " : Node {\n"
           << "            @display(\"p=50," << 50 * i + 70 << "\");\n"
           << "        }\n";
    }

    for (int i = 0; i < nbOfClient; ++i) {
        ss << "        client" << i << " : Node {\n"
           << "            @display(\"p=250," << 50 * i + 70 << "\");\n"
           << "        }\n";
    }

    ss << "    connections:\n";

    for (int i = 0; i < nbOfClient; ++i) {
        for (int j = i + 1; j < nbOfClient; ++j) {
            ss << "        client" << i << ".gate++ <--> Channel <--> client" << j << ".gate++;\n";
        }
        for (int j = 0; j < nbOfServer; ++j) {
            ss << "        client" << i << ".gate++ <--> Channel <--> server" << j << ".gate++;\n";
        }
    }

    ss << "}";

    std::ofstream outFile("netShape.ned");
    if (!outFile.is_open()) {
        std::cerr << "Unable to create output file" << std::endl;
        return 1;
    }

    outFile << ss.str();
    outFile.close();

    return 0;
}
