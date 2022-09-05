#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <cstdlib>
#include <iostream>
#include <string>

#include "FairLogger.h"
#include "FairParRootFileIo.h"
#include "FairRootFileSink.h"
#include "FairParSet.h"
#include "G4RunManager.hh"
#include "R3BCave.h"
#include "R3BNeuland.h"
#include "TG4EventAction.h"
#include "TG4RunManager.h"
#include "TString.h"
#include "FairFieldFactory.h"
#include "R3BFieldPar.h"
// #include "R3BNeulandField.h"
#include "RunManager.h"

class TStopwatch;
class FairRunSim;
class FairBoxGenerator;
class FairPrimaryGenerator;
class RunManger;

class Simulator{


   public:
    Simulator(const RunConfig&);
    ~Simulator();
    void timer_start();
    void timer_stop();
    // void create_field() {
    //     run->SetField(magField);
    // }
    void SetPar();
    void particle_gen();
    void geom_setup();
    void init();
    void par_output();
    void Run(); 
    void Start();
    void Print(){
        // std::cout << "inside: " << fRunConfiguration.parFile << std::endl;
    }

    void SetFolder(const char* name){ folderName = std::string{name};}
   private:
    TStopwatch* timer;
    FairRunSim* fRun;
    FairBoxGenerator* boxGen;
    FairPrimaryGenerator* primGen;

    R3BCave* cave = nullptr;
    R3BNeuland* neuland = nullptr;
    FairParRootFileIo* parFileIO{new FairParRootFileIo(true)};
    // R3BNeulandField* magField = new R3BNeulandField();
    RunManager* fRunMan;

    const TString workDirectory = "/u/yanwang/software/R3BRoot";
    TString simufile = "test.simu.root";
    TString parafile = "test.para.root";
    Int_t eventNum = 200;
    Int_t printEventNum = 10;
    const RunConfig& fRunConfiguration;
    std::string folderName = "./out/";
};
#endif
