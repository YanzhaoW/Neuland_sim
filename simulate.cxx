
#include "RunManager.h"

int main() {
    // Initilizer initer{};

    // YAML::Node node = YAML::LoadFile("../test.yaml");
    // std::cout << node["person"].size() <<std::endl;
    // for(auto it = node.begin(); it != node.end(); ++it){
    //     YAML::Node key = it->second;
    //     std::cout << key.size() << std::endl;
    // }
    // std::cout << node.size() <<std::endl;
    // std::cout << node["person"] << std::endl;

    auto runs = RunManager::GetInstance();
    runs->Load_Yaml("../test.yaml");
    runs->Parse_Yaml();

    runs->Start();

    


    return EXIT_SUCCESS;
}
