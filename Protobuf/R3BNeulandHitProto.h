#ifndef R3BNEULANDHITPROTO_H
#define R3BNEULANDHITPROTO_H


#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "TCAConnector.h"
#include "NeulandHit.pb.h"
#include "FairRunAna.h"


class R3BNeulandHitProto : public FairTask {
public:
    R3BNeulandHitProto(TString output = "protoData.bin", TString input = "NeulandHits"):fHits{input}, fout{output, std::ios::binary | std::ios::trunc}{};
    ~ R3BNeulandHitProto() override {};
    InitStatus Init() override; 
    void Exec(Option_t* ) override;
    void Finish() override ;
    void FinishEvent() override ;
    void addHit(ProtoHits::NeulandHit* , R3BNeulandHit* );
private:
    TCAInputConnector<R3BNeulandHit> fHits;
    std::ofstream fout;
    ProtoHits::NeulandData* PBData;
    FairRunAna* run = FairRunAna::Instance();
};

#endif
