#include <iostream>

#include "TCollection.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

void FilePrint(TDirectory* dir) {
    TIter next(dir->GetListOfKeys());
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next()))) {
        std::cout << key->GetClassName() << std::endl;
        std::cout << key->GetName() << std::endl;
        std::cout << "----------" << std::endl;
    }
}

int main() {
    auto file = new TFile("test.digi.root");
    if (!file) std::cerr << "File failed to open" << std::endl;
    auto dir =
        dynamic_cast<TDirectory*>(file->FindObjectAny("R3BNeulandDigitizer"));
    if (!dir) {
        std::cerr << "can't find the object" << std::endl;
    } else {
        FilePrint(dir);
    }

    auto canvas = new TCanvas{};
    auto hNHits = dynamic_cast<TH1F*>(dir->FindObjectAny("hNHits"));
    hNHits->Scale(1/hNHits->GetMaximum());
    hNHits->SetTitle("Distribution of number of hits from paddles");
    hNHits->Draw("HIST");
    canvas->Print("hNHits.png");

    auto hElightPri = dynamic_cast<TH1F*>(dir->FindObjectAny("hElightPri"));
    hElightPri->Draw();
    canvas->Print("hElightPri.png");
    auto hElightSec = dynamic_cast<TH1F*>(dir->FindObjectAny("hElightSec"));
    hElightSec->Draw();
    canvas->Print("hElightSec.png");
    auto hTimeDiff = dynamic_cast<TH1F*>(dir->FindObjectAny("hTimeDiff"));
    hTimeDiff->Draw();
    canvas->Print("hTimeDiff.png");
    auto hELightDiff = dynamic_cast<TH1F*>(dir->FindObjectAny("hELightDiff"));
    hELightDiff->Draw();
    canvas->Print("hELightDiff.png");




    // drawing light comparisons:
    hElightSec->SetTitle("light intensity for the two strongest PMT hits");
    hElightSec->SetStats(0);
    hElightSec->SetXTitle("Energy (MeV)");
    hElightSec->SetYTitle("Entries");
    hElightSec->Draw();
    hElightSec->SetLineColor(kRed);
    hElightPri->SetStats(0);
    hElightPri->Draw("SAME");
    auto legend = new TLegend(0.6, 0.7, 0.85, 0.85);
    legend->AddEntry(hElightPri, "Strongest hit", "l");
    legend->AddEntry(hElightSec, "Second strongest hit", "l");
    legend->Draw();
    canvas->Print("plot.png");



    return 0;
}
