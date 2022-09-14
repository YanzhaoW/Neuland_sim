#ifndef ANALYSER_H
#define ANALYSER_H

#include "RunManager.h"

#include "TString.h"

class FairRunAna;
class TStopwatch;

class Analyser
{
  public:
    Analyser(const RunConfig&);
    ~Analyser();
    int Start();
    void Run();

  private:
    void SetPars();
    void AddTasks();

    TStopwatch* timer;

    const RunConfig& fRunConfiguration;
    FairRunAna* run;
    std::string workDir = "";
    std::string anlysFile = "";
    std::string simuFile = "";
    std::string paraFile = "";
};

#endif
