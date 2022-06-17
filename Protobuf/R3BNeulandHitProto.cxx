#include "R3BNeulandHitProto.h"
#include "NeulandHit.pb.h"
#include "google/protobuf/stubs/common.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"
#include <iostream>

void R3BNeulandHitProto::addHit(ProtoHits::NeulandHit* pbhit, R3BNeulandHit* hit){
    pbhit->set_event(run->GetEventHeader()->GetMCEntryNumber());
    pbhit->set_paddle(hit->GetPaddle());
    pbhit->set_tdcl(hit->GetTdcL());
    pbhit->set_tdcr(hit->GetTdcR());
    pbhit->set_time(hit->GetT());
    pbhit->set_qdcl(hit->GetQdcL());
    pbhit->set_qdcr(hit->GetQdcR());
    pbhit->set_energy(hit->GetE());

    pbhit->mutable_position()->set_x(hit->GetPosition().X());
    pbhit->mutable_position()->set_y(hit->GetPosition().Y());
    pbhit->mutable_position()->set_z(hit->GetPosition().Z());

    pbhit->mutable_pixel()->set_x(hit->GetPixel().X());
    pbhit->mutable_pixel()->set_y(hit->GetPixel().Y());
    pbhit->mutable_pixel()->set_z(hit->GetPixel().Z());
}

InitStatus R3BNeulandHitProto::Init() {
    fHits.Init();
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    PBData = new ProtoHits::NeulandData();
    return kSUCCESS;
}


void R3BNeulandHitProto::Exec(Option_t* ){
    const auto hits = fHits.Retrieve();
    auto event = PBData->add_events();
    for(const auto& hit:hits){
        addHit(event->add_hits(), hit);
        // std::cout<< hit->GetPosition().X()<<"," << hit->GetPosition().Y()<<"," << hit->GetPosition().Z() <<"," << hit->GetE() << std::endl;
    }
}


void R3BNeulandHitProto::Finish(){
    if(!PBData->SerializeToOstream(&fout)){
        std::cerr << "failed to write a protobuf binary." << std::endl;
    }
    // google::protobuf::ShutdownProtobufLibrary();
}
void R3BNeulandHitProto::FinishEvent(){}
