#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauCollectionSelectorTight.h" // RecoHadTauSelectorTight

#include <cmath> // fabs

RecoHadTauSelectorTight::RecoHadTauSelectorTight(int index, bool debug)
  : RecoHadTauSelectorBase(index, debug)
{ 
  min_pt_ = 20.;
  max_absEta_ = 2.3;
  max_dz_ = 0.2;
  min_decayModeFinding_ = 1;
  min_id_mva_dR03_ = 5; // Tight
  min_raw_mva_dR03_ = -1.e+6;
  min_id_mva_dR05_ = -1000;
  min_raw_mva_dR05_ = -1.e+6;
  min_id_cut_dR03_ = -1000;
  max_raw_cut_dR03_ = 1.e+6;
  min_id_cut_dR05_ = -1000;
  max_raw_cut_dR05_ = 1.e+6;
  min_antiElectron_ = -1000;
  min_antiMuon_ = -1000;
}
