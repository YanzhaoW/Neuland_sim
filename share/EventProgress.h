#ifndef EVENTPROGRESS_H
#define EVENTPROGRESS_H

#include "FairTask.h"
#include "FairLogger.h"
#include "TStopwatch.h"
#include "TString.h"
#include <limits>

class EventProgress : public FairTask {
    public:
        explicit EventProgress(Int_t NOfEvent = 0);
        ~EventProgress() override;

        EventProgress(const EventProgress&) = delete;
        EventProgress(EventProgress&&) = delete;
        EventProgress& operator=(const EventProgress&) = delete;
        EventProgress& operator=(const EventProgress&&) = delete;

        InitStatus Init() override;
        void Exec(Option_t* t) override;
        void FinishEvent() override;
        void FinishTask() override;

    private:
        UInt_t fEventMax;
        UInt_t EventNum = 0;
        Float_t fPercent = 0;
        UInt_t fremaining_time_sec = std::numeric_limits<UInt_t>::max();
        TStopwatch* fwatch = new TStopwatch();
        void TimeEval();
        TString GetTimeStr();



};

#endif
