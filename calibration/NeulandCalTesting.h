#ifndef NEULANDCALTESTING_H
#define NEULANDCALTESTING_H

#include "NeulandTask.h"
#include "R3BNeulandCalData.h"
#include "TCAConnector.h"

class FairRootManager;
class R3BEventHeader;
class TH1I;
class TH1D;
class TH2D;
class R3BNeulandHit;
class TCanvas;


class NeulandCalTesting : public NeulandTask
    {
    public:
        NeulandCalTesting(const char* name = "NeulandCalTesting", const Int_t iVerbose = 0);
        ~ NeulandCalTesting() override;


        InitStatus Init() override;
        InitStatus ReInit() override;
        void SetParContainers() override;
        void Exec(Option_t*) override;
        void FinishTask() override;
        void FinishEvent() override;
    private:
        void GetDuplicates(std::vector<Int_t> &);
        void SetParameter();
        TCAInputConnector<R3BNeulandCalData> fCalData;
        TCAInputConnector<R3BNeulandHit> fHitData;
        R3BEventHeader* fEventHeader;
        FairRootManager* ioman;
        TH1I* fNhits;
        TH1I* fNBar2;
        TH1I* fNBar3;
        TH1I* fEvents;
        TH1I* fSizeVs2Sigs;
        TH1I* fSizeVs4Sigs;
        TH1D* hTOFc;
        TH2D* fXYplanes;
        TCanvas* canvas;
        TCanvas* canvas1;

        int fEventNum = 0;
        int fMaxHitNum = 0;
        Int_t fSize = 0;
        const Double_t fDistanceToTarget = 1520;
        const Double_t clight = 29.9792458;
    };




#endif
