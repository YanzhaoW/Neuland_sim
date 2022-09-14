#include "Simulator.h"
#include "TSystem.h"
#include "FairRunSim.h"
#include "FairBoxGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TStopwatch.h"
#include "SimNodes.h"

Simulator::Simulator(const RunConfig& runconfig): 
    timer{new TStopwatch},
    fRun{new FairRunSim},
    boxGen{new FairBoxGenerator},
    primGen{new FairPrimaryGenerator},
    fRunConfiguration{runconfig} 
{
    fRunMan = RunManager::GetInstance();
    // FairLogger::GetLogger()->SetLogVerbosityLevel("verylow");
}

Simulator::~Simulator() {
    auto sink = fRun->GetSink();
    sink->Close();
    timer_stop();
    delete timer;
    delete fRun;
    LOG(INFO) << "simulator has been deconstructed succesfully.";
}

void Simulator::timer_start() { timer->Start(); }

void Simulator::timer_stop() {
    timer->Stop();
    std::cout << "CPU time: " << timer->CpuTime() << std::endl;
}

void Simulator::SetPar(){

    workDir = fRunConfiguration.workDir;
    simufile = fRunConfiguration.simFile;
    eventNum = fRunConfiguration.eventNum;
    printEventNum = fRunConfiguration.eventPrint;
    parafile = fRunConfiguration.parFile;

    // FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    FairLogger::GetLogger()->SetLogScreenLevel("WARN");
    // FairLogger::GetLogger()->SetLogToScreen(false);
    gSystem->Setenv("GEOMPATH", (workDir + std::string{ "geometry" }).c_str());
    gSystem->Setenv("CONFIG_DIR", (workDir + std::string{ "gconfig" }).c_str());

    fRun->SetName("TGeant4");
    fRun->SetStoreTraj(false);
    fRun->SetMaterials("media_r3b.geo");
    fRun->SetSink(new FairRootFileSink(static_cast<TString>(simufile)));
}


void Simulator::particle_gen() {
    boxGen->SetPDGType(fRunConfiguration.particleId);
    boxGen->SetMultiplicity(fRunConfiguration.particleMulti);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);

    auto energy = fRunConfiguration.energy / 1000.0;
    boxGen->SetEkinRange(energy, energy + 0.0);
    primGen->AddGenerator(boxGen);
    fRun->SetGenerator(primGen);
}

void Simulator::geom_setup() {
    // Geometry: Cave

    cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    fRun->AddModule(cave);
    // Geometry: Neuland
    auto neulandNode = dynamic_cast<NeulandNode*>((fRunMan->GetNodes()).at(RunManager::NeulandStr));
    auto vec_pos = neulandNode->GetPosition();
    if (vec_pos.size() < 3)
        LOG(ERROR) << "position of Neuland is not set correctly";
    else{
        neuland = new R3BNeuland(neulandNode->GetDp(), {vec_pos[0], vec_pos[1], vec_pos[2]});
        fRun->AddModule(neuland);
    }

}

void Simulator::init() {
    fRun->Init();
    auto grun = G4RunManager::GetRunManager();
    grun->SetPrintProgress(printEventNum);
    auto event = dynamic_cast<TG4EventAction*>(
            const_cast<G4UserEventAction*>(grun->GetUserEventAction()));
    event->VerboseLevel(0);
}

void Simulator::par_output() {
    parFileIO->open(static_cast<TString>(parafile));
    auto rtdb = fRun->GetRuntimeDb();
    // auto fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    // fieldPar->SetParameters(magField);
    // fieldPar->setChanged();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();
}

void Simulator::Run() { fRun->Run(eventNum); }

void Simulator::Start(){
    timer_start();
    SetPar();
    particle_gen();
    geom_setup();
    // create_field();
    init();
    par_output();
}
