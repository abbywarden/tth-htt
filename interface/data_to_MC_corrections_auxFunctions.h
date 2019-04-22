#ifndef DATA_TO_MC_CORRECTIONS_AUXFUNCTIONS_H
#define DATA_TO_MC_CORRECTIONS_AUXFUNCTIONS_H

#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // lutWrapperBase, vLutWrapperBase

#include <string> // std::string
#include <cmath> // std::min(), std::max()
#include <vector> // std::vector<>
#include <map> // std::map<>

// forward declarations
class TFile;
class TauTriggerSFs2017;
enum class TriggerSFsys;

typedef double (TauTriggerSFs2017::*getTriggerEfficiencyFunc)(double, double, double, int) const;

namespace aux
{
  std::string
  getEtaBinLabel(double etaMin,
                 double etaMax);

  std::string
  getHadTauIdxLabel(int idx);

  std::string
  getHadTauSelectionLabel_2016(const std::string & hadTauSelection);

  std::string
  getHadTauSelectionLabel_2017(const std::string & hadTauSelection);

  std::string
  getHadTauDecayModeLabel(int hadTauDecayMode);

  void
  clearCollection(std::vector<lutWrapperBase *> & collection);

  void
  clearCollection(std::map<int, std::vector<lutWrapperBase *>> & collection);

  double
  get_from_lut(const std::map<int, std::vector<lutWrapperBase *>> & corrections,
               double hadTau_pt,
               double hadTau_eta,
               int hadTau_decayMode,
               bool isDEBUG);

  double constexpr
  compSF(double eff_data,
         double eff_mc)
  {
    return std::min(eff_data / std::max(1.e-6, eff_mc), 1.e+1);
  }

  void 
  loadTriggerEff_1e_2017(vLutWrapperBase & effTrigger_1e_data,
                         vLutWrapperBase & effTrigger_1e_mc, 
                         std::map<std::string, TFile *> & inputFiles);

  void
  loadTriggerEff_1e_1tau_lepLeg_2017(vLutWrapperBase & effTrigger_1e1tau_lepLeg_data,
                                     vLutWrapperBase & effTrigger_1e1tau_lepLeg_mc,
                                     std::map<std::string, TFile *> & inputFiles);

  void 
  loadTriggerEff_1m_2017(vLutWrapperBase & effTrigger_1m_data,
                         vLutWrapperBase & effTrigger_1m_mc,
                         std::map<std::string, TFile *> & inputFiles);
 
  void
  loadTriggerEff_1m_1tau_lepLeg_2017(vLutWrapperBase & effTrigger_1m1tau_lepLeg_data,
                                     vLutWrapperBase & effTrigger_1m1tau_lepLeg_mc,
                                     std::map<std::string, TFile *> & inputFiles);

  void 
  loadTriggerEff_1e_2018(vLutWrapperBase & effTrigger_1e_data,
                         vLutWrapperBase & effTrigger_1e_mc,
                         std::map<std::string, TFile *> & inputFiles);

  void
  loadTriggerEff_1e_1tau_lepLeg_2018(vLutWrapperBase & effTrigger_1e1tau_lepLeg_data,
                                     vLutWrapperBase & effTrigger_1e1tau_lepLeg_mc,
                                     std::map<std::string, TFile *> & inputFiles);

  void 
  loadTriggerEff_1m_2018(vLutWrapperBase & effTrigger_1m_data,
                         vLutWrapperBase & effTrigger_1m_mc,
                         std::map<std::string, TFile *> & inputFiles);
 
  void
  loadTriggerEff_1m_1tau_lepLeg_2018(vLutWrapperBase & effTrigger_1m1tau_lepLeg_data,
                                     vLutWrapperBase & effTrigger_1m1tau_lepLeg_mc,
                                     std::map<std::string, TFile *> & inputFiles);

  getTriggerEfficiencyFunc
  getTriggerFuncMC(TriggerSFsys triggerSF_option);

  getTriggerEfficiencyFunc
  getTriggerFuncData(TriggerSFsys triggerSF_option);
}

#endif // DATA_TO_MC_CORRECTIONS_AUXFUNCTIONS_H
