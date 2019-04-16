#include "tthAnalysis/HiggsToTauTau/interface/data_to_MC_corrections_auxFunctions.h"

#include "TauAnalysisTools/TauTriggerSFs/interface/TauTriggerSFs2017.h" // TauTriggerSFs2017

#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // lutWrapperBase, vLutWrapperBase
#include "tthAnalysis/HiggsToTauTau/interface/cmsException.h" // cmsException()

#include <TString.h> // Form()
#include <TFile.h> // TFile()

#include <boost/algorithm/string/replace.hpp> // boost::replace_all_copy()

#include <cassert> // assert()

namespace aux
{
  std::string
  getEtaBinLabel(double etaMin,
                 double etaMax)
  {
    std::string etaBinLabel_tstring;
    if(etaMin != -1. && etaMax != -1.)
    {
      etaBinLabel_tstring = Form("%1.4gto%1.4g", etaMin, etaMax);
    }
    else if(etaMin != -1.)
    {
      etaBinLabel_tstring = Form("Gt%1.4g", etaMin);
    }
    else if(etaMax != -1.)
    {
      etaBinLabel_tstring = Form("Lt%1.4g", etaMax);
    }
    else
    {
      assert(0);
    }
    const std::string etaBinLabel = boost::replace_all_copy(etaBinLabel_tstring, ".", "p");
    return etaBinLabel;
  }

  std::string
  getHadTauIdxLabel(int idx)
  {
    std::string hadTauIdxLabel;
    switch(idx)
    {
      case 0:  hadTauIdxLabel = "_lead";    break;
      case 1:  hadTauIdxLabel = "_sublead"; break;
      case 2:  hadTauIdxLabel = "_third";   break;
      case 3:  hadTauIdxLabel = "_fourth";  break;	
      default: assert(0);
    }
    return hadTauIdxLabel;
  }

  std::string
  getHadTauSelectionLabel(const std::string & hadTauSelection)
  {
    std::string hadTauSelectionLabel;
    if     (hadTauSelection == "dR03mvaVVLoose") hadTauSelectionLabel = "VLooseIso"; // CV: custom WP for which no trigger efficiency turn-on curve has been measured
    else if(hadTauSelection == "dR03mvaVLoose" ) hadTauSelectionLabel = "VLooseIso";
    else if(hadTauSelection == "dR03mvaLoose"  ) hadTauSelectionLabel = "LooseIso";
    else if(hadTauSelection == "dR03mvaMedium" ) hadTauSelectionLabel = "MediumIso";
    else if(hadTauSelection == "dR03mvaTight"  ) hadTauSelectionLabel = "TightIso";
    else if(hadTauSelection == "dR03mvaVTight" ) hadTauSelectionLabel = "VTightIso";
    else if(hadTauSelection == "dR03mvaVVTight") hadTauSelectionLabel = "VVTightIso";
    else throw cmsException(__func__, __LINE__)
           << "Invalid Configuration parameter 'hadTauSelection' = " << hadTauSelection;
    return hadTauSelectionLabel;
  }

  std::string
  get_hadTauSelection_TauTriggerSFs2017(const std::string & hadTauSelection)
  {
    std::string hadTauSelection_TauTriggerSFs2017;
    if     (hadTauSelection == "dR03mvaVVLoose") hadTauSelection_TauTriggerSFs2017 = "medium"; // CV: trigger efficiency turn-on curve has not been measured for this working-point
    else if(hadTauSelection == "dR03mvaVLoose" ) hadTauSelection_TauTriggerSFs2017 = "medium"; // CV: trigger efficiency turn-on curve has not been measured for this working-point
    else if(hadTauSelection == "dR03mvaLoose"  ) hadTauSelection_TauTriggerSFs2017 = "medium"; // CV: trigger efficiency turn-on curve has not been measured for this working-point
    else if(hadTauSelection == "dR03mvaMedium" ) hadTauSelection_TauTriggerSFs2017 = "medium";
    else if(hadTauSelection == "dR03mvaTight"  ) hadTauSelection_TauTriggerSFs2017 = "tight";
    else if(hadTauSelection == "dR03mvaVTight" ) hadTauSelection_TauTriggerSFs2017 = "vtight";
    else if(hadTauSelection == "dR03mvaVVTight") hadTauSelection_TauTriggerSFs2017 = "vtight"; // CV: trigger efficiency turn-on curve has not been measured for this working-point
    else throw cmsException(__func__, __LINE__)
           << "Invalid Configuration parameter 'hadTauSelection' = " << hadTauSelection
         ;
    return hadTauSelection_TauTriggerSFs2017;
  }

  std::string
  getHadTauDecayModeLabel(int hadTauDecayMode)
  {
    std::string hadTauDecayModeLabel;
    if     (hadTauDecayMode ==  0                        ) hadTauDecayModeLabel = "dm0";  // 1-prong without pi0s
    else if(hadTauDecayMode ==  1 || hadTauDecayMode == 2) hadTauDecayModeLabel = "dm1";  // 1-prong with pi0s
    else if(hadTauDecayMode == 10                        ) hadTauDecayModeLabel = "dm10"; // 3-prong
    else throw cmsException(__func__, __LINE__)
           << "Invalid Configuration parameter 'hadTauDecayMode' = " << hadTauDecayMode;
    return hadTauDecayModeLabel;
  }

  void
  clearCollection(std::vector<lutWrapperBase *> & collection)
  {
    for(lutWrapperBase * it: collection)
    {
      delete it;
    }
    collection.clear();
  }

  void
  clearCollection(std::map<int, std::vector<lutWrapperBase *>> & collection)
  {
    for(auto & kv: collection)
    {
      for(lutWrapperBase * it: kv.second)
      {
        delete it;
      }
      kv.second.clear();
    }
  }

  double
  get_from_lut(const std::map<int, std::vector<lutWrapperBase *>> & corrections,
               double hadTau_pt,
               double hadTau_eta,
               int hadTau_decayMode,
               bool isDEBUG)
  {
    if(! corrections.count(hadTau_decayMode))
    {
      throw cmsException(__func__, __LINE__)
              << "Invalid parameter 'hadTauDecayMode' = " << hadTau_decayMode;
    }
    const double sf = get_from_lut(corrections.at(hadTau_decayMode), hadTau_pt, hadTau_eta, isDEBUG);
    return sf;
  }

  //-------------------------------------------------------------------------------------------------
  void 
  loadTriggerEff_1e_2017(vLutWrapperBase& effTrigger_1e_data, vLutWrapperBase& effTrigger_1e_mc, 
			 std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1e = { -1., 1.48, 2.1 };
    assert(etaBinEdges_1e.size() > 0);
    const std::size_t numEtaBins_1e = etaBinEdges_1e.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1e; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1e[idxEtaBin];
      const double etaMax = etaBinEdges_1e[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1e_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Electron_Ele32orEle35_eff.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1e_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Electron_Ele32orEle35_eff.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }

  void
  loadTriggerEff_1e_1tau_lepLeg_2017(vLutWrapperBase& effTrigger_1e1tau_lepLeg_data, vLutWrapperBase& effTrigger_1e1tau_lepLeg_mc, 
				     std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1e_1tau_lepLeg = { -1., 1.48, 2.1 };
    assert(etaBinEdges_1e_1tau_lepLeg.size() > 0);
    const std::size_t numEtaBins_1e_1tau_lepLeg = etaBinEdges_1e_1tau_lepLeg.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1e_1tau_lepLeg; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1e_1tau_lepLeg[idxEtaBin];
      const double etaMax = etaBinEdges_1e_1tau_lepLeg[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1e1tau_lepLeg_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Electron_EleTau_Ele24_eff.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1e1tau_lepLeg_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Electron_EleTau_Ele24_eff.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }

  void 
  loadTriggerEff_1m_2017(vLutWrapperBase& effTrigger_1m_data, vLutWrapperBase& effTrigger_1m_mc, 
			 std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1m = { -1., 0.9, 1.2, 2.1 };
    assert(etaBinEdges_1m.size() > 0);
    const std::size_t numEtaBins_1m = etaBinEdges_1m.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1m; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1m[idxEtaBin];
      const double etaMax = etaBinEdges_1m[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1m_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Muon_IsoMu24orIsoMu27_eff.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1m_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Muon_IsoMu24orIsoMu27_eff.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }
 
  void
  loadTriggerEff_1m_1tau_lepLeg_2017(vLutWrapperBase& effTrigger_1m1tau_lepLeg_data, vLutWrapperBase& effTrigger_1m1tau_lepLeg_mc, 
				     std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1m1tau_lepLeg = { -1., 0.9, 1.2, 2.1 };
    assert(etaBinEdges_1m1tau_lepLeg.size() > 0);
    const std::size_t numEtaBins_1m1tau_lepLeg = etaBinEdges_1m1tau_lepLeg.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1m1tau_lepLeg; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1m1tau_lepLeg[idxEtaBin];
      const double etaMax = etaBinEdges_1m1tau_lepLeg[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1m1tau_lepLeg_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Muon_MuTau_IsoMu20_eff.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1m1tau_lepLeg_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2017/Muon_MuTau_IsoMu20_eff.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }

  void 
  loadTriggerEff_1e_2018(vLutWrapperBase& effTrigger_1e_data, vLutWrapperBase& effTrigger_1e_mc, 
			 std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1e = { -1., 1.0, 1.48, 1.65, 2.1 };
    assert(etaBinEdges_1e.size() > 0);
    const std::size_t numEtaBins_1e = etaBinEdges_1e.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1e; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1e[idxEtaBin];
      const double etaMax = etaBinEdges_1e[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1e_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Electron_Run2018_Ele32orEle35.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1e_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Electron_Run2018_Ele32orEle35.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }

  void
  loadTriggerEff_1e_1tau_lepLeg_2018(vLutWrapperBase& effTrigger_1e1tau_lepLeg_data, vLutWrapperBase& effTrigger_1e1tau_lepLeg_mc, 
				     std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1e_1tau_lepLeg = { -1., 1.0, 1.48, 1.65, 2.1 };
    assert(etaBinEdges_1e_1tau_lepLeg.size() > 0);
    const std::size_t numEtaBins_1e_1tau_lepLeg = etaBinEdges_1e_1tau_lepLeg.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1e_1tau_lepLeg; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1e_1tau_lepLeg[idxEtaBin];
      const double etaMax = etaBinEdges_1e_1tau_lepLeg[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1e1tau_lepLeg_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Electron_Run2018_Ele24.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1e1tau_lepLeg_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Electron_Run2018_Ele24.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }

  void 
  loadTriggerEff_1m_2018(vLutWrapperBase& effTrigger_1m_data, vLutWrapperBase& effTrigger_1m_mc, 
			 std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1m = { -1., 0.9, 1.2, 2.1 };
    assert(etaBinEdges_1m.size() > 0);
    const std::size_t numEtaBins_1m = etaBinEdges_1m.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1m; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1m[idxEtaBin];
      const double etaMax = etaBinEdges_1m[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1m_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Muon_Run2018_IsoMu24orIsoMu27.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1m_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Muon_Run2018_IsoMu24orIsoMu27.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }
 
  void
  loadTriggerEff_1m_1tau_lepLeg_2018(vLutWrapperBase& effTrigger_1m1tau_lepLeg_data, vLutWrapperBase& effTrigger_1m1tau_lepLeg_mc, 
				     std::map<std::string, TFile *>& inputFiles)
  {
    const std::vector<double> etaBinEdges_1m1tau_lepLeg = { -1., 0.9, 1.2, 2.1 };
    assert(etaBinEdges_1m1tau_lepLeg.size() > 0);
    const std::size_t numEtaBins_1m1tau_lepLeg = etaBinEdges_1m1tau_lepLeg.size() - 1;

    for(std::size_t idxEtaBin = 0; idxEtaBin < numEtaBins_1m1tau_lepLeg; ++idxEtaBin)
    {
      const double etaMin = etaBinEdges_1m1tau_lepLeg[idxEtaBin];
      const double etaMax = etaBinEdges_1m1tau_lepLeg[idxEtaBin + 1];
      const std::string etaBinLabel = aux::getEtaBinLabel(etaMin, etaMax);

      effTrigger_1m1tau_lepLeg_data.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Muon_Run2018_IsoMu20.root",
        Form("ZMassEta%s_Data", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
      effTrigger_1m1tau_lepLeg_mc.push_back(new lutWrapperTGraph(
        inputFiles,
        "tthAnalysis/HiggsToTauTau/data/triggerSF/2018/Muon_Run2018_IsoMu20.root",
        Form("ZMassEta%s_MC", etaBinLabel.data()),
        lut::kXptYabsEta, -1., -1., lut::kLimit, etaMin, etaMax, lut::kCut
      ));
    }
  }
  //-------------------------------------------------------------------------------------------------

  double 
  getTauTriggerEfficiencyData_2017(const TauTriggerSFs2017* effTrigger_tauLeg, 
				   double hadTau_pt, double hadTau_eta, double hadTau_phi, double hadTau_decayMode, 
				   TriggerSFsys triggerSF_option)
  {
    double eff = 1.;
    if      ( triggerSF_option == TriggerSFsys::central   ) eff = effTrigger_tauLeg->getTriggerEfficiencyData(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else if ( triggerSF_option == TriggerSFsys::shiftUp   ) eff = effTrigger_tauLeg->getTriggerEfficiencyDataUncertUp(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else if ( triggerSF_option == TriggerSFsys::shiftDown ) eff = effTrigger_tauLeg->getTriggerEfficiencyDataUncertDown(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else assert(0);
    return eff;
  }

  double 
  getTauTriggerEfficiencyMC_2017(const TauTriggerSFs2017* effTrigger_tauLeg, 
				 double hadTau_pt, double hadTau_eta, double hadTau_phi, double hadTau_decayMode, 
				 TriggerSFsys triggerSF_option)
  {
    double eff = 1.;
    if      ( triggerSF_option == TriggerSFsys::central   ) eff = effTrigger_tauLeg->getTriggerEfficiencyMC(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else if ( triggerSF_option == TriggerSFsys::shiftUp   ) eff = effTrigger_tauLeg->getTriggerEfficiencyMCUncertUp(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else if ( triggerSF_option == TriggerSFsys::shiftDown ) eff = effTrigger_tauLeg->getTriggerEfficiencyMCUncertDown(hadTau_pt, hadTau_eta, hadTau_phi, hadTau_decayMode);
    else assert(0);
    return eff;
  }
}
