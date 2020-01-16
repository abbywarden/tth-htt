#include "tthAnalysis/HiggsToTauTau/interface/Data_to_MC_CorrectionInterface_2018.h"

#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // lutWrapperTH2
#include "tthAnalysis/HiggsToTauTau/interface/cmsException.h" // cmsException()
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h" // as_integer()

#include "TauPOG/TauIDSFs/interface/TauIDSFTool.h" // TauIDSFTool

#include <cmath> // std::fabs(), std::sqrt()

Data_to_MC_CorrectionInterface_2018::Data_to_MC_CorrectionInterface_2018(const edm::ParameterSet & cfg)
  : Data_to_MC_CorrectionInterface_Base(cfg)
{
  // Reconstruction efficiencies of electrons, measured by EGamma POG
  // https://twiki.cern.ch/twiki/pub/CMS/EgammaIDRecipesRun2/egammaEffi.txt_EGM2D_updatedAll.root
  // Downloaded from: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=104#102X_series_Dataset_2018_Autumn
  sfElectronID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/el_scaleFactors_gsf.root",
    "EGamma_SF2D",
    lut::kXetaYpt, -2.5, +2.5, lut::kLimit, 10., -1., lut::kLimit_and_Cut
  ));

  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_ele_2018_3l/passttH/egammaEffi.txt_EGM2D.root
  sfElectronID_and_Iso_tight_to_loose_woTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_e_3l.root", "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_ele_2018_2lss/passttH/egammaEffi.txt_EGM2D.root
  sfElectronID_and_Iso_tight_to_loose_wTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_e_2lss.root", "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // combined
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2018_ele_pt.root
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2018_ele_eta.root
  sfElectronID_and_Iso_tight_to_loose_errors_up_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_e_error.root", "histo_eff_data_max",
    lut::kXabsEtaYpt
  ));
  sfElectronID_and_Iso_tight_to_loose_errors_down_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_e_error.root", "histo_eff_data_min",
    lut::kXabsEtaYpt
  ));

  // https://gitlab.cern.ch/cms-muonPOG/MuonReferenceEfficiencies/raw/513f1017b1b216a4d3e9f5f0cf858ea7569a134b/EfficienciesStudies/2018/rootfiles/RunABCD_SF_ID.root
  // Downloaded from: https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffs2018?rev=8#Scale_Factors_with_statistical_e ("Lepton ID: ABCD")
  sfMuonID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/RunABCD_SF_ID_ptGt20.root", "NUM_LooseID_DEN_TrackerMuons_pt_abseta",
    lut::kXptYabsEta, 20., -1., lut::kLimit_and_Cut, 0., 2.4, lut::kLimit
  ));
  // https://gitlab.cern.ch/cms-muonPOG/MuonReferenceEfficiencies/raw/513f1017b1b216a4d3e9f5f0cf858ea7569a134b/EfficienciesStudies/2018/Jpsi/rootfiles/RunABCD_SF_ID.root
  // Downloaded from: https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffs2018?rev=8#Scale_Factors_with_statistical_e ("Lepton ID: ABCD for low-pT")
  sfMuonID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/RunABCD_SF_ID_ptLt20.root", "NUM_LooseID_DEN_genTracks_pt_abseta",
    lut::kXptYabsEta, -1., 20., lut::kLimit_and_Cut, 0., 2.4, lut::kLimit
  ));

  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_muon_2018_3l/passttH/egammaEffi.txt_EGM2D.root
  sfMuonID_and_Iso_tight_to_loose_woTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_m_3l.root", "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_muon_2018_2lss/passttH/egammaEffi.txt_EGM2D.root
  sfMuonID_and_Iso_tight_to_loose_wTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_m_2lss.root", "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // combined
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2018_muon_pt.root
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2018_muon_eta.root
  sfMuonID_and_Iso_tight_to_loose_errors_up_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_m_error.root", "histo_eff_data_max",
    lut::kXabsEtaYpt
  ));
  sfMuonID_and_Iso_tight_to_loose_errors_down_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2018/lepMVAEffSF_m_error.root", "histo_eff_data_min",
    lut::kXabsEtaYpt
  ));

  if(applyHadTauSF_)
  {
    const std::string tauIDSFTool_era = "2018ReReco";
    tauIdSFs_ = new TauIDSFTool(tauIDSFTool_era, tauIDSF_str_, tauIDSF_level_str_, false);
    initAntiEle_tauIDSFs(tauIDSFTool_era);
    initAntiMu_tauIDSFs(tauIDSFTool_era);
  }
}

Data_to_MC_CorrectionInterface_2018::~Data_to_MC_CorrectionInterface_2018()
{}

double
Data_to_MC_CorrectionInterface_2018::getWeight_leptonTriggerEff() const
{
  throw cmsException(this, __func__, __LINE__)
    << "Not available in 2018 era"
  ;
}

double
Data_to_MC_CorrectionInterface_2018::getSF_leptonTriggerEff(TriggerSFsys central_or_shift) const
{
#pragma message "Setting lepton trigger SFs to 1 in 2018"
  return 1.;
}
