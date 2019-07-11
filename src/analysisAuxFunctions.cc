#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h"

#include "tthAnalysis/HiggsToTauTau/interface/RecoElectron.h" // RecoElectron
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuon.h" // RecoMuon
#include "tthAnalysis/HiggsToTauTau/interface/RecoJet.h" // RecoJet
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetAK8.h" // RecoJetAK8
#include "tthAnalysis/HiggsToTauTau/interface/GenLepton.h" // GenLepton
#include "tthAnalysis/HiggsToTauTau/interface/cmsException.h" // cmsException()
#include "tthAnalysis/HiggsToTauTau/interface/leptonGenMatchingAuxFunctions.h" // countLeptonGenMatches()
#include "tthAnalysis/HiggsToTauTau/interface/hadTauGenMatchingAuxFunctions.h" // countHadTauGenMatches()
#include "tthAnalysis/HiggsToTauTau/interface/LocalFileInPath.h" // LocalFileInPath

#include <TMath.h> // TMath::Sqrt

#include <numeric> // std::accumulate()

double
get_BtagWP(int era,
           Btag btag,
           BtagWP wp)
{
  return BtagWP_map.at(era).at(btag).at(wp);
}

bool
isHigherPt(const Particle * particle1,
           const Particle * particle2)
{
  return particle1->pt() > particle2->pt();
}

bool
isHigherConePt(const RecoLepton * particle1,
               const RecoLepton * particle2)
{
  return (particle1->cone_pt() > particle2->cone_pt());
}

bool
isHigherCSV(const RecoJet * jet1,
            const RecoJet * jet2)
{
  return jet1->BtagCSV() > jet2->BtagCSV();
}

bool
isHigherCSV_ak8(const RecoJetAK8 * jet1,
                const RecoJetAK8 * jet2)
{
  double jet1BtagCSV = 0.;
  if(jet1->subJet1()) jet1BtagCSV += jet1->subJet1()->BtagCSV();
  if(jet1->subJet2()) jet1BtagCSV += jet1->subJet2()->BtagCSV();
  double jet2BtagCSV = 0.;
  if(jet2->subJet1()) jet2BtagCSV += jet2->subJet1()->BtagCSV();
  if(jet2->subJet2()) jet2BtagCSV += jet2->subJet2()->BtagCSV();
  return jet1BtagCSV > jet2BtagCSV;
}

int
get_selection(const std::string & selectionString)
{
  if(selectionString == "Loose")    return kLoose;
  if(selectionString == "Fakeable") return kFakeable;
  if(selectionString == "Tight")    return kTight;
  throw cmsException(__func__) << "Invalid Configuration parameter for object selection = " << selectionString;
}

int
get_era(const std::string & eraString)
{
  if(eraString == "2016")
  {
    return kEra_2016;
  }
  else if(eraString == "2017")
  {
    return kEra_2017;
  }
  else if(eraString == "2018")
  {
    return kEra_2018;
  }
  throw cmsException(__func__) << "Invalid Configuration parameter 'era' = " << eraString;
}

std::string
get_era(int era)
{
  switch(era)
  {
    case kEra_2016: return "2016";
    case kEra_2017: return "2017";
    case kEra_2018: return "2018";
    default: throw cmsException(__func__, __LINE__) << "Invalid era: " << era;
  }
}

int
get_tau_id_wp_int(const std::string & tauId_str)
{
  const std::string wp_type = tauId_str.substr(0, 7);
  if(! TauID_PyMap.count(wp_type))
  {
    throw cmsException(__func__, __LINE__) << "Unrecognizable tau ID selection: " << tauId_str;
  }
  return get_tau_id_wp_int(TauID_PyMap.at(wp_type), tauId_str.substr(7));
}

int
get_tau_id_wp_int(TauID tauID,
                  const std::string & wp_str)
{
  assert(TauID_levels.count(tauID));
  const int nof_levels = TauID_levels.at(tauID);
  assert(TauID_level_strings.count(nof_levels));
  const std::vector<std::string> & levels = TauID_level_strings.at(nof_levels);
  const auto wp_str_it = std::find(levels.begin(), levels.end(), wp_str);
  if(wp_str_it == levels.end())
  {
    throw cmsException(__func__, __LINE__)
      << "Invalid tau ID string requested for tau ID " << as_integer(tauID) << ": " << wp_str
    ;
  }
  return static_cast<int>(std::distance(levels.begin(), wp_str_it)) + 1;
}

std::string
get_tau_id_wp_str(TauID tauID,
                  int wp_int)
{
  assert(TauID_levels.count(tauID));
  const int nof_levels = TauID_levels.at(tauID);
  if(wp_int < 1 || wp_int > nof_levels)
  {
    throw cmsException(__func__, __LINE__)
      << "Invalid level requested for tau ID " << as_integer(tauID) << ": " << wp_int
    ;
  }
  assert(TauID_level_strings.count(nof_levels));
  const std::vector<std::string> & levels = TauID_level_strings.at(nof_levels);
  assert(static_cast<int>(levels.size()) == nof_levels);
  return levels.at(wp_int - 1);
}

int
getHadTau_genPdgId(const RecoHadTau * hadTau)
{
  int hadTau_genPdgId = -1;
  if     (hadTau->genHadTau()) hadTau_genPdgId = 15;
  else if(hadTau->genLepton()) hadTau_genPdgId = std::abs(hadTau->genLepton()->pdgId());
  return hadTau_genPdgId;
}

double
get_BtagWeight(const std::vector<const RecoJet *> & jets)
{
  double btag_weight = 1.;
  for(const RecoJet * jet: jets)
  {
    btag_weight *= jet->BtagWeight();
  }
  return btag_weight;
}

Particle::LorentzVector
compMHT(const std::vector<const RecoLepton *> & leptons,
        const std::vector<const RecoHadTau *> & hadTaus,
        const std::vector<const RecoJet *> & jets)
{
  math::PtEtaPhiMLorentzVector mht_p4(0,0,0,0);
  for(const RecoLepton * lepton: leptons)
  {
    mht_p4 += lepton->p4();
  }
  for(const RecoHadTau * hadTau: hadTaus)
  {
    mht_p4 += hadTau->p4();
  }
  for(const RecoJet * jet: jets)
  {
    mht_p4 += jet->p4();
  }
  return mht_p4;
}

double
compMEt_LD(const Particle::LorentzVector & met_p4,
           const Particle::LorentzVector & mht_p4)
{
  return met_coef * met_p4.pt() + mht_coef * mht_p4.pt();
}

double
comp_Smin(const Particle::LorentzVector & visP4,
          double metPx,
          double metPy)
{
  const double visPt = visP4.pt();
  const double visMass = visP4.mass();
  const double visMass2 = visMass * visMass;
  const double visEt = std::sqrt(visMass2 + visPt * visPt);
  const double metEt = std::sqrt(metPx * metPx + metPy * metPy);
  const double Smin = std::sqrt(visMass2 + 2. * (visEt * metEt - (visP4.px() * metPx + visP4.py() * metPy)));
  return Smin;
}


std::vector<const RecoLepton *>
mergeLeptonCollectionsNoSort(const std::vector<const RecoElectron *> & electrons,
                             const std::vector<const RecoMuon *> & muons)
{
  std::vector<const RecoLepton *> leptons;
  const std::size_t nLeptons = electrons.size() + muons.size();
  if(nLeptons > 0)
  {
    leptons.reserve(nLeptons);
    leptons.insert(leptons.end(), electrons.begin(), electrons.end());
    leptons.insert(leptons.end(), muons.begin(), muons.end());
  }
  return leptons;
}

std::vector<const RecoLepton *>
mergeLeptonCollections(const std::vector<const RecoElectron *> & electrons,
                       const std::vector<const RecoMuon *> & muons)
{
  return mergeLeptonCollections(electrons, muons, isHigherPt);
}

template<>
void
printCollection<RecoLepton>(const std::string & collection_name,
                            const std::vector<const RecoLepton *> & leptons)
{
  std::cout << " (#" << collection_name << " = " << leptons.size() << ")\n";
  for(std::size_t idxLepton = 0; idxLepton < leptons.size(); ++idxLepton)
  {
    std::cout << collection_name << " #" << idxLepton << ": ";
    const RecoLepton * const lepton = leptons[idxLepton];

    const RecoElectron * const electron = dynamic_cast<const RecoElectron *>(lepton);
    if(electron)
    {
      std::cout << (*electron) << std::endl;
      continue;
    }

    const RecoMuon * const muon = dynamic_cast<const RecoMuon *>(lepton);
    if(muon)
    {
      std::cout << (*muon) << std::endl;
      continue;
    }

    std::cout << (*lepton) << std::endl;
  }
}

int
countFakeElectrons(const std::vector<const RecoLepton *> & leptons)
{
  int numGenMatchedLeptons = 0;
  int numGenMatchedPhotons = 0;
  int numGenMatchedJets = 0;
  for(const RecoLepton * const lepton: leptons)
  {
    if(lepton->is_electron())
    {
      countLeptonGenMatches(lepton, numGenMatchedLeptons, numGenMatchedPhotons, numGenMatchedJets);
    }
  }
  return numGenMatchedJets;
}

int
countFakeMuons(const std::vector<const RecoLepton *> & leptons)
{
  int numGenMatchedLeptons = 0;
  int numGenMatchedPhotons = 0;
  int numGenMatchedJets = 0;
  for(const RecoLepton * const lepton: leptons)
  {
    if(lepton->is_muon())
    {
      countLeptonGenMatches(lepton, numGenMatchedLeptons, numGenMatchedPhotons, numGenMatchedJets);
    }
  }
  return numGenMatchedJets;
}

int
countFakeHadTaus(const std::vector<const RecoHadTau *> & hadTaus)
{
  int numGenMatchedHadTaus = 0;
  int numGenMatchedElectrons = 0;
  int numGenMatchedMuons = 0;
  int numGenMatchedJets = 0;
  for(const RecoHadTau * const hadTau: hadTaus)
  {
    countHadTauGenMatches(hadTau, numGenMatchedHadTaus, numGenMatchedElectrons, numGenMatchedMuons, numGenMatchedJets);
  }
  return numGenMatchedJets;
}

int
nCombinationsK(int n,
               int k)
{
  assert(n >= k && n >= 0 && k >= 0);
  if(k > n)     return 0;
  if(k * 2 > n) k = n-k;
  if(k == 0)    return 1;

  int result = n;
  for(int i = 2; i <= k; ++i)
  {
    result *= (n - i + 1);
    result /= i;
  }
  return result;
}

int
countTrigObjs_passingL1(const std::vector<TrigObj> & trigObjs,
                        int Id,
                        double min_l1pt,
                        double min_l1pt_2)
{
  return std::count_if(
    trigObjs.begin(), trigObjs.end(),
    [Id, min_l1pt, min_l1pt_2](const TrigObj & trigObj) -> bool
    {
      return
        trigObj.id() == Id                 &&
        trigObj.l1pt() > min_l1pt          &&
        (
          trigObj.l1pt_2() > min_l1pt_2 ||
          min_l1pt_2 == -1.
        )
      ;
    }
  );
}

bool
contains(const std::vector<std::string> & list_of_strings,
         const std::string & keyWord)
{
  return std::find(list_of_strings.begin(), list_of_strings.end(), keyWord) != list_of_strings.end();
}

std::string
findFile(const std::string & fileName)
{
  const LocalFileInPath inputFile(fileName);
  if(inputFile.fullPath().empty())
  {
    throw cmsException(__func__, __LINE__)
      << "Cannot find file = " << fileName
    ;
  }
  return inputFile.fullPath();
}

bool
isfailsLowMassVeto(const std::vector<const RecoLepton *> & preselLeptons)
{
  bool failsLowMassVeto = false;
  for(auto lepton1_it = preselLeptons.begin(); lepton1_it != preselLeptons.end(); ++lepton1_it)
  {
    const RecoLepton * lepton1 = *lepton1_it;
    for(auto lepton2_it = lepton1_it + 1; lepton2_it != preselLeptons.end(); ++lepton2_it)
    {
      const RecoLepton * lepton2 = *lepton2_it;
      const double mass = (lepton1->p4() + lepton2->p4()).mass();
      if(mass < 12.)
      {
        failsLowMassVeto = true;
        break;
      }
    }
  }
  return failsLowMassVeto;
}

bool
isfailsZbosonMassVeto(const std::vector<const RecoLepton *> & preselLeptons)
{
  bool failsZbosonMassVeto = false;
  for(auto lepton1_it = preselLeptons.begin(); lepton1_it != preselLeptons.end(); ++lepton1_it)
  {
    const RecoLepton * lepton1 = *lepton1_it;
    for(auto lepton2_it = lepton1_it + 1; lepton2_it != preselLeptons.end(); ++lepton2_it)
    {
      const RecoLepton * lepton2 = *lepton2_it;
      if(lepton1->pdgId() == -lepton2->pdgId())
      {
        // pair of same flavor leptons of opposite charge
        const double mass = (lepton1->p4() + lepton2->p4()).mass();
        if(std::fabs(mass - z_mass) < z_window )
        {
          failsZbosonMassVeto = true;
          break;
        }
      }
    }
  }
  return failsZbosonMassVeto;
}

bool
isfailsHtoZZVeto(const std::vector<const RecoLepton *> & preselLeptons)
{
  bool failsHtoZZVeto = false;
  for(auto lepton1_it = preselLeptons.begin(); lepton1_it != preselLeptons.end(); ++lepton1_it)
  {
    const RecoLepton * lepton1 = *lepton1_it;
    for(auto lepton2_it = lepton1_it + 1; lepton2_it != preselLeptons.end(); ++lepton2_it)
    {
      const RecoLepton * lepton2 = *lepton2_it;
      if(lepton1->pdgId() == -lepton2->pdgId())
      {
        // first pair of same flavor leptons of opposite charge
        for(auto lepton3_it = preselLeptons.begin(); lepton3_it != preselLeptons.end(); ++lepton3_it)
        {
          const RecoLepton * lepton3 = *lepton3_it;
          if(lepton3 == lepton1 || lepton3 == lepton2)
          {
            continue;
          }
          for(auto lepton4_it = lepton3_it + 1; lepton4_it != preselLeptons.end(); ++lepton4_it)
          {
            const RecoLepton * lepton4 = *lepton4_it;
            if(lepton4 == lepton1 || lepton4 == lepton2)
            {
              continue;
            }

            if(lepton3->pdgId() == -lepton4->pdgId())
            {
              // second pair of same flavor leptons of opposite charge
              const double mass = (lepton1->p4() + lepton2->p4() + lepton3->p4() + lepton4->p4()).mass();
              if(mass < 140.)
              {
                failsHtoZZVeto = true;
                break;
              }
            }
          }
        }
      }
    }
  }
  return failsHtoZZVeto;
}
