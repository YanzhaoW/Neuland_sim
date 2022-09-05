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
#include "TCanvas.h"
#include "TStyle.h"

NeulandCalTesting::NeulandCalTesting(const char* name, const Int_t iVerbose)
    : NeulandTask(name, 0 , iVerbose)
    , fCalData("NeulandCalData")
    , fHitData("NeulandHits")
{}

NeulandCalTesting::~NeulandCalTesting(){
    delete fNhits;
    delete fXYplanes;
    delete hTOFc;
    delete fNBar2;
    delete fNBar3;
    delete fSizeVs2Sigs;
    delete fSizeVs4Sigs;
    delete fEvents;
}



InitStatus NeulandCalTesting::Init(){
    ioman = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)ioman->GetObject("EventHeader.");
    if (fEventHeader == nullptr)
    {
        throw std::runtime_error("R3BNeulandCal2Hit: No R3BEventHeader");
    }

    fCalData.Init();
    fHitData.Init();

    fNhits = new TH1I("fNhits", "Number of hits per event", 200, 0.5, 200.5);
    fEvents = new TH1I("fEvents", "number of Events with less or more than 2 signals", 2, 0.5, 2.5);
    fXYplanes = new TH2D("fXYplanes", "hits position xy", 300, -150, 150, 300, -150, 150);
    hTOFc = new TH1D("hTOFc", "hTOFc", 6000, -100, 500);
    Register(hTOFc);
    Register(fNhits);
    Register(fEvents);

    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);

    Int_t binN = 340;
    canvas1 = new TCanvas("canvas1", "Events vs number of signals");
    fSizeVs2Sigs = new TH1I("fSizeVs2Sigs", "Events with 2 signals", binN, 0.5, binN + 0.5);
    fSizeVs2Sigs -> SetLineColor(kGreen);
    fSizeVs2Sigs->SetMinimum(1);
    fSizeVs4Sigs = new TH1I("fSizeVs4Sigs", "Events with more than 2 signals", binN, 0.5, binN + 0.5);
    fSizeVs4Sigs -> SetLineColor(kRed);
    fSizeVs2Sigs->Draw();
    gPad->SetLogy();
    fSizeVs4Sigs -> Draw("SAME");
    gPad->BuildLegend(0.6, 0.7, 0.95, 0.95);
    Register(canvas1);

    binN = 30;
    canvas = new TCanvas("canvas", "number of bars");
    fNBar2 = new TH1I("fNBar2", "Number of bars with two signals", binN, 0.5, 0.5 + binN);
    fNBar2 -> SetLineColor(kGreen);
    fNBar2->SetMinimum(1);
    fNBar3 = new TH1I("fNBar3", "Number of bars with more than 2 signals", binN, 0.5, 0.5 + binN);
    fNBar3 -> SetLineColor(kRed);
    fNBar2->Draw();
    gPad->SetLogy();
    fNBar3->Draw("SAME");
    gPad->BuildLegend(0.55, 0.7, 0.95, 0.95);
    Register(canvas);

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
    fSize = calData.size();
    fMaxHitNum = std::max(fMaxHitNum, fSize);

    // if (size == 352){
    if (fSize > 0){
        // std::cout << "analyzing event " << fEventNum << std::endl;
        
        std::vector<Int_t> BarIDs;
        for (auto calDataPtrPtr = calData.begin(); calDataPtrPtr != calData.end(); calDataPtrPtr++){

            const auto barID = (*calDataPtrPtr)->GetBarId();
            BarIDs.push_back(barID);
            // std::cout << "barID: " << barID << std::endl;



        }
        GetDuplicates(BarIDs);

        // std::cout << "Event number :\t"<<  fEventNum << std::endl;
        // std::cout << "===========================" << std::endl;
    }
    fNhits->Fill(hitData.size());
    for (const R3BNeulandHit* hit : hitData){
        auto hitTime = hit->GetT();
        // std::cout << hitTime << "\t bool: "  << std::isnan(hitTime)<< "\t barID: \t" <<(*hitPtrPtr)->GetPaddle() << std::endl;
        if(!std::isnan(hitTime)){
            Double_t tofc;
            tofc = hit->GetT() - (hit -> GetPosition().Mag() - fDistanceToTarget) / clight;
            hTOFc->Fill(tofc);


            Double_t randx = (std::rand() / (float)RAND_MAX);
            fXYplanes->Fill(hit ->GetPosition().X() +  5. * (randx - 0.5),
                                      hit ->GetPosition().Y() +  5. * (randx - 0.5));
        }
    }
    // std::cout <<   calData.size() << std::endl;

}


void NeulandCalTesting::FinishEvent(){
    // std::cout << "Event number :\t"<<  fEventNum << std::endl;
    // std::cout << "--------------------" << std::endl;
    // if (fEventNum % 10000 == 0)
    //     std::cout << std::endl << "First entry: " << fEvents->GetBinContent(1) << " Events converted." << fEvents->GetBinContent(2) <<" Percentage: " << fEvents->GetBinContent(2)/(fEvents->GetBinContent(2)+fEvents->GetBinContent(1))*100 <<"%"<< std::endl;
    Plot(10000);
    
}

void NeulandCalTesting::GetDuplicates(std::vector<Int_t> &vec){
    std::sort(vec.begin(), vec.end());
    short int count_2 = 0;
    short int count_3 = 0;
    Int_t buffer = vec[0];
    short int count = 0;
    for(const auto it : vec){
        if(buffer == it){
            // std::cout << it << std::endl;
            count++;
            // std::cout << "count: " << count << std::endl;
        }else{
            // fNhits->Fill(count);
            if(count == 2){
                count_2 ++;
            }
            else if(count > 2){
                count_3 ++;
            }
            // if(count != 2)
            //     std::cout << ">>>>>>>>>>>case occurs at bar " << buffer << " with " << count << " signals !" << std::endl;
            count = 1;
            buffer = it;
            // std::cout << "-------------" << std::endl;
            // std::cout << it << std::endl;
        }


    }

    if(count_2 > 0)
        fNBar2->Fill(count_2);
    if(count_3 > 0){
        fNBar3->Fill(count_3);
        fSizeVs4Sigs->Fill(fSize);
    }
    else if(count_2 > 0)
        fSizeVs2Sigs->Fill(fSize);
}

void NeulandCalTesting::FinishTask(){
    fNBar2->Write();
    fNBar3->Write();
    fNhits->Write();
    fSizeVs2Sigs->Write();
    fSizeVs4Sigs->Write();
    fXYplanes->Write();
    hTOFc->Write();
    canvas->Write();
    canvas1->Write();

    std::cout << "Task finished!" << std::endl;
    std::cout << "max number of hits:" << fMaxHitNum << std::endl;
}
