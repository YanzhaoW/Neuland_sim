#include "NeulandTask.h"
#include "FairLogger.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TList.h"
#include "TH1.h"
#include "TString.h"

NeulandTask::NeulandTask(Bool_t a)
    : fApp (a), FairTask()
{
    if(fApp)
        app = new TApplication("histograms", 0, 0);
}

NeulandTask::NeulandTask(const char *name, Bool_t a, Int_t iVerbose)
    : fApp (a), FairTask(name, iVerbose)
{
    if(fApp)
        app = new TApplication("histograms", 0, 0);
}

NeulandTask::~NeulandTask()
{
    if(fApp)
        canvasList->Clear(); // This doesn't delete the objects in the list.
    delete app;
}

void NeulandTask::Register(TCanvas* c)
{
    if(fApp)
        canvasList->AddLast(c);
}

void NeulandTask::Register(TH1* h, Option_t* option){
    if(!fApp)
        return void();
    TString canvasName = TString::Format("histgram_%d", canvasList->GetSize());
    auto canvas =  new TCanvas(canvasName, canvasName);
    h->Draw(option);
    Register(canvas);
}

void NeulandTask::Plot(Int_t event_gap)
{

    if(!fApp)
        return void();
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

