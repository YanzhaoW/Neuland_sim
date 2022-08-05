#include "EventProgress.h"
#include <iostream>
#include "FairRootManager.h"
#include "FairTask.h"

EventProgress::EventProgress(Int_t NOfEvent) : fEventMax (NOfEvent){}

EventProgress::~EventProgress(){
    delete fwatch;
}

InitStatus EventProgress::Init() {

    fwatch -> Start(true);

    FairRootManager* man = FairRootManager::Instance();
    if (!man)
    {
        LOG(FATAL) << "R3BNeulandEventProgress: No root manager";
        return kFATAL;
    }

    if (fEventMax <= 0) 
        fEventMax = man->CheckMaxEventNo();

    return kSUCCESS;
}

void EventProgress::Exec(Option_t *){
    
    if (EventNum == 0)
        return;
    fPercent = static_cast<float>(EventNum) / fEventMax;
    std::cout<< "Analysing event: " << "\033[0;33m" << std::setw(9) << EventNum <<"\033[0m";
    std::cout <<"\tProgress: " << "\033[0;33m" << std::setw(3) << std::fixed << std::setprecision(3) << fPercent * 100 << "%\033[0m";
    std::cout << "\tTime remaining: \033[0;33m" << GetTimeStr() << "\033[0m   ";
    std::cout  << "\r" <<std::flush;
}

void EventProgress::FinishEvent(){
    EventNum++;
}

void EventProgress::FinishTask(){
    std::cout << std::endl;
}

void EventProgress::TimeEval(){
    Double_t remain_time_sec = 0;
    Double_t from_start_sec = fwatch -> RealTime();
    fwatch->Continue();
    if(EventNum != 0){
        remain_time_sec = static_cast<Double_t>(fEventMax - EventNum) / EventNum * from_start_sec;
        if (fremaining_time_sec > remain_time_sec)
            fremaining_time_sec = static_cast<Int_t>(remain_time_sec);
    }
}


TString EventProgress::GetTimeStr(){
    TimeEval();
    UInt_t hour = fremaining_time_sec / 3600;
    UInt_t minite = (fremaining_time_sec - hour*3600) / 60;
    UInt_t sec = fremaining_time_sec - hour * 3600 - minite*60;
    return TString::Format("%d h, %d min, %d sec", hour, minite, sec);
}
