#include "HistWriter.h"
#include "RunManager.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TFolder.h"
#include "TH1.h"
#include "TKey.h"
#include "TSystem.h"

#include <boost/algorithm/string.hpp>

HistWriter::HistWriter()
    : runMan(RunManager::GetInstance())
{
    SetPar();
}

HistWriter::~HistWriter() 
{ 
    fRootFile->Close();
}

void HistWriter::SetPar()
{
    auto configs = runMan->GetConfigs();
    std::transform(configs->begin(), configs->end(), std::back_inserter(fAnlysFileNames), [](RunConfig rc) {
        return boost::filesystem::path(rc.anlysFile.c_str());
    });
    std::transform(configs->begin(), configs->end(), std::back_inserter(fAnlysFileNames), [](RunConfig rc) {
        return boost::filesystem::path(rc.simFile.c_str());
    });

    fFolder = runMan->GetFileDir();
}

void HistWriter::Write()
{
    if (CreateHistFile() != 0)
    {
        return;
    }

    for (auto const& it : fAnlysFileNames)
    {
        if (CopyHists(it) != 0)
        {
            std::cerr << "Histograms in file " << it << " failed to copy!" << std::endl;
            return;
        }
    }

    std::cout << "histograms are successfully written into root file " << fRootFile->GetName() << std::endl;
}

int HistWriter::CreateHistFile()
{
    auto filename = fFolder + fFileName;
    fRootFile = new TFile{ filename.c_str(), "RECREATE" };
    if (fRootFile->IsZombie())
    {
        std::cerr << "root file is not created successfully! " << std::endl;
        return -1;
    }
    return 0;
}

int HistWriter::CopyHists(boost::filesystem::path file)
{
    auto filename = file.string();
    auto basename = file.stem().string();

    assert(fRootFile != nullptr);

    auto dir = fRootFile->mkdir(basename.c_str());

    if (!dir)
    {
        dir = fRootFile->GetDirectory(basename.c_str());
        if (dir)
        {
            std::cerr << "directory not created!" << std::endl;
        }
        else
        {
            std::cout << "directory already existed" << std::endl;
        }
        return -1;
    }
    else
    {
        return CopyHistsToDir(filename, dir);
    }
}

int HistWriter::CopyHistsToDir(std::string datafile, TDirectory* outdir)
{
    auto dataFile = new TFile{ datafile.c_str(), "READ" };
    if (!dataFile)
    {
        std::cerr << "file " << datafile << " cannot be read!" << std::endl;
        return -1;
    }
    CopyToDirRecursive(dynamic_cast<TDirectory*>(dataFile), outdir);

    dataFile->Close();
    return 0;
}

void HistWriter::CopyToDirRecursive(TDirectory* dir, TDirectory* outDir) noexcept
{
    TIter next{ dir->GetListOfKeys() };
    TKey* key;

    while ((key = dynamic_cast<TKey*>(next())))
    {
        auto obj = key->ReadObj();
        if (auto d = dynamic_cast<TDirectory*>(obj))
        {
            CopyToDirRecursive(d, outDir);
        }
        else if (auto h = dynamic_cast<TH1*>(obj))
        {
            auto objName = TString{ dir->GetName() } + TString{ "_" } + TString{ obj->GetName() };

            outDir->WriteObject(obj, objName);
        }
    }
}

void HistWriter::ToCsv(){
    fRootFile->Close();
    auto filename = fFolder + fFileName;
    fRootFile = new TFile{ filename.c_str(), "READ" };
    std::map<std::string, std::vector<Hist>> histograms;

    TIter next{ fRootFile->GetListOfKeys() };
    TKey* key;

    //creating folder for csv files:
    auto fD = fFolder + fCsvRFolder;
    auto dir = boost::filesystem::path(fD.c_str());
    if(boost::filesystem::create_directories(dir))
    {
        std::cout << "Directory " << fD << " is created. " << std::endl;
    }
    else{
        std::cout <<  "Directory " << fD << " already existed. " << std::endl;
    }

    //extract all histograms from the root file.
    while ((key = dynamic_cast<TKey*>(next())))
    {
        auto obj = key->ReadObj();
        if (auto d = dynamic_cast<TDirectory*>(obj))
        {
            auto run = HistRun{d};
            run.ExtractHist(histograms);
        }
    }

    for(auto it = histograms.begin(); it != histograms.end(); it++){
        std::cout << "writing csv file " << it->first << ".csv..." << std::endl;

        auto filepath = fFolder + fCsvRFolder + it->first + std::string{".csv"};
        auto ofile = std::ofstream{filepath.c_str(), std::ios_base::trunc};
        if(! ofile.is_open()){
            std::cerr << "file failed to open" << std::endl;
            return;
        }

        auto title = bool{true};

        for(auto const &hist : it->second){
            if(title){
                auto dim = hist.GetHist()->GetDimension();
                if(dim == 1)
                    ofile << "xbin,entry,energy,multiplicity,particleID" << std::endl;
                if(dim == 2)
                    ofile << "xbin,ybin,entry,energy,multiplicity,particleID" << std::endl;
                title = false;
            }
            WriteToCsv(hist, ofile);
        }
        ofile.close();
    }

}

int HistWriter::WriteToCsv( const Hist& h, std::ofstream& ofile) const{
    auto hist = h.GetHist();
    auto dim = hist->GetDimension();

    if(dim == 1){
        for(auto i_x = 1; i_x <= hist->GetNbinsX(); i_x++){
            ofile << hist->GetBinCenter(i_x) << "," << hist->GetBinContent(i_x)<< "," << h.GetEnergy()<< "," << h.GetMulti()<< "," << h.GetId() << std::endl;
        }
    }
    if(dim == 2) {
        for(auto i_x = 1; i_x <= hist->GetNbinsX(); i_x++){
            for(auto i_y = 1; i_y <= hist->GetNbinsY(); i_y++){
                ofile << hist->GetXaxis()->GetBinCenter(i_x) << "," <<hist->GetYaxis()->GetBinCenter(i_y) << "," << hist->GetBinContent(i_x, i_y)<< "," << h.GetEnergy()<< "," << h.GetMulti()<< "," << h.GetId() << std::endl;
            }
        }
    }
    return 0;
}

HistRun::HistRun(TDirectory* d) : fDir{d}
{
    fFileName = fDir->GetName();
    SetEMI();
}

void HistRun::SetEMI(){
    std::vector<std::string> splits;

    boost::algorithm::split(splits, fFileName, boost::is_any_of("_"));
    fEnergy = std::stod(splits[2]);
    fMulti = std::stoi(splits[4]);
    fId = std::stoi(splits[6]);
}

void HistRun::ExtractHist(std::map<std::string, std::vector<Hist>>& map){
    TIter next{ fDir->GetListOfKeys() };
    TKey* key;

    fDir->ls();
    while ((key = dynamic_cast<TKey*>(next())))
    {
        auto obj = key->ReadObj();
        if (auto h = dynamic_cast<TH1*>(obj))
        {
            map[h->GetName()].emplace_back(h, fEnergy, fMulti, fId);
        }
    }
}
