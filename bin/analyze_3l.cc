#include "FWCore/ParameterSet/interface/ParameterSet.h" // edm::ParameterSet
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h" // edm::readPSetsFrom()
#include "FWCore/Utilities/interface/Exception.h" // cms::Exception
#include "PhysicsTools/FWLite/interface/TFileService.h" // fwlite::TFileService
#include "DataFormats/FWLite/interface/InputSource.h" // fwlite::InputSource
#include "DataFormats/FWLite/interface/OutputFiles.h" // fwlite::OutputFiles
#include "DataFormats/Math/interface/LorentzVector.h" // math::PtEtaPhiMLorentzVector
#include "DataFormats/Math/interface/deltaR.h" // deltaR

#include <TBenchmark.h> // TBenchmark
#include <TString.h> // TString, Form
#include <TError.h> // gErrorAbortLevel, kError
#include <TMatrixD.h> // TMatrixD
#include <TMath.h> // TMath::
#include <TH2.h> // TH2

#include "tthAnalysis/HiggsToTauTau/interface/RecoLepton.h" // RecoLepton
#include "tthAnalysis/HiggsToTauTau/interface/RecoJet.h" // RecoJet
#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTau.h" // RecoHadTau
#include "tthAnalysis/HiggsToTauTau/interface/GenLepton.h" // GenLepton
#include "tthAnalysis/HiggsToTauTau/interface/GenJet.h" // GenJet
#include "tthAnalysis/HiggsToTauTau/interface/GenHadTau.h" // GenHadTau
#include "tthAnalysis/HiggsToTauTau/interface/RecoMEt.h" // RecoMEt
#include "tthAnalysis/HiggsToTauTau/interface/EventInfo.h" // EventInfo
#include "tthAnalysis/HiggsToTauTau/interface/TMVAInterface.h" // TMVAInterface
#include "tthAnalysis/HiggsToTauTau/interface/XGBInterface.h" // XGBInterface
#include "tthAnalysis/HiggsToTauTau/interface/mvaAuxFunctions.h" // check_mvaInputs, get_mvaInputVariables
#include "tthAnalysis/HiggsToTauTau/interface/mvaInputVariables.h" // auxiliary functions for computing input variables of the MVA used for signal extraction in the 3l category
#include "tthAnalysis/HiggsToTauTau/interface/LeptonFakeRateInterface.h" // LeptonFakeRateInterface
#include "tthAnalysis/HiggsToTauTau/interface/JetToTauFakeRateInterface.h" // JetToTauFakeRateInterface
#include "tthAnalysis/HiggsToTauTau/interface/RecoElectronReader.h" // RecoElectronReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonReader.h" // RecoMuonReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauReader.h" // RecoHadTauReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReader.h" // RecoJetReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoMEtReader.h" // RecoMEtReader
#include "tthAnalysis/HiggsToTauTau/interface/MEtFilterReader.h" // MEtFilterReader
#include "tthAnalysis/HiggsToTauTau/interface/GenLeptonReader.h" // GenLeptonReader
#include "tthAnalysis/HiggsToTauTau/interface/GenParticleReader.h" // GenParticleReader
#include "tthAnalysis/HiggsToTauTau/interface/GenHadTauReader.h" // GenHadTauReader
#include "tthAnalysis/HiggsToTauTau/interface/GenPhotonReader.h" // GenPhotonReader
#include "tthAnalysis/HiggsToTauTau/interface/GenJetReader.h" // GenJetReader
#include "tthAnalysis/HiggsToTauTau/interface/LHEInfoReader.h" // LHEInfoReader
#include "tthAnalysis/HiggsToTauTau/interface/EventInfoReader.h" // EventInfoReader
#include "tthAnalysis/HiggsToTauTau/interface/convert_to_ptrs.h" // convert_to_ptrs
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionCleaner.h" // RecoElectronCollectionCleaner, RecoMuonCollectionCleaner, RecoHadTauCollectionCleaner, RecoJetCollectionCleaner
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionGenMatcher.h" // RecoElectronCollectionGenMatcher, RecoMuonCollectionGenMatcher, RecoHadTauCollectionGenMatcher, RecoJetCollectionGenMatcher
#include "tthAnalysis/HiggsToTauTau/interface/RecoElectronCollectionSelectorLoose.h" // RecoElectronCollectionSelectorLoose
#include "tthAnalysis/HiggsToTauTau/interface/RecoElectronCollectionSelectorFakeable.h" // RecoElectronCollectionSelectorFakeable
#include "tthAnalysis/HiggsToTauTau/interface/RecoElectronCollectionSelectorTight.h" // RecoElectronCollectionSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorLoose.h" // RecoMuonCollectionSelectorLoose
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorFakeable.h" // RecoMuonCollectionSelectorFakeable
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorTight.h" // RecoMuonCollectionSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauCollectionSelectorLoose.h" // RecoHadTauCollectionSelectorLoose
#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauCollectionSelectorFakeable.h" // RecoHadTauCollectionSelectorFakeable
#include "tthAnalysis/HiggsToTauTau/interface/RecoHadTauCollectionSelectorTight.h" // RecoHadTauCollectionSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelector.h" // RecoJetCollectionSelector
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelectorBtag.h" // RecoJetCollectionSelectorBtagLoose, RecoJetCollectionSelectorBtagMedium
#include "tthAnalysis/HiggsToTauTau/interface/RunLumiEventSelector.h" // RunLumiEventSelector
#include "tthAnalysis/HiggsToTauTau/interface/MEtFilterSelector.h" // MEtFilterSelector
#include "tthAnalysis/HiggsToTauTau/interface/ElectronHistManager.h" // ElectronHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MuonHistManager.h" // MuonHistManager
#include "tthAnalysis/HiggsToTauTau/interface/HadTauHistManager.h" // HadTauHistManager
#include "tthAnalysis/HiggsToTauTau/interface/JetHistManager.h" // JetHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MEtHistManager.h" // MEtHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MEtFilterHistManager.h" // MEtFilterHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MVAInputVarHistManager.h" // MVAInputVarHistManager
#include "tthAnalysis/HiggsToTauTau/interface/EvtHistManager_3l.h" // EvtHistManager_3l
#include "tthAnalysis/HiggsToTauTau/interface/EvtYieldHistManager.h" // EvtYieldHistManager
#include "tthAnalysis/HiggsToTauTau/interface/CutFlowTableHistManager.h" // CutFlowTableHistManager
#include "tthAnalysis/HiggsToTauTau/interface/WeightHistManager.h" // WeightHistManager
#include "tthAnalysis/HiggsToTauTau/interface/GenEvtHistManager.h" // GenEvtHistManager
#include "tthAnalysis/HiggsToTauTau/interface/LHEInfoHistManager.h" // LHEInfoHistManager
#include "tthAnalysis/HiggsToTauTau/interface/leptonTypes.h" // getLeptonType, kElectron, kMuon
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h" // getBTagWeight_option, getHadTau_genPdgId, isHigherPt, isMatched
#include "tthAnalysis/HiggsToTauTau/interface/leptonGenMatchingAuxFunctions.h" // getLeptonGenMatch_definitions_3lepton, getLeptonGenMatch_string, getLeptonGenMatch_int
#include "tthAnalysis/HiggsToTauTau/interface/fakeBackgroundAuxFunctions.h" // getWeight_3L
#include "tthAnalysis/HiggsToTauTau/interface/hltPath.h" // hltPath, create_hltPaths, hltPaths_isTriggered, hltPaths_delete
#include "tthAnalysis/HiggsToTauTau/interface/hltPathReader.h" // hltPathReader
#include "tthAnalysis/HiggsToTauTau/interface/Data_to_MC_CorrectionInterface_2016.h"
#include "tthAnalysis/HiggsToTauTau/interface/Data_to_MC_CorrectionInterface_2017.h"
#include "tthAnalysis/HiggsToTauTau/interface/Data_to_MC_CorrectionInterface_2018.h"
#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // loadTH2, get_sf_from_TH2
#include "tthAnalysis/HiggsToTauTau/interface/cutFlowTable.h" // cutFlowTableType
#include "tthAnalysis/HiggsToTauTau/interface/NtupleFillerBDT.h" // NtupleFillerBDT
#include "tthAnalysis/HiggsToTauTau/interface/TTreeWrapper.h" // TTreeWrapper
#include "tthAnalysis/HiggsToTauTau/interface/SyncNtupleManager.h" // SyncNtupleManager
#include "tthAnalysis/HiggsToTauTau/interface/hltFilter.h" // hltFilter()
#include "tthAnalysis/HiggsToTauTau/interface/EvtWeightManager.h" // EvtWeightManager

#include "tthAnalysis/HiggsToTauTau/interface/HadTopTagger.h" // HadTopTagger
#include "tthAnalysis/HiggsToTauTau/interface/HadTopTagger_boosted.h" // HadTopTagger_boosted
#include "tthAnalysis/HiggsToTauTau/interface/HadTopTagger_semi_boosted.h" // HadTopTagger_semi_boosted
#include "tthAnalysis/HiggsToTauTau/interface/HadTopTagger_semi_boosted_AK8.h" // HadTopTagger_semi_boosted
#include "tthAnalysis/HiggsToTauTau/interface/hadTopTaggerAuxFunctions.h" // isGenMatchedJetTriplet
#include "tthAnalysis/HiggsToTauTau/interface/hadTopTaggerAuxFunctions_geral.h" // isGenMatchedJetTriplet tags
#include "tthAnalysis/HiggsToTauTau/interface/HadTopKinFit.h" // HadTopKinFit
#include "tthAnalysis/HiggsToTauTau/interface/mvaAuxFunctions_Hj_and_Hjj_taggers.h" // comp_mvaOutput_Hj_tagger, comp_mvaOutput_Hjj_tagger

#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelectorHTTv2.h" // RecoJetSelectorHTTv2
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetHTTv2.h"
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReaderHTTv2.h" // RecoJetReaderHTTv2
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReaderAK12.h" // RecoJetReaderAK12
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReaderAK8.h" // RecoJetReaderAK8

#include "tthAnalysis/HiggsToTauTau/interface/JetHistManagerHTTv2.h" // JetHistManagerHTTv2
#include "tthAnalysis/HiggsToTauTau/interface/JetHistManagerAK12.h" // JetHistManagerAK12

#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelectorAK12.h" // RecoJetSelectorAK12
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelectorAK8.h" // RecoJetSelectorAK8
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionCleanerSubJets.h" // RecoJetCollectionCleanerAK8SubJets
#include <boost/math/special_functions/sign.hpp> // boost::math::sign()

#include <iostream> // std::cerr, std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <string> // std::string
#include <vector> // std::vector<>
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <fstream> // std::ofstream
#include <assert.h> // assert
#include <array> // std::array<>
#include <tuple> // std::tuple<>, std::get<>(), std::make_tuple()

typedef math::PtEtaPhiMLorentzVector LV;
typedef std::vector<std::string> vstring;

enum { kFR_disabled, kFR_3lepton };

//const int hadTauSelection_antiElectron = 1; // vLoose
//const int hadTauSelection_antiMuon = 1; // Loose
const int hadTauSelection_antiElectron = -1; // not applied
const int hadTauSelection_antiMuon = -1; // not applied

/**
 * @brief Produce datacard and control plots for 3l categories.
 */
int main(int argc, char* argv[])
{
//--- throw an exception in case ROOT encounters an error
  gErrorAbortLevel = kError;

//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "<analyze_3l>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("analyze_3l");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") )
    throw cms::Exception("analyze_3l")
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfg_analyze = cfg.getParameter<edm::ParameterSet>("analyze_3l");

  std::string treeName = cfg_analyze.getParameter<std::string>("treeName");

  std::string process_string = cfg_analyze.getParameter<std::string>("process");
  bool isSignal = ( process_string == "signal" ) ? true : false;

  std::string histogramDir = cfg_analyze.getParameter<std::string>("histogramDir");
  bool isMCClosure_e = histogramDir.find("mcClosure_e") != std::string::npos;
  bool isMCClosure_m = histogramDir.find("mcClosure_m") != std::string::npos;

  std::string era_string = cfg_analyze.getParameter<std::string>("era");
  const int era = get_era(era_string);

  // single lepton triggers
  vstring triggerNames_1e = cfg_analyze.getParameter<vstring>("triggers_1e");
  std::vector<hltPath*> triggers_1e = create_hltPaths(triggerNames_1e, "triggers_1e");
  bool use_triggers_1e = cfg_analyze.getParameter<bool>("use_triggers_1e");
  vstring triggerNames_1mu = cfg_analyze.getParameter<vstring>("triggers_1mu");
  std::vector<hltPath*> triggers_1mu = create_hltPaths(triggerNames_1mu, "triggers_1mu");
  bool use_triggers_1mu = cfg_analyze.getParameter<bool>("use_triggers_1mu");
  // double lepton triggers
  vstring triggerNames_2e = cfg_analyze.getParameter<vstring>("triggers_2e");
  std::vector<hltPath*> triggers_2e = create_hltPaths(triggerNames_2e, "triggers_2e");
  bool use_triggers_2e = cfg_analyze.getParameter<bool>("use_triggers_2e");
  vstring triggerNames_1e1mu = cfg_analyze.getParameter<vstring>("triggers_1e1mu");
  std::vector<hltPath*> triggers_1e1mu = create_hltPaths(triggerNames_1e1mu, "triggers_1e1mu");
  bool use_triggers_1e1mu = cfg_analyze.getParameter<bool>("use_triggers_1e1mu");
  vstring triggerNames_2mu = cfg_analyze.getParameter<vstring>("triggers_2mu");
  std::vector<hltPath*> triggers_2mu = create_hltPaths(triggerNames_2mu, "triggers_2mu");
  bool use_triggers_2mu = cfg_analyze.getParameter<bool>("use_triggers_2mu");
  // triple lepton triggers
  vstring triggerNames_3e = cfg_analyze.getParameter<vstring>("triggers_3e");
  std::vector<hltPath*> triggers_3e = create_hltPaths(triggerNames_3e, "triggers_3e");
  bool use_triggers_3e = cfg_analyze.getParameter<bool>("use_triggers_3e");
  vstring triggerNames_2e1mu = cfg_analyze.getParameter<vstring>("triggers_2e1mu");
  std::vector<hltPath*> triggers_2e1mu = create_hltPaths(triggerNames_2e1mu, "triggers_2e1mu");
  bool use_triggers_2e1mu = cfg_analyze.getParameter<bool>("use_triggers_2e1mu");
  vstring triggerNames_1e2mu = cfg_analyze.getParameter<vstring>("triggers_1e2mu");
  std::vector<hltPath*> triggers_1e2mu = create_hltPaths(triggerNames_1e2mu, "triggers_1e2mu");
  bool use_triggers_1e2mu = cfg_analyze.getParameter<bool>("use_triggers_1e2mu");
  vstring triggerNames_3mu = cfg_analyze.getParameter<vstring>("triggers_3mu");
  std::vector<hltPath*> triggers_3mu = create_hltPaths(triggerNames_3mu, "triggers_3mu");
  bool use_triggers_3mu = cfg_analyze.getParameter<bool>("use_triggers_3mu");

  bool apply_offline_e_trigger_cuts_1e = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_1e");
  bool apply_offline_e_trigger_cuts_1mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_1mu");
  bool apply_offline_e_trigger_cuts_2e = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_2e");
  bool apply_offline_e_trigger_cuts_1e1mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_1e1mu");
  bool apply_offline_e_trigger_cuts_2mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_2mu");
  bool apply_offline_e_trigger_cuts_3e = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_3e");
  bool apply_offline_e_trigger_cuts_2e1mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_2e1mu");
  bool apply_offline_e_trigger_cuts_1e2mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_1e2mu");
  bool apply_offline_e_trigger_cuts_3mu = cfg_analyze.getParameter<bool>("apply_offline_e_trigger_cuts_3mu");

  const std::string electronSelection_string = cfg_analyze.getParameter<std::string>("electronSelection");
  const std::string muonSelection_string     = cfg_analyze.getParameter<std::string>("muonSelection");
  std::cout << "electronSelection_string = " << electronSelection_string << "\n"
               "muonSelection_string     = " << muonSelection_string     << '\n'
  ;
  const int electronSelection = get_selection(electronSelection_string);
  const int muonSelection     = get_selection(muonSelection_string);

  bool apply_leptonGenMatching = cfg_analyze.getParameter<bool>("apply_leptonGenMatching");
  std::vector<leptonGenMatchEntry> leptonGenMatch_definitions = getLeptonGenMatch_definitions_3lepton(apply_leptonGenMatching);
  std::cout << "leptonGenMatch_definitions:" << std::endl;
  std::cout << leptonGenMatch_definitions;

  TString hadTauSelection_string = cfg_analyze.getParameter<std::string>("hadTauSelection").data();
  TObjArray* hadTauSelection_parts = hadTauSelection_string.Tokenize("|");
  assert(hadTauSelection_parts->GetEntries() >= 1);
  std::string hadTauSelection_part2 = ( hadTauSelection_parts->GetEntries() == 2 ) ? (dynamic_cast<TObjString*>(hadTauSelection_parts->At(1)))->GetString().Data() : "";
  delete hadTauSelection_parts;

  enum { kOS, kSS };
  std::string chargeSumSelection_string = cfg_analyze.getParameter<std::string>("chargeSumSelection");
  int chargeSumSelection = -1;
  if      ( chargeSumSelection_string == "OS" ) chargeSumSelection = kOS;
  else if ( chargeSumSelection_string == "SS" ) chargeSumSelection = kSS;
  else throw cms::Exception("analyze_3l")
    << "Invalid Configuration parameter 'chargeSumSelection' = " << chargeSumSelection_string << " !!\n";

  int minNumJets = cfg_analyze.getParameter<int>("minNumJets");
  std::cout << "minNumJets = " << minNumJets << std::endl;

  bool isMC = cfg_analyze.getParameter<bool>("isMC");
  bool isMC_tH = ( process_string == "tHq" || process_string == "tHW" ) ? true : false;
  bool hasLHE = cfg_analyze.getParameter<bool>("hasLHE");
  std::string central_or_shift = cfg_analyze.getParameter<std::string>("central_or_shift");
  double lumiScale = ( process_string != "data_obs" ) ? cfg_analyze.getParameter<double>("lumiScale") : 1.;
  bool apply_genWeight = cfg_analyze.getParameter<bool>("apply_genWeight");
  bool apply_hlt_filter = cfg_analyze.getParameter<bool>("apply_hlt_filter");
  bool apply_met_filters = cfg_analyze.getParameter<bool>("apply_met_filters");
  edm::ParameterSet cfgMEtFilter = cfg_analyze.getParameter<edm::ParameterSet>("cfgMEtFilter");
  MEtFilterSelector metFilterSelector(cfgMEtFilter, isMC);
  const bool useNonNominal = cfg_analyze.getParameter<bool>("useNonNominal");
  const bool useNonNominal_jetmet = useNonNominal || ! isMC;

  const edm::ParameterSet syncNtuple_cfg = cfg_analyze.getParameter<edm::ParameterSet>("syncNtuple");
  const std::string syncNtuple_tree = syncNtuple_cfg.getParameter<std::string>("tree");
  const std::string syncNtuple_output = syncNtuple_cfg.getParameter<std::string>("output");
  const bool sync_requireGenMatching = syncNtuple_cfg.getParameter<bool>("requireGenMatching");
  const bool do_sync = ! syncNtuple_tree.empty() && ! syncNtuple_output.empty();

  const edm::ParameterSet additionalEvtWeight = cfg_analyze.getParameter<edm::ParameterSet>("evtWeight");
  const bool applyAdditionalEvtWeight = additionalEvtWeight.getParameter<bool>("apply");
  EvtWeightManager * eventWeightManager = nullptr;
  if(applyAdditionalEvtWeight)
  {
    eventWeightManager = new EvtWeightManager(additionalEvtWeight);
  }

  bool isDEBUG = cfg_analyze.getParameter<bool>("isDEBUG");
  if ( isDEBUG ) std::cout << "Warning: DEBUG mode enabled -> trigger selection will not be applied for data !!" << std::endl;

  checkOptionValidity(central_or_shift, isMC);
  const int jetToLeptonFakeRate_option = getJetToLeptonFR_option(central_or_shift);
  const int hadTauPt_option            = getHadTauPt_option     (central_or_shift);
  const int lheScale_option            = getLHEscale_option     (central_or_shift);
  const int jetBtagSF_option           = getBTagWeight_option   (central_or_shift);
  const PUsys puSys_option             = getPUsys_option        (central_or_shift);

  const int met_option   = useNonNominal_jetmet ? kMEt_central_nonNominal : getMET_option(central_or_shift, isMC);
  const int jetPt_option = useNonNominal_jetmet ? kJet_central_nonNominal : getJet_option(central_or_shift, isMC);

  std::cout
    << "central_or_shift = "               << central_or_shift           << "\n"
       " -> jetToLeptonFakeRate_option = " << jetToLeptonFakeRate_option << "\n"
       " -> hadTauPt_option            = " << hadTauPt_option            << "\n"
       " -> lheScale_option            = " << lheScale_option            << "\n"
       " -> jetBtagSF_option           = " << jetBtagSF_option           << "\n"
       " -> met_option                 = " << met_option                 << "\n"
       " -> jetPt_option               = " << jetPt_option               << '\n'
  ;

  edm::ParameterSet cfg_dataToMCcorrectionInterface;
  cfg_dataToMCcorrectionInterface.addParameter<std::string>("era", era_string);
  cfg_dataToMCcorrectionInterface.addParameter<std::string>("hadTauSelection", hadTauSelection_part2);
  cfg_dataToMCcorrectionInterface.addParameter<int>("hadTauSelection_antiElectron", hadTauSelection_antiElectron);
  cfg_dataToMCcorrectionInterface.addParameter<int>("hadTauSelection_antiMuon", hadTauSelection_antiMuon);
  cfg_dataToMCcorrectionInterface.addParameter<std::string>("central_or_shift", central_or_shift);
  Data_to_MC_CorrectionInterface_Base * dataToMCcorrectionInterface = nullptr;
  switch(era)
  {
    case kEra_2016: dataToMCcorrectionInterface = new Data_to_MC_CorrectionInterface_2016(cfg_dataToMCcorrectionInterface); break;
    case kEra_2017: dataToMCcorrectionInterface = new Data_to_MC_CorrectionInterface_2017(cfg_dataToMCcorrectionInterface); break;
    case kEra_2018: dataToMCcorrectionInterface = new Data_to_MC_CorrectionInterface_2018(cfg_dataToMCcorrectionInterface); break;
    default: throw cmsException("analyze_3l", __LINE__) << "Invalid era = " << era;
  }

  std::string applyFakeRateWeights_string = cfg_analyze.getParameter<std::string>("applyFakeRateWeights");
  int applyFakeRateWeights = -1;
  if      ( applyFakeRateWeights_string == "disabled" ) applyFakeRateWeights = kFR_disabled;
  else if ( applyFakeRateWeights_string == "3lepton"  ) applyFakeRateWeights = kFR_3lepton;
  else throw cms::Exception("analyze_3l")
    << "Invalid Configuration parameter 'applyFakeRateWeights' = " << applyFakeRateWeights_string << " !!\n";

  bool selectBDT = ( cfg_analyze.exists("selectBDT") ) ? cfg_analyze.getParameter<bool>("selectBDT") : false;

  LeptonFakeRateInterface* leptonFakeRateInterface = 0;
  if ( applyFakeRateWeights == kFR_3lepton || selectBDT ) {
    edm::ParameterSet cfg_leptonFakeRateWeight = cfg_analyze.getParameter<edm::ParameterSet>("leptonFakeRateWeight");
    leptonFakeRateInterface = new LeptonFakeRateInterface(cfg_leptonFakeRateWeight, jetToLeptonFakeRate_option);
  }

  bool fillGenEvtHistograms = cfg_analyze.getParameter<bool>("fillGenEvtHistograms");
  edm::ParameterSet cfg_EvtYieldHistManager = cfg_analyze.getParameter<edm::ParameterSet>("cfgEvtYieldHistManager");

  std::string branchName_electrons = cfg_analyze.getParameter<std::string>("branchName_electrons");
  std::string branchName_muons = cfg_analyze.getParameter<std::string>("branchName_muons");
  std::string branchName_hadTaus = cfg_analyze.getParameter<std::string>("branchName_hadTaus");
  std::string branchName_jets = cfg_analyze.getParameter<std::string>("branchName_jets");
  std::string branchName_jetsHTTv2 = cfg_analyze.getParameter<std::string>("branchName_jetsHTTv2");
  std::string branchName_subjetsHTTv2 = cfg_analyze.getParameter<std::string>("branchName_subjetsHTTv2");
  //std::string branchName_jetsAK12 = cfg_analyze.getParameter<std::string>("branchName_jetsAK12");
  //std::string branchName_subjetsAK12 = cfg_analyze.getParameter<std::string>("branchName_subjetsAK12");
  std::string branchName_jetsAK8 = cfg_analyze.getParameter<std::string>("branchName_jetsAK8");
  std::string branchName_subjetsAK8 = cfg_analyze.getParameter<std::string>("branchName_subjetsAK8");
  std::string branchName_met = cfg_analyze.getParameter<std::string>("branchName_met");

  std::string branchName_genLeptons = cfg_analyze.getParameter<std::string>("branchName_genLeptons");
  std::string branchName_genHadTaus = cfg_analyze.getParameter<std::string>("branchName_genHadTaus");
  std::string branchName_genPhotons = cfg_analyze.getParameter<std::string>("branchName_genPhotons");
  std::string branchName_genJets = cfg_analyze.getParameter<std::string>("branchName_genJets");


  std::string branchName_genTopQuarks = cfg_analyze.getParameter<std::string>("branchName_genTopQuarks");
  std::string branchName_genBJets = cfg_analyze.getParameter<std::string>("branchName_genBJets");
  std::string branchName_genWBosons = cfg_analyze.getParameter<std::string>("branchName_genWBosons");
  std::string branchName_genWJets = cfg_analyze.getParameter<std::string>("branchName_genWJets");
  std::string branchName_genQuarkFromTop = cfg_analyze.getParameter<std::string>("branchName_genQuarkFromTop");

  const bool redoGenMatching = cfg_analyze.getParameter<bool>("redoGenMatching");

  std::string selEventsFileName_input = cfg_analyze.getParameter<std::string>("selEventsFileName_input");
  std::cout << "selEventsFileName_input = " << selEventsFileName_input << std::endl;
  RunLumiEventSelector* run_lumi_eventSelector = 0;
  if ( selEventsFileName_input != "" ) {
    edm::ParameterSet cfgRunLumiEventSelector;
    cfgRunLumiEventSelector.addParameter<std::string>("inputFileName", selEventsFileName_input);
    cfgRunLumiEventSelector.addParameter<std::string>("separator", ":");
    run_lumi_eventSelector = new RunLumiEventSelector(cfgRunLumiEventSelector);
  }

  std::string selEventsFileName_output = cfg_analyze.getParameter<std::string>("selEventsFileName_output");
  std::cout << "selEventsFileName_output = " << selEventsFileName_output << std::endl;

  fwlite::InputSource inputFiles(cfg);
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  TTreeWrapper * inputTree = new TTreeWrapper(treeName.data(), inputFiles.files(), maxEvents);

  std::cout << "Loaded " << inputTree -> getFileCount() << " file(s).\n";

//--- prepare sync Ntuple
  SyncNtupleManager * snm = nullptr;
  if(do_sync)
  {
    snm = new SyncNtupleManager(syncNtuple_output, syncNtuple_tree);
    snm->initializeBranches();
    snm->initializeHLTBranches({
      triggers_1e, triggers_1mu, triggers_2e, triggers_1e1mu, triggers_2mu,
      triggers_3e, triggers_2e1mu, triggers_1e2mu, triggers_3mu
    });
  }

//--- declare event-level variables
  EventInfo eventInfo(isSignal, isMC, isMC_tH);
  EventInfoReader eventInfoReader(&eventInfo, puSys_option);
  inputTree -> registerReader(&eventInfoReader);

  hltPathReader hltPathReader_instance({ triggers_1e, triggers_1mu, triggers_2e, triggers_1e1mu, triggers_2mu, triggers_3e, triggers_2e1mu, triggers_1e2mu, triggers_3mu });
  inputTree -> registerReader(&hltPathReader_instance);

  if(eventWeightManager)
  {
    inputTree->registerReader(eventWeightManager);
  }

//--- declare particle collections
  const bool readGenObjects = isMC && !redoGenMatching;
  RecoMuonReader* muonReader = new RecoMuonReader(era, branchName_muons, readGenObjects);
  inputTree -> registerReader(muonReader);
  RecoMuonCollectionGenMatcher muonGenMatcher;
  RecoMuonCollectionSelectorLoose preselMuonSelector(era, -1, isDEBUG);
  RecoMuonCollectionSelectorFakeable fakeableMuonSelector(era, -1, isDEBUG);
  RecoMuonCollectionSelectorTight tightMuonSelector(era, -1, isDEBUG);

  RecoElectronReader* electronReader = new RecoElectronReader(era, branchName_electrons, readGenObjects);
  //electronReader->readUncorrected(useNonNominal);
  inputTree -> registerReader(electronReader);
  RecoElectronCollectionGenMatcher electronGenMatcher;
  RecoElectronCollectionCleaner electronCleaner(0.05, isDEBUG);
  RecoElectronCollectionSelectorLoose preselElectronSelector(era, -1, isDEBUG);
  RecoElectronCollectionSelectorFakeable fakeableElectronSelector(era, -1, isDEBUG);
  RecoElectronCollectionSelectorTight tightElectronSelector(era, -1, isDEBUG);

  RecoHadTauReader* hadTauReader = new RecoHadTauReader(era, branchName_hadTaus, readGenObjects);
  hadTauReader->setHadTauPt_central_or_shift(hadTauPt_option);
  inputTree -> registerReader(hadTauReader);
  RecoHadTauCollectionGenMatcher hadTauGenMatcher;
  RecoHadTauCollectionCleaner hadTauCleaner(0.3, isDEBUG);
  RecoHadTauCollectionSelectorLoose preselHadTauSelector(era, -1, isDEBUG);
  preselHadTauSelector.set_if_looser(hadTauSelection_part2);
  preselHadTauSelector.set_min_antiElectron(hadTauSelection_antiElectron);
  preselHadTauSelector.set_min_antiMuon(hadTauSelection_antiMuon);
  RecoHadTauCollectionSelectorFakeable fakeableHadTauSelector(era, -1, isDEBUG);
  fakeableHadTauSelector.set_if_looser(hadTauSelection_part2);
  fakeableHadTauSelector.set_min_antiElectron(hadTauSelection_antiElectron);
  fakeableHadTauSelector.set_min_antiMuon(hadTauSelection_antiMuon);
  RecoHadTauCollectionSelectorTight tightHadTauSelector(era, -1, isDEBUG);
  tightHadTauSelector.set(hadTauSelection_part2);
  tightHadTauSelector.set_min_antiElectron(hadTauSelection_antiElectron);
  tightHadTauSelector.set_min_antiMuon(hadTauSelection_antiMuon);

  RecoJetReaderHTTv2* jetReaderHTTv2 = new RecoJetReaderHTTv2(era, branchName_jetsHTTv2, branchName_subjetsHTTv2);
  inputTree -> registerReader(jetReaderHTTv2);
  RecoJetCollectionSelectorHTTv2 jetSelectorHTTv2(era);
  RecoJetCollectionCleanerHTTv2SubJets jetCleanerHTTv2(0.4, isDEBUG); //jetCleanerHTTv2(0.01, isDEBUG); //to clean against leptons and hadronic taus

  /*
  RecoJetReaderAK12* jetReaderAK12 = new RecoJetReaderAK12(era, branchName_jetsAK12, branchName_subjetsAK12);
  inputTree -> registerReader(jetReaderAK12);
  RecoJetCollectionSelectorAK12 jetSelectorAK12(era);
  RecoJetCollectionCleanerAK12 jetCleanerAK12(0.01, isDEBUG); //to clean against leptons and hadronic taus
  */

  RecoJetReaderAK8* jetReaderAK8 = new RecoJetReaderAK8(era, branchName_jetsAK8, branchName_subjetsAK8);
  inputTree -> registerReader(jetReaderAK8);
  RecoJetCollectionSelectorAK8 jetSelectorAK8(era);
  RecoJetCollectionCleanerAK8SubJets jetCleanerAK8(0.4, isDEBUG); //to clean against leptons and hadronic taus

  RecoJetReader* jetReader = new RecoJetReader(era, isMC, branchName_jets, readGenObjects);
  jetReader->setPtMass_central_or_shift(jetPt_option);
  jetReader->setBranchName_BtagWeight(jetBtagSF_option);
  inputTree -> registerReader(jetReader);
  RecoJetCollectionGenMatcher jetGenMatcher;
  RecoJetCollectionCleaner jetCleaner(0.4, isDEBUG);
  RecoJetCollectionCleaner jetCleaner_large8(0.8, isDEBUG);
  RecoJetCollectionCleaner jetCleaner_large12(1.2, isDEBUG);
  RecoJetCollectionSelector jetSelector(era, -1, isDEBUG);
  RecoJetCollectionSelectorBtagLoose jetSelectorBtagLoose(era, -1, isDEBUG);
  RecoJetCollectionSelectorBtagMedium jetSelectorBtagMedium(era, -1, isDEBUG);

//--- declare missing transverse energy
  RecoMEtReader* metReader = new RecoMEtReader(era, isMC, branchName_met);
  metReader->setMEt_central_or_shift(met_option);
  inputTree -> registerReader(metReader);

  MEtFilter metFilters;
  MEtFilterReader* metFilterReader = new MEtFilterReader(&metFilters, era);
  inputTree -> registerReader(metFilterReader);

  GenLeptonReader* genLeptonReader = 0;
  GenHadTauReader* genHadTauReader = 0;
  GenPhotonReader* genPhotonReader = 0;
  GenJetReader* genJetReader = 0;
  LHEInfoReader* lheInfoReader = 0;
  if ( isMC ) {
    if ( ! readGenObjects ) {
      if ( branchName_genLeptons != "" ) {
        genLeptonReader = new GenLeptonReader(branchName_genLeptons);
        inputTree -> registerReader(genLeptonReader);
      }
      if ( branchName_genHadTaus != "" ) {
        genHadTauReader = new GenHadTauReader(branchName_genHadTaus);
        inputTree -> registerReader(genHadTauReader);
      }
      if ( branchName_genPhotons != "" ) {
        genPhotonReader = new GenPhotonReader(branchName_genPhotons);
        inputTree -> registerReader(genPhotonReader);
      }
      if ( branchName_genJets != "" ) {
        genJetReader = new GenJetReader(branchName_genJets);
        inputTree -> registerReader(genJetReader);
      }
    }
    lheInfoReader = new LHEInfoReader(hasLHE);
    inputTree -> registerReader(lheInfoReader);
  }

  GenParticleReader* genTopQuarkReader = new GenParticleReader(branchName_genTopQuarks);
  GenParticleReader* genBJetReader = new GenParticleReader(branchName_genBJets);
  GenParticleReader* genWBosonReader = new GenParticleReader(branchName_genWBosons);
  GenParticleReader* genWJetReader = new GenParticleReader(branchName_genWJets);
  GenParticleReader* genQuarkFromTopReader = new GenParticleReader(branchName_genQuarkFromTop);

  if ( isMC ) {
	  inputTree -> registerReader(genTopQuarkReader);
	  inputTree -> registerReader(genBJetReader);
	  inputTree -> registerReader(genWBosonReader);
	  inputTree -> registerReader(genWJetReader);
    inputTree -> registerReader(genQuarkFromTopReader);
  }

  //--- initialize hadronic top tagger BDT
  HadTopTagger* hadTopTagger = new HadTopTagger();
  HadTopTagger_boosted* hadTopTagger_boosted = new HadTopTagger_boosted();
  HadTopTagger_semi_boosted* hadTopTagger_semi_boosted = new HadTopTagger_semi_boosted();
  HadTopTagger_semi_boosted_AK8* hadTopTagger_semi_boosted_fromAK8 = new HadTopTagger_semi_boosted_AK8();

  std::string mvaFileName_Hj_tagger = "tthAnalysis/HiggsToTauTau/data/Hj_deepcsv_BDTG_2017.weights.xml";
  std::vector<std::string> mvaInputVariables_Hj_tagger = {
    "Jet25_lepdrmin", "max(Jet25_bDiscriminator,0.)",
    "max(Jet25_qg,0.)", "Jet25_lepdrmax", "Jet25_pt" };
  TMVAInterface mva_Hj_tagger(mvaFileName_Hj_tagger, mvaInputVariables_Hj_tagger);

  std::string mvaFileName_Hjj_tagger = "tthAnalysis/HiggsToTauTau/data/Hjj_csv_BDTG.weights.xml";
  std::vector<std::string> mvaInputVariables_Hjj_tagger;
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_minlepmass");
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_sumbdt");
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_dr");
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_minjdr");
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_mass");
  mvaInputVariables_Hjj_tagger.push_back("bdtJetPair_minjOvermaxjdr");
  TMVAInterface mva_Hjj_tagger(mvaFileName_Hjj_tagger, mvaInputVariables_Hjj_tagger);
//--- initialize BDTs used to discriminate ttH vs. ttV and ttH vs. ttbar
//    in 3l category of ttH multilepton analysis
  std::string mvaFileName_3l_ttV = "tthAnalysis/HiggsToTauTau/data/multilep_BDTs_2018/3l_ttV_BDTG.weights.xml";
  std::vector<std::string> mvaInputVariables_3l_ttV;
  mvaInputVariables_3l_ttV.push_back("max(abs(LepGood_eta[iLepFO_Recl[0]]),abs(LepGood_eta[iLepFO_Recl[1]]))");
  mvaInputVariables_3l_ttV.push_back("nJet25_Recl");
  mvaInputVariables_3l_ttV.push_back("mindr_lep1_jet");
  mvaInputVariables_3l_ttV.push_back("mindr_lep2_jet");
  mvaInputVariables_3l_ttV.push_back("MT_met_lep1");
  mvaInputVariables_3l_ttV.push_back("LepGood_conePt[iLepFO_Recl[2]]");
  mvaInputVariables_3l_ttV.push_back("LepGood_conePt[iLepFO_Recl[0]]");
  TMVAInterface mva_3l_ttV(mvaFileName_3l_ttV, mvaInputVariables_3l_ttV, { "iLepFO_Recl[0]", "iLepFO_Recl[1]", "iLepFO_Recl[2]" });

  std::string mvaFileName_3l_ttbar = "tthAnalysis/HiggsToTauTau/data/multilep_BDTs_2018/3l_ttbar_BDTG.weights.xml";
  std::vector<std::string> mvaInputVariables_3l_ttbar;
  mvaInputVariables_3l_ttbar.push_back("max(abs(LepGood_eta[iLepFO_Recl[0]]),abs(LepGood_eta[iLepFO_Recl[1]]))");
  mvaInputVariables_3l_ttbar.push_back("nJet25_Recl");
  mvaInputVariables_3l_ttbar.push_back("mindr_lep1_jet");
  mvaInputVariables_3l_ttbar.push_back("mindr_lep2_jet");
  mvaInputVariables_3l_ttbar.push_back("MT_met_lep1");
  //mvaInputVariables_3l_ttbar.push_back("mhtJet25_Recl");
  //mvaInputVariables_3l_ttbar.push_back("avg_dr_jet");
  TMVAInterface mva_3l_ttbar(mvaFileName_3l_ttbar, mvaInputVariables_3l_ttbar, { "iLepFO_Recl[0]", "iLepFO_Recl[1]", "iLepFO_Recl[2]" });
  std::map<std::string, double> mvaInputs_3l;
  //2D map for ttbar vs ttV
  const LocalFileInPath mapFileName_fip("tthAnalysis/HiggsToTauTau/data/multilep_BDTs_2018/binning_3l.root");
  TFile * fmap = TFile::Open(mapFileName_fip.fullPath().c_str(), "read");
  TH2F * hTargetBinning = static_cast<TH2F *>(fmap->Get("hTargetBinning"));


// BDT with boosted stuff
std::string mvaFileName_XGB_oldVar = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_oldVar_evtLevelSUM_TTH_9Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_oldVar = {
  "lep1_conePt", "lep2_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mT_lep1", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "Hj_tagger_fromCSVsort4th"
};
XGBInterface mva_XGB_oldVar(mvaFileName_XGB_oldVar, mvaInputVariables_XGB_oldVar);
std::map<std::string, double> mvaInputs_XGB_oldVar;

std::string mvaFileName_XGB_Updated = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_Updated_evtLevelSUM_TTH_20Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Updated = {
  "lep1_conePt", "lep2_conePt",
  "lep3_conePt", "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge"
};
XGBInterface mva_XGB_Updated(mvaFileName_XGB_Updated, mvaInputVariables_XGB_Updated);
std::map<std::string, double> mvaInputs_XGB_Updated;

std::string mvaFileName_XGB_Boosted_AK8_noISO = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_Boosted_AK8_noISO_evtLevelSUM_TTH_22Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Boosted_AK8_noISO = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss",
  "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge", "HTT_boosted", "HTT_semi_boosted_fromAK8"
};
XGBInterface mva_XGB_Boosted_AK8_noISO(mvaFileName_XGB_Boosted_AK8_noISO, mvaInputVariables_XGB_Boosted_AK8_noISO);
std::map<std::string, double> mvaInputs_XGB_Boosted_AK8_noISO;

std::string mvaFileName_XGB_Boosted_AK8 = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_Boosted_AK8_evtLevelSUM_TTH_22Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Boosted_AK8 = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge",
  "HTT_boosted", "HTT_semi_boosted_fromAK8"
};
XGBInterface mva_XGB_Boosted_AK8(mvaFileName_XGB_Boosted_AK8, mvaInputVariables_XGB_Boosted_AK8);
std::map<std::string, double> mvaInputs_XGB_Boosted_AK8;

std::string mvaFileName_XGB_Boosted_AK12_noISO = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_Boosted_AK12_noISO_evtLevelSUM_TTH_22Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Boosted_AK12_noISO = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge",
  "minDR_AK12_lep", "HTT_semi_boosted", "HadTop_pt_semi_boosted"
};
XGBInterface mva_XGB_Boosted_AK12_noISO(mvaFileName_XGB_Boosted_AK12_noISO, mvaInputVariables_XGB_Boosted_AK12_noISO);
std::map<std::string, double> mvaInputs_XGB_Boosted_AK12_noISO;

std::string mvaFileName_XGB_Boosted_AK12 = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_Boosted_AK12_evtLevelSUM_TTH_22Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Boosted_AK12 = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge",
  "HTT_semi_boosted", "HTT_boosted"
};
XGBInterface mva_XGB_Boosted_AK12(mvaFileName_XGB_Boosted_AK12, mvaInputVariables_XGB_Boosted_AK12);
std::map<std::string, double> mvaInputs_XGB_Boosted_AK12;

std::string mvaFileName_XGB_AK12 = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_AK12_evtLevelSUM_TTH_22Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_AK12 = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge",
  "resolved_and_semi", "HTT_semi_boosted"
};
XGBInterface mva_XGB_AK12(mvaFileName_XGB_AK12, mvaInputVariables_XGB_AK12);
std::map<std::string, double> mvaInputs_XGB_AK12;

std::string mvaFileName_XGB_Boosted_AK12_basic = "tthAnalysis/HiggsToTauTau/data/evtLevel_2018March/3l_0tau/3l_0tau_XGB_AK12_basic_evtLevelSUM_TTH_21Var.pkl";
std::vector<std::string> mvaInputVariables_XGB_Boosted_AK12_basic = {
  "lep1_conePt", "lep2_conePt", "lep3_conePt",
  "mindr_lep1_jet", "mindr_lep2_jet", "mindr_lep3_jet",
  "mT_lep1", "mT_lep2", "mT_lep3", "max_lep_eta",
  "nJet", "res-HTT_CSVsort4rd", "HadTop_pt_CSVsort4rd", "Hj_tagger_fromCSVsort4th",
  "min_lep_dr_os", "dr_lss", "nBJetMedium", "nElectron", "ptmiss", "sumLeptonCharge", "N_jetAK12"
};
XGBInterface mva_XGB_Boosted_AK12_basic(mvaFileName_XGB_Boosted_AK12_basic, mvaInputVariables_XGB_Boosted_AK12_basic);
std::map<std::string, double> mvaInputs_XGB_Boosted_AK12_basic;

std::vector<std::string> mvaInputVariables_3l = get_mvaInputVariables(mvaInputVariables_3l_ttV, mvaInputVariables_3l_ttbar);


//--- open output file containing run:lumi:event numbers of events passing final event selection criteria
  std::ostream* selEventsFile = ( selEventsFileName_output != "" ) ? new std::ofstream(selEventsFileName_output.data(), std::ios::out) : 0;

//--- declare histograms
  struct preselHistManagerType
  {
    ElectronHistManager* electrons_;
    MuonHistManager* muons_;
    HadTauHistManager* hadTaus_;
    JetHistManager* jets_;
    JetHistManager* BJets_loose_;
    JetHistManager* BJets_medium_;
    MEtHistManager* met_;
    MEtFilterHistManager* metFilters_;
    EvtHistManager_3l* evt_;
    EvtYieldHistManager* evtYield_;
  };
  std::map<int, preselHistManagerType*> preselHistManagers;
  struct selHistManagerType
  {
    ElectronHistManager* electrons_;
    std::map<std::string, ElectronHistManager*> electrons_in_categories_;
    MuonHistManager* muons_;
    std::map<std::string, MuonHistManager*> muons_in_categories_;
    HadTauHistManager* hadTaus_;
    JetHistManager* jets_;
    JetHistManager* leadJet_;
    JetHistManager* subleadJet_;
    JetHistManager* BJets_loose_;
    JetHistManager* leadBJet_loose_;
    JetHistManager* subleadBJet_loose_;
    JetHistManager* BJets_medium_;
    MEtHistManager* met_;
    MEtFilterHistManager* metFilters_;
    MVAInputVarHistManager* mvaInputVariables_3l_;
    EvtHistManager_3l* evt_;
    std::map<std::string, EvtHistManager_3l*> evt_in_categories_;
    std::map<std::string, EvtHistManager_3l*> evt_in_categoriesFat_;
    std::map<std::string, EvtHistManager_3l*> evt_in_decayModes_;
    std::map<std::string, std::map<std::string, EvtHistManager_3l*>> evt_in_categories_and_decayModes_; // key = category, decayMode
    std::map<std::string, std::map<std::string, EvtHistManager_3l*>> evt_in_categoriesFat_and_decayModes_; // key = category, decayMode
    EvtYieldHistManager* evtYield_;
    WeightHistManager* weights_;
  };
  std::map<int, selHistManagerType*> selHistManagers;

  vstring categories = {
    "3l_0tau_bl_neg", "3l_0tau_bl_pos", "3l_0tau_bt_neg", "3l_0tau_bt_pos"
  };
  vstring categoriesFat = {
    "3l_0tau_bl_neg_0J", "3l_0tau_bl_pos_0J", "3l_0tau_bt_neg_0J", "3l_0tau_bt_pos_0J", "3l_0tau_1pHTTv2", "3l_0tau_1pAK8"
  };

  for ( std::vector<leptonGenMatchEntry>::const_iterator leptonGenMatch_definition = leptonGenMatch_definitions.begin();
	leptonGenMatch_definition != leptonGenMatch_definitions.end(); ++leptonGenMatch_definition ) {

    std::string process_and_genMatch = process_string;
    if ( apply_leptonGenMatching ) process_and_genMatch += leptonGenMatch_definition->name_;

    int idxLepton = leptonGenMatch_definition->idx_;

    preselHistManagerType* preselHistManager = new preselHistManagerType();
    preselHistManager->electrons_ = new ElectronHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/electrons", histogramDir.data()), central_or_shift));
    preselHistManager->electrons_->bookHistograms(fs);
    preselHistManager->muons_ = new MuonHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/muons", histogramDir.data()), central_or_shift));
    preselHistManager->muons_->bookHistograms(fs);
    preselHistManager->hadTaus_ = new HadTauHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/hadTaus", histogramDir.data()), central_or_shift));
    preselHistManager->hadTaus_->bookHistograms(fs);
    preselHistManager->jets_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/jets", histogramDir.data()), central_or_shift));
    preselHistManager->jets_->bookHistograms(fs);
    preselHistManager->BJets_loose_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/BJets_loose", histogramDir.data()), central_or_shift));
    preselHistManager->BJets_loose_->bookHistograms(fs);
    preselHistManager->BJets_medium_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/BJets_medium", histogramDir.data()), central_or_shift));
    preselHistManager->BJets_medium_->bookHistograms(fs);
    preselHistManager->met_ = new MEtHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/met", histogramDir.data()), central_or_shift));
    preselHistManager->met_->bookHistograms(fs);
    preselHistManager->metFilters_ = new MEtFilterHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/metFilters", histogramDir.data()), central_or_shift));
    preselHistManager->metFilters_->bookHistograms(fs);
    preselHistManager->evt_ = new EvtHistManager_3l(makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/evt", histogramDir.data()), era_string, central_or_shift));
    preselHistManager->evt_->bookHistograms(fs);
    edm::ParameterSet cfg_EvtYieldHistManager_presel = makeHistManager_cfg(process_and_genMatch,
      Form("%s/presel/evtYield", histogramDir.data()), central_or_shift);
    cfg_EvtYieldHistManager_presel.addParameter<edm::ParameterSet>("runPeriods", cfg_EvtYieldHistManager);
    cfg_EvtYieldHistManager_presel.addParameter<bool>("isMC", isMC);
    preselHistManager->evtYield_ = new EvtYieldHistManager(cfg_EvtYieldHistManager_presel);
    preselHistManager->evtYield_->bookHistograms(fs);
    preselHistManagers[idxLepton] = preselHistManager;

    selHistManagerType* selHistManager = new selHistManagerType();
    selHistManager->electrons_ = new ElectronHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/electrons", histogramDir.data()), central_or_shift));
    selHistManager->electrons_->bookHistograms(fs);
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      TString histogramDir_category = histogramDir.data();
      histogramDir_category.ReplaceAll("3l",  category->data());
      selHistManager->electrons_in_categories_[*category] = new ElectronHistManager(makeHistManager_cfg(process_and_genMatch,
        Form("%s/sel/electrons", histogramDir_category.Data()), central_or_shift));
      selHistManager->electrons_in_categories_[*category]->bookHistograms(fs);
    }
    selHistManager->muons_ = new MuonHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/muons", histogramDir.data()), central_or_shift));
    selHistManager->muons_->bookHistograms(fs);
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      TString histogramDir_category = histogramDir.data();
      histogramDir_category.ReplaceAll("3l", category->data());
      selHistManager->muons_in_categories_[*category] = new MuonHistManager(makeHistManager_cfg(process_and_genMatch,
        Form("%s/sel/muons", histogramDir_category.Data()), central_or_shift));
      selHistManager->muons_in_categories_[*category]->bookHistograms(fs);
    }
    selHistManager->hadTaus_ = new HadTauHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/hadTaus", histogramDir.data()), central_or_shift));
    selHistManager->hadTaus_->bookHistograms(fs);
    selHistManager->jets_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/jets", histogramDir.data()), central_or_shift));
    selHistManager->jets_->bookHistograms(fs);
    selHistManager->leadJet_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/leadJet", histogramDir.data()), central_or_shift, 0));
    selHistManager->leadJet_->bookHistograms(fs);
    selHistManager->subleadJet_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/subleadJet", histogramDir.data()), central_or_shift, 1));
    selHistManager->subleadJet_->bookHistograms(fs);
    selHistManager->BJets_loose_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/BJets_loose", histogramDir.data()), central_or_shift));
    selHistManager->BJets_loose_->bookHistograms(fs);
    selHistManager->leadBJet_loose_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/leadBJet_loose", histogramDir.data()), central_or_shift, 0));
    selHistManager->leadBJet_loose_->bookHistograms(fs);
    selHistManager->subleadBJet_loose_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/subleadBJet_loose", histogramDir.data()), central_or_shift, 1));
    selHistManager->subleadBJet_loose_->bookHistograms(fs);
    selHistManager->BJets_medium_ = new JetHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/BJets_medium", histogramDir.data()), central_or_shift));
    selHistManager->BJets_medium_->bookHistograms(fs);
    selHistManager->met_ = new MEtHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/met", histogramDir.data()), central_or_shift));
    selHistManager->met_->bookHistograms(fs);
    selHistManager->metFilters_ = new MEtFilterHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/metFilters", histogramDir.data()), central_or_shift));
    selHistManager->metFilters_->bookHistograms(fs);
    selHistManager->mvaInputVariables_3l_ = new MVAInputVarHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/mvaInputs_3l", histogramDir.data()), central_or_shift));
    selHistManager->mvaInputVariables_3l_->bookHistograms(fs, mvaInputVariables_3l);
    selHistManager->evt_ = new EvtHistManager_3l(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/evt", histogramDir.data()), era_string, central_or_shift));
    selHistManager->evt_->bookHistograms(fs);
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      TString histogramDir_category = histogramDir.data();
      histogramDir_category.ReplaceAll("3l", category->data());
      selHistManager->evt_in_categories_[*category] = new EvtHistManager_3l(makeHistManager_cfg(process_and_genMatch,
        Form("%s/sel/evt", histogramDir_category.Data()), era_string, central_or_shift));
      selHistManager->evt_in_categories_[*category]->bookHistograms(fs);
    }
    for ( vstring::const_iterator category = categoriesFat.begin();
	  category != categoriesFat.end(); ++category ) {
      TString histogramDir_category = histogramDir.data();
      histogramDir_category.ReplaceAll("3l",  category->data());
      selHistManager->evt_in_categoriesFat_[*category] = new EvtHistManager_3l(makeHistManager_cfg(process_and_genMatch,
        Form("%s/sel/evt", histogramDir_category.Data()), era_string, central_or_shift));
      selHistManager->evt_in_categoriesFat_[*category]->bookHistograms(fs);
    }
    const vstring decayModes_evt = eventInfo.getDecayModes();
    if(isSignal)
    {
      for(const std::string & decayMode_evt: decayModes_evt)
      {
	std::string decayMode_and_genMatch = decayMode_evt;
	if ( apply_leptonGenMatching ) decayMode_and_genMatch += leptonGenMatch_definition -> name_;

	selHistManager -> evt_in_decayModes_[decayMode_evt] = new EvtHistManager_3l(makeHistManager_cfg(
          decayMode_and_genMatch,
	  Form("%s/sel/evt", histogramDir.data()),
	  era_string,
	  central_or_shift
        ));
	selHistManager -> evt_in_decayModes_[decayMode_evt] -> bookHistograms(fs);

	for ( vstring::const_iterator category = categories.begin();
	      category != categories.end(); ++category ) {
	  TString histogramDir_category = histogramDir.data();
	  histogramDir_category.ReplaceAll("3l",  category->data());
	  selHistManager -> evt_in_categories_and_decayModes_[*category][decayMode_evt] = new EvtHistManager_3l(makeHistManager_cfg(
            decayMode_and_genMatch,
	    Form("%s/sel/evt", histogramDir_category.Data()),
	    era_string,
	    central_or_shift
          ));
	  selHistManager -> evt_in_categories_and_decayModes_[*category][decayMode_evt] -> bookHistograms(fs);
	}
  for ( vstring::const_iterator category = categoriesFat.begin();
	      category != categoriesFat.end(); ++category ) {
	  TString histogramDir_category = histogramDir.data();
	  histogramDir_category.ReplaceAll("3l",  category->data());
	  selHistManager -> evt_in_categoriesFat_and_decayModes_[*category][decayMode_evt] = new EvtHistManager_3l(makeHistManager_cfg(
            decayMode_and_genMatch,
	    Form("%s/sel/evt", histogramDir_category.Data()),
	    era_string,
	    central_or_shift
          ));
	  selHistManager -> evt_in_categoriesFat_and_decayModes_[*category][decayMode_evt] -> bookHistograms(fs);
	}

      }
    }
    edm::ParameterSet cfg_EvtYieldHistManager_sel = makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/evtYield", histogramDir.data()), central_or_shift);
    cfg_EvtYieldHistManager_sel.addParameter<edm::ParameterSet>("runPeriods", cfg_EvtYieldHistManager);
    cfg_EvtYieldHistManager_sel.addParameter<bool>("isMC", isMC);
    selHistManager->evtYield_ = new EvtYieldHistManager(cfg_EvtYieldHistManager_sel);
    selHistManager->evtYield_->bookHistograms(fs);
    selHistManager->weights_ = new WeightHistManager(makeHistManager_cfg(process_and_genMatch,
      Form("%s/sel/weights", histogramDir.data()), central_or_shift));
    selHistManager->weights_->bookHistograms(fs, { "genWeight", "pileupWeight", "triggerWeight", "data_to_MC_correction", "fakeRate" });
    selHistManagers[idxLepton] = selHistManager;
  }

  GenEvtHistManager* genEvtHistManager_beforeCuts = 0;
  GenEvtHistManager* genEvtHistManager_afterCuts = 0;
  LHEInfoHistManager* lheInfoHistManager = 0;
  if ( isMC ) {
    genEvtHistManager_beforeCuts = new GenEvtHistManager(makeHistManager_cfg(process_string,
      Form("%s/unbiased/genEvt", histogramDir.data()), central_or_shift));
    genEvtHistManager_beforeCuts->bookHistograms(fs);
    genEvtHistManager_afterCuts = new GenEvtHistManager(makeHistManager_cfg(process_string,
      Form("%s/sel/genEvt", histogramDir.data()), central_or_shift));
    genEvtHistManager_afterCuts->bookHistograms(fs);
    lheInfoHistManager = new LHEInfoHistManager(makeHistManager_cfg(process_string,
      Form("%s/sel/lheInfo", histogramDir.data()), central_or_shift));
    lheInfoHistManager->bookHistograms(fs);

    if(eventWeightManager)
    {
      genEvtHistManager_beforeCuts->bookHistograms(fs, eventWeightManager);
      genEvtHistManager_afterCuts->bookHistograms(fs, eventWeightManager);
    }
  }

  NtupleFillerBDT<float, int> * bdt_filler = nullptr;
  typedef std::remove_pointer<decltype(bdt_filler)>::type::float_type float_type;
  typedef std::remove_pointer<decltype(bdt_filler)>::type::int_type   int_type;
  if ( selectBDT ) {
    bdt_filler = new std::remove_pointer<decltype(bdt_filler)>::type(
      makeHistManager_cfg(process_string, Form("%s/sel/evtntuple", histogramDir.data()), central_or_shift)
    );
    bdt_filler -> register_variable<float_type>(
      "lep1_pt", "lep1_conePt", "lep1_eta", "lep1_tth_mva", "mindr_lep1_jet", "mT_lep1",
      "lep2_pt", "lep2_conePt", "lep2_eta", "lep2_tth_mva", "mindr_lep2_jet", "mT_lep2",
      "lep3_pt", "lep3_conePt", "lep3_eta", "lep3_tth_mva", "mindr_lep3_jet", "mT_lep3",
      "avg_dr_jet", "ptmiss",  "htmiss", "dr_leps",
      "lumiScale", "genWeight", "evtWeight",
      "lep1_genLepPt", "lep2_genLepPt", "lep3_genLepPt",
      "lep1_fake_prob", "lep2_fake_prob", "lep3_fake_prob",
      "lep1_frWeight", "lep2_frWeight", "lep3_frWeight",
      "mvaOutput_3l_ttV", "mvaOutput_3l_ttbar", "mvaDiscr_3l",
      "mbb_loose", "mbb_medium",
      "dr_lss", "dr_los1", "dr_los2",
      "mvaOutput_Hj_tagger", "Hj_tagger_fromCSVsort4th",
      /// Xanda: add the other 3l BDT variables
      "minDR_HTTv2_lep",
      "minDR_AK12_lep", //"minDR_AK12_lep",
      ///
      "res-HTT", "res-HTT_IHEP",
      "res-HTT_CSVsort4rd", "res-HTT_highestCSV",
      "res-HTT_CSVsort4rd_WithKinFit", "res-HTT_highestCSV_WithKinFit",
      "HTTv2_lead_pt", "AK12_lead_pt",
      "HadTop_pt",  "genTopPt",
      "HadTop_pt_multilep",
      "HadTop_pt_CSVsort4rd", "HadTop_pt_highestCSV",
      "HadTop_pt_CSVsort4rd_WithKinFit", "HadTop_pt_highestCSV_WithKinFit",
      "HTT_semi_boosted_fromAK8", "genTopPt_semi_boosted_fromAK8", "HadTop_pt_semi_boosted_fromAK8", "minDR_AK8_lep", //"AK8_without_subjets",
      "W_pt_semi_boosted_fromAK8", "b_pt_semi_boosted_fromAK8",
      "genTopPt_multilep",
      "genTopPt_CSVsort4rd", "genTopPt_highestCSV",
      "genTopPt_CSVsort4rd_WithKinFit", "genTopPt_highestCSV_WithKinFit",
      "HTTv2_lead_pt", "AK12_lead_pt",
      ////
      "HTT_boosted", "genTopPt_boosted", "HadTop_pt_boosted",
      "HTT_boosted_WithKinFit", "genTopPt_boosted_WithKinFit", "HadTop_pt_boosted_WithKinFit",
      "HTT_semi_boosted", "genTopPt_semi_boosted", "HadTop_pt_semi_boosted",
      "HTT_semi_boosted_WithKinFit", "genTopPt_semi_boosted_WithKinFit", "HadTop_pt_semi_boosted_WithKinFit", "DR_W_b_gen_AK12", "DR_W_b_gen_AK8"
    );
    bdt_filler -> register_variable<int_type>(
      "nJet", "nBJetLoose", "nBJetMedium",
      "lep1_isTight", "lep2_isTight", "lep3_isTight",
      "nHTTv2", "nElectron", "nMuon", "sumLeptonCharge",
      "N_jetAK12", "N_jetAK8", "hadtruth_semi_boosted", "hadtruth", "hadtruth_boosted", "hadtruth_semi_boosted_fromAK8",
      ////
      "bWj1Wj2_isGenMatchedWithKinFit", "bWj1Wj2_isGenMatched_IHEP",
      "bWj1Wj2_isGenMatched_CSVsort4rd", "bWj1Wj2_isGenMatched_highestCSV",
      "bWj1Wj2_isGenMatched_CSVsort4rd_WithKinFit", "bWj1Wj2_isGenMatched_highestCSV_WithKinFit",
      /////
      "bWj1Wj2_isGenMatched_boosted",
      "bWj1Wj2_isGenMatched_boosted_WithKinFit",
      "bWj1Wj2_isGenMatched_semi_boosted", "bWj1Wj2_isGenMatched_semi_boosted_WithKinFit",
      "bWj1Wj2_isGenMatched_semi_boosted_fromAK8",
      "cleanedJets_fromAK12", "cleanedJets_fromAK8",
      "resolved_and_semi", "boosted_and_semi",
      "resolved_and_semi_AK8", "boosted_and_semi_AK8",
      "resolved_and_boosted"
    );
    bdt_filler -> bookTree(fs);
  }

  int analyzedEntries = 0;
  int selectedEntries = 0;
  double selectedEntries_weighted = 0.;
  TH1* histogram_analyzedEntries = fs.make<TH1D>("analyzedEntries", "analyzedEntries", 1, -0.5, +0.5);
  TH1* histogram_selectedEntries = fs.make<TH1D>("selectedEntries", "selectedEntries", 1, -0.5, +0.5);
  cutFlowTableType cutFlowTable;
  const edm::ParameterSet cutFlowTableCfg = makeHistManager_cfg(
    process_string, Form("%s/sel/cutFlow", histogramDir.data()), central_or_shift
  );
  const std::vector<std::string> cuts = {
    "run:ls:event selection",
    "trigger",
    ">= 3 presel leptons",
    "presel lepton trigger match",
    ">= 2 jets (1)",
    ">= 2 loose b-jets || 1 medium b-jet (1)",
    "3 sel leptons",
    "fakeable lepton trigger match",
    "HLT filter matching",
    ">= 2 jets (2)",
    ">= 2 loose b-jets || 1 medium b-jet (2)",
    "sel tau veto",
    "m(ll) > 12 GeV",
    "lead lepton pT > 25 GeV && sublead lepton pT > 15 GeV && third lepton pT > 10 GeV",
    "sel lepton charge",
    "Z-boson mass veto",
    "H->ZZ*->4l veto",
    "met LD",
    "MEt filters",
    "signal region veto",
  };
  CutFlowTableHistManager * cutFlowHistManager = new CutFlowTableHistManager(cutFlowTableCfg, cuts);
  cutFlowHistManager->bookHistograms(fs);

  while(inputTree -> hasNextEvent() && (! run_lumi_eventSelector || (run_lumi_eventSelector && ! run_lumi_eventSelector -> areWeDone())))
  {
    if(inputTree -> canReport(reportEvery))
    {
      std::cout << "processing Entry " << inputTree -> getCurrentMaxEventIdx()
                << " or " << inputTree -> getCurrentEventIdx() << " entry in #"
                << (inputTree -> getProcessedFileCount() - 1)
                << " (" << eventInfo
                << ") file (" << selectedEntries << " Entries selected)\n";
    }
    ++analyzedEntries;
    //if ( analyzedEntries > 50000 ) break;
    //std::cout << "analyzedEntries = " << analyzedEntries << std::endl;
    histogram_analyzedEntries->Fill(0.);

    if (run_lumi_eventSelector && !(*run_lumi_eventSelector)(eventInfo))
    {
      continue;
    }
    cutFlowTable.update("run:ls:event selection");
    cutFlowHistManager->fillHistograms("run:ls:event selection", lumiScale);

    if ( isDEBUG ) {
      std::cout << "event #" << inputTree -> getCurrentMaxEventIdx() << ' ' << eventInfo << '\n';
    }

    if(run_lumi_eventSelector)
    {
      std::cout << "processing Entry " << inputTree -> getCurrentMaxEventIdx() << ": " << eventInfo << '\n';
      if(inputTree -> isOpen())
      {
        std::cout << "input File = " << inputTree -> getCurrentFileName() << '\n';
      }
    }

//--- build collections of generator level particles (before any cuts are applied, to check distributions in unbiased event samples)
    std::vector<GenLepton> genLeptons;
    std::vector<GenLepton> genElectrons;
    std::vector<GenLepton> genMuons;
    std::vector<GenHadTau> genHadTaus;
    std::vector<GenPhoton> genPhotons;
    std::vector<GenJet> genJets;
    if ( isMC && fillGenEvtHistograms ) {
      if ( genLeptonReader ) {
        genLeptons = genLeptonReader->read();
        for ( std::vector<GenLepton>::const_iterator genLepton = genLeptons.begin();
              genLepton != genLeptons.end(); ++genLepton ) {
          int abs_pdgId = std::abs(genLepton->pdgId());
          if      ( abs_pdgId == 11 ) genElectrons.push_back(*genLepton);
          else if ( abs_pdgId == 13 ) genMuons.push_back(*genLepton);
        }
      }
      if ( genHadTauReader ) {
        genHadTaus = genHadTauReader->read();
      }
      if ( genPhotonReader ) {
        genPhotons = genPhotonReader->read();
      }
      if ( genJetReader ) {
        genJets = genJetReader->read();
      }
      if ( isDEBUG ) {
        printCollection("genLeptons", genLeptons);
        printCollection("genHadTaus", genHadTaus);
        printCollection("genPhotons", genPhotons);
        printCollection("genJets", genJets);
      }
    }

    //---------------------------------------------------------------------------
    // CV: functionality temporarily disabled,
    //     as all generator level information is stored in RecoLepton, RecoHadTau and RecoJet branches in case tthProdNtuple workflow is used
    //std::vector<GenParticle> genNuFromTau;
    //std::vector<GenParticle> genTau;
    //std::vector<GenParticle> genLepFromTop;
    //std::vector<GenParticle> genBQuarkFromTop;
    //std::vector<GenParticle> genNuFromTop;
    //std::vector<GenParticle> genLepFromTau;
    //std::vector<GenParticle> genTop;
    //std::vector<GenParticle> genVbosons;
    //if ( isMC && writeSelEventsFile && (era == kEra_2016 || era == kEra_2017) ) {
    //  genHadTaus = genHadTauReader->read();
    //  genNuFromTau = genNuFromTauReader->read();
    //  genTau = genTauReader->read();
    //  genLepFromTop = genLepFromTopReader->read();
    //  genBQuarkFromTop = genBQuarkFromTopReader->read();
    //  genNuFromTop = genNuFromTopReader->read();
    //  genLepFromTau = genLepFromTauReader->read();
    //  genTop = genTopReader->read();
    //  genVbosons = genVbosonsReader->read();
    //}
    //---------------------------------------------------------------------------

    double evtWeight_inclusive = 1.;
    if(isMC)
    {
      if(apply_genWeight)    evtWeight_inclusive *= boost::math::sign(eventInfo.genWeight);
      if(isMC_tH)            evtWeight_inclusive *= eventInfo.genWeight_tH;
      if(eventWeightManager) evtWeight_inclusive *= eventWeightManager->getWeight();
      lheInfoReader->read();
      evtWeight_inclusive *= lheInfoReader->getWeight_scale(lheScale_option);
      evtWeight_inclusive *= eventInfo.pileupWeight;
      evtWeight_inclusive *= lumiScale;
      genEvtHistManager_beforeCuts->fillHistograms(genElectrons, genMuons, genHadTaus, genPhotons, genJets, evtWeight_inclusive);
      if(eventWeightManager)
      {
        genEvtHistManager_beforeCuts->fillHistograms(eventWeightManager, evtWeight_inclusive);
      }
    }

    bool isTriggered_1e = hltPaths_isTriggered(triggers_1e, isDEBUG);
    bool isTriggered_1mu = hltPaths_isTriggered(triggers_1mu, isDEBUG);
    bool isTriggered_2e = hltPaths_isTriggered(triggers_2e, isDEBUG);
    bool isTriggered_1e1mu = hltPaths_isTriggered(triggers_1e1mu, isDEBUG);
    bool isTriggered_2mu = hltPaths_isTriggered(triggers_2mu, isDEBUG);
    bool isTriggered_3e = hltPaths_isTriggered(triggers_3e, isDEBUG);
    bool isTriggered_2e1mu = hltPaths_isTriggered(triggers_2e1mu, isDEBUG);
    bool isTriggered_1e2mu = hltPaths_isTriggered(triggers_1e2mu, isDEBUG);
    bool isTriggered_3mu = hltPaths_isTriggered(triggers_3mu, isDEBUG);
    if ( isDEBUG ) {
      std::cout << "isTriggered:"
		<< " 1e = " << isTriggered_1e << ","
		<< " 1mu = " << isTriggered_1mu << ","
		<< " 2e = " << isTriggered_2e << ","
		<< " 1e1mu = " << isTriggered_1e1mu << ","
		<< " 2mu = " << isTriggered_2mu << ","
		<< " 3e = " << isTriggered_3e << ","
		<< " 2e1mu = " << isTriggered_2e1mu << ","
		<< " 1e2mu = " << isTriggered_1e2mu << ","
		<< " 3mu = " << isTriggered_3mu << std::endl;
    }

    bool selTrigger_1e = use_triggers_1e && isTriggered_1e;
    bool selTrigger_1mu = use_triggers_1mu && isTriggered_1mu;
    bool selTrigger_2e = use_triggers_2e && isTriggered_2e;
    bool selTrigger_1e1mu = use_triggers_1e1mu && isTriggered_1e1mu;
    bool selTrigger_2mu = use_triggers_2mu && isTriggered_2mu;
    bool selTrigger_3e = use_triggers_3e && isTriggered_3e;
    bool selTrigger_2e1mu = use_triggers_2e1mu && isTriggered_2e1mu;
    bool selTrigger_1e2mu = use_triggers_1e2mu && isTriggered_1e2mu;
    bool selTrigger_3mu = use_triggers_3mu && isTriggered_3mu;
    if ( !(selTrigger_1e || selTrigger_1mu   ||
	   selTrigger_2e || selTrigger_1e1mu || selTrigger_2mu   ||
	   selTrigger_3e || selTrigger_2e1mu || selTrigger_1e2mu || selTrigger_3mu) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS trigger selection." << std::endl;
	std::cout << " (selTrigger_3mu = " << selTrigger_3mu
		  << ", selTrigger_1e2mu = " << selTrigger_1e2mu
		  << ", selTrigger_2e1mu = " << selTrigger_2e1mu
		  << ", selTrigger_3e = " << selTrigger_3e
		  << ", selTrigger_2mu = " << selTrigger_2mu
		  << ", selTrigger_1e1mu = " << selTrigger_1e1mu
		  << ", selTrigger_2e = " << selTrigger_2e
		  << ", selTrigger_1mu = " << selTrigger_1mu
		  << ", selTrigger_1e = " << selTrigger_1e << ")" << std::endl;
      }
      continue;
    }

    //--- build collections of generator level particles
    //std::cout << "built gen variable" << std::endl;
    std::vector<GenParticle> genTopQuarks;
    std::vector<GenParticle> genBJets;
    std::vector<GenParticle> genWBosons;
    std::vector<GenParticle> genWJets;
    std::vector<GenParticle> genQuarkFromTop;
    if ( isMC ) {
      genTopQuarks = genTopQuarkReader->read();
      genBJets = genBJetReader->read();
      genWBosons = genWBosonReader->read();
      genWJets = genWJetReader->read();
      genQuarkFromTop = genQuarkFromTopReader->read();
    }

//--- rank triggers by priority and ignore triggers of lower priority if a trigger of higher priority has fired for given event;
//    the triggers are ranked by primary dataset (PD).
//    The ranking of the PDs is as follows: DoubleMuon, MuonEG, DoubleEG, SingleMuon, SingleElectron
// CV: see https://cmssdt.cern.ch/lxr/source/HLTrigger/Configuration/python/HLT_GRun_cff.py?v=CMSSW_8_0_24 for association of triggers paths to PD
//     this logic is necessary to avoid that the same event is selected multiple times when processing different primary datasets
    if ( !isMC && !isDEBUG ) {

      //bool isTriggered_SingleElectron = isTriggered_1e;
      bool isTriggered_SingleMuon = isTriggered_1mu;
      bool isTriggered_DoubleEG = isTriggered_2e || isTriggered_3e;
      bool isTriggered_DoubleMuon = isTriggered_2mu || isTriggered_3mu;
      bool isTriggered_MuonEG = isTriggered_1e1mu || isTriggered_2e1mu || isTriggered_1e2mu;

      bool selTrigger_SingleElectron = selTrigger_1e;
      bool selTrigger_SingleMuon = selTrigger_1mu;
      bool selTrigger_DoubleEG = selTrigger_2e || selTrigger_3e;
      //bool selTrigger_DoubleMuon = selTrigger_2mu || selTrigger_3mu;
      bool selTrigger_MuonEG = selTrigger_1e1mu || selTrigger_2e1mu || selTrigger_1e2mu;
      if ( selTrigger_SingleElectron && (isTriggered_DoubleEG || isTriggered_SingleMuon || isTriggered_DoubleMuon || isTriggered_MuonEG) ) {
	if ( run_lumi_eventSelector ) {
      std::cout << "event " << eventInfo.str() << " FAILS trigger selection." << std::endl;
	  std::cout << " (selTrigger_SingleElectron = " << selTrigger_SingleElectron
		    << ", isTriggered_DoubleEG = " << isTriggered_DoubleEG
		    << ", isTriggered_SingleMuon = " << isTriggered_SingleMuon
		    << ", isTriggered_DoubleMuon = " << isTriggered_DoubleMuon
		    << ", isTriggered_MuonEG = " << isTriggered_MuonEG << ")" << std::endl;
	}
	continue;
      }
      if ( selTrigger_DoubleEG && (isTriggered_DoubleMuon || isTriggered_MuonEG) ) {
	if ( run_lumi_eventSelector ) {
      std::cout << "event " << eventInfo.str() << " FAILS trigger selection." << std::endl;
	  std::cout << " (selTrigger_DoubleEG = " << selTrigger_DoubleEG
		    << ", isTriggered_DoubleMuon = " << isTriggered_DoubleMuon
		    << ", isTriggered_MuonEG = " << isTriggered_MuonEG << ")" << std::endl;
	}
	continue;
      }
      if ( selTrigger_SingleMuon && (isTriggered_DoubleEG || isTriggered_DoubleMuon || isTriggered_MuonEG) ) {
	if ( run_lumi_eventSelector ) {
      std::cout << "event " << eventInfo.str() << " FAILS trigger selection." << std::endl;
	  std::cout << " (selTrigger_SingleMuon = " << selTrigger_SingleMuon
		    << ", isTriggered_DoubleEG = " << isTriggered_DoubleEG
		    << ", isTriggered_DoubleMuon = " << isTriggered_DoubleMuon
		    << ", isTriggered_MuonEG = " << isTriggered_MuonEG << ")" << std::endl;
	}
	continue;
      }
      if ( selTrigger_MuonEG && isTriggered_DoubleMuon ) {
	if ( run_lumi_eventSelector ) {
      std::cout << "event " << eventInfo.str() << " FAILS trigger selection." << std::endl;
	  std::cout << " (selTrigger_MuonEG = " << selTrigger_MuonEG
		    << ", isTriggered_DoubleMuon = " << isTriggered_DoubleMuon << ")" << std::endl;
	}
	continue;
      }
    }
    cutFlowTable.update("trigger");
    cutFlowHistManager->fillHistograms("trigger", lumiScale);

    if ( (selTrigger_3mu   && !apply_offline_e_trigger_cuts_3mu)   ||
	 (selTrigger_2e1mu && !apply_offline_e_trigger_cuts_2e1mu) ||
	 (selTrigger_1e2mu && !apply_offline_e_trigger_cuts_1e2mu) ||
	 (selTrigger_3e    && !apply_offline_e_trigger_cuts_3e)    ||
	 (selTrigger_2mu   && !apply_offline_e_trigger_cuts_2mu)   ||
	 (selTrigger_1e1mu && !apply_offline_e_trigger_cuts_1e1mu) ||
	 (selTrigger_2e    && !apply_offline_e_trigger_cuts_2e)    ||
	 (selTrigger_1mu   && !apply_offline_e_trigger_cuts_1mu)   ||
	 (selTrigger_1e    && !apply_offline_e_trigger_cuts_1e)    ) {
      fakeableElectronSelector.disable_offline_e_trigger_cuts();
      tightElectronSelector.disable_offline_e_trigger_cuts();
    } else {
      fakeableElectronSelector.enable_offline_e_trigger_cuts();
      tightElectronSelector.enable_offline_e_trigger_cuts();
    }

//--- build collections of electrons, muons and hadronic taus;
//    resolve overlaps in order of priority: muon, electron,
    std::vector<RecoMuon> muons = muonReader->read();
    std::vector<const RecoMuon*> muon_ptrs = convert_to_ptrs(muons);
    std::vector<const RecoMuon*> cleanedMuons = muon_ptrs; // CV: no cleaning needed for muons, as they have the highest priority in the overlap removal
    std::vector<const RecoMuon*> preselMuons = preselMuonSelector(cleanedMuons, isHigherConePt);
    std::vector<const RecoMuon*> fakeableMuons = fakeableMuonSelector(preselMuons, isHigherConePt);
    std::vector<const RecoMuon*> tightMuons = tightMuonSelector(fakeableMuons, isHigherConePt);
    if(isDEBUG || run_lumi_eventSelector)
    {
      //printCollection("preselMuons",   preselMuons);
      //printCollection("fakeableMuons", fakeableMuons);
      //printCollection("tightMuons",    tightMuons);
    }

    std::vector<RecoElectron> electrons = electronReader->read();
    std::vector<const RecoElectron*> electron_ptrs = convert_to_ptrs(electrons);
    std::vector<const RecoElectron*> cleanedElectrons = electronCleaner(electron_ptrs, preselMuons);
    std::vector<const RecoElectron*> preselElectrons = preselElectronSelector(cleanedElectrons, isHigherConePt);
    std::vector<const RecoElectron*> fakeableElectrons = fakeableElectronSelector(preselElectrons, isHigherConePt);
    std::vector<const RecoElectron*> tightElectrons = tightElectronSelector(fakeableElectrons, isHigherConePt);
    if(isDEBUG || run_lumi_eventSelector)
    {
      //printCollection("preselElectrons",   preselElectrons);
      //printCollection("fakeableElectrons", fakeableElectrons);
      //printCollection("tightElectrons",    tightElectrons);
    }

    std::vector<const RecoLepton*> preselLeptonsFull = mergeLeptonCollections(preselElectrons, preselMuons, isHigherConePt);
    std::vector<const RecoLepton*> fakeableLeptonsFull = mergeLeptonCollections(fakeableElectrons, fakeableMuons, isHigherConePt);
    std::vector<const RecoLepton*> tightLeptonsFull = mergeLeptonCollections(tightElectrons, tightMuons, isHigherConePt);

    std::vector<const RecoLepton*> preselLeptons = pickFirstNobjects(preselLeptonsFull, 3);
    std::vector<const RecoLepton*> fakeableLeptons = pickFirstNobjects(fakeableLeptonsFull, 3);
    std::vector<const RecoLepton*> tightLeptons = getIntersection(fakeableLeptons, tightLeptonsFull, isHigherConePt);

    std::vector<const RecoLepton*> selLeptons;
    std::vector<const RecoMuon*> selMuons;
    std::vector<const RecoElectron*> selElectrons;
    if(electronSelection == muonSelection)
    {
      // for SR, flip region and fake CR
      // doesn't matter if we supply electronSelection or muonSelection here
      selLeptons = selectObjects(muonSelection, preselLeptons, fakeableLeptons, tightLeptons);
      selMuons = getIntersection(preselMuons, selLeptons, isHigherConePt);
      selElectrons = getIntersection(preselElectrons, selLeptons, isHigherConePt);
    }
    else
    {
      // for MC closure
      // make sure that neither electron nor muon selections are loose
      assert(electronSelection != kLoose && muonSelection != kLoose);
      selMuons = selectObjects(muonSelection, preselMuons, fakeableMuons, tightMuons);
      selElectrons = selectObjects(electronSelection, preselElectrons, fakeableElectrons, tightElectrons);
      std::vector<const RecoLepton*> selLeptons_full = mergeLeptonCollections(selElectrons, selMuons, isHigherConePt);
      selLeptons = getIntersection(fakeableLeptons, selLeptons_full, isHigherConePt);
    }

    std::vector<RecoHadTau> hadTaus = hadTauReader->read();
    std::vector<const RecoHadTau*> hadTau_ptrs = convert_to_ptrs(hadTaus);
    std::vector<const RecoHadTau*> cleanedHadTaus = hadTauCleaner(hadTau_ptrs, preselMuons, preselElectrons);
    std::vector<const RecoHadTau*> fakeableHadTaus = fakeableHadTauSelector(cleanedHadTaus, isHigherPt);
    std::vector<const RecoHadTau*> selHadTaus = tightHadTauSelector(cleanedHadTaus, isHigherPt);

    if(isDEBUG || run_lumi_eventSelector)
    {
      //printCollection("selMuons", selMuons);
      //printCollection("selElectrons", selElectrons);
      //printCollection("selLeptons", selLeptons);
      //printCollection("selHadTaus", selHadTaus);
    }
//--- build collections of jets reconstructed by hep-top-tagger (HTTv2) algorithm
    std::vector<RecoJetHTTv2> jetsHTTv2 = jetReaderHTTv2->read();
    std::vector<const RecoJetHTTv2*> jet_ptrsHTTv2raw = convert_to_ptrs(jetsHTTv2);
    std::vector<const RecoJetHTTv2*> cleanedJetsHTTv2 = jetCleanerHTTv2(jet_ptrsHTTv2raw, selMuons, selElectrons);
    std::vector<const RecoJetHTTv2*> jet_ptrsHTTv2rawSel = jetSelectorHTTv2(jet_ptrsHTTv2raw, isHigherPt);
    std::vector<const RecoJetHTTv2*> sel_HTTv2           =  jetSelectorHTTv2(cleanedJetsHTTv2, isHigherPt);
   //std::cout << "after load HTTv2"  << std::endl;

    /*
//--- build collections of jets reconstructed by anti-kT algorithm with dR=1.2 (AK12)
    std::vector<RecoJetAK12> jetsAK12 = jetReaderAK12->read();
    std::vector<const RecoJetAK12*> jet_ptrsAK12raw1 = convert_to_ptrs(jetsAK12);
    std::vector<const RecoJetAK12*> jet_ptrsAK12raw = jetSelectorAK12(jet_ptrsAK12raw1, isHigherPt);
    std::vector<const RecoJetAK12*> jet_ptrsAK12 = jetCleanerAK12(jet_ptrsAK12raw, fakeableMuons, fakeableElectrons, selHadTaus);
    //std::cout << "after load ak12"  << std::endl;
    */

//--- build collections of jets reconstructed by anti-kT algorithm with dR=0.8 (AK8)
    std::vector<RecoJetAK8> jetsAK8 = jetReaderAK8->read();
    std::vector<const RecoJetAK8*> jet_ptrsAK8raw1 = convert_to_ptrs(jetsAK8);
    std::vector<const RecoJetAK8*> jet_ptrsAK8raw = jetSelectorAK8(jet_ptrsAK8raw1, isHigherPt);;
    std::vector<const RecoJetAK8*> jet_ptrsAK8 = jetCleanerAK8(jet_ptrsAK8raw, selLeptons, selElectrons);
    //std::cout << "after load ak8"  << std::endl;
//--- build collections of jets and select subset of jets passing b-tagging criteria
    std::vector<RecoJet> jets = jetReader->read();
    std::vector<const RecoJet*> jet_ptrs = convert_to_ptrs(jets);
    std::vector<const RecoJet*> cleanedJets = jetCleaner(jet_ptrs, fakeableLeptons, fakeableHadTaus);
    std::vector<const RecoJet*> selJets = jetSelector(cleanedJets, isHigherPt);
    std::vector<const RecoJet*> selBJets_loose = jetSelectorBtagLoose(cleanedJets, isHigherPt);
    std::vector<const RecoJet*> selBJets_medium = jetSelectorBtagMedium(cleanedJets, isHigherPt);
    if(isDEBUG || run_lumi_eventSelector)
    {
      //printCollection("uncleanedJets", jet_ptrs);
      //printCollection("selJets",       selJets);
    }

//--- cleaned RecoJet collection from AK12 as well
    /*
    // -- to make the semi-boosted tagger but keep b-tag ordering consistent in cat2
    std::vector<const RecoJet*> cleanedJets_fromAK12;
    cleanedJets_fromAK12 = jetCleaner_large12(selJets, jet_ptrsAK12);
    */
    std::vector<const RecoJet*> cleanedJets_fromAK8;
    cleanedJets_fromAK8 = jetCleaner_large8(selJets, jet_ptrsAK8);
    //else cleanedJets = jetCleaner(selJets, jet_ptrsAK8);

//--- build collections of generator level particles (after some cuts are applied, to safe computing time)
    if ( isMC && redoGenMatching && !fillGenEvtHistograms ) {
      if ( genLeptonReader ) {
	genLeptons = genLeptonReader->read();
	for ( std::vector<GenLepton>::const_iterator genLepton = genLeptons.begin();
	      genLepton != genLeptons.end(); ++genLepton ) {
	  int abs_pdgId = std::abs(genLepton->pdgId());
	  if      ( abs_pdgId == 11 ) genElectrons.push_back(*genLepton);
	  else if ( abs_pdgId == 13 ) genMuons.push_back(*genLepton);
	}
      }
      if ( genHadTauReader ) {
        genHadTaus = genHadTauReader->read();
      }
      if ( genPhotonReader ) {
        genPhotons = genPhotonReader->read();
      }
      if ( genJetReader ) {
        genJets = genJetReader->read();
      }
    }

//--- match reconstructed to generator level particles
    if ( isMC && redoGenMatching ) {
      muonGenMatcher.addGenLeptonMatch(preselMuons, genLeptons, 0.2);
      muonGenMatcher.addGenHadTauMatch(preselMuons, genHadTaus, 0.2);
      muonGenMatcher.addGenJetMatch(preselMuons, genJets, 0.2);

      electronGenMatcher.addGenLeptonMatch(preselElectrons, genLeptons, 0.2);
      electronGenMatcher.addGenHadTauMatch(preselElectrons, genHadTaus, 0.2);
      electronGenMatcher.addGenPhotonMatch(preselElectrons, genPhotons, 0.2);
      electronGenMatcher.addGenJetMatch(preselElectrons, genJets, 0.2);

      hadTauGenMatcher.addGenLeptonMatch(selHadTaus, genLeptons, 0.2);
      hadTauGenMatcher.addGenHadTauMatch(selHadTaus, genHadTaus, 0.2);
      hadTauGenMatcher.addGenJetMatch(selHadTaus, genJets, 0.2);

      jetGenMatcher.addGenLeptonMatch(selJets, genLeptons, 0.2);
      jetGenMatcher.addGenHadTauMatch(selJets, genHadTaus, 0.2);
      jetGenMatcher.addGenJetMatch(selJets, genJets, 0.2);
    }

//--- apply preselection
    // require exactly three leptons passing loose preselection criteria to avoid overlap with 4l category
    if ( !(preselLeptonsFull.size() >= 3) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS preselLeptons selection." << std::endl;
  //printCollection("preselLeptons", preselLeptonsFull);
      }
      continue;
    }
    cutFlowTable.update(">= 3 presel leptons");
    cutFlowHistManager->fillHistograms(">= 3 presel leptons", lumiScale);
    const RecoLepton* preselLepton_lead = preselLeptonsFull[0];
    const RecoLepton* preselLepton_sublead = preselLeptonsFull[1];
    const RecoLepton* preselLepton_third = preselLeptonsFull[2];
    const leptonGenMatchEntry& preselLepton_genMatch = getLeptonGenMatch(leptonGenMatch_definitions, preselLepton_lead, preselLepton_sublead, preselLepton_third);
    int idxPreselLepton_genMatch = preselLepton_genMatch.idx_;
    assert(idxPreselLepton_genMatch != kGen_LeptonUndefined3);

    // require that trigger paths match event category (with event category based on preselLeptons)
    if ( !((preselElectrons.size() >= 3 &&                            (selTrigger_3e    || selTrigger_2e  || selTrigger_1e                                      )) ||
	   (preselElectrons.size() >= 2 && preselMuons.size() >= 1 && (selTrigger_2e1mu || selTrigger_2e  || selTrigger_1e1mu || selTrigger_1mu || selTrigger_1e)) ||
	   (preselElectrons.size() >= 1 && preselMuons.size() >= 2 && (selTrigger_1e2mu || selTrigger_2mu || selTrigger_1e1mu || selTrigger_1mu || selTrigger_1e)) ||
	   (                               preselMuons.size() >= 3 && (selTrigger_3mu   || selTrigger_2mu || selTrigger_1mu                                     ))) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS trigger selection for given preselLepton multiplicity." << std::endl;
	std::cout << " (#preselElectrons = " << preselElectrons.size()
		  << ", #preselMuons = " << preselMuons.size()
		  << ", selTrigger_3mu = " << selTrigger_3mu
		  << ", selTrigger_1e2mu = " << selTrigger_1e2mu
		  << ", selTrigger_2e1mu = " << selTrigger_2e1mu
		  << ", selTrigger_3e = " << selTrigger_3e
		  << ", selTrigger_2mu = " << selTrigger_2mu
		  << ", selTrigger_1e1mu = " << selTrigger_1e1mu
		  << ", selTrigger_2e = " << selTrigger_2e
		  << ", selTrigger_1mu = " << selTrigger_1mu
		  << ", selTrigger_1e = " << selTrigger_1e << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("presel lepton trigger match");
    cutFlowHistManager->fillHistograms("presel lepton trigger match", lumiScale);

    // apply requirement on jets (incl. b-tagged jets) and hadronic taus on preselection level
    if ( !((int)selJets.size() >= minNumJets) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selJets selection = " << (int)selJets.size() << std::endl;
  //printCollection("selJets", selJets);
      }
      continue;
    }
    cutFlowTable.update(Form(">= %i jets (1)", minNumJets));
    cutFlowHistManager->fillHistograms(">= N jets (1)", lumiScale);
    if ( !(selBJets_loose.size() >= 2 || selBJets_medium.size() >= 1) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selBJets selection." << std::endl;
	//printCollection("selJets", selJets);
	//printCollection("selBJets_loose", selBJets_loose);
	//printCollection("selBJets_medium", selBJets_medium);
      }
      continue;
    }
    cutFlowTable.update(">= 2 loose b-jets || 1 medium b-jet (1)");
    cutFlowHistManager->fillHistograms(">= 2 loose b-jets || 1 medium b-jet (1)", lumiScale);

//--- compute MHT and linear MET discriminant (met_LD)
    RecoMEt met = metReader->read();
    Particle::LorentzVector mht_p4 = compMHT(fakeableLeptons, {}, selJets);
    double met_LD = compMEt_LD(met.p4(), mht_p4);

//--- fill histograms with events passing preselection
    preselHistManagerType* preselHistManager = preselHistManagers[idxPreselLepton_genMatch];
    assert(preselHistManager != 0);
    preselHistManager->electrons_->fillHistograms(preselElectrons, 1.);
    preselHistManager->muons_->fillHistograms(preselMuons, 1.);
    preselHistManager->hadTaus_->fillHistograms(selHadTaus, 1.);
    preselHistManager->jets_->fillHistograms(selJets, 1.);
    preselHistManager->BJets_loose_->fillHistograms(selBJets_loose, 1.);
    preselHistManager->BJets_medium_->fillHistograms(selBJets_medium, 1.);
    preselHistManager->met_->fillHistograms(met, mht_p4, met_LD, 1.);
    preselHistManager->metFilters_->fillHistograms(metFilters, 1.);
    preselHistManager->evt_->fillHistograms(
      preselElectrons.size(), preselMuons.size(), selHadTaus.size(),
      selJets.size(), selBJets_loose.size(), selBJets_medium.size(),
      -1., -1., -1.,
      -1., -1., -1., -1., -1., -1., -1., -1.,
      1.);
    preselHistManager->evtYield_->fillHistograms(eventInfo, 1.);

//--- apply final event selection
    // require exactly three leptons passing tight selection criteria of final event selection
    if ( !(selLeptons.size() >= 3) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selLeptons selection." << std::endl;
	//printCollection("selLeptons", selLeptons);
	//printCollection("preselLeptons", preselLeptons);
      }
      continue;
    }
    cutFlowTable.update(">= 3 sel leptons", 1.);
    cutFlowHistManager->fillHistograms(">= 3 sel leptons", 1.);
    const RecoLepton* selLepton_lead = selLeptons[0];
    int selLepton_lead_type = getLeptonType(selLepton_lead->pdgId());
    const RecoLepton* selLepton_sublead = selLeptons[1];
    int selLepton_sublead_type = getLeptonType(selLepton_sublead->pdgId());
    const RecoLepton* selLepton_third = selLeptons[2];
    int selLepton_third_type = getLeptonType(selLepton_third->pdgId());
    const leptonGenMatchEntry& selLepton_genMatch = getLeptonGenMatch(leptonGenMatch_definitions, selLepton_lead, selLepton_sublead, selLepton_third);
    int idxSelLepton_genMatch = selLepton_genMatch.idx_;
    assert(idxSelLepton_genMatch != kGen_LeptonUndefined3);
    if ( isDEBUG ) {
      std::cout << "selLepton_genMatch = " << getLeptonGenMatch_string(leptonGenMatch_definitions, idxSelLepton_genMatch) << std::endl;
      if ( idxSelLepton_genMatch != kGen_3l0g0j ) {
	std::cout << "--> CHECK!" << std::endl;
	//printCollection("selLeptons", selLeptons);
      }
    }

//--- compute event-level weight for data/MC correction of b-tagging efficiency and mistag rate
//   (using the method "Event reweighting using scale factors calculated with a tag and probe method",
//    described on the BTV POG twiki https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration )
    double evtWeight = 1.;
    double btagWeight = 1.;
    double triggerWeight = 1.;
    double leptonSF_weight = 1.;
    if ( isMC ) {
      evtWeight *= evtWeight_inclusive;
      btagWeight = get_BtagWeight(selJets);
      evtWeight *= btagWeight;
      if ( isDEBUG ) {
	std::cout << "lumiScale = " << lumiScale << std::endl;
	if ( apply_genWeight ) std::cout << "genWeight = " << boost::math::sign(eventInfo.genWeight) << std::endl;
	std::cout << "pileupWeight = " << eventInfo.pileupWeight << std::endl;
	std::cout << "btagWeight = " << btagWeight << std::endl;
      }
    }

    double weight_data_to_MC_correction = 1.;
    if ( isMC ) {
      dataToMCcorrectionInterface->setLeptons(
        selLepton_lead_type, selLepton_lead->pt(), selLepton_lead->eta(),
	selLepton_sublead_type, selLepton_sublead->pt(), selLepton_sublead->eta(),
        selLepton_third_type, selLepton_third->pt(), selLepton_third->eta());

//--- apply data/MC corrections for trigger efficiency
      double sf_triggerEff = dataToMCcorrectionInterface->getSF_leptonTriggerEff();
      if ( isDEBUG ) {
	std::cout << "sf_triggerEff = " << sf_triggerEff << std::endl;
      }
      triggerWeight *= sf_triggerEff;
      weight_data_to_MC_correction *= sf_triggerEff;

//--- apply data/MC corrections for efficiencies for lepton to pass loose identification and isolation criteria
      leptonSF_weight *= dataToMCcorrectionInterface->getSF_leptonID_and_Iso_loose();

//--- apply data/MC corrections for efficiencies of leptons passing the loose identification and isolation criteria
//    to also pass the tight identification and isolation criteria
      if ( electronSelection == kFakeable && muonSelection == kFakeable ) {
        leptonSF_weight *= dataToMCcorrectionInterface->getSF_leptonID_and_Iso_fakeable_to_loose();
      } else if ( electronSelection >= kFakeable && muonSelection >= kFakeable ) {
        // apply loose-to-tight lepton ID SFs if either of the following is true:
        // 1) both electron and muon selections are tight -> corresponds to SR
        // 2) electron selection is relaxed to fakeable and muon selection is kept as tight -> corresponds to MC closure w/ relaxed e selection
        // 3) muon selection is relaxed to fakeable and electron selection is kept as tight -> corresponds to MC closure w/ relaxed mu selection
        // we allow (2) and (3) so that the MC closure regions would more compatible w/ the SR (1) in comparison
        leptonSF_weight *= dataToMCcorrectionInterface->getSF_leptonID_and_Iso_tight_to_loose_woTightCharge();
      }
      weight_data_to_MC_correction *= leptonSF_weight;

      evtWeight *= weight_data_to_MC_correction;
    }

    double weight_fakeRate = 1.;
    if ( !selectBDT ) {
      if ( applyFakeRateWeights == kFR_3lepton ) {
	double prob_fake_lepton_lead = 1.;
	if      ( std::abs(selLepton_lead->pdgId()) == 11 ) prob_fake_lepton_lead = leptonFakeRateInterface->getWeight_e(selLepton_lead->cone_pt(), selLepton_lead->absEta());
	else if ( std::abs(selLepton_lead->pdgId()) == 13 ) prob_fake_lepton_lead = leptonFakeRateInterface->getWeight_mu(selLepton_lead->cone_pt(), selLepton_lead->absEta());
	else assert(0);
	bool passesTight_lepton_lead = isMatched(*selLepton_lead, tightElectrons) || isMatched(*selLepton_lead, tightMuons);
	double prob_fake_lepton_sublead = 1.;
	if      ( std::abs(selLepton_sublead->pdgId()) == 11 ) prob_fake_lepton_sublead = leptonFakeRateInterface->getWeight_e(selLepton_sublead->cone_pt(), selLepton_sublead->absEta());
	else if ( std::abs(selLepton_sublead->pdgId()) == 13 ) prob_fake_lepton_sublead = leptonFakeRateInterface->getWeight_mu(selLepton_sublead->cone_pt(), selLepton_sublead->absEta());
	else assert(0);
	bool passesTight_lepton_sublead = isMatched(*selLepton_sublead, tightElectrons) || isMatched(*selLepton_sublead, tightMuons);
	double prob_fake_lepton_third = 1.;
	if      ( std::abs(selLepton_third->pdgId()) == 11 ) prob_fake_lepton_third = leptonFakeRateInterface->getWeight_e(selLepton_third->cone_pt(), selLepton_third->absEta());
	else if ( std::abs(selLepton_third->pdgId()) == 13 ) prob_fake_lepton_third = leptonFakeRateInterface->getWeight_mu(selLepton_third->cone_pt(), selLepton_third->absEta());
	else assert(0);
	bool passesTight_lepton_third = isMatched(*selLepton_third, tightElectrons) || isMatched(*selLepton_third, tightMuons);
        weight_fakeRate = getWeight_3L(
          prob_fake_lepton_lead, passesTight_lepton_lead,
	  prob_fake_lepton_sublead, passesTight_lepton_sublead,
	  prob_fake_lepton_third, passesTight_lepton_third);
	if ( isDEBUG ) {
	  std::cout << "weight_fakeRate = " << weight_fakeRate << std::endl;
	}
	evtWeight *= weight_fakeRate;
      }
    }
    if ( isDEBUG ) {
      std::cout << "evtWeight = " << evtWeight << std::endl;
    }

    // require exactly three leptons passing tight selection criteria, to avoid overlap with 4l channel
    if ( !(tightLeptonsFull.size() <= 3) ) {
      if ( run_lumi_eventSelector ) {
        std::cout << "event " << eventInfo.str() << " FAILS tightLeptons selection.\n";
        //printCollection("tightLeptonsFull", tightLeptonsFull);
      }
      continue;
    }

    // require that trigger paths match event category (with event category based on fakeableLeptons)
    if ( !((fakeableElectrons.size() >= 3 &&                              (selTrigger_3e    || selTrigger_2e  || selTrigger_1e                                      )) ||
	   (fakeableElectrons.size() >= 2 && fakeableMuons.size() >= 1 && (selTrigger_2e1mu || selTrigger_2e  || selTrigger_1e1mu || selTrigger_1mu || selTrigger_1e)) ||
	   (fakeableElectrons.size() >= 1 && fakeableMuons.size() >= 2 && (selTrigger_1e2mu || selTrigger_2mu || selTrigger_1e1mu || selTrigger_1mu || selTrigger_1e)) ||
	   (                                 fakeableMuons.size() >= 3 && (selTrigger_3mu   || selTrigger_2mu || selTrigger_1mu                                     ))) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS trigger selection for given fakeableLepton multiplicity." << std::endl;
	std::cout << " (#fakeableElectrons = " << fakeableElectrons.size()
		  << ", #fakeableMuons = " << fakeableMuons.size()
		  << ", selTrigger_3mu = " << selTrigger_3mu
		  << ", selTrigger_1e2mu = " << selTrigger_1e2mu
		  << ", selTrigger_2e1mu = " << selTrigger_2e1mu
		  << ", selTrigger_3e = " << selTrigger_3e
		  << ", selTrigger_2mu = " << selTrigger_2mu
		  << ", selTrigger_1e1mu = " << selTrigger_1e1mu
		  << ", selTrigger_2e = " << selTrigger_2e
		  << ", selTrigger_1mu = " << selTrigger_1mu
		  << ", selTrigger_1e = " << selTrigger_1e << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("fakeable lepton trigger match", evtWeight);
    cutFlowHistManager->fillHistograms("fakeable lepton trigger match", evtWeight);

//--- apply HLT filter
    if(apply_hlt_filter)
    {
      const std::map<hltPathsE, bool> trigger_bits = {
        { hltPathsE::trigger_1e,    selTrigger_1e    },
        { hltPathsE::trigger_1mu,   selTrigger_1mu   },
        { hltPathsE::trigger_2e,    selTrigger_2e    },
        { hltPathsE::trigger_2mu,   selTrigger_2mu   },
        { hltPathsE::trigger_1e1mu, selTrigger_1e1mu },
        { hltPathsE::trigger_1e2mu, selTrigger_1e2mu },
        { hltPathsE::trigger_2e1mu, selTrigger_2e1mu },
        { hltPathsE::trigger_3e,    selTrigger_3e    },
        { hltPathsE::trigger_3mu,   selTrigger_3mu   },
      };
      if(! hltFilter(trigger_bits, fakeableLeptons, {}))
      {
        if(run_lumi_eventSelector || isDEBUG)
        {
          std::cout << "event " << eventInfo.str() << " FAILS HLT filter matching\n";
        }
        continue;
      }
    }
    cutFlowTable.update("HLT filter matching", evtWeight);
    cutFlowHistManager->fillHistograms("HLT filter matching", evtWeight);

    // apply requirement on jets (incl. b-tagged jets) and hadronic taus on level of final event selection
    if ( !((int)selJets.size() >= minNumJets) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selJets selection -- " << (int)selJets.size() << std::endl;
	//printCollection("selJets", selJets);
      }
      continue;
    }
    cutFlowTable.update(Form(">= %i jets (2)", minNumJets), evtWeight);
    cutFlowHistManager->fillHistograms(">= N jets (2)", evtWeight);
    if ( !(selBJets_loose.size() >= 2 || selBJets_medium.size() >= 1)) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selBJets selection." << std::endl;
	//printCollection("selJets", selJets);
	//printCollection("selBJets_loose", selBJets_loose);
	//printCollection("selBJets_medium", selBJets_medium);
      }
      continue;
    }
    cutFlowTable.update(">= 2 loose b-jets || 1 medium b-jet (2)", evtWeight);
    cutFlowHistManager->fillHistograms(">= 2 loose b-jets || 1 medium b-jet (2)", evtWeight);

    if ( selHadTaus.size() > 0 ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS selHadTaus veto." << std::endl;
	//printCollection("selHadTaus", selHadTaus);
      }
      continue;
    }
    cutFlowTable.update("sel tau veto");
    cutFlowHistManager->fillHistograms("sel tau veto", lumiScale);

    bool failsLowMassVeto = false;
    for ( std::vector<const RecoLepton*>::const_iterator lepton1 = preselLeptonsFull.begin();
    lepton1 != preselLeptonsFull.end(); ++lepton1 ) {
      for ( std::vector<const RecoLepton*>::const_iterator lepton2 = lepton1 + 1;
      lepton2 != preselLeptonsFull.end(); ++lepton2 ) {
	double mass = ((*lepton1)->p4() + (*lepton2)->p4()).mass();
	if ( mass < 12. ) {
	  failsLowMassVeto = true;
	}
      }
    }
    if ( failsLowMassVeto ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS low mass lepton pair veto." << std::endl;
      }
      continue;
    }
    cutFlowTable.update("m(ll) > 12 GeV", evtWeight);
    cutFlowHistManager->fillHistograms("m(ll) > 12 GeV", evtWeight);

    const double minPt_lead = 25.;
    const double minPt_sublead = 15.;
    const double minPt_third = 10.;
    // CV: according to Giovanni, the pT cuts should be applied on cone_pt
    //    (combined efficiency of single lepton, double lepton, and triple lepton triggers assumed to be high,
    //     even if one or two leptons and fakes and hence cone_pt may be significantly smaller than lepton_pt,
    //     on which pT cuts are applied on trigger level)
    if ( !(selLepton_lead->cone_pt() > minPt_lead && selLepton_sublead->cone_pt() > minPt_sublead && selLepton_third->cone_pt() > minPt_third) ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS lepton pT selection." << std::endl;
	std::cout << " (leading selLepton pT = " << selLepton_lead->pt() << ", minPt_lead = " << minPt_lead
		  << ", subleading selLepton pT = " << selLepton_sublead->pt() << ", minPt_sublead = " << minPt_sublead
		  << ", third selLepton pT = " << selLepton_third->pt() << ", minPt_third = " << minPt_third << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("lead lepton pT > 25 GeV && sublead lepton pT > 15 GeV && third lepton pT > 10 GeV", evtWeight);
    cutFlowHistManager->fillHistograms("lead lepton pT > 25 GeV && sublead lepton pT > 15 GeV && third lepton pT > 10 GeV", evtWeight);

    int sumLeptonCharge = selLepton_lead->charge() + selLepton_sublead->charge() + selLepton_third->charge();
    bool isCharge_SS = std::abs(sumLeptonCharge) >  1;
    bool isCharge_OS = std::abs(sumLeptonCharge) <= 1;
    if ( chargeSumSelection == kOS && isCharge_SS ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS lepton charge selection." << std::endl;
	std::cout << " (leading selLepton charge = " << selLepton_lead->charge()
		  << ", subleading selLepton charge = " << selLepton_sublead->charge()
		  << ", third selLepton charge = " << selLepton_third->charge() << ", chargeSumSelection = OS)" << std::endl;
      }
      continue;
    }
    if ( chargeSumSelection == kSS && isCharge_OS ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS lepton charge selection." << std::endl;
	std::cout << " (leading selLepton charge = " << selLepton_lead->charge()
		  << ", subleading selLepton charge = " << selLepton_sublead->charge()
		  << ", third selLepton charge = " << selLepton_third->charge() << ", chargeSumSelection = SS)" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("sel lepton charge", evtWeight);
    cutFlowHistManager->fillHistograms("sel lepton charge", evtWeight);

    bool isSameFlavor_OS = false;
    double massSameFlavor_OS = -1.;
    for ( std::vector<const RecoLepton*>::const_iterator lepton1 = preselLeptonsFull.begin();
    lepton1 != preselLeptonsFull.end(); ++lepton1 ) {
      for ( std::vector<const RecoLepton*>::const_iterator lepton2 = lepton1 + 1;
      lepton2 != preselLeptonsFull.end(); ++lepton2 ) {
	if ( (*lepton1)->pdgId() == -(*lepton2)->pdgId() ) { // pair of same flavor leptons of opposite charge
	  isSameFlavor_OS = true;
	  double mass = ((*lepton1)->p4() + (*lepton2)->p4()).mass();
	  if ( std::fabs(mass - z_mass) < std::fabs(massSameFlavor_OS - z_mass) ) massSameFlavor_OS = mass;
	}
      }
    }

    bool failsZbosonMassVeto = isSameFlavor_OS && std::fabs(massSameFlavor_OS - z_mass) < z_window;
    if ( failsZbosonMassVeto ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS Z-boson veto." << std::endl;
      }
      continue;
    }
    cutFlowTable.update("Z-boson mass veto", evtWeight);
    cutFlowHistManager->fillHistograms("Z-boson mass veto", evtWeight);

    bool failsHtoZZVeto = false;
    for ( std::vector<const RecoLepton*>::const_iterator lepton1 = preselLeptonsFull.begin();
    lepton1 != preselLeptonsFull.end(); ++lepton1 ) {
      for ( std::vector<const RecoLepton*>::const_iterator lepton2 = lepton1 + 1;
      lepton2 != preselLeptonsFull.end(); ++lepton2 ) {
	if ( (*lepton1)->pdgId() == -(*lepton2)->pdgId() ) { // first pair of same flavor leptons of opposite charge
    for ( std::vector<const RecoLepton*>::const_iterator lepton3 = preselLeptonsFull.begin();
    lepton3 != preselLeptonsFull.end(); ++lepton3 ) {
	    if ( (*lepton3) == (*lepton1) || (*lepton3) == (*lepton2) ) continue;
	    for ( std::vector<const RecoLepton*>::const_iterator lepton4 = lepton3 + 1;
      lepton4 != preselLeptonsFull.end(); ++lepton4 ) {
	      if ( (*lepton4) == (*lepton1) || (*lepton4) == (*lepton2) ) continue;
	      if ( (*lepton3)->pdgId() == -(*lepton4)->pdgId() ) { // second pair of same flavor leptons of opposite charge
		double mass = ((*lepton1)->p4() + (*lepton2)->p4() + (*lepton3)->p4() + (*lepton4)->p4()).mass();
		if ( mass < 140. ) failsHtoZZVeto = true;
	      }
	    }
	  }
	}
      }
    }
    if ( failsHtoZZVeto ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS H->ZZ*->4l veto." << std::endl;
      }
      continue;
    }
    cutFlowTable.update("H->ZZ*->4l veto", evtWeight);
    cutFlowHistManager->fillHistograms("H->ZZ*->4l veto", evtWeight);

    double met_LD_cut = 0.;
    if      ( selJets.size() >= 4 ) met_LD_cut = -1.; // MET LD cut not applied
    else if ( isSameFlavor_OS     ) met_LD_cut = 45.;
    else                            met_LD_cut = 30.;
    if ( met_LD_cut > 0 && met_LD < met_LD_cut ) {
      if ( run_lumi_eventSelector ) {
    std::cout << "event " << eventInfo.str() << " FAILS MET LD selection." << std::endl;
	std::cout << " (met_LD = " << met_LD << ", met_LD_cut = " << met_LD_cut << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("met LD", evtWeight);
    cutFlowHistManager->fillHistograms("met LD", evtWeight);

    if ( apply_met_filters ) {
      if ( !metFilterSelector(metFilters) ) {
	if ( run_lumi_eventSelector ) {
      std::cout << "event " << eventInfo.str() << " FAILS MEt filters." << std::endl;
	}
	continue;
      }
    }
    cutFlowTable.update("MEt filters", evtWeight);
    cutFlowHistManager->fillHistograms("MEt filters", evtWeight);

    bool failsSignalRegionVeto = false;
    if ( isMCClosure_e || isMCClosure_m ) {
      bool applySignalRegionVeto = (isMCClosure_e && countFakeElectrons(selLeptons) >= 1) || (isMCClosure_m && countFakeMuons(selLeptons) >= 1);
      if ( applySignalRegionVeto && tightLeptons.size() >= 3 ) failsSignalRegionVeto = true;
    } else if ( electronSelection == kFakeable || muonSelection == kFakeable ) {
      if ( tightLeptons.size() >= 3 ) failsSignalRegionVeto = true;
    }
    if ( failsSignalRegionVeto ) {
      if ( run_lumi_eventSelector ) {
	std::cout << "event " << eventInfo.str() << " FAILS overlap w/ the SR: "
	             "# tight leptons = " << tightLeptons.size() << " >= 3\n"
        ;
	//printCollection("tightLeptons", tightLeptons);
      }
      continue; // CV: avoid overlap with signal region
    }
    cutFlowTable.update("signal region veto", evtWeight);
    cutFlowHistManager->fillHistograms("signal region veto", evtWeight);

//--- compute output of hadronic top tagger BDT
// it returns the gen-triplets organized in top/anti-top
bool calculate_matching = isMC && selectBDT;
std::map<int, Particle::LorentzVector> genVar;
std::map<int, Particle::LorentzVector> genVarAnti;
if (calculate_matching) {
  genVar = isGenMatchedJetTripletVar(genTopQuarks, genBJets, genWBosons, genQuarkFromTop, kGenTop);
  genVarAnti = isGenMatchedJetTripletVar(genTopQuarks, genBJets, genWBosons, genQuarkFromTop, kGenAntiTop);
}
// add overlaps semi-boosted resolved / semi-boosted and boosted / ...
bool resolved_and_semi = false;
bool boosted_and_semi = false;
bool resolved_and_semi_AK8 = false;
bool boosted_and_semi_AK8 = false;
bool resolved_and_boosted = false;

// resolved HTT
double max_mvaOutput_HTT_2016 = 0.;
bool max_truth_HTT_2016 = false;
double genTopPt_2016 = 0.;
double HadTop_pt_2016 = 0.;

// done with tmva ==> starts from -1.
double max_mvaOutput_HTT_multilep = -1.;
bool max_truth_multilep = false;
double HadTop_pt_multilep = 0.;
double genTopPt_multilep = 0.;
double b_pt_multilep_1 = 0.1;
double Wj1_pt_multilep_1 = 0.1;
double Wj2_pt_multilep_1 = 0.1;

double max_mvaOutput_HTT_CSVsort4rd = 0.;
bool max_truth_HTT_CSVsort4rd = false;
double HadTop_pt_CSVsort4rd = 0.;
double genTopPt_CSVsort4rd = 0.;
double b_pt_CSVsort4rd_1 = 0.1;
double Wj1_pt_CSVsort4rd_1 = 0.1;
double Wj2_pt_CSVsort4rd_1 = 0.1;

double max_mvaOutput_HTT_highestCSV = 0.;
bool max_truth_HTT_highestCSV = false;
double HadTop_pt_highestCSV = 0.;
double genTopPt_highestCSV = 0.;

double max_mvaOutput_HTT_CSVsort4rd_WithKinFit = 0.;
bool max_truth_HTT_CSVsort4rd_WithKinFit = false;
double HadTop_pt_CSVsort4rd_WithKinFit = 0.;
double genTopPt_CSVsort4rd_WithKinFit = 0.;

double max_mvaOutput_HTT_highestCSV_WithKinFit = 0.;
bool max_truth_HTT_highestCSV_WithKinFit = false;
double HadTop_pt_highestCSV_WithKinFit = 0.;
double genTopPt_highestCSV_WithKinFit = 0.;

bool hadtruth = false;
for ( std::vector<const RecoJet*>::const_iterator selBJet = selJets.begin(); selBJet != selJets.end(); ++selBJet ) {
  //btag_iterator++;
  for ( std::vector<const RecoJet*>::const_iterator selWJet1 = selJets.begin(); selWJet1 != selJets.end(); ++selWJet1 ) {
   if ( &(*selWJet1) == &(*selBJet) ) continue;
   for ( std::vector<const RecoJet*>::const_iterator selWJet2 = selWJet1 + 1; selWJet2 != selJets.end(); ++selWJet2 ) {
if ( &(*selWJet2) == &(*selBJet) ) continue;
if ( &(*selWJet2) == &(*selWJet1) ) continue;
bool isGenMatched = false;
double genTopPt_teste = 0.;
const std::map<int, double> bdtResult = (*hadTopTagger)(**selBJet, **selWJet1, **selWJet2, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti, true );
// genTopPt_teste is filled with the result of gen-matching
if ( isGenMatched ) hadtruth = true;
// save genpt of all options
double HadTop_pt = ((*selBJet)->p4() + (*selWJet1)->p4() + (*selWJet2)->p4()).pt();

//std::cout << "resolved HTT testing " << max_mvaOutput_HTT_2016 << " "
//<< max_mvaOutput_HTT_2016_2 << "  === "
//<<  bdtResult.at(kXGB_with_kinFit) << " "
//<< std::endl;
//std::cout << "            pts \n " <<
//Wj1_pt_1 << " " <<  Wj2_pt_1 << " " << b_pt_1 << " \n" <<
//Wj1_pt_2 << " " <<  Wj2_pt_2  << " " << b_pt_2  << " \n" <<
//Wj1_pt_med << " " <<  Wj2_pt_med  << " " <<  b_pt_med  << " --- " <<
//(*selWJet1)->pt() << " " << (*selWJet2)->pt() << " " << (*selBJet)->pt()
//<< std::endl;

if ( bdtResult.at(kXGB_with_kinFit) > max_mvaOutput_HTT_2016 ) { // HTT_2016
  max_truth_HTT_2016 = isGenMatched;
  max_mvaOutput_HTT_2016 = bdtResult.at(kXGB_with_kinFit);
  genTopPt_2016 = genTopPt_teste;
  HadTop_pt_2016 = HadTop_pt;
}

if ( bdtResult.at(kXGB_CSVsort4rd) > max_mvaOutput_HTT_CSVsort4rd ) {
  max_truth_HTT_CSVsort4rd = isGenMatched;
  max_mvaOutput_HTT_CSVsort4rd = bdtResult.at(kXGB_CSVsort4rd);
  HadTop_pt_CSVsort4rd = HadTop_pt;
  genTopPt_CSVsort4rd = genTopPt_teste;
  Wj1_pt_CSVsort4rd_1 = (*selWJet1)->pt();
  Wj2_pt_CSVsort4rd_1 = (*selWJet2)->pt();
  b_pt_CSVsort4rd_1   = (*selBJet)->pt();
}

/*
if ( bdtResult.at(kXGB_CSVsort4rd_withKinFit) > max_mvaOutput_HTT_highestCSV_WithKinFit ) {
  max_truth_HTT_CSVsort4rd_WithKinFit = isGenMatched;
  max_mvaOutput_HTT_CSVsort4rd_WithKinFit = bdtResult.at(kXGB_CSVsort4rd_withKinFit);
  HadTop_pt_CSVsort4rd_WithKinFit = HadTop_pt;
  genTopPt_CSVsort4rd_WithKinFit = genTopPt_teste;
}
*/

if ((*selBJet)->BtagCSV() > (*selWJet1)->BtagCSV() && (*selBJet)->BtagCSV() > (*selWJet2)->BtagCSV() ) {

  /*
  if ( bdtResult.at(kXGB_highestCSV) > max_mvaOutput_HTT_highestCSV ) {
    max_truth_HTT_highestCSV = isGenMatched;
    max_mvaOutput_HTT_highestCSV = bdtResult.at(kXGB_highestCSV);
    HadTop_pt_highestCSV = HadTop_pt;
    genTopPt_highestCSV = genTopPt_teste;
  }

  if ( bdtResult.at(kXGB_highestCSV_withKinFit) > max_mvaOutput_HTT_highestCSV_WithKinFit ) {
    max_truth_HTT_highestCSV_WithKinFit = isGenMatched;
    max_mvaOutput_HTT_highestCSV_WithKinFit = bdtResult.at(kXGB_highestCSV_withKinFit);
    HadTop_pt_highestCSV_WithKinFit = HadTop_pt;
    genTopPt_highestCSV_WithKinFit = genTopPt_teste;
  }
  */

  if ( bdtResult.at(kXGB_multilep) > max_mvaOutput_HTT_multilep ) {
    max_truth_multilep = isGenMatched;
    max_mvaOutput_HTT_multilep = bdtResult.at(kXGB_multilep);
    HadTop_pt_multilep = HadTop_pt;
    genTopPt_multilep = genTopPt_teste;
    Wj1_pt_multilep_1 = (*selWJet1)->pt();
    Wj2_pt_multilep_1 = (*selWJet2)->pt();
    b_pt_multilep_1   = (*selBJet)->pt();
  }
} // close if b candidate is the highest btagged one

}
  }
}
/*
std::cout << "------- resolved HTT result "
<< max_mvaOutput_HTT_2016 << " "
<< max_mvaOutput_HTT_2016_2 << " "
<< max_truth_HTT_2016_med << " Njets = "
<< selJets.size() << " "
//<< max_mvaOutput_HTT_highestCSV << " "
//<< max_mvaOutput_HTT_highestCSV_WithKinFit << " "
<< std::endl;
std::cout << "            pts \n " <<
Wj1_pt_1 << " " <<  Wj2_pt_1 << " " << b_pt_1 << " \n" <<
Wj1_pt_2 << " " <<  Wj2_pt_2  << " " << b_pt_2  << " \n" <<
Wj1_pt_med << " " <<  Wj2_pt_med  << " " <<  b_pt_med  << " \n"
"---------------------------------------------------------------"
<< std::endl;
*/


//--- boosted hTT
double HTT_boosted = 0.;
bool bWj1Wj2_isGenMatched_boosted = false;
double genTopPt_boosted = 0.;
double HadTop_pt_boosted = 0.;

double HTT_boosted_WithKinFit = 0.;
bool bWj1Wj2_isGenMatched_boosted_WithKinFit = false;
double genTopPt_boosted_WithKinFit = 0.;
double HadTop_pt_boosted_WithKinFit = 0.;

bool hadtruth_boosted = false;
double minDR_HTTv2_lep = 0.;

for ( std::vector<const RecoJetHTTv2*>::const_iterator jetIter = sel_HTTv2.begin();
jetIter != sel_HTTv2.end(); ++jetIter ) {
  bool isGenMatched = false;
  double genTopPt_teste = 0.;
  const std::map<int, double> bdtResult = (*hadTopTagger_boosted)(**jetIter, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
  if (isGenMatched) {hadtruth_boosted = true;}

  if ( bdtResult.at(kXGB_boosted_no_kinFit) > HTT_boosted ) {
    bWj1Wj2_isGenMatched_boosted = isGenMatched;
    HTT_boosted = bdtResult.at(kXGB_boosted_no_kinFit);
    HadTop_pt_boosted = (*jetIter)->pt();
    genTopPt_boosted = genTopPt_teste;

    minDR_HTTv2_lep = std::min(
      deltaR(selLepton_lead->p4(), (*jetIter)->p4()),
      deltaR(selLepton_sublead->p4(), (*jetIter)->p4())
    );
  }
}

double HTT_boosted_noISO = 0.;
//bool bWj1Wj2_isGenMatched_boosted = false;
//double genTopPt_boosted_noISO = 0.;
//double HadTop_pt_boosted_noISO = 0.;

for ( std::vector<const RecoJetHTTv2*>::const_iterator jetIter = jet_ptrsHTTv2rawSel.begin();
jetIter != jet_ptrsHTTv2rawSel.end(); ++jetIter ) {
  bool isGenMatched = false;
  double genTopPt_teste = 0.;
  const std::map<int, double> bdtResult = (*hadTopTagger_boosted)(**jetIter, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
  //if (isGenMatched) {hadtruth_boosted = true;}

  if ( bdtResult.at(kXGB_boosted_no_kinFit) > HTT_boosted ) {
    //bWj1Wj2_isGenMatched_boosted = isGenMatched;
    HTT_boosted_noISO = bdtResult.at(kXGB_boosted_no_kinFit);
    //HadTop_pt_boosted_noISO = (*jetIter)->pt();
    //genTopPt_boosted_noISO = genTopPt_teste;
  }
}
//std::cout << "------- HTT_boosted HTT result "
//<< HTT_boosted << " "
//<< HTT_boosted_2 << " "
//<< HTT_boosted_med << " Njets = "
//<< sel_HTTv2.size() << " "
//<< std::endl;

// to test feseability of semi-boosted fatJet-jet isolation
double DR_W_b_gen_AK12 = 0.;
double DR_W_b_gen_AK8 = 0.;

// -- semi-boosted hTT
double HTT_semi_boosted = 0.;
bool bWj1Wj2_isGenMatched_semi_boosted = false;
double genTopPt_semi_boosted = 0.;
double HadTop_pt_semi_boosted = 0.;

double HTT_semi_boosted_WithKinFit = 0.;
bool bWj1Wj2_isGenMatched_semi_boosted_WithKinFit = false;
double genTopPt_semi_boosted_WithKinFit = 0.;
double HadTop_pt_semi_boosted_WithKinFit = 0.;

bool hadtruth_semi_boosted = false;
double minDR_AK12_lep = -1.;

/*
//for (auto i: btag_order) std::cout << i << " ";
for ( std::vector<const RecoJet*>::const_iterator selBJet = cleanedJets_fromAK12.begin(); selBJet != cleanedJets_fromAK12.end(); ++selBJet )  { // cleanedJets.size()
for ( std::vector<const RecoJetAK12*>::const_iterator jetIter = jet_ptrsAK12.begin();
      jetIter != jet_ptrsAK12.end(); ++jetIter ) {
    if ( !((*jetIter)->subJet1() && (*jetIter)->subJet2()) ) continue;
    bool isGenMatched = false;
    double genTopPt_teste = 0.;
    double HadTop_pt = ((*jetIter)->p4() + (*selBJet)->p4()).pt();
    //bool fatjet_isGenMatched = false;
    const std::map<int, double> bdtResult = (*hadTopTagger_semi_boosted)(**jetIter, **selBJet, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
    if (isGenMatched) {hadtruth_semi_boosted = true;}

    if ( bdtResult.at(kXGB_semi_boosted_no_kinFit) > HTT_semi_boosted ) {
      bWj1Wj2_isGenMatched_semi_boosted = isGenMatched;
      HTT_semi_boosted = bdtResult.at(kXGB_semi_boosted_no_kinFit);
      HadTop_pt_semi_boosted = HadTop_pt;
      genTopPt_semi_boosted = genTopPt_teste;
      minDR_AK12_lep = std::min(
        deltaR(selLepton_lead->p4(), (*jetIter)->p4()),
        deltaR(selLepton_sublead->p4(), (*jetIter)->p4())
      );
    }

    if (calculate_matching) {
      if (deltaR(genVar[kGenTopW],(*jetIter)->p4()) < deltaR(genVarAnti[kGenTopW],(*jetIter)->p4()) ) {
        if ( DR_W_b_gen_AK12 > 0. && deltaR(genVar[kGenTopB], genVar[kGenTopW]) < DR_W_b_gen_AK12) DR_W_b_gen_AK12 = deltaR(genVar[kGenTopB], genVar[kGenTopW]);
      } else if ( DR_W_b_gen_AK12 > 0. && deltaR(genVarAnti[kGenTopB], genVarAnti[kGenTopW]) < DR_W_b_gen_AK12) DR_W_b_gen_AK12 = deltaR(genVarAnti[kGenTopB], genVarAnti[kGenTopW]);
    }

  }
}
if (genTopPt_CSVsort4rd == genTopPt_semi_boosted)  resolved_and_semi = true;
if (genTopPt_semi_boosted == genTopPt_boosted)  boosted_and_semi = true
*/
//std::cout << "semi-boosted HTT " << HTT_semi_boosted << " "
//<< HTT_semi_boosted_WithKinFit  << " "
//<< minDR_AK12_L << " "
//<< minDR_AK12_lep << " "
//<< DR_AK12_tau << " "
//<< std::endl;

//double HTT_semi_boosted_noISO = 0.;
//bool bWj1Wj2_isGenMatched_semi_boosted = false;
//double genTopPt_semi_boosted_noISO = 0.;
//double HadTop_pt_semi_boosted_noISO = 0.;
//bool hadtruth_semi_boosted = false;
//double minDR_AK12_lep_noISO = -1.;
/*
//for (auto i: btag_order) std::cout << i << " ";
for ( std::vector<const RecoJet*>::const_iterator selBJet = cleanedJets_fromAK12.begin(); selBJet != cleanedJets_fromAK12.end(); ++selBJet )  { // cleanedJets.size()
for ( std::vector<const RecoJetAK12*>::const_iterator jetIter = jet_ptrsAK12raw.begin();
      jetIter != jet_ptrsAK12raw.end(); ++jetIter ) {
    if ( !((*jetIter)->subJet1() && (*jetIter)->subJet2()) ) continue;
    bool isGenMatched = false;
    double genTopPt_teste = 0.;
    double HadTop_pt = ((*jetIter)->p4() + (*selBJet)->p4()).pt();
    //bool fatjet_isGenMatched = false;
    const std::map<int, double> bdtResult = (*hadTopTagger_semi_boosted)(**jetIter, **selBJet, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
    //if (isGenMatched) {hadtruth_semi_boosted = true;}

    if ( bdtResult.at(kXGB_semi_boosted_no_kinFit) > HTT_semi_boosted_noISO ) {
      //bWj1Wj2_isGenMatched_semi_boosted = isGenMatched;
      HTT_semi_boosted_noISO = bdtResult.at(kXGB_semi_boosted_no_kinFit);
      HadTop_pt_semi_boosted_noISO = HadTop_pt;
      //genTopPt_semi_boosted = genTopPt_teste;
      minDR_AK12_lep_noISO = std::min(
        deltaR(selLepton_lead->p4(), (*jetIter)->p4()),
        deltaR(selLepton_sublead->p4(), (*jetIter)->p4())
      );
    }

  }
}
*/

// -- semi-boosted hTT -- AK8
double HTT_semi_boosted_fromAK8 = 0.;
bool bWj1Wj2_isGenMatched_semi_boosted_fromAK8 = false;
double genTopPt_semi_boosted_fromAK8 = 0.;
double HadTop_pt_semi_boosted_fromAK8 = 0.;
double W_pt_semi_boosted_fromAK8 = 0.;
double b_pt_semi_boosted_fromAK8 = 0.;

bool hadtruth_semi_boosted_fromAK8 = false;
double minDR_AK8_lep = -1.;
for ( std::vector<const RecoJet*>::const_iterator selBJet = cleanedJets_fromAK8.begin(); selBJet != cleanedJets_fromAK8.end(); ++selBJet )  { // cleanedJets.size()
for ( std::vector<const RecoJetAK8*>::const_iterator jetIter = jet_ptrsAK8.begin();
      jetIter != jet_ptrsAK8.end(); ++jetIter ) {
    bool isGenMatched = false;
    double genTopPt_teste = 0.;
    double HadTop_pt = ((*jetIter)->p4() + (*selBJet)->p4()).pt();
    //bool fatjet_isGenMatched = false;
    const std::map<int, double> bdtResult = (*hadTopTagger_semi_boosted_fromAK8)(**jetIter, **selBJet, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
    if (isGenMatched) {hadtruth_semi_boosted_fromAK8 = true;}

    if ( bdtResult.at(kXGB_semi_boosted_AK8_no_kinFit) > HTT_semi_boosted_fromAK8 ) {
      bWj1Wj2_isGenMatched_semi_boosted_fromAK8 = isGenMatched;
      HTT_semi_boosted_fromAK8 = bdtResult.at(kXGB_semi_boosted_AK8_no_kinFit);
      HadTop_pt_semi_boosted_fromAK8 = HadTop_pt;
      genTopPt_semi_boosted_fromAK8 = genTopPt_teste;
      minDR_AK8_lep = std::min(
        deltaR(selLepton_lead->p4(), (*jetIter)->p4()),
        deltaR(selLepton_sublead->p4(), (*jetIter)->p4())
      );
      b_pt_semi_boosted_fromAK8 = (*selBJet)->pt() ;
      W_pt_semi_boosted_fromAK8 = (*jetIter)->pt() ;
    }

    if (calculate_matching) {
      if (deltaR(genVar[kGenTopW],(*jetIter)->p4()) < deltaR(genVarAnti[kGenTopW],(*jetIter)->p4()) ) {
        if ( DR_W_b_gen_AK8 > 0. && deltaR(genVar[kGenTopB], genVar[kGenTopW]) < DR_W_b_gen_AK8) DR_W_b_gen_AK8 = deltaR(genVar[kGenTopB], genVar[kGenTopW]);
      } else if ( DR_W_b_gen_AK8 > 0. && deltaR(genVarAnti[kGenTopB], genVarAnti[kGenTopW]) < DR_W_b_gen_AK8) DR_W_b_gen_AK8 = deltaR(genVarAnti[kGenTopB], genVarAnti[kGenTopW]);
    }

  }
}
if (genTopPt_CSVsort4rd == genTopPt_semi_boosted_fromAK8)  resolved_and_semi_AK8 = true;
if (genTopPt_semi_boosted_fromAK8 == genTopPt_boosted)  boosted_and_semi_AK8 = true;

double HTT_semi_boosted_fromAK8_noISO = 0.;
//bool bWj1Wj2_isGenMatched_semi_boosted_fromAK8 = false;
//double genTopPt_semi_boosted_fromAK8 = 0.;
//double HadTop_pt_semi_boosted_fromAK8_noISO = 0.;
for ( std::vector<const RecoJet*>::const_iterator selBJet = cleanedJets_fromAK8.begin(); selBJet != cleanedJets_fromAK8.end(); ++selBJet )  { // cleanedJets.size()
for ( std::vector<const RecoJetAK8*>::const_iterator jetIter = jet_ptrsAK8raw.begin();
      jetIter != jet_ptrsAK8raw.end(); ++jetIter ) {
    bool isGenMatched = false;
    double genTopPt_teste = 0.;
    //double HadTop_pt = ((*jetIter)->p4() + (*selBJet)->p4()).pt();
    //bool fatjet_isGenMatched = false;
    const std::map<int, double> bdtResult = (*hadTopTagger_semi_boosted_fromAK8)(**jetIter, **selBJet, calculate_matching, isGenMatched, genTopPt_teste, genVar, genVarAnti);
    //if (isGenMatched) {hadtruth_semi_boosted_fromAK8 = true;}
    if ( bdtResult.at(kXGB_semi_boosted_AK8_no_kinFit) > HTT_semi_boosted_fromAK8_noISO ) {
      //bWj1Wj2_isGenMatched_semi_boosted_fromAK8 = isGenMatched;
      HTT_semi_boosted_fromAK8_noISO = bdtResult.at(kXGB_semi_boosted_AK8_no_kinFit);
      //HadTop_pt_semi_boosted_fromAK8_noISO = HadTop_pt;
      //genTopPt_semi_boosted_fromAK8 = genTopPt_teste;
    }

  }
}

// Wj1_pt_multilep_1,  Wj2_pt_multilep_1, b_pt_multilep_1
std::map<std::string, double> mvaInputs_Hj_tagger;
double mvaOutput_Hj_tagger = 0.;
for ( std::vector<const RecoJet*>::const_iterator selJet = selJets.begin();
selJet != selJets.end(); ++selJet ) {
  if ((*selJet)->pt()==Wj1_pt_multilep_1 || (*selJet)->pt()==Wj2_pt_multilep_1 || (*selJet)->pt()==b_pt_multilep_1) continue;
  double mvaOutput = comp_mvaOutput_Hj_tagger(
    *selJet, fakeableLeptons,
mvaInputs_Hj_tagger, mva_Hj_tagger,
    eventInfo);
  if ( mvaOutput > mvaOutput_Hj_tagger ) mvaOutput_Hj_tagger = mvaOutput;
}

//std::map<std::string, double> Hj_tagger_fromCSVsort4th;
double Hj_tagger_fromCSVsort4th = 0.;
for ( std::vector<const RecoJet*>::const_iterator selJet = selJets.begin();
selJet != selJets.end(); ++selJet ) {
  if ((*selJet)->pt()==Wj1_pt_CSVsort4rd_1 || (*selJet)->pt()==Wj2_pt_CSVsort4rd_1 || (*selJet)->pt()==b_pt_CSVsort4rd_1) continue;
  double mvaOutput = comp_mvaOutput_Hj_tagger(
    *selJet, fakeableLeptons, mvaInputs_Hj_tagger, mva_Hj_tagger,
    eventInfo);
  if ( mvaOutput > Hj_tagger_fromCSVsort4th ) Hj_tagger_fromCSVsort4th = mvaOutput;
}

//--- compute output of BDTs used to discriminate ttH vs. ttV and ttH vs. ttbar
//    in 3l category of ttH multilepton analysis
    const double lep1_conePt = comp_lep1_conePt(*selLepton_lead);
    const double lep2_conePt = comp_lep2_conePt(*selLepton_sublead);
    const double lep3_conePt = comp_lep3_conePt(*selLepton_third);
    const double mindr_lep1_jet = comp_mindr_lep1_jet(*selLepton_lead, selJets);
    const double mindr_lep2_jet = comp_mindr_lep2_jet(*selLepton_sublead, selJets);
    const double mindr_lep3_jet = comp_mindr_lep3_jet(*selLepton_third, selJets);
    const double avg_dr_jet = comp_avg_dr_jet(selJets);
    const double max_lep12_eta = std::max(selLepton_lead->absEta(), selLepton_sublead->absEta());
    const double mT_lep1           = comp_MT_met_lep1(selLepton_lead->p4(), met.pt(), met.phi());
    const double mT_lep2           = comp_MT_met_lep2(selLepton_sublead->p4(), met.pt(), met.phi());
    const double mT_lep3           = comp_MT_met_lep3(selLepton_third->p4(), met.pt(), met.phi());
    const double max_dr_jet        = comp_max_dr_jet(selJets);
    const double mbb               = selBJets_medium.size() > 1 ? (selBJets_medium[0]->p4() + selBJets_medium[1]->p4()).mass() : -1.;
    const double mbb_loose         = selBJets_loose.size() > 1 ? (selBJets_loose[0]->p4() + selBJets_loose[1]->p4()).mass() : -1.;
    const double min_dr_lep_jet    = std::min({ mindr_lep1_jet, mindr_lep2_jet, mindr_lep3_jet });
    const double dr_leps           = deltaR(selLepton_lead->p4(), selLepton_sublead->p4());
    const double max_lep_eta       = std::max({ selLepton_lead->absEta(), selLepton_sublead->absEta(), selLepton_third->absEta() });
    double dr_lss  = -1.;
    double dr_los1 = -1.;
    double dr_los2 = -1.;
    // it does not assume mis-charge identification
    if (selLepton_lead->charge()*selLepton_sublead->charge() > 0){
      dr_lss  = deltaR(selLepton_sublead -> p4(), selLepton_lead -> p4());
      dr_los1 = deltaR(selLepton_third -> p4(), selLepton_lead -> p4());
      dr_los2 = deltaR(selLepton_third  -> p4(), selLepton_sublead -> p4());
    } else {
      dr_lss  = deltaR(selLepton_third -> p4(), selLepton_lead -> p4());
      dr_los1 = deltaR(selLepton_sublead -> p4(), selLepton_third -> p4());
      dr_los2 = deltaR(selLepton_sublead  -> p4(), selLepton_lead -> p4());
    }

    mvaInputs_3l["max(abs(LepGood_eta[iLepFO_Recl[0]]),abs(LepGood_eta[iLepFO_Recl[1]]))"] = max_lep12_eta;
    mvaInputs_3l["MT_met_lep1"]                = comp_MT_met_lep1(selLepton_lead->cone_p4(), met.pt(), met.phi());
    mvaInputs_3l["nJet25_Recl"]                = comp_n_jet25_recl(selJets);
    mvaInputs_3l["mindr_lep1_jet"]             = mindr_lep1_jet;
    mvaInputs_3l["mindr_lep2_jet"]             = mindr_lep2_jet;
    mvaInputs_3l["LepGood_conePt[iLepFO_Recl[0]]"] = lep1_conePt;
    mvaInputs_3l["LepGood_conePt[iLepFO_Recl[2]]"] = lep3_conePt;
    //mvaInputs_3l["avg_dr_jet"]                 = avg_dr_jet;
    //mvaInputs_3l["mhtJet25_Recl"]              = mht_p4.pt();

    check_mvaInputs(mvaInputs_3l, eventInfo);

    double mvaOutput_3l_ttV = mva_3l_ttV(mvaInputs_3l);
    double mvaOutput_3l_ttbar = mva_3l_ttbar(mvaInputs_3l);
    int ibin = hTargetBinning->FindBin(mvaOutput_3l_ttbar, mvaOutput_3l_ttV);
    Double_t mvaDiscr_3l = hTargetBinning->GetBinContent(ibin);
    //std::cout << "mvaDiscr_3l = " << mvaDiscr_3l << std::endl;

    /*
//--- compute integer discriminant based on both BDT outputs,
//    as defined in Table 16 (10) of AN-2015/321 (AN-2016/211) for analysis of 2015 (2016) data
    Double_t mvaDiscr_3l = -1;
    if(era == kEra_2016 || era == kEra_2017)
    {
      if      ( mvaOutput_3l_ttbar > +0.30 && mvaOutput_3l_ttV >  +0.25 ) mvaDiscr_3l = 5.;
      else if ( mvaOutput_3l_ttbar > +0.30 && mvaOutput_3l_ttV <= +0.25 ) mvaDiscr_3l = 4.;
      else if ( mvaOutput_3l_ttbar > -0.30 && mvaOutput_3l_ttV >  +0.25 ) mvaDiscr_3l = 3.;
      else if ( mvaOutput_3l_ttbar > -0.30 && mvaOutput_3l_ttV <= +0.25 ) mvaDiscr_3l = 2.;
      else                                                                mvaDiscr_3l = 1.;
    } else assert(0);
    */

    // BDT with boosted stuff
    //mvaInputs_XGB_Boosted_AK12_basic
    /*
    mvaInputs_XGB_Boosted_AK12_basic["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Boosted_AK12_basic["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Boosted_AK12_basic["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Boosted_AK12_basic["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Boosted_AK12_basic["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Boosted_AK12_basic["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Boosted_AK12_basic["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Boosted_AK12_basic["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Boosted_AK12_basic["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Boosted_AK12_basic["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Boosted_AK12_basic["nJet"] = selJets.size();
    mvaInputs_XGB_Boosted_AK12_basic["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12_basic["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12_basic["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Boosted_AK12_basic["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Boosted_AK12_basic["dr_lss"] = dr_lss;
    mvaInputs_XGB_Boosted_AK12_basic["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Boosted_AK12_basic["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Boosted_AK12_basic["ptmiss"] = met.pt();
    mvaInputs_XGB_Boosted_AK12_basic["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_Boosted_AK12_basic["N_jetAK12"] = jet_ptrsAK12.size();
    */
    double mva_Boosted_AK12_basic = 1.0; //mva_XGB_Boosted_AK12_basic(mvaInputs_XGB_Boosted_AK12_basic);
    //std::cout<<" mva_Boosted_AK12_basic "<<mva_Boosted_AK12_basic<<std::endl;

    // mvaInputs_XGB_Boosted_AK12
    /*
    mvaInputs_XGB_Boosted_AK12["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Boosted_AK12["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Boosted_AK12["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Boosted_AK12["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Boosted_AK12["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Boosted_AK12["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Boosted_AK12["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Boosted_AK12["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Boosted_AK12["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Boosted_AK12["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Boosted_AK12["nJet"] = selJets.size();
    mvaInputs_XGB_Boosted_AK12["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Boosted_AK12["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Boosted_AK12["dr_lss"] = dr_lss;
    mvaInputs_XGB_Boosted_AK12["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Boosted_AK12["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Boosted_AK12["ptmiss"] = met.pt();
    mvaInputs_XGB_Boosted_AK12["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_Boosted_AK12["HTT_semi_boosted"] = HTT_semi_boosted;
    mvaInputs_XGB_Boosted_AK12["HTT_boosted"] = HTT_boosted;
    */
    double mva_Boosted_AK12 = 1.0; // mva_XGB_Boosted_AK12(mvaInputs_XGB_Boosted_AK12);
    //std::cout<<" mva_Boosted_AK12 "<<mva_Boosted_AK12<<std::endl;

    // mvaInputs_XGB_AK12
    /*
    mvaInputs_XGB_AK12["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_AK12["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_AK12["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_AK12["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_AK12["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_AK12["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_AK12["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_AK12["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_AK12["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_AK12["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_AK12["nJet"] = selJets.size();
    mvaInputs_XGB_AK12["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_AK12["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_AK12["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_AK12["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_AK12["dr_lss"] = dr_lss;
    mvaInputs_XGB_AK12["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_AK12["nElectron"] = selElectrons.size();
    mvaInputs_XGB_AK12["ptmiss"] = met.pt();
    mvaInputs_XGB_AK12["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_AK12["resolved_and_semi"] = resolved_and_semi;
    mvaInputs_XGB_AK12["HTT_semi_boosted"] = HTT_semi_boosted;
    */
    double mva_AK12 = 1.0; // mva_XGB_AK12(mvaInputs_XGB_AK12);
    //std::cout<<" mva_AK12 "<<mva_AK12<<std::endl;

    // mvaInputs_XGB_Boosted_AK12_noISO
    /*
    mvaInputs_XGB_Boosted_AK12_noISO["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Boosted_AK12_noISO["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Boosted_AK12_noISO["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Boosted_AK12_noISO["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Boosted_AK12_noISO["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Boosted_AK12_noISO["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Boosted_AK12_noISO["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Boosted_AK12_noISO["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Boosted_AK12_noISO["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Boosted_AK12_noISO["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Boosted_AK12_noISO["nJet"] = selJets.size();
    mvaInputs_XGB_Boosted_AK12_noISO["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12_noISO["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK12_noISO["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Boosted_AK12_noISO["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Boosted_AK12_noISO["dr_lss"] = dr_lss;
    mvaInputs_XGB_Boosted_AK12_noISO["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Boosted_AK12_noISO["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Boosted_AK12_noISO["ptmiss"] = met.pt();
    mvaInputs_XGB_Boosted_AK12_noISO["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_Boosted_AK12_noISO["minDR_AK12_lep"] = minDR_AK12_lep_noISO;
    mvaInputs_XGB_Boosted_AK12_noISO["HTT_semi_boosted"] = HTT_semi_boosted_noISO;
    mvaInputs_XGB_Boosted_AK12_noISO["HadTop_pt_semi_boosted"] = HadTop_pt_semi_boosted_noISO;
    */
    double mva_Boosted_AK12_noISO = 1.0; // mva_XGB_Boosted_AK12_noISO(mvaInputs_XGB_Boosted_AK12_noISO);
    //std::cout<<" mva_Boosted_AK12_noISO "<<mva_Boosted_AK12_noISO<<std::endl;

    //mvaInputs_XGB_Boosted_AK8
    mvaInputs_XGB_Boosted_AK8["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Boosted_AK8["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Boosted_AK8["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Boosted_AK8["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Boosted_AK8["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Boosted_AK8["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Boosted_AK8["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Boosted_AK8["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Boosted_AK8["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Boosted_AK8["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Boosted_AK8["nJet"] = selJets.size();
    mvaInputs_XGB_Boosted_AK8["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK8["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK8["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Boosted_AK8["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Boosted_AK8["dr_lss"] = dr_lss;
    mvaInputs_XGB_Boosted_AK8["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Boosted_AK8["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Boosted_AK8["ptmiss"] = met.pt();
    mvaInputs_XGB_Boosted_AK8["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_Boosted_AK8["HTT_boosted"] = HTT_boosted;
    mvaInputs_XGB_Boosted_AK8["HTT_semi_boosted_fromAK8"] = HTT_semi_boosted_fromAK8;
    double mva_Boosted_AK8 = mva_XGB_Boosted_AK8(mvaInputs_XGB_Boosted_AK8);
    //std::cout<<" mva_Boosted_AK8 "<<mva_Boosted_AK8<<std::endl;

    // mvaInputs_XGB_Boosted_AK8_noISO
    mvaInputs_XGB_Boosted_AK8_noISO["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Boosted_AK8_noISO["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Boosted_AK8_noISO["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Boosted_AK8_noISO["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Boosted_AK8_noISO["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Boosted_AK8_noISO["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Boosted_AK8_noISO["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Boosted_AK8_noISO["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Boosted_AK8_noISO["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Boosted_AK8_noISO["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Boosted_AK8_noISO["nJet"] = selJets.size();
    mvaInputs_XGB_Boosted_AK8_noISO["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK8_noISO["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Boosted_AK8_noISO["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Boosted_AK8_noISO["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Boosted_AK8_noISO["dr_lss"] = dr_lss;
    mvaInputs_XGB_Boosted_AK8_noISO["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Boosted_AK8_noISO["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Boosted_AK8_noISO["ptmiss"] = met.pt();
    mvaInputs_XGB_Boosted_AK8_noISO["sumLeptonCharge"] = sumLeptonCharge;
    mvaInputs_XGB_Boosted_AK8_noISO["HTT_boosted"] = HTT_boosted_noISO;
    mvaInputs_XGB_Boosted_AK8_noISO["HTT_semi_boosted_fromAK8"] = HTT_semi_boosted_fromAK8_noISO;
    double mva_Boosted_AK8_noISO = mva_XGB_Boosted_AK8_noISO(mvaInputs_XGB_Boosted_AK8_noISO);
    //std::cout<<" mva_Boosted_AK8_noISO "<<mva_Boosted_AK8_noISO<<std::endl;

    // mvaInputs_XGB_Updated
    mvaInputs_XGB_Updated["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_Updated["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_Updated["lep3_conePt"] = lep3_conePt;
    mvaInputs_XGB_Updated["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_Updated["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_Updated["mindr_lep3_jet"] = mindr_lep3_jet;
    mvaInputs_XGB_Updated["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_Updated["mT_lep2"] = mT_lep2;
    mvaInputs_XGB_Updated["mT_lep3"] = mT_lep3;
    mvaInputs_XGB_Updated["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_Updated["nJet"] = selJets.size();
    mvaInputs_XGB_Updated["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_Updated["HadTop_pt_CSVsort4rd"] = HadTop_pt_CSVsort4rd;
    mvaInputs_XGB_Updated["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    mvaInputs_XGB_Updated["min_lep_dr_os"] = std::min(dr_los1, dr_los2);
    mvaInputs_XGB_Updated["dr_lss"] = dr_lss;
    mvaInputs_XGB_Updated["nBJetMedium"] = selBJets_medium.size();
    mvaInputs_XGB_Updated["nElectron"] = selElectrons.size();
    mvaInputs_XGB_Updated["ptmiss"] = met.pt();
    mvaInputs_XGB_Updated["sumLeptonCharge"] = sumLeptonCharge;
    double mva_Updated = mva_XGB_Updated(mvaInputs_XGB_Updated);
    //std::cout<<" mva_Updated "<<mva_Updated<<std::endl;

    //mvaInputs_XGB_oldVar
    mvaInputs_XGB_oldVar["lep1_conePt"] = lep1_conePt;
    mvaInputs_XGB_oldVar["lep2_conePt"] = lep2_conePt;
    mvaInputs_XGB_oldVar["mindr_lep1_jet"] = mindr_lep1_jet;
    mvaInputs_XGB_oldVar["mindr_lep2_jet"] = mindr_lep2_jet;
    mvaInputs_XGB_oldVar["mT_lep1"] = mT_lep1;
    mvaInputs_XGB_oldVar["max_lep_eta"] = max_lep_eta;
    mvaInputs_XGB_oldVar["nJet"] = selJets.size();
    mvaInputs_XGB_oldVar["res-HTT_CSVsort4rd"] = max_mvaOutput_HTT_CSVsort4rd;
    mvaInputs_XGB_oldVar["Hj_tagger_fromCSVsort4th"] = Hj_tagger_fromCSVsort4th;
    double mva_oldVar = mva_XGB_oldVar(mvaInputs_XGB_oldVar);
    //std::cout<<" mva_oldVar "<<mva_oldVar<<std::endl;

//--- fill histograms with events passing final selection
    std::string category = "3l_0tau";
    if ( selBJets_medium.size() >= 2 ) category += "_bt";
    else category += "_bl";
    if      ( sumLeptonCharge < 0 ) category += "_neg";
    else if ( sumLeptonCharge > 0 ) category += "_pos";

    std::string categoryFat = "3l_0tau";
    if      ( sel_HTTv2.size() > 0 ) categoryFat += "_1pHTTv2";
    else if ( jet_ptrsAK8.size() > 0 ) categoryFat += "_1pAK8";
    else {
      if ( selBJets_medium.size() >= 2 ) categoryFat += "_bt";
      else categoryFat += "_bl";
      if      ( sumLeptonCharge < 0 ) categoryFat += "_neg";
      else if ( sumLeptonCharge > 0 ) categoryFat += "_pos";
      categoryFat += "_0J";
    }
    //std::cout<<category<<" "<<categoryFat<<std::endl;

    selHistManagerType* selHistManager = selHistManagers[idxSelLepton_genMatch];
    assert(selHistManager != 0);

    selHistManager->electrons_->fillHistograms(selElectrons, evtWeight);
    ElectronHistManager* selHistManager_electrons_category = selHistManager->electrons_in_categories_[category];
    if ( selHistManager_electrons_category ) {
      selHistManager_electrons_category->fillHistograms(selElectrons, evtWeight);
    }
    selHistManager->muons_->fillHistograms(selMuons, evtWeight);
    MuonHistManager* selHistManager_muons_category = selHistManager->muons_in_categories_[category];
    if ( selHistManager_muons_category ) {
      selHistManager_muons_category->fillHistograms(selMuons, evtWeight);
    }
    selHistManager->hadTaus_->fillHistograms(selHadTaus, evtWeight);
    selHistManager->jets_->fillHistograms(selJets, evtWeight);
    selHistManager->leadJet_->fillHistograms(selJets, evtWeight);
    selHistManager->subleadJet_->fillHistograms(selJets, evtWeight);
    selHistManager->BJets_loose_->fillHistograms(selBJets_loose, evtWeight);
    selHistManager->leadBJet_loose_->fillHistograms(selBJets_loose, evtWeight);
    selHistManager->subleadBJet_loose_->fillHistograms(selBJets_loose, evtWeight);
    selHistManager->BJets_medium_->fillHistograms(selBJets_medium, evtWeight);
    selHistManager->met_->fillHistograms(met, mht_p4, met_LD, evtWeight);
    selHistManager->metFilters_->fillHistograms(metFilters, evtWeight);
    selHistManager->mvaInputVariables_3l_->fillHistograms(mvaInputs_3l, evtWeight);
    selHistManager->evt_->fillHistograms(
      selElectrons.size(), selMuons.size(), selHadTaus.size(),
      selJets.size(), selBJets_loose.size(), selBJets_medium.size(),
      mvaOutput_3l_ttV, mvaOutput_3l_ttbar, mvaDiscr_3l,
      mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
      mva_Boosted_AK8, mva_Boosted_AK8_noISO,
      mva_Updated, mva_oldVar,
      evtWeight);
    EvtHistManager_3l* selHistManager_evt_category = selHistManager->evt_in_categories_[category];
    if ( selHistManager_evt_category ) { // CV: pointer is zero when running on OS control region to estimate "charge_flip" background
      selHistManager_evt_category->fillHistograms(
      selElectrons.size(), selMuons.size(), selHadTaus.size(),
      selJets.size(), selBJets_loose.size(), selBJets_medium.size(),
      mvaOutput_3l_ttV, mvaOutput_3l_ttbar, mvaDiscr_3l,
      mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
      mva_Boosted_AK8, mva_Boosted_AK8_noISO,
      mva_Updated, mva_oldVar,
      evtWeight);
    }
    EvtHistManager_3l* selHistManager_evt_categoryFat = selHistManager->evt_in_categoriesFat_[categoryFat];
    if ( selHistManager_evt_categoryFat ) { // CV: pointer is zero when running on OS control region to estimate "charge_flip" background
      selHistManager_evt_categoryFat->fillHistograms(
      selElectrons.size(), selMuons.size(), selHadTaus.size(),
      selJets.size(), selBJets_loose.size(), selBJets_medium.size(),
      mvaOutput_3l_ttV, mvaOutput_3l_ttbar, mvaDiscr_3l,
      mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
      mva_Boosted_AK8, mva_Boosted_AK8_noISO,
      mva_Updated, mva_oldVar,
      evtWeight);
    }
    if(isSignal)
    {
      const std::string decayModeStr = eventInfo.getDecayModeString();
      if(! decayModeStr.empty())
      {
        selHistManager -> evt_in_decayModes_[decayModeStr] -> fillHistograms(
          selElectrons.size(),
          selMuons.size(),
          selHadTaus.size(),
          selJets.size(),
          selBJets_loose.size(),
          selBJets_medium.size(),
          mvaOutput_3l_ttV,
          mvaOutput_3l_ttbar,
          mvaDiscr_3l,
          mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
          mva_Boosted_AK8, mva_Boosted_AK8_noISO,
          mva_Updated, mva_oldVar,
          evtWeight
        );
  EvtHistManager_3l* selHistManager_evt_category_decMode = selHistManager->evt_in_categories_and_decayModes_[category][decayModeStr];
  if ( selHistManager_evt_category_decMode ) { // CV: pointer is zero when running on OS control region to estimate "charge_flip" background
          selHistManager_evt_category_decMode->fillHistograms(
	    selElectrons.size(),
            selMuons.size(),
	    selHadTaus.size(),
	    selJets.size(),
	    selBJets_loose.size(),
	    selBJets_medium.size(),
	    mvaOutput_3l_ttV,
	    mvaOutput_3l_ttbar,
	    mvaDiscr_3l,
      mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
      mva_Boosted_AK8, mva_Boosted_AK8_noISO,
      mva_Updated, mva_oldVar,
	    evtWeight
	  );
	}
  EvtHistManager_3l* selHistManager_evt_categoryFat_and_decayModes_ = selHistManager->evt_in_categoriesFat_and_decayModes_[categoryFat][decayModeStr];
	if ( selHistManager_evt_categoryFat_and_decayModes_ ) { // CV: pointer is zero when running on OS control region to estimate "charge_flip" background
          selHistManager_evt_categoryFat_and_decayModes_->fillHistograms(
	    selElectrons.size(),
            selMuons.size(),
	    selHadTaus.size(),
	    selJets.size(),
	    selBJets_loose.size(),
	    selBJets_medium.size(),
	    mvaOutput_3l_ttV,
	    mvaOutput_3l_ttbar,
	    mvaDiscr_3l,
      mva_Boosted_AK12_basic, mva_Boosted_AK12, mva_AK12, mva_Boosted_AK12_noISO,
      mva_Boosted_AK8, mva_Boosted_AK8_noISO,
      mva_Updated, mva_oldVar,
	    evtWeight
	  );
	}
      }
    }
    selHistManager->evtYield_->fillHistograms(eventInfo, evtWeight);
    selHistManager->weights_->fillHistograms("genWeight", eventInfo.genWeight);
    selHistManager->weights_->fillHistograms("pileupWeight", eventInfo.pileupWeight);
    selHistManager->weights_->fillHistograms("triggerWeight", triggerWeight);
    selHistManager->weights_->fillHistograms("data_to_MC_correction", weight_data_to_MC_correction);
    selHistManager->weights_->fillHistograms("fakeRate", weight_fakeRate);

    if ( isMC ) {
      genEvtHistManager_afterCuts->fillHistograms(genElectrons, genMuons, genHadTaus, genPhotons, genJets, evtWeight_inclusive);
      lheInfoHistManager->fillHistograms(*lheInfoReader, evtWeight);
      if(eventWeightManager)
      {
        genEvtHistManager_afterCuts->fillHistograms(eventWeightManager, evtWeight_inclusive);
      }
    }

    if ( selEventsFile ) {
      (*selEventsFile) << eventInfo.run << ':' << eventInfo.lumi << ':' << eventInfo.event << '\n';
    }

    const bool isGenMatched = isMC &&
      ((apply_leptonGenMatching && selLepton_genMatch.numGenMatchedLeptons_ == 3) || ! apply_leptonGenMatching)
    ;

    if ( bdt_filler ) {
      double lep1_genLepPt = ( selLepton_lead->genLepton()    ) ? selLepton_lead->genLepton()->pt()    : 0.;
      double lep2_genLepPt = ( selLepton_sublead->genLepton() ) ? selLepton_sublead->genLepton()->pt() : 0.;
      double lep3_genLepPt = ( selLepton_third->genLepton()   ) ? selLepton_third->genLepton()->pt()   : 0.;

      //FR weights for bdt ntuple
      std::cout << "filling bdt tree "<< applyFakeRateWeights_string << std::endl;
      double prob_fake_lepton_lead = 1.;
      if      ( std::abs(selLepton_lead->pdgId()) == 11 ) prob_fake_lepton_lead = leptonFakeRateInterface->getWeight_e(selLepton_lead->cone_pt(), selLepton_lead->absEta());
      else if ( std::abs(selLepton_lead->pdgId()) == 13 ) prob_fake_lepton_lead = leptonFakeRateInterface->getWeight_mu(selLepton_lead->cone_pt(), selLepton_lead->absEta());
      double prob_fake_lepton_sublead = 1.;
      if      ( std::abs(selLepton_sublead->pdgId()) == 11 ) prob_fake_lepton_sublead = leptonFakeRateInterface->getWeight_e(selLepton_sublead->cone_pt(), selLepton_sublead->absEta());
      else if ( std::abs(selLepton_sublead->pdgId()) == 13 ) prob_fake_lepton_sublead = leptonFakeRateInterface->getWeight_mu(selLepton_sublead->cone_pt(), selLepton_sublead->absEta());
      double prob_fake_lepton_third = 1.;
      if      ( std::abs(selLepton_third->pdgId()) == 11 ) prob_fake_lepton_third = leptonFakeRateInterface->getWeight_e(selLepton_third->cone_pt(), selLepton_third->absEta());
      else if ( std::abs(selLepton_third->pdgId()) == 13 ) prob_fake_lepton_third = leptonFakeRateInterface->getWeight_mu(selLepton_third->cone_pt(), selLepton_third->absEta());

      bdt_filler -> operator()({ eventInfo.run, eventInfo.lumi, eventInfo.event })
          ("lep1_pt",             selLepton_lead -> pt())
          ("nElectron",           selElectrons.size())
          ("sumLeptonCharge",     sumLeptonCharge)
          ("nMuon",               selMuons.size())
          ("lep1_conePt",         lep1_conePt)
          ("lep1_eta",            selLepton_lead -> eta())
          ("lep1_tth_mva",        selLepton_lead -> mvaRawTTH())
          ("mindr_lep1_jet",      TMath::Min(10., mindr_lep1_jet))
          ("mT_lep1",             comp_MT_met_lep1(*selLepton_lead, met.pt(), met.phi()))
          ("lep2_pt",             selLepton_sublead -> pt())
          ("lep2_conePt",         lep2_conePt)
          ("lep2_eta",            selLepton_sublead -> eta())
          ("lep2_tth_mva",        selLepton_sublead -> mvaRawTTH())
          ("mindr_lep2_jet",      TMath::Min(10., mindr_lep2_jet))
          ("mT_lep2",             comp_MT_met_lep1(*selLepton_sublead, met.pt(), met.phi()))
          ("lep3_pt",             selLepton_third -> pt())
          ("lep3_conePt",         lep3_conePt)
          ("lep3_eta",            selLepton_third -> eta())
          ("lep3_tth_mva",        selLepton_third -> mvaRawTTH())
          ("mindr_lep3_jet",      TMath::Min(10., mindr_lep3_jet))
          ("mT_lep3",             comp_MT_met_lep1(*selLepton_third, met.pt(), met.phi()))
          ("avg_dr_jet",          avg_dr_jet)
          ("ptmiss",              met.pt())
          ("htmiss",              mht_p4.pt())
          ("dr_leps",             deltaR(selLepton_lead -> p4(), selLepton_sublead -> p4()))
          ("lep1_genLepPt",       (selLepton_lead->genLepton() != 0) ? selLepton_lead->genLepton()->pt() : 0.)
          ("lep2_genLepPt",       (selLepton_sublead->genLepton() != 0) ? selLepton_sublead->genLepton() ->pt() : 0.)
          ("lep3_genLepPt",       (selLepton_third->genLepton() != 0) ? selLepton_third->genLepton() ->pt() : 0.)
          ("lep1_frWeight",       lep1_genLepPt > 0 ? 1.0 : prob_fake_lepton_lead)
          ("lep2_frWeight",       lep2_genLepPt > 0 ? 1.0 : prob_fake_lepton_sublead)
          ("lep3_frWeight",       lep3_genLepPt > 0 ? 1.0 : prob_fake_lepton_third)
          ("lep1_fake_prob",      prob_fake_lepton_lead)
          ("lep2_fake_prob",      prob_fake_lepton_sublead)
          ("lep3_fake_prob",      prob_fake_lepton_third)
          ("mvaOutput_3l_ttV",    mvaOutput_3l_ttV)
          ("mvaOutput_3l_ttbar",  mvaOutput_3l_ttbar)
          ("mvaDiscr_3l",         mvaDiscr_3l)
          ("lumiScale",           lumiScale)
          ("genWeight",           eventInfo.genWeight)
          ("evtWeight",           evtWeight)
          ("mbb_loose",           selBJets_loose.size()>1 ?  (selBJets_loose[0]->p4()+selBJets_loose[1]->p4()).mass() : -1000 )
          ("mbb_medium",          selBJets_medium.size()>1 ?  (selBJets_medium[0]->p4()+selBJets_medium[1]->p4()).mass() : -1000 )
          ("nJet",                selJets.size())
          ("nBJetLoose",          selBJets_loose.size())
          ("nBJetMedium",         selBJets_medium.size())
          ("lep1_isTight",        int(selLepton_lead -> isTight()))
          ("lep2_isTight",        int(selLepton_sublead -> isTight()))
          ("lep3_isTight",        int(selLepton_third -> isTight()))
          ("dr_lss",              dr_lss)
          ("dr_los1",             dr_los1)
          ("dr_los2",             dr_los2)

          ("hadtruth",               hadtruth)

          ("res-HTT",                max_mvaOutput_HTT_2016)
          ("HadTop_pt",              HadTop_pt_2016)
          ("genTopPt", genTopPt_2016)

          ("bWj1Wj2_isGenMatchedWithKinFit", max_truth_HTT_2016)
          ("bWj1Wj2_isGenMatched_IHEP",                    max_truth_multilep)
          ("bWj1Wj2_isGenMatched_CSVsort4rd",              max_truth_HTT_CSVsort4rd)
          ("bWj1Wj2_isGenMatched_highestCSV",              max_truth_HTT_highestCSV)
          ("bWj1Wj2_isGenMatched_CSVsort4rd_WithKinFit",   max_truth_HTT_CSVsort4rd_WithKinFit)
          ("bWj1Wj2_isGenMatched_highestCSV_WithKinFit",   max_truth_HTT_highestCSV_WithKinFit)

          ("res-HTT_CSVsort4rd",                 max_mvaOutput_HTT_CSVsort4rd)
          ("res-HTT_highestCSV",                 max_mvaOutput_HTT_highestCSV)
          ("res-HTT_CSVsort4rd_WithKinFit",      max_mvaOutput_HTT_CSVsort4rd_WithKinFit)
          ("res-HTT_highestCSV_WithKinFit",      max_mvaOutput_HTT_highestCSV_WithKinFit)
          ("res-HTT_IHEP",                       max_mvaOutput_HTT_multilep)

          ("HadTop_pt_multilep",              HadTop_pt_multilep)
          ("HadTop_pt_CSVsort4rd",            HadTop_pt_CSVsort4rd)
          ("HadTop_pt_highestCSV",            HadTop_pt_highestCSV)
          ("HadTop_pt_CSVsort4rd_WithKinFit", HadTop_pt_CSVsort4rd_WithKinFit)
          ("HadTop_pt_highestCSV_WithKinFit", HadTop_pt_highestCSV_WithKinFit)

          ("genTopPt_multilep",               genTopPt_multilep)
          ("genTopPt_CSVsort4rd",             genTopPt_CSVsort4rd)
          ("genTopPt_highestCSV",             genTopPt_highestCSV)
          ("genTopPt_CSVsort4rd_WithKinFit",  genTopPt_CSVsort4rd_WithKinFit)
          ("genTopPt_highestCSV_WithKinFit",  genTopPt_highestCSV_WithKinFit)


          ("hadtruth_boosted",               hadtruth_boosted)
          ("hadtruth_semi_boosted",               hadtruth_semi_boosted)

          ("nHTTv2",                         sel_HTTv2.size())
          ("HTTv2_lead_pt",                  sel_HTTv2.size() > 0 ? sel_HTTv2[0]->pt() : -1 )
          ("minDR_HTTv2_lep",                minDR_HTTv2_lep)

          ("HTT_boosted",                     HTT_boosted)
          ("bWj1Wj2_isGenMatched_boosted",    bWj1Wj2_isGenMatched_boosted)
          ("genTopPt_boosted",            genTopPt_boosted)
          ("HadTop_pt_boosted",           HadTop_pt_boosted)
          ("HTT_boosted_WithKinFit",      HTT_boosted_WithKinFit)
          ("bWj1Wj2_isGenMatched_boosted_WithKinFit", bWj1Wj2_isGenMatched_boosted_WithKinFit)
          ("genTopPt_boosted_WithKinFit",         genTopPt_boosted_WithKinFit)
          ("HadTop_pt_boosted_WithKinFit",        HadTop_pt_boosted_WithKinFit)

          ("HTT_semi_boosted",                     HTT_semi_boosted)
          ("bWj1Wj2_isGenMatched_semi_boosted",    bWj1Wj2_isGenMatched_semi_boosted)
          ("genTopPt_semi_boosted",            genTopPt_semi_boosted)
          ("HadTop_pt_semi_boosted",           HadTop_pt_semi_boosted)
          ("HTT_semi_boosted_WithKinFit",                  HTT_semi_boosted_WithKinFit)
          ("bWj1Wj2_isGenMatched_semi_boosted_WithKinFit", bWj1Wj2_isGenMatched_semi_boosted_WithKinFit)
          ("genTopPt_semi_boosted_WithKinFit",         genTopPt_semi_boosted_WithKinFit)
          ("HadTop_pt_semi_boosted_WithKinFit",        HadTop_pt_semi_boosted_WithKinFit)

          //("N_jetAK12",     jet_ptrsAK12.size())
          //("AK12_lead_pt",                  jet_ptrsAK12.size() > 0 ? jet_ptrsAK12[0]->pt() : -1 )
          ("N_jetAK12",            0 )
          ("AK12_lead_pt",        -1.)
          ("minDR_AK12_lep",                minDR_AK12_lep)
          //("minDR_AK12_2_lep",                minDR_AK12_2_lep)
          //("cleanedJets_fromAK12",       cleanedJets_fromAK12.size())
          ("cleanedJets_fromAK12",       0)

          ("HTT_semi_boosted_fromAK8",                     HTT_semi_boosted_fromAK8)
          ("bWj1Wj2_isGenMatched_semi_boosted_fromAK8",    bWj1Wj2_isGenMatched_semi_boosted_fromAK8)
          ("genTopPt_semi_boosted_fromAK8",            genTopPt_semi_boosted_fromAK8)
          ("HadTop_pt_semi_boosted_fromAK8",           HadTop_pt_semi_boosted_fromAK8)

          ("N_jetAK8",     jet_ptrsAK8.size())
          ("cleanedJets_fromAK8",       cleanedJets_fromAK8.size())
          ("W_pt_semi_boosted_fromAK8",           W_pt_semi_boosted_fromAK8)
          ("b_pt_semi_boosted_fromAK8",           b_pt_semi_boosted_fromAK8)
          //("AK8_without_subjets",        AK8_without_subjets)
          ("minDR_AK8_lep",                minDR_AK8_lep)
          ("resolved_and_semi_AK8",     resolved_and_semi_AK8)
          ("boosted_and_semi_AK8",      boosted_and_semi_AK8)
          ("hadtruth_semi_boosted_fromAK8", hadtruth_semi_boosted_fromAK8)
          ("DR_W_b_gen_AK12", DR_W_b_gen_AK12)
          ("DR_W_b_gen_AK8", DR_W_b_gen_AK8)

          ("resolved_and_semi",         resolved_and_semi)
          ("boosted_and_semi",          boosted_and_semi)
          ("resolved_and_boosted",      resolved_and_boosted)

          ("mvaOutput_Hj_tagger",    mvaOutput_Hj_tagger)
          ("Hj_tagger_fromCSVsort4th", Hj_tagger_fromCSVsort4th)
        .fill()
      ;
    }

    if(snm)
    {
      snm->read(eventInfo);
      snm->read(selLeptons);
      snm->read(preselMuons,     fakeableMuons,     tightMuons);
      snm->read(preselElectrons, fakeableElectrons, tightElectrons);
      snm->read(selJets);

      snm->read({
        triggers_1e, triggers_1mu, triggers_2e, triggers_1e1mu, triggers_2mu,
        triggers_3e, triggers_2e1mu, triggers_1e2mu, triggers_3mu
      });
      snm->read(isGenMatched, selBJets_medium.size(), selBJets_loose.size());

      snm->read(met.pt(),                               FloatVariableType::PFMET);
      snm->read(met.phi(),                              FloatVariableType::PFMETphi);
      snm->read(mht_p4.pt(),                            FloatVariableType::MHT);
      snm->read(met_LD,                                 FloatVariableType::metLD);

      snm->read(mindr_lep1_jet,                         FloatVariableType::mindr_lep1_jet);
      snm->read(mindr_lep2_jet,                         FloatVariableType::mindr_lep2_jet);
      snm->read(mindr_lep3_jet,                         FloatVariableType::mindr_lep3_jet);
      // mindr_lep4_jet not filled

      // mindr_tau1_jet not filled
      // mindr_tau2_jet not filled

      snm->read(avg_dr_jet,                             FloatVariableType::avg_dr_jet);
      // avr_dr_lep_tau not filled
      snm->read(max_dr_jet,                             FloatVariableType::max_dr_jet);
      // max_dr_lep_tau not filled
      // min_dr_tau_jet not filled
      // min_dr_lep_tau not filled
      snm->read(min_dr_lep_jet,                         FloatVariableType::min_dr_lep_jet);

      snm->read(dr_leps,                                FloatVariableType::dr_leps);
      // dr_taus not filled

      // dr_lep_tau_ss not filled
      // dr_lep1_tau1 not filled
      // dr_lep1_tau2 not filled
      // dr_lep2_tau1 not filled
      // dr_lep3_tau1 not filled
      // dr_lep2_tau2 not filled

      snm->read(max_lep_eta,                            FloatVariableType::max_lep_eta);

      snm->read(mT_lep1,                                FloatVariableType::mT_met_lep1);
      snm->read(mT_lep2,                                FloatVariableType::mT_met_lep2);
      snm->read(mT_lep3,                                FloatVariableType::mT_met_lep3);
      // mT_met_lep4 not filled

      // mTauTauVis not filled
      // mvis_l1tau not filled
      // mvis_l2tau not filled

      snm->read(mbb,                                    FloatVariableType::mbb);
      snm->read(mbb_loose,                              FloatVariableType::mbb_loose);

      // cosThetaS_hadTau not filled
      // HTT not filled
      // HadTop_pt not filled
      // Hj_tagger not filled

      // mvaOutput_plainKin_ttV not filled
      // mvaOutput_plainKin_tt not filled
      // mvaOutput_plainKin_1B_VT not filled
      // mvaOutput_HTT_SUM_VT not filled

      // mvaOutput_plainKin_SUM_VT not filled

      // mvaOutput_2lss_ttV not filled
      // mvaOutput_2lss_tt not filled
      // mvaOutput_2lss_1tau_plainKin_tt not filled
      // mvaOutput_2lss_1tau_plainKin_ttV not filled
      // mvaOutput_2lss_1tau_plainKin_1B_M not filled
      // mvaOutput_2lss_1tau_plainKin_SUM_M not filled
      // mvaOutput_2lss_1tau_HTT_SUM_M not filled
      // mvaOutput_2lss_1tau_HTTMEM_SUM_M not filled

      snm->read(mvaOutput_3l_ttV,                       FloatVariableType::mvaOutput_3l_ttV);
      snm->read(mvaOutput_3l_ttbar,                     FloatVariableType::mvaOutput_3l_ttbar);
      // mvaOutput_plainKin_SUM_M not filled
      // mvaOutput_plainKin_1B_M not filled

      snm->read(weight_fakeRate,                        FloatVariableType::FR_weight);
      snm->read(triggerWeight,                          FloatVariableType::triggerSF_weight);
      snm->read(leptonSF_weight,                        FloatVariableType::leptonSF_weight);
      // tauSF_weight not filled
      snm->read(btagWeight,                             FloatVariableType::bTagSF_weight);
      snm->read(eventInfo.pileupWeight,                 FloatVariableType::PU_weight);
      snm->read(boost::math::sign(eventInfo.genWeight), FloatVariableType::MC_weight);

      // mvaOutput_2lss_ttV not filled
      // mvaOutput_2lss_tt not filled
      // mvaOutput_2lss_1tau_plainKin_tt not filled
      // mvaOutput_2lss_1tau_plainKin_ttV not filled
      // mvaOutput_2lss_1tau_plainKin_1B_M not filled
      // mvaOutput_2lss_1tau_plainKin_SUM_M not filled
      // mvaOutput_2lss_1tau_HTT_SUM_M not filled
      // mvaOutput_2lss_1tau_HTTMEM_SUM_M not filled

      snm->read(eventInfo.genWeight,                    FloatVariableType::genWeight);

      if((sync_requireGenMatching && isGenMatched) || ! sync_requireGenMatching)
      {
        snm->fill();
      }
      else
      {
        snm->reset();
      }
    }

    ++selectedEntries;
    selectedEntries_weighted += evtWeight;
    histogram_selectedEntries->Fill(0.);
  }

  if(snm)
  {
    snm->write();
  }

  std::cout << "max num. Entries = " << inputTree -> getCumulativeMaxEventCount()
            << " (limited by " << maxEvents << ") processed in "
            << inputTree -> getProcessedFileCount() << " file(s) (out of "
            << inputTree -> getFileCount() << ")\n"
            << " analyzed = " << analyzedEntries << '\n'
            << " selected = " << selectedEntries << " (weighted = " << selectedEntries_weighted << ")\n\n"
            << "cut-flow table" << std::endl;
  cutFlowTable.print(std::cout);
  std::cout << std::endl;

  std::cout << "sel. Entries by gen. matching:" << std::endl;
  for ( std::vector<leptonGenMatchEntry>::const_iterator leptonGenMatch_definition = leptonGenMatch_definitions.begin();
	leptonGenMatch_definition != leptonGenMatch_definitions.end(); ++leptonGenMatch_definition ) {

    std::string process_and_genMatch = process_string;
    if ( apply_leptonGenMatching ) process_and_genMatch += leptonGenMatch_definition->name_;

    int idxLepton = leptonGenMatch_definition->idx_;

    const TH1* histogram_EventCounter = selHistManagers[idxLepton]->evt_->getHistogram_EventCounter();
    std::cout << " " << process_and_genMatch << " = " << histogram_EventCounter->GetEntries() << " (weighted = " << histogram_EventCounter->Integral() << ")" << std::endl;
  }
  std::cout << std::endl;

  delete dataToMCcorrectionInterface;

  delete leptonFakeRateInterface;

  delete run_lumi_eventSelector;

  delete selEventsFile;

  delete muonReader;
  delete electronReader;
  delete hadTauReader;
  delete jetReader;
  delete metReader;
  delete metFilterReader;
  delete genLeptonReader;
  delete genHadTauReader;
  delete genPhotonReader;
  delete genJetReader;
  delete lheInfoReader;

  delete hadTopTagger;
  delete hadTopTagger_semi_boosted;
  delete hadTopTagger_boosted;
  delete hadTopTagger_semi_boosted_fromAK8;

  delete genEvtHistManager_beforeCuts;
  delete genEvtHistManager_afterCuts;
  delete lheInfoHistManager;
  delete cutFlowHistManager;
  delete eventWeightManager;

  hltPaths_delete(triggers_1e);
  hltPaths_delete(triggers_2e);
  hltPaths_delete(triggers_1mu);
  hltPaths_delete(triggers_2mu);
  hltPaths_delete(triggers_1e1mu);

  delete inputTree;
  delete snm;

  clock.Show("analyze_3l");

  return EXIT_SUCCESS;
}
