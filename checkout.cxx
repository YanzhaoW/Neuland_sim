#include <iostream>
#include <fstream>

#include "R3BNeulandGeoPar.h"
#include "R3BNeulandPoint.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandDigitizer.h"
#include "R3BDetector.h"
#include "R3BNeuland.h"
#include "R3BNeulandHitMon.h"
#include "DigitizingTamex.h"

#include "FairRootManager.h"
#include "FairTask.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairFileSource.h"
#include "FairRootFileSink.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"
#include "FairEventManager.h"

#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"
#include "R3BNeulandHitProto.h"

int main(int argc, char *argv[]){
    TStopwatch timer;
    timer.Start();

    const TString workDirectory = "/u/yanwang/software/src/R3BRoot";
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    auto run = new FairRunAna();
    run->SetSource(new FairFileSource("test.simu.root"));
    run->SetSink(new FairRootFileSink("test.digi.root"));

    auto io = new FairParRootFileIo();
    io->open("test.para.root");           
    auto runtimeDb = run->GetRuntimeDb();
    runtimeDb->setFirstInput(io);

    run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTamex()));
    // run.AddTask(new R3BNeulandHitProto(argv[1]));
    run->Init();

    run->Run(0,0);
    std::cout << "*************" << std::endl;
    timer.Stop();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    // LOG(DEBUG)<< "----------------exiting------------" ;

    // Don't clean the run object. otherwise it will cause errors.
    // run->Delete();

    return 0;

}

// class mytask : public FairTask{
//     public:
//         mytask(TString input = "NeulandHits", TString output = "mytask", const Option_t* option = ""):fHits{input}, fOut{output}{}
//         ~ mytask() override {}
//         InitStatus Init() override {
//             fout.open("position.csv", std::ios::trunc);
//             fout << "Event,X,Y,Z,Energy" << std::endl;
//             fHits.Init();
//             return kSUCCESS;
//         }
//         void Exec(Option_t* ) override {
//             const auto hits = fHits.Retrieve();
//             std::cout << ">>>>>>>>>>Event:\t" << nEvent<< std::endl;
//             for(const auto& hit:hits){
//                 // std::cout << "hit position:\t" << hit->GetPosition().X() << "\t" <<hit->GetPosition().Y()<< "\t" <<hit->GetPosition().Z() - 1650 << std::endl;
//                 fout << nEvent <<"," << hit->GetPosition().X()<<"," << hit->GetPosition().Y()<<"," << hit->GetPosition().Z() <<"," << hit->GetE() << std::endl;
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

