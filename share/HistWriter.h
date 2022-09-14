#ifndef HISTWRITER_H
#define HISTWRITER_H

#include <boost/filesystem.hpp>
#include <map>
#include <string>
#include <vector>

class RunManager;
class TFolder;
class TFile;
class TDirectory;
class TObject;
class TH1;

class Hist
{
  public:
    Hist(TH1* hist, double e, int mul, int id)
        : fHist(hist)
        , fEnergy(e)
        , fMulti(mul)
        , fId(id)
    {
    }
    // getters:
    TH1* GetHist() const { return fHist; }
    double GetEnergy() const { return fEnergy; }
    int GetMulti() const { return fMulti; }
    int GetId() const { return fId; }

  private:
    TH1* fHist;
    double fEnergy = 0.0;
    int fMulti = 0;
    int fId = 0;
};

class HistWriter
{
  public:
    HistWriter();
    ~HistWriter();
    void Write();
    void ToCsv();

  private:
    int CreateHistFile();
    int CopyHists(boost::filesystem::path);
    int CopyHistsToDir(std::string datafile, TDirectory* dir);
    void CopyToDirRecursive(TDirectory* HistDir, TDirectory* outDir) noexcept;
    int WriteToCsv(const Hist&, std::ofstream&) const;

    RunManager* runMan = nullptr;
    void SetPar();
    std::vector<boost::filesystem::path> fAnlysFileNames;
    std::string fFolder;
    std::string fFileName = "histogram.root";
    std::string fCsvRFolder = "histograms/";
    TFile* fRootFile = nullptr;
};

class HistRun
{
  public:
    HistRun(TDirectory*);
    void ExtractHist(std::map<std::string, std::vector<Hist>>&);

  private:
    TDirectory* fDir;
    std::string fFileName;
    void SetEMI();
    double fEnergy = 0.0;
    int fMulti = 0;
    int fId = 0;
};

#endif
