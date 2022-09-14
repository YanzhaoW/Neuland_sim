#ifndef SIMNODES_H
#define SIMNODES_H

#include "yaml-cpp/yaml.h"
#include <string>
#include <vector>
#include <memory>

class YamlNodes
{
  protected:
    typedef YAML::Node node;
    std::string fName;
    std::string DetStr = "detector";

  public:
    YamlNodes(const std::string name);
    virtual void Parse(const node& n) = 0;
    std::string GetName() const { return fName; }
};

class SimuNode : public YamlNodes
{
  public:
    SimuNode();
    void Parse(const node& n) override;
    const std::vector<float>& GetEnergy() const { return energy; }
    const std::vector<int>& GetParticleID() const { return ParticleID; }
    const std::vector<short>& GetParticleMulti() const { return ParticleMulti; }
    int GetEventNum() const { return eventNum; }
    int GetEventPrint() const { return eventPrint; }
    std::string GetWorkDir() const { return workDir; }
    std::string GetOutDir() const { return outDir; }

  private:
    std::vector<float> energy;
    std::vector<int> ParticleID;
    std::vector<short> ParticleMulti;
    std::string workDir;
    std::string outDir;
    int eventNum = 0;
    int eventPrint = 0;
};

class NeulandNode : public YamlNodes
{
  public:
    NeulandNode();
    void Parse(const node& n) override;
    bool Is_active() const { return fActive; }
    short GetDp() const { return fDp; }
    std::vector<double> GetPosition() const { return fPos; }

  private:
    bool fActive = false;
    short fDp = 0;
    std::vector<double> fPos;
};

#endif
