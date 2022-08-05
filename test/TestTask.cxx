#include "TestTask.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "R3BNeulandHit.h"
#include "TApplication.h"
#include "FairLogger.h"
#include "TH2D.h"

TestTask::TestTask():fHitData("NeulandHits"){
}

TestTask::~TestTask(){
    delete hTOFc;
}

InitStatus TestTask::Init(){

    fHitData.Init();
    auto canvas = new TCanvas("canvas", "canvas for plots");
    hTOFc = new TH1D("hTOFc", "hTOFc", 8000, -6000, 6000);
    hTOFc->Draw();
    Register(canvas);

    auto canvasPlaneXY = new TCanvas("NeulandPlaneXY", "NeulandPlaneXY", 10, 10, 850, 850);
    ahXYperPlane = new TH2D("hHitXYPlane",
            "Hit XY Plane",
            300,
            -150,
            150,
            300,
            -150,
            150);
    ahXYperPlane->Draw("COLZ");
    Register(canvasPlaneXY);


    return kSUCCESS;
}


void TestTask::Exec(Option_t*){
    const auto hits = fHitData.Retrieve();

    for(const auto hit : hits){
        double randx = (std::rand() / (float)RAND_MAX);
        const Double_t tcorr = hit->GetT() - (hit->GetPosition().Mag() - fDistanceToTarget) / clight;
        hTOFc->Fill(tcorr);
        ahXYperPlane -> Fill (hit->GetPosition().X() +  5. * (randx - 0.5), hit->GetPosition().Y() + 5. * (randx - 0.5));
    }
}


void TestTask::FinishTask() {

}
void TestTask::FinishEvent(){
    Plot(100000);
}
