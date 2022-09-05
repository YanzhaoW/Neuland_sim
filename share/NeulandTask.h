#ifndef NEULANDTASK_H
#define NEULANDTASK_H

#include "FairTask.h"

class TCanvas;
class TApplication;
class TList;
class TH1;

class NeulandTask : public FairTask{
    public:
        NeulandTask(Bool_t a = true);
        NeulandTask(const char *name,  Bool_t a = true, Int_t iVerbose=1);
        ~NeulandTask();
        void Register(TCanvas* c);
        void Register(TH1* h, Option_t* option = "");
        void Plot(Int_t event_gap = 1000);

    private:
        Bool_t fApp = true;
        TList* canvasList = new TList();
        TApplication* app =  nullptr;
};

#endif
