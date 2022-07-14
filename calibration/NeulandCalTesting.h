#ifndef NEULANDCALTESTING_H
#define NEULANDCALTESTING_H

#include "FairTask.h"
#include "R3BNeulandCalData.h"
#include "TCAConnector.h"

class FairRootManager;
class R3BEventHeader;
class TH1I;
class TH1D;
class TH2D;
class R3BNeulandHit;


class NeulandCalTesting : public FairTask
    {
    public:
        NeulandCalTesting(const char* name = "NeulandCalTesting", const Int_t iVerbose = 0);
        ~ NeulandCalTesting() override = default;


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
        TH1D* fSignalTimes;
        TH1D* fSignalTimesTrig;
        TH1D* fHitTimes;
        TH2D* fXYplanes;

        int fEventNum = 0;
        int fMaxHitNum = 0;
        const Double_t fDistanceToTarget = 1520;
        const Double_t clight = 29.9792458;
    };




#endif
