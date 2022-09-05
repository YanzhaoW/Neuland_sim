#include "SimNodes.h"
#include <iostream>
#include <math.h>

YamlNodes::YamlNodes(const std::string name)
    : fName(name)
{
}

SimuNode::SimuNode()
    : YamlNodes("simulation")
{
}

void SimuNode::Parse(const node& root_node)
{
    node simu = root_node[fName];

    if (!simu.IsMap())
        return;

    int n = 0;

    if (simu["energy"].IsMap())
    {
        float Emin, Emax, Esep = 0.0;
        Emin = simu["energy"]["min"].as<float>();
        Emax = simu["energy"]["max"].as<float>();
        Esep = simu["energy"]["sep"].as<float>();
        n = floor((Emax - Emin) / Esep);
        energy = std::vector<float>(n+1, 0);
        for (int i = 0; i < n+1; i++)
            energy[i] = Emin + i * Esep;
    }

    // parsing particle parameters:
    if (simu["particle"].IsMap())
    {
        if (simu["particle"]["ID"].IsSequence())
        {
            n = simu["particle"]["ID"].size();
            ParticleID = std::vector<int>(n, 0);
            for (int i = 0; i < n; i++)
                ParticleID[i] = simu["particle"]["ID"][i].as<int>();
        }

        if (simu["particle"]["multi"].IsSequence())
        {
            n = simu["particle"]["multi"].size();
            ParticleMulti = std::vector<short>(n, 0);
            for (int i = 0; i < n; i++)
                ParticleMulti[i] = simu["particle"]["multi"][i].as<int>();
        }
    }

    // other infromation:
    if (simu["fileIO"].IsMap())
    {
        if (simu["fileIO"]["sim"].IsScalar())
            simFile = simu["fileIO"]["sim"].as<std::string>();

        if (simu["fileIO"]["par"].IsScalar())
            parFile = simu["fileIO"]["par"].as<std::string>();
    }

    if (simu["event"].IsMap())
    {
        if (simu["event"]["number"].IsScalar())
            eventNum = simu["event"]["number"].as<int>();

        if (simu["event"]["print"].IsScalar())
            eventPrint = simu["event"]["print"].as<int>();
    }
}

NeulandNode::NeulandNode()
    : YamlNodes("neuland")
{
}

void NeulandNode::Parse(const node& root_node)
{
    node neuland = root_node[DetStr][fName];
    if (!neuland.IsMap())
        return;
    int n = 0;

    if (neuland["active"].IsScalar())
        fActive = neuland["active"].as<bool>();

    if (neuland["dp"].IsScalar())
        fDp = neuland["dp"].as<short>();

    if (neuland["position"].IsSequence())
    {
        n = neuland["position"].size();
        fPos = std::vector<double>(n, 0);
        for (int i = 0; i < n; i++)
            fPos[i] = neuland["position"][i].as<double>();
    }
}

