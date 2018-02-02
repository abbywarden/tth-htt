#ifndef tthAnalysis_HiggsToTauTau_RecoJetCollectionSelectorBtag_h
#define tthAnalysis_HiggsToTauTau_RecoJetCollectionSelectorBtag_h

#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionSelector.h" // ParticleCollectionSelector
#include "tthAnalysis/HiggsToTauTau/interface/RecoJet.h" // RecoJet

class RecoJetSelectorBtag
{
public:
  explicit RecoJetSelectorBtag(int era,
                               int index,
                               bool debug);

  /**
   * @brief Check if jet given as function argument passes pT and eta cuts
   *        (pT > 25 GeV and |eta| < 2.4, cf. Section 3.1 of AN-2015/321)
   * @return True if jet passes selection; false otherwise
   */
  bool operator()(const RecoJet & jet) const;

protected:
  int era_;
  bool debug_;
  Double_t min_pt_;      ///< lower cut threshold on pT
  Double_t max_absEta_;  ///< upper cut threshold on absolute value of eta
  Double_t min_BtagCSV_; ///< lower cut threshold on CSV b-tagging discriminator value
};

class RecoJetSelectorBtagLoose 
  : public RecoJetSelectorBtag
{
public:
  explicit RecoJetSelectorBtagLoose(int era, int index = -1, bool debug = false);
};

typedef ParticleCollectionSelector<RecoJet, RecoJetSelectorBtagLoose> RecoJetCollectionSelectorBtagLoose;

class RecoJetSelectorBtagMedium 
  : public RecoJetSelectorBtag
{
public:
  explicit RecoJetSelectorBtagMedium(int era, int index = -1, bool debug = false);
};

typedef ParticleCollectionSelector<RecoJet, RecoJetSelectorBtagMedium> RecoJetCollectionSelectorBtagMedium;

#endif // tthAnalysis_HiggsToTauTau_RecoJetCollectionSelectorBtag_h

