#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include "yaml-cpp/yaml.h"
#include <map>
#include <string>
#include <vector>

class YamlNodes;
class Simulator;

class RunConfig
{
  public:
    float energy = 0.0f;
    int particleId = 0;
    short particleMulti = 0;
    int eventNum = 0;
    int eventPrint = 0;
    std::string simFile;
    std::string parFile;

    static int ePrecision;
    RunConfig() = default;
    RunConfig(float E, int id, short Multi, int eNum, int ePri)
        : energy(E)
        , particleId(id)
        , particleMulti(Multi)
        , eventNum(eNum)
        , eventPrint(ePri)
    {
    }
    void GenerateFileNames();
};
class RunManager
{
  public:
    typedef YAML::Node node;

  private:
    explicit RunManager(const std::string = "");
    static RunManager* runs;

  public:
    ~RunManager();
    RunManager(const RunManager& runs) = delete;
    void operator=(const RunManager&) = delete;
    RunManager(const RunManager&& runs) = delete;
    void operator=(const RunManager&&) = delete;

    void Add_Node(YamlNodes*);
    const std::map<std::string, YamlNodes*>& GetNodes() { return fNodes; }

    void Load_Yaml(const std::string filename);
    void Parse_Yaml();
    void Start();
    void Print() const;

    const std::vector<int>& GetPIDs() { return fChildPID; }

    const static std::string NeulandStr;
    static RunManager* GetInstance(const std::string str = "")
    {
        if (runs == nullptr)
        {
            runs = new RunManager(str);
        }
        return runs;
    }

  private:
    void Parse_Yaml(std::string);
    void Init();
    int SpawnChild(const RunConfig&);
    void SpawnChildren();
    void Process(const RunConfig&);
    void CreateProcess(const RunConfig&);
    static void handle_sigint(int sig);

    node root_node;
    std::unique_ptr<Simulator> fSimulator;

    std::unique_ptr<std::vector<RunConfig>> fRunConfigs;
    std::map<std::string, YamlNodes*> fNodes;
    std::vector<int> fChildPID;
    std::string fYaml = "";
    int fNumOfRuns = 0;
};

#endif
