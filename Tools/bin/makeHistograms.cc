#include "diphoton-analysis/Tools/interface/sampleList.hh"
#include "diphoton-analysis/Tools/interface/utilities.hh"

#include "TCanvas.h"
#include "TString.h"
#include "TH1.h"
#include "TFile.h"

void allSamples(const std::string &region);

int main()
{
  init();

  TFile *output = new TFile("data/Minv_histos.root", "recreate");
  output->mkdir("BB");
  output->mkdir("BE");
  output->cd("BE");
  allSamples("BE");
  output->cd("BB");
  allSamples("BB");
  output->Write();
  output->Close();

}

void allSamples(const std::string &region)
{

  int nBins = 120;
  double xMin = 0.0;
  double xMax = 6000.;

  std::map<std::string, std::string> cuts;
  cuts["BB"] = "isGood*(Diphoton.Minv>500 && Photon1.pt>75 && Photon2.pt>75 && abs(Photon1.eta)<1.4442 && abs(Photon2.eta)<1.4442)";
  cuts["BE"] = "isGood*(Diphoton.Minv>500 && Photon1.pt>75 && Photon2.pt>75 && ( (abs(Photon1.eta)<1.4442 && abs(Photon2.eta)>1.56 && abs(Photon2.eta)<2.5) || (abs(Photon2.eta)<1.4442 && abs(Photon1.eta)>1.56 && abs(Photon1.eta)<2.5) ))";

  std::vector<std::string> samples = getSampleList();

  std::vector<int> stringScales = {3000, 3500, 4000, 4500, 5000, 5500, 6000};

  for(auto isample : samples) {
    std::string sampleCut = cuts[region];
    // skip the Sherpa GEN trees
    if( isample.compare("ggGen") == 0) continue;
    // apply weights for all samples except data
    if( isample.compare("data") != 0 ) sampleCut+="*weightAll";
    else sampleCut+="*HLT_DoublePhoton60_v1";
    // apply k-factor to Sherpa GG sample
    if( isample.compare("gg") == 0) sampleCut += "*" + kfactorString(region);
    std::cout << "Making histograms for sample " << isample << " with cut\n" << sampleCut << std::endl;
    TH1F *hist = new TH1F(isample.c_str(), isample.c_str(), nBins, xMin, xMax);
    std::cout << "Making histograms for sample " << hist->GetName() << " with cut\n" << sampleCut << std::endl;
    chains[isample]->Project(isample.c_str(), "Minv",  sampleCut.c_str());
  }

}
