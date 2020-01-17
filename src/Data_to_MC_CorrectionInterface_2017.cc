#include "tthAnalysis/HiggsToTauTau/interface/Data_to_MC_CorrectionInterface_2017.h"

#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // lutWrapperTH2
#include "tthAnalysis/HiggsToTauTau/interface/cmsException.h" // cmsException()
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h" // as_integer()

#include "TauPOG/TauIDSFs/interface/TauIDSFTool.h" // TauIDSFTool

#include <cmath> // std::fabs(), std::sqrt()

Data_to_MC_CorrectionInterface_2017::Data_to_MC_CorrectionInterface_2017(const edm::ParameterSet & cfg)
  : Data_to_MC_CorrectionInterface_Base(cfg)
{
  // Reconstruction efficiencies of electrons with pT < 20 GeV, measured by EGamma POG
  // https://twiki.cern.ch/twiki/pub/CMS/Egamma2017DataRecommendations/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root
  // Downloaded from: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=104#94X_series_Fall17V2_Scale_factor
  sfElectronID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/el_scaleFactors_gsf_ptLt20.root",
    "EGamma_SF2D",
    lut::kXetaYpt, -2.5, +2.5, lut::kLimit, -1., 20., lut::kLimit_and_Cut
  ));
  // Reconstruction efficiencies of electrons with pT > 20 GeV, measured by EGamma POG
  // https://twiki.cern.ch/twiki/pub/CMS/Egamma2017DataRecommendations/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root
  // Downloaded from: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=104#94X_series_Fall17V2_Scale_factor
  sfElectronID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/el_scaleFactors_gsf_ptGt20.root",
    "EGamma_SF2D",
    lut::kXetaYpt, -2.5, +2.5, lut::kLimit, 20., -1., lut::kLimit_and_Cut
  ));
  // Loose electron SFs, measured by the CERN group
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_loose_ele_2017.root
  sfElectronID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/TnP_loose_ele_2017.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_loosettH_ele_2017.root
  sfElectronID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/TnP_loosettH_ele_2017.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));

  // Loose-to-tight electron ID SFs, measured by the CERN group
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_ele_2017_3l/passttH/egammaEffi.txt_EGM2D.root
  sfElectronID_and_Iso_tight_to_loose_woTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_e_3l.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_ele_2017_2lss/passttH/egammaEffi.txt_EGM2D.root
  sfElectronID_and_Iso_tight_to_loose_wTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_e_2lss.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // combined
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2017_ele_pt.root
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2017_ele_eta.root
  sfElectronID_and_Iso_tight_to_loose_errors_up_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_e_error.root", "histo_eff_data_max",
    lut::kXabsEtaYpt
  ));
  sfElectronID_and_Iso_tight_to_loose_errors_down_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_e_error.root", "histo_eff_data_min",
    lut::kXabsEtaYpt
  ));

  // Loose muon SFs, measured by the CERN group
  sfMuonID_and_Iso_loose_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/TnP_loose_muon_2017.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));

  // Loose-to-tight muon ID SFs, measured by the CERN group
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_muon_2017_3l/passttH/egammaEffi.txt_EGM2D.root
  sfMuonID_and_Iso_tight_to_loose_woTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_m_3l.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/TnP_ttH_muon_2017_2lss/passttH/egammaEffi.txt_EGM2D.root
  sfMuonID_and_Iso_tight_to_loose_wTightCharge_.push_back(new lutWrapperTH2(
    inputFiles_,
    "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_m_2lss.root",
    "EGamma_SF2D",
    lut::kXabsEtaYpt
  ));
  // combined
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2017_muon_pt.root
  // /afs/cern.ch/user/b/balvarez/work/public/ttHAnalysis/error/SFttbar_2017_muon_eta.root
  sfMuonID_and_Iso_tight_to_loose_errors_up_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_m_error.root", "histo_eff_data_max",
    lut::kXabsEtaYpt
  ));
  sfMuonID_and_Iso_tight_to_loose_errors_down_.push_back(new lutWrapperTH2(
    inputFiles_, "tthAnalysis/HiggsToTauTau/data/leptonSF/2017/lepMVAEffSF_m_error.root", "histo_eff_data_min",
    lut::kXabsEtaYpt
  ));

  if(applyHadTauSF_)
  {
    const std::string tauIDSFTool_era = "2017ReReco";
    tauIdSFs_ = new TauIDSFTool(tauIDSFTool_era, tauIDSF_str_, tauIDSF_level_str_, false);
    initAntiEle_tauIDSFs(tauIDSFTool_era);
    initAntiMu_tauIDSFs(tauIDSFTool_era);
  }
}

Data_to_MC_CorrectionInterface_2017::~Data_to_MC_CorrectionInterface_2017()
{}

double
Data_to_MC_CorrectionInterface_2017::getWeight_leptonTriggerEff() const
{
  throw cmsException(this, __func__, __LINE__)
    << "Not available for 2017 era"
  ;
}

double
Data_to_MC_CorrectionInterface_2017::getSF_leptonTriggerEff(TriggerSFsys central_or_shift) const
{
  // see Table 12 in AN2018/098v18
  double sf = 1.;
  double sfErr = 0.;

  const double lepton_pt_lead = std::max(lepton_pt_[0], lepton_pt_[1]);
  if(numElectrons_ == 2 && numMuons_ == 0)
  {
    if  (lepton_pt_lead >= 30.) { sf = 0.991; sfErr = 0.002; }
    else                        { sf = 0.937; sfErr = 0.027; }
  }
  else if(numElectrons_ == 1 && numMuons_ == 1)
  {
    if     (lepton_pt_lead >= 50.) { sf = 1.000; sfErr = 0.001; }
    else if(lepton_pt_lead >= 35.) { sf = 0.983; sfErr = 0.003; }
    else                           { sf = 0.952; sfErr = 0.008; }
  }
  else if(numElectrons_ == 0 && numMuons_ == 2)
  {
    if  (lepton_pt_lead >= 35.) { sf = 0.994; sfErr = 0.001; }
    else                        { sf = 0.972; sfErr = 0.006; }
  }
  else if((numElectrons_ + numMuons_) >= 3)
  {
    sf = 1.;
    sfErr = 0.050;
  }

  switch(central_or_shift)
  {
    case TriggerSFsys::central:   return sf;
    case TriggerSFsys::shiftUp:   return sf + sfErr;
    case TriggerSFsys::shiftDown: return sf - sfErr;
    default: throw cmsException(this, __func__, __LINE__)
                     << "Invalid option: " << static_cast<int>(central_or_shift)
                   ;
  }

  return sf;
}
