#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauSelectorFakeable_1l_2tau.h" // RecoHadTauSelectorFakeable_1l_2tau

#include <cmath> // fabs

RecoHadTauSelectorFakeable_1l_2tau::RecoHadTauSelectorFakeable_1l_2tau(int index, bool debug)
  : RecoHadTauSelectorFakeable(debug)
  , index_(index)
{
  // CV: require loose WP of MVA-based discriminator trained for dR=0.3 isolation cone
  //     instead of loose cut on tau isolation pT sum computed within dR=0.5 isolation cone
  min_id_mva_dR03_ = 2;
  max_raw_cut_dR05_ = 1.e+6;
  // CV: require leading (subleading) hadronic tau candidates selected in 1l_2tau channel to pass very loose (loose) WP of discriminator against electrons
  if ( index == 0 ) {
    min_antiElectron_ = 1;
  } else if ( index == 1 ) {
    min_antiElectron_ = 2;
  } else assert(0);
  // CV: require hadronic tau candidates selected in 1l_2tau channel to pass loose WP of discriminator against muons 
  min_antiMuon_ = 1;
}
