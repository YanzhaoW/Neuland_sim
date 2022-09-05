#include "RunManager.h"

#include "SimNodes.h"
#include "Simulator.h"
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

RunManager* RunManager::runs = nullptr;
const std::string RunManager::NeulandStr = "neuland";
int RunConfig::ePrecision = 9;

void RunConfig::GenerateFileNames()
{
    auto out = std::ostringstream{};
    out.precision(ePrecision);

    out << "E_" << energy << "_Mul_" << particleMulti << "_Id_" << particleId << ".root";

    simFile = std::string{ "sim_" } + out.str();
    parFile = std::string{ "par_" } + out.str();
}

RunManager::RunManager(const std::string filename)
{
    if (!filename.empty())
        Load_Yaml(filename);
    // Implement_fun(NeulandStr, &RunManager::NeulandParse);
    Add_Node(new SimuNode());
    Add_Node(new NeulandNode());
}

RunManager::~RunManager() {}

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

void RunManager::Init()
{
    auto simuNode = dynamic_cast<SimuNode*>(fNodes.at("simulation"));

    auto energies = simuNode->GetEnergy();
    auto particle_ids = simuNode->GetParticleID();
    auto particle_multi = simuNode->GetParticleMulti();
    auto eventNum = simuNode->GetEventNum();
    auto eventPrint = simuNode->GetEventPrint();

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

void RunManager::Process(const RunConfig& rconfig)
{
    auto simRun = Simulator{ rconfig };
    // simRun.Print();
    simRun.Start();
    simRun.Run();
}
void RunManager::CreateProcess(const RunConfig& rconfig)
{
    Process(rconfig);
    this->~RunManager();
    exit(0);
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
