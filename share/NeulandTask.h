#ifndef NEULANDTASK_H
#define NEULANDTASK_H

#include "FairTask.h"

class TCanvas;
class TApplication;
class TList;

class NeulandTask : public FairTask{
    public:
        NeulandTask();
        NeulandTask(const char *name, Int_t iVerbose=1);
        ~NeulandTask();
        void Register(TCanvas* c);
        void Plot(Int_t event_gap = 1000);

    private:
        TList* canvasList = new TList();
        TApplication* app =  nullptr;
};

#endif
