#ifndef tthAnalysis_HiggsToTauTau_Data_to_MC_CorrectionInterface_1l_1tau_trigger_h
#define tthAnalysis_HiggsToTauTau_Data_to_MC_CorrectionInterface_1l_1tau_trigger_h

#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // lutWrapperBase, vLutWrapperBase

// forward declarations
class TauTriggerSFs2017;
enum class TriggerSFsys;

class Data_to_MC_CorrectionInterface_1l_1tau_trigger
{
public:
  Data_to_MC_CorrectionInterface_1l_1tau_trigger(const edm::ParameterSet & cfg);
  ~Data_to_MC_CorrectionInterface_1l_1tau_trigger();

  //-----------------------------------------------------------------------------
  // set HLT trigger bits
  // (to be called once per event, before calling any of the getSF.. functions)
  void
  setTriggerBits(bool isTriggered_1e,
                 bool isTriggered_1e1tau,
                 bool isTriggered_1m,
                 bool isTriggered_1m1tau);
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // set lepton type, pT and eta as well as hadTau pT, eta and decay mode
  // (to be called once per event, before calling any of the getSF.. functions)
  void
  setLeptons(int lepton_type,
             double lepton_pt,
             double lepton_eta);

  void
  setHadTaus(double hadTau_pt, double hadTau_eta, double hadTau_phi, int hadTau_decayMode);
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // trigger efficiency turn-on curves for MC
  double
  getWeight_triggerEff() const;

  double
  getWeight_triggerEff(TriggerSFsys central_or_shift) const;
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  // data/MC correction for trigger efficiency 
  double
  getSF_triggerEff() const;

  double
  getSF_triggerEff(TriggerSFsys central_or_shift) const;
  //-----------------------------------------------------------------------------

protected:

  //-----------------------------------------------------------------------------
  // data/MC corrections for trigger efficiencies in 2017 ReReco data and Summer17 MC

  vLutWrapperBase effTrigger_1e_data_;
  vLutWrapperBase effTrigger_1e_mc_;
  vLutWrapperBase effTrigger_1e1tau_lepLeg_data_;
  vLutWrapperBase effTrigger_1e1tau_lepLeg_mc_;

  vLutWrapperBase effTrigger_1m_data_;
  vLutWrapperBase effTrigger_1m_mc_;
  vLutWrapperBase effTrigger_1m1tau_lepLeg_data_;
  vLutWrapperBase effTrigger_1m1tau_lepLeg_mc_;

  TauTriggerSFs2017 * effTrigger_1e1tau_tauLeg_;
  TauTriggerSFs2017 * effTrigger_1m1tau_tauLeg_;
  //-----------------------------------------------------------------------------

  std::map<std::string, TFile *> inputFiles_;

  std::string era_str_;
  int era_;
  std::string hadTauSelection_;
  bool isDEBUG_;
  TriggerSFsys triggerSF_option_;

  bool isTriggered_1e_;
  bool isTriggered_1e1tau_;
  bool isTriggered_1m_;
  bool isTriggered_1m1tau_;

  int lepton_type_;
  double lepton_pt_;
  double lepton_eta_;

  int hadTau_genPdgId_;
  double hadTau_pt_;
  double hadTau_eta_;
  double hadTau_phi_;
  int hadTau_decayMode_;
};

#endif // tthAnalysis_HiggsToTauTau_data_to_MC_corrections_1l_1tau_trigger_h
