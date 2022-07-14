#include "NeulandCalTesting.h"
#include <iostream>
#include "R3BEventHeader.h"
#include <algorithm>
#include <vector>
#include "TObject.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "R3BNeulandHit.h"

NeulandCalTesting::NeulandCalTesting(const char* name, const Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalData("NeulandCalData")
    , fHitData("NeulandHits")
{}

InitStatus NeulandCalTesting::Init(){
    ioman = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)ioman->GetObject("EventHeader.");
    if (fEventHeader == nullptr)
    {
        throw std::runtime_error("R3BNeulandCal2Hit: No R3BEventHeader");
    }

    fCalData.Init();
    fHitData.Init();

    fNhits = new TH1I("fNhits", "Number of PMT signals for bars", 10, 0.5, 10.5);
    fSignalTimes = new TH1D("fSignalTimes", "arriving time of PMT signals", 500, 0.0, 25000.0);
    fSignalTimesTrig = new TH1D("fSignalTimes", "arriving time of PMT signals", 500, 0.0, 25000.0);
    fHitTimes = new TH1D("fHitTimes", "arriving time of hits", 5000, -100, 500);
    fXYplanes = new TH2D("fXYplanes", "hits position xy", 300, -150, 150, 300, -150, 150);


    return kSUCCESS;
}


InitStatus NeulandCalTesting::ReInit(){
    return kSUCCESS;
}



void NeulandCalTesting::SetParContainers(){
}


void NeulandCalTesting::Exec(Option_t*){
    fEventNum++;

    const auto start = fEventHeader->GetTStart();
    const bool beam = !std::isnan(start);

    auto hitData = fHitData.Retrieve();
    auto calData = fCalData.Retrieve();
    int size = calData.size();
    fMaxHitNum = std::max(fMaxHitNum, size);

    // if (size == 352){
    if (size >= 2){
        // std::cout << "analyzing event " << fEventNum << std::endl;
        
        std::vector<Int_t> BarIDs;
        for (auto calDataPtrPtr = calData.begin(); calDataPtrPtr != calData.end(); calDataPtrPtr++){

            const auto barID = (*calDataPtrPtr)->GetBarId();
            BarIDs.push_back(barID);
            // std::cout << "barID: " << barID << std::endl;



            // time recording:
            fSignalTimes->Fill((*calDataPtrPtr)->GetTime());
        }
        GetDuplicates(BarIDs);

        // std::cout << "Event number :\t"<<  fEventNum << std::endl;
        // std::cout << "===========================" << std::endl;


    }
    for (auto hitPtrPtr = hitData.begin(); hitPtrPtr != hitData.end(); hitPtrPtr++){
        auto hitTime =(*hitPtrPtr)->GetT();
        // std::cout << hitTime << "\t bool: "  << std::isnan(hitTime)<< "\t barID: \t" <<(*hitPtrPtr)->GetPaddle() << std::endl;
        if(!std::isnan(hitTime)){
            Double_t tofc;
            tofc = (*hitPtrPtr)->GetT() - ((*hitPtrPtr)->GetPosition().Mag() - fDistanceToTarget) / clight;
            fHitTimes->Fill(tofc);


            Double_t randx = (std::rand() / (float)RAND_MAX);
            fXYplanes->Fill((*hitPtrPtr)->GetPosition().X() +  5. * (randx - 0.5),
                                      (*hitPtrPtr)->GetPosition().Y() +  5. * (randx - 0.5));
        }
    }
    // std::cout <<   calData.size() << std::endl;

}


void NeulandCalTesting::FinishEvent(){
    // std::cout << "Event number :\t"<<  fEventNum << std::endl;
    // std::cout << "--------------------" << std::endl;
    if (fEventNum % 10000 == 0)
        std::cout << "\rR3BNeulandCal2Hit " << fEventNum << " Events converted." << std::flush;
    
}

void NeulandCalTesting::GetDuplicates(std::vector<Int_t> &vec){
    std::sort(vec.begin(), vec.end());
    short int count = 0;
    Int_t buffer = vec[0];
    for(const auto it : vec){
        if(buffer == it){
            // std::cout << it << std::endl;
            count++;
            // std::cout << "count: " << count << std::endl;
        }else{
            fNhits->Fill(count);
            // if(count != 2)
            //     std::cout << ">>>>>>>>>>>case occurs at bar " << buffer << " with " << count << " signals !" << std::endl;
            count = 1;
            buffer = it;
            // std::cout << "-------------" << std::endl;
            // std::cout << it << std::endl;
        }


    }

}

void NeulandCalTesting::FinishTask(){
    fNhits->Write();
    fSignalTimes->Write();
    fSignalTimesTrig->Write();
    fHitTimes->Write();
    fXYplanes->Write();

    std::cout << "Task finished!" << std::endl;
    std::cout << "max number of hits:" << fMaxHitNum << std::endl;
}
