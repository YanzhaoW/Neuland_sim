#include <fstream>
#include <iostream>

#include "DigitizingTacQuila.h"
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

// int main(int argc, char *argv[]){
//     TStopwatch timer;
//     timer.Start();

//     const TString workDirectory = "/u/yanwang/software/src/R3BRoot";
//     gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
//     gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
//     FairLogger::GetLogger()->SetLogScreenLevel("INFO");
//     auto run = new FairRunAna();
//     run->SetSource(new FairFileSource("test.simu.root"));
//     run->SetSink(new FairRootFileSink("test.digi.root"));

//     auto io = new FairParRootFileIo();
//     io->open("test.para.root");
//     auto runtimeDb = run->GetRuntimeDb();
//     runtimeDb->setFirstInput(io);

//     run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTamex()));
//     // run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTacQuila()));
//     run->AddTask(new R3BNeulandHitMon());
//     // run->AddTask(new NeulandCalTesting());
//     // run.AddTask(new R3BNeulandHitProto(argv[1]));
//     run->Init();

//     run->Run(0,0);

//     // include this otherwie it will get errors
//     auto sink = run->GetSink();
//     sink->Close();

//     std::cout << "*************" << std::endl;
//     timer.Stop();
//     std::cout << "Macro finished successfully." << std::endl;
//     std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
//     // LOG(DEBUG)<< "----------------exiting------------" ;

//     // Don't clean the run object. otherwise it will cause errors.
//     delete run;

//     return 0;

// }

int main(int argc, char* argv[])
{
    Int_t NMAX = 0;
    TStopwatch timer;
    timer.Start();

    const TString workDirectory = "/u/yanwang/software/src/R3BRoot";
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    auto run = new FairRunAna();
    run->SetSource(new FairFileSource("output.root"));
    run->SetSink(new FairRootFileSink("test.cal.root"));

    auto rtdb = run->GetRuntimeDb();

    auto parList = new TList();
    parList->Add(new TObjString("../parameters/loscalpar_v1.root"));
    parList->Add(new TObjString("../parameters/params_tcal_180522xx.root"));
    parList->Add(new TObjString("../parameters/params_sync_s522_0999_310522x.root"));

    auto parIO = new FairParRootFileIo(false);

    parIO->open(parList);
    rtdb->setFirstInput(parIO);

    rtdb->addRun(999);
    rtdb->getContainer("LosTCalPar");
    rtdb->setInputVersion(999, (char*)"LosTCalPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(999, (char*)"LandTCalPar", 1, 1);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(999, (char*)"NeulandHitPar", 1, 1);

    // run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTamex()));
    // auto onlinespectra = new R3BNeulandOnlineSpectra();
    // onlinespectra->SetDistanceToTarget(1520);
    // run->AddTask(onlinespectra);
    run->AddTask(new NeulandCalTesting());
    // run.AddTask(new R3BNeulandHitProto(argv[1]));
    // run->AddTask(new TestTask());
    run->AddTask(new EventProgress(NMAX));
    run->Init();

    run->Run(0, NMAX);
    std::cout << "*************" << std::endl;
    timer.Stop();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    // LOG(DEBUG)<< "----------------exiting------------" ;

    // include this otherwie it will get errors
    auto sink = run->GetSink();
    sink->Close();
    // Don't clean the run object. otherwise it will cause errors.
    // run->Delete();

    return 0;
}

// class mytask : public FairTask{
//     public:
//         mytask(TString input = "NeulandHits", TString output = "mytask", const Option_t* option = ""):fHits{input},
//         fOut{output}{} ~ mytask() override {} InitStatus Init() override {
//             fout.open("position.csv", std::ios::trunc);
//             fout << "Event,X,Y,Z,Energy" << std::endl;
//             fHits.Init();
//             return kSUCCESS;
//         }
//         void Exec(Option_t* ) override {
//             const auto hits = fHits.Retrieve();
//             std::cout << ">>>>>>>>>>Event:\t" << nEvent<< std::endl;
//             for(const auto& hit:hits){
//                 // std::cout << "hit position:\t" << hit->GetPosition().X() << "\t" <<hit->GetPosition().Y()<< "\t"
//                 <<hit->GetPosition().Z() - 1650 << std::endl; fout << nEvent <<"," << hit->GetPosition().X()<<"," <<
//                 hit->GetPosition().Y()<<"," << hit->GetPosition().Z() <<"," << hit->GetE() << std::endl;
//             }
//             std::cout << ">>>>>>>>>>" << std::endl;
//         }
//         void Finish() override {
//             fout.close();
//         }
//         void FinishEvent() override {
//             nEvent++;
//         }
//     private:
//         TCAInputConnector<R3BNeulandHit> fHits;
//         TString fOut;
//         int nEvent = 0;
//         std::ofstream fout;
// };

