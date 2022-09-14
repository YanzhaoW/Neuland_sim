#include "Analyser.h"

#include "DigitizingTamex.h"
#include "R3BDetector.h"
#include "R3BNeuland.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandHitMon.h"
#include "R3BNeulandPoint.h"

#include "FairEventManager.h"
#include "FairFileSource.h"
#include "FairLogger.h"
#include "FairParRootFileIo.h"
#include "FairRootFileSink.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"

#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"
// #include "R3BNeulandHitProto.h"
#include "EventProgress.h"
#include "NeulandCalTesting.h"
#include "R3BNeulandOnlineSpectra.h"
#include "TestTask.h"

Analyser::Analyser(const RunConfig& rconfig):
    fRunConfiguration(rconfig),
    run{new FairRunAna{}}
{
}

Analyser::~Analyser()
{
    auto sink = run->GetSink();
    sink->Close();
    std::cout << "*************" << std::endl;
    timer->Stop();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer->RealTime() << "s, CPU time: " << timer->CpuTime() << "s" << std::endl;
    delete timer;
}
void Analyser::SetPars()
{
    workDir = fRunConfiguration.workDir;
    anlysFile = fRunConfiguration.anlysFile;
    simuFile = fRunConfiguration.simFile;
    paraFile = fRunConfiguration.parFile;

    gSystem->Setenv("GEOMPATH", (workDir + std::string{ "geometry" }).c_str());
    gSystem->Setenv("CONFIG_DIR", (workDir + std::string{ "gconfig" }).c_str());
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
}

void Analyser::AddTasks()
{
    run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTamex()));
    // run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTacQuila()));
    run->AddTask(new R3BNeulandHitMon());
    // run->AddTask(new NeulandCalTesting());
    // run.AddTask(new R3BNeulandHitProto(argv[1]));
}

int Analyser::Start()
{
    SetPars();
    timer = new TStopwatch{};
    timer->Start();


    if (gSystem->AccessPathName(simuFile.c_str()))
    {
        LOG(ERROR) << "simulation root file: " <<simuFile <<" doesn't exist! ";
        return 1;
    }

    run->SetSource(new FairFileSource(simuFile.c_str()));
    run->SetSink(new FairRootFileSink(anlysFile.c_str()));

    if (gSystem->AccessPathName(paraFile.c_str()))
    {
        LOG(ERROR) << "parameter root file doesn't exist! ";
        return 2;
    }

    auto io = new FairParRootFileIo();
    io->open(paraFile.c_str());
    auto runtimeDb = run->GetRuntimeDb();
    runtimeDb->setFirstInput(io);

    AddTasks();
    run->Init();

    return 0;
}

void Analyser::Run() { run->Run(0, 0); }
