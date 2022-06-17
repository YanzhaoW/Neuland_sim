#include <cstdlib>
#include <iostream>

#include "FairBoxGenerator.h"
#include "FairLogger.h"
#include "FairParRootFileIo.h"
#include "FairPrimaryGenerator.h"
#include "FairRootFileSink.h"
#include "FairRunSim.h"
#include "FairParSet.h"
#include "G4RunManager.hh"
#include "R3BCave.h"
#include "R3BNeuland.h"
#include "TG4EventAction.h"
#include "TG4RunManager.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"
#include "FairFieldFactory.h"
#include "R3BFieldPar.h"
#include "R3BNeulandField.h"

class Initilizer {
   private:
    TStopwatch* timer{new TStopwatch};
    FairRunSim* run{new FairRunSim};
    FairBoxGenerator* boxGen{new FairBoxGenerator{2112, 4}};
    FairPrimaryGenerator* primGen{new FairPrimaryGenerator};

    R3BCave* cave{nullptr};
    R3BNeuland* neuland{nullptr};
    FairParRootFileIo* parFileIO{new FairParRootFileIo(true)};
    R3BNeulandField* magField = new R3BNeulandField();

    const TString workDirectory = "/data/ywang/software/R3BRoot";
    const TString simufile = "test.simu.root";
    const TString parafile = "test.para.root";
    const Int_t eventNum = 100;
    const Int_t printEventNum = 100;

   public:
    Initilizer() {
        FairLogger::GetLogger()->SetLogVerbosityLevel("verylow");
        FairLogger::GetLogger()->SetLogScreenLevel("nolog");
        // FairLogger::GetLogger()->SetLogToScreen(false);
        gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
        gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
        gSystem->Setenv("G4FORCENUMBEROFTHREADS", "4");
    };
    void timer_start() { timer->Start(); }

    void timer_stop() {
        timer->Stop();
        std::cout << "CPU time: " << timer->CpuTime() << std::endl;
    }

    void create_field() {
        run->SetField(magField);
    }
    void run_config() {
        run->SetName("TGeant4");
        run->SetStoreTraj(false);
        run->SetMaterials("media_r3b.geo");
        run->SetSink(new FairRootFileSink(simufile));
    }

    void particle_gen() {
        boxGen->SetXYZ(0, 0, 0.);
        boxGen->SetThetaRange(0., 3.);
        boxGen->SetPhiRange(0., 360.);
        boxGen->SetEkinRange(0.6, 0.6);
        primGen->AddGenerator(boxGen);
        run->SetGenerator(primGen);
    }

    void geom_setup() {
        // Geometry: Cave

        cave = new R3BCave("CAVE");
        cave->SetGeometryFileName("r3b_cave.geo");
        run->AddModule(cave);
        // Geometry: Neuland
        neuland = new R3BNeuland(30, {0., 0., 1650.});
        run->AddModule(neuland);
    }

    void run_init() {
        run->Init();
        auto grun = G4RunManager::GetRunManager();
        grun->SetPrintProgress(printEventNum);
        auto event = dynamic_cast<TG4EventAction*>(
            const_cast<G4UserEventAction*>(grun->GetUserEventAction()));
        event->VerboseLevel(0);
    }

    void par_output() {
        parFileIO->open(parafile);
        auto rtdb = run->GetRuntimeDb();
        // auto fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
        // fieldPar->SetParameters(magField);
        // fieldPar->setChanged();
        rtdb->setOutput(parFileIO);
        rtdb->saveOutput();
    }
    void run_Run() { run->Run(eventNum); }

    ~Initilizer() {
        delete timer;
        delete boxGen;
        delete primGen;
        delete cave;
        delete neuland;
        delete parFileIO;
    }
};

int main() {
    Initilizer initer{};
    initer.timer_start();
    initer.run_config();
    initer.particle_gen();
    initer.geom_setup();
    initer.create_field();
    initer.run_init();
    initer.par_output();
    initer.run_Run();
    initer.timer_stop();
    return EXIT_SUCCESS;
}
