#ifndef TESTTASK_H
#define TESTTASK_H

#include "NeulandTask.h"
#include "TCAConnector.h"

class TH1D;
class TH2D;
class R3BNeulandHit;
class TCanvas;
class TApplication;

class TestTask : public NeulandTask {
    public:
        TestTask();
        ~TestTask();
        InitStatus Init() override;
        // void SetParContainers() override;
        void Exec(Option_t*) override;
        void FinishTask() override;
        void FinishEvent() override;
        
    private:
        TH1D* hTOFc;
        TH2D* ahXYperPlane;
        TCAInputConnector<R3BNeulandHit> fHitData;
        const double fDistanceToTarget = 1520;
        const double clight = 29.9792458;
};



#endif
