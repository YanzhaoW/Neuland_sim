
#include <boost/program_options.hpp>
#include "RunManager.h"
#include <iostream>
#include "HistWriter.h"

namespace po = boost::program_options;

int main(int argc, char** argv) {

    po::options_description desc ("Allowed options");
    desc.add_options ()
    ("help,h", "print usage message")
    ("simu,s", "execute simulation algorithm")
    ("anal,a", "execute analytical algorithm")
    ("file,f", po::value<std::string>(), "set configuration yaml file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if(vm.count("help")){
        std::cout << desc << std::endl;
        return 1;
    }

    auto runs = RunManager::GetInstance();

    if(vm.count("simu"))
        runs->SetSimu(true);
    else 
        runs->SetSimu(false);

    if(vm.count("anal"))
        runs->SetAnal(true);
    else
        runs->SetAnal(false);

    if(vm.count("file")){
        std::cout << "yaml file is" << vm["file"].as<std::string>() << std::endl;
    }

    runs->Load_Yaml("../test.yaml");
    runs->Parse_Yaml();

    runs->Start();

    
    auto hist = HistWriter{};
    hist.Write();
    hist.ToCsv();



    return EXIT_SUCCESS;
}
