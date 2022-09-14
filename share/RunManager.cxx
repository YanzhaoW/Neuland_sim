#include "RunManager.h"

#include "SimNodes.h"
#include "Simulator.h"
#include "Analyser.h"
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include "TDatime.h"
#include <boost/filesystem.hpp>

RunManager* RunManager::runs = nullptr;
const std::string RunManager::NeulandStr = "neuland";
int RunConfig::ePrecision = 9;

RunConfig::RunConfig(float E, int id, short Multi, int eNum, int ePri)
    : energy(E)
    , particleId(id)
    , particleMulti(Multi)
    , eventNum(eNum)
    , eventPrint(ePri)
    , fRunManager(RunManager::GetInstance())
{
}
void RunConfig::GenerateFileNames()
{
    auto out = std::ostringstream{};
    out.precision(ePrecision);

    out << "E_" << energy << "_Mul_" << particleMulti << "_Id_" << particleId << ".root";

    auto fileDir = fRunManager->GetFileDir();

    simFile = fileDir + std::string{ "sim_" } + out.str();
    std::cout << "Simulation file: " << simFile << std::endl;

    parFile = fileDir + std::string{ "par_" } + out.str();
    std::cout << "parameter file: " << parFile << std::endl;

    anlysFile = fileDir + std::string{ "anlys_" } + out.str();
    std::cout << "analytical file: " << anlysFile << std::endl;
}


RunManager::RunManager(const std::string filename)
{
    if (!filename.empty())
        Load_Yaml(filename);
    // Implement_fun(NeulandStr, &RunManager::NeulandParse);
    fTime = new TDatime{};
    Add_Node(new SimuNode());
    Add_Node(new NeulandNode());
}

RunManager::~RunManager() {
    delete fTime;
}

void RunManager::Load_Yaml(const std::string filename) { root_node = YAML::LoadFile(filename); }

void RunManager::Parse_Yaml()
{
    for (auto it = fNodes.begin(); it != fNodes.end(); it++)
    {
        it->second->Parse(root_node);
    }
}

void RunManager::Print() const
{
    std::cout << "-- information of run configs: " << std::endl;
    std::cout << "Number of runs: " << fNumOfRuns << std::endl << std::endl;
    std::cout << "energy\tID\tMultiplicity\tEventNum\tEventPrint" << std::endl;

    for (auto const it : *fRunConfigs)
    {
        std::cout << it.energy << "\t" << it.particleId << "\t" << it.particleMulti << "\t\t" << it.eventNum << "\t\t"
                  << it.eventPrint << std::endl;
    }
}

void RunManager::Add_Node(YamlNodes* n) { fNodes[n->GetName()] = n; }

void RunManager::Mkdir(std::string fD){
    auto dir = boost::filesystem::path(fD.c_str());
    if(boost::filesystem::create_directories(dir))
    {
        std::cout << "Directory " << fD << " is created. " << std::endl;
    }
    else{
        std::cout <<  "Directory " << fD << " already existed. " << std::endl;
    }

}

void RunManager::Init()
{
    auto simuNode = dynamic_cast<SimuNode*>(fNodes.at("simulation"));

    auto energies = simuNode->GetEnergy();
    auto particle_ids = simuNode->GetParticleID();
    auto particle_multi = simuNode->GetParticleMulti();
    auto eventNum = simuNode->GetEventNum();
    auto eventPrint = simuNode->GetEventPrint();
    auto workDir = simuNode->GetWorkDir();
    fDataOut = simuNode->GetOutDir();

    fileDir = fDataOut + RunManager::GetInstance()->GetDate() + std::string{"/"};

    Mkdir(fileDir);

    fNumOfRuns = energies.size() * particle_ids.size() * particle_multi.size();

    fRunConfigs = std::unique_ptr<std::vector<RunConfig>>(new std::vector<RunConfig>(fNumOfRuns));
    auto it_config = fRunConfigs->begin();

    for (auto const it_energy : energies)
    {
        for (auto const it_id : particle_ids)
        {
            for (auto const it_multi : particle_multi)
            {
                *it_config = RunConfig{ it_energy, it_id, it_multi, eventNum, eventPrint };
                it_config->SetWorkDir(workDir);
                it_config->SetOutDir(fDataOut);
                it_config->GenerateFileNames();
                it_config++;
            }
        }
    }

    fChildPID = std::vector<int>(fNumOfRuns);
}

void RunManager::SpawnChildren()
{
    for (auto i = 0; i < fNumOfRuns; i++)
    {
        fChildPID[i] = SpawnChild((*fRunConfigs)[i]);
    }
}

int RunManager::SpawnChild(const RunConfig& rc)
{
    int id = fork();
    if (id < 0)
    {
        return -1;
    }
    else if (id > 0)
    {
        return id;
    }
    else
    {
        CreateProcess(rc);
        std::cerr << "Error occurs on child process" << getpid() << std::endl;
        return -2;
    }
}

void RunManager::SimuProcess(const RunConfig& rconfig)
{
    auto simRun = Simulator{ rconfig };
    // simRun.Print();
    simRun.Start();
    simRun.Run();
}

void RunManager::AnalProcess(const RunConfig& rconfig)
{
    auto anaRun = Analyser{ rconfig };
    // simRun.Print();
    auto res = anaRun.Start();
    if(res == 0)
        anaRun.Run();
}

void RunManager::CreateProcess(const RunConfig& rconfig)
{
    if (is_simu)
        CreateSideProcess(&RunManager::SimuProcess, rconfig);
    if (is_anal)
        CreateSideProcess(&RunManager::AnalProcess, rconfig);
    this->~RunManager();
    exit(0);
}


void RunManager::CreateSideProcess(processFun f, const RunConfig& rconfig){
    int id = fork();

    if(id == 0)
    {
        std::cout << "creating side process" << std::endl;
        (this->*f)(rconfig);
        this->~RunManager();
        exit(0);
    }
    else if (id > 0){
        sideChildPID = id;
    }
    else {
        std::cerr << "side process failed to create!" << std::endl;
    }
    struct sigaction sa;
    sa.sa_handler = &handle_side_sigint;
    sigaction(SIGINT, &sa, NULL);
    std::cout << "child process waiting.............." << std::endl;
    wait(NULL);
}

void RunManager::handle_side_sigint(int sig)
{
    auto pid = GetInstance()->GetSidePIDs();
    if (pid > 0)
    {
        kill(pid, SIGINT);
        std::cout << "side process " << pid << " is killed." << std::endl;
    }
    GetInstance()->~RunManager();
    exit(1);
}


void RunManager::handle_sigint(int sig)
{
    for (auto const it : GetInstance()->GetPIDs())
    {
        if (it > 0)
        {
            std::cout << "process " << it << " is killed." << std::endl;
            kill(it, SIGINT);
        }
    }
}

void RunManager::Start()
{
    Init();
    SpawnChildren();

    struct sigaction sa;
    sa.sa_handler = &handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    int wstatus;
    int child_id;
    while ((child_id = wait(&wstatus)) > 0)
    {
        if (WIFEXITED(wstatus))
        {
            if (WEXITSTATUS(wstatus) == 0)
            {
                std::cout << "child process on ID " << child_id << " has been executed successfully." << std::endl;
            }
            else
            {
                std::cout << "child process on ID " << child_id << " has been failed." << std::endl;
            }
        }
    }
}

std::string RunManager::GetDate()
{
    if (is_simu){
        auto out = std::ostringstream{};
        out << fTime->GetMonth() << "-" << fTime->GetDay() << "-" << fTime->GetYear() << "-" << fTime->GetHour()<< "h-"  << fTime->GetMinute() << "m-"  << fTime->GetSecond() << "s";

        return out.str();
    }
    else{
        return GetLastDate();
    }
}

std::string RunManager::GetLastDate(){
    namespace fs = boost::filesystem;
    auto dirs = std::vector<fs::path>{};
    std::copy(fs::directory_iterator(fs::path(fDataOut.c_str())), fs::directory_iterator(), std::back_inserter(dirs));
    return dirs.back().filename().string();

}



