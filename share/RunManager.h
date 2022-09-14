#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include "yaml-cpp/yaml.h"
#include <map>
#include <string>
#include <vector>

class YamlNodes;
class Simulator;
class TDatime;
class RunManager;

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
    std::string anlysFile;
    std::string workDir;
    std::string outDir;

    static int ePrecision;
    RunConfig() = default;
    RunConfig(float E, int id, short Multi, int eNum, int ePri);
    void GenerateFileNames();
    void SetWorkDir(std::string dir) { workDir = dir; }
    void SetOutDir(std::string dir) { outDir = dir; }
  private:
    RunManager* fRunManager;

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

    void Load_Yaml(const std::string filename);
    void Parse_Yaml();
    void Start();
    void Print() const;
    void SetSimu(bool option) {is_simu = option;}
    void SetAnal(bool option) {is_anal = option;}
    std::string GetDate();

    // geters:
    const std::map<std::string, YamlNodes*>& GetNodes() { return fNodes; }
    const std::vector<int>& GetPIDs() const { return fChildPID; }
    const int GetSidePIDs() const { return sideChildPID; }
    const std::string GetFileDir() const {return fileDir;}
    const std::vector<RunConfig>* GetConfigs() {return fRunConfigs.get();} // not ownership transfering

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
    void SimuProcess(const RunConfig&);
    void AnalProcess(const RunConfig&);
    void CreateProcess(const RunConfig&);
    typedef void (RunManager::*processFun)(const RunConfig&);
    void CreateSideProcess(processFun, const RunConfig&);
    static void handle_sigint(int sig);
    static void handle_side_sigint(int sig);
    std::string GetLastDate();
    void Mkdir(std::string);

    node root_node;
    TDatime* fTime = nullptr;;
    std::unique_ptr<Simulator> fSimulator;
    std::unique_ptr<std::vector<RunConfig>> fRunConfigs;
    std::map<std::string, YamlNodes*> fNodes;
    std::vector<int> fChildPID;
    int sideChildPID = 0;
    std::string fYaml = "";
    std::string fDataOut = "";
    std::string fileDir;
    bool is_simu = true;
    bool is_anal = true;
    int fNumOfRuns = 0;
};

#endif
