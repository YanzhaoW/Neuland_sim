#include "NeulandTask.h"
#include "FairLogger.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TList.h"

NeulandTask::NeulandTask()
    : FairTask()
{
    app = new TApplication("histograms", 0, 0);
}

NeulandTask::NeulandTask(const char *name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    app = new TApplication("histograms", 0, 0);
}

NeulandTask::~NeulandTask()
{
    delete canvasList;
    delete app;
}

void NeulandTask::Register(TCanvas* c)
{
    std::cout << "program passing here" << std::endl;
    canvasList->AddLast(c);
    std::cout << "program passing here" << std::endl;
}

void NeulandTask::Plot(Int_t event_gap)
{

    TIter next(canvasList);
    FairRootManager* man = FairRootManager::Instance();
    if (!man)
    {
        LOG(FATAL) << "R3BNeulandEventProgress: No root manager";
    }

    if (man->GetEntryNr() % event_gap == 0)
    {
        while (TObject* obj = next())
        {
            TCanvas* c = dynamic_cast<TCanvas*>(obj);
            c->Update();
            c->Draw();
        }
    }
}

