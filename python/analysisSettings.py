from tthAnalysis.NanoAOD.triggers import Triggers

trigger_runs_2016 = Triggers('2016').runs
trigger_runs_2017 = Triggers('2017').runs
trigger_runs_2018 = Triggers('2018').runs

# Integrated luminosity

# Reproduced https://github.com/HEP-KBFI/tth-nanoAOD/blob/dde7a1e5640f7a66ad635cb6f618dd1119bc1244/test/datasets/txt/datasets_data_2016_17Jul18.txt#L50
# Official figures: https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/688.html & PAS LUM-17-001
lumi_2016 = 35.922e+3 # 1/pb (uncertainty: 2.5%)

# Reproduced https://github.com/HEP-KBFI/tth-nanoAOD/blob/dde7a1e5640f7a66ad635cb6f618dd1119bc1244/test/datasets/txt/datasets_data_2017_31Mar18.txt#L38
# Official figures: https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/761/1.html & PAS LUM-17-004
lumi_2017 = 41.529e+3 # 1/pb (uncertainty: 2.3%)

# Reproduced https://github.com/HEP-KBFI/tth-nanoAOD/blob/dde7a1e5640f7a66ad635cb6f618dd1119bc1244/test/datasets/txt/datasets_data_2018_17Sep18.txt#L24
# Official figures: https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/860.html & PAS LUM-18-002
lumi_2018 = 59.741e+3 # 1/pb (uncertainty: 2.5%)

# Reproduced

def get_lumi(era):
  if era == "2016":
    return lumi_2016
  elif era == "2017":
    return lumi_2017
  elif era == "2018":
    return lumi_2018
  else:
    raise ValueError("Invalid era: %s" % era)

# Systematic uncertainties

class systematics(object):

  # Basic definitions

  central = [ "central" ]

  JES                  = [ "CMS_ttHl_JESUp",                  "CMS_ttHl_JESDown"                  ]
  JER                  = [ "CMS_ttHl_JERUp",                  "CMS_ttHl_JERDown"                  ]
  UnclusteredEn        = [ "CMS_ttHl_UnclusteredEnUp",        "CMS_ttHl_UnclusteredEnDown"        ]
  tauES                = [ "CMS_ttHl_tauESUp",                "CMS_ttHl_tauESDown"                ]
  tauIDSF              = [ "CMS_ttHl_tauIDSFUp",              "CMS_ttHl_tauIDSFDown"              ]
  triggerSF            = [ "CMS_ttHl_triggerUp",              "CMS_ttHl_triggerDown"              ]
  DYMCReweighting      = [ "CMS_ttHl_DYMCReweightingUp",      "CMS_ttHl_DYMCReweightingDown"      ]
  DYMCNormScaleFactors = [ "CMS_ttHl_DYMCNormScaleFactorsUp", "CMS_ttHl_DYMCNormScaleFactorsDown" ]
  EWK_jet              = [ "CMS_ttHl_EWK_jetUp",              "CMS_ttHl_EWK_jetDown"              ]
  EWK_bjet             = [ "CMS_ttHl_EWK_btagUp",             "CMS_ttHl_EWK_btagDown"             ]
  MET_ResponseSyst     = [ "MET_RespUp",                      "MET_RespDown"                      ]
  MET_ResolutionSyst   = [ "MET_ResolUp",                     "MET_ResolDown"                     ]

  triggerSF_2lss   = [
    "CMS_ttHl_trigger_2lssUp",     "CMS_ttHl_trigger_2lssDown",
    "CMS_ttHl_trigger_2lssEEUp",   "CMS_ttHl_trigger_2lssEEDown",
    "CMS_ttHl_trigger_2lssEMuUp",  "CMS_ttHl_trigger_2lssEMuDown",
    "CMS_ttHl_trigger_2lssMuMuUp", "CMS_ttHl_trigger_2lssMuMuDown",
  ]
  triggerSF_3l     = [ "CMS_ttHl_trigger_3lUp",     "CMS_ttHl_trigger_3lDown"     ]
  triggerSF_1l1tau = [ "CMS_ttHl_trigger_1l1tauUp", "CMS_ttHl_trigger_1l1tauDown" ]
  triggerSF_0l2tau = [ "CMS_ttHl_trigger_0l2tauUp", "CMS_ttHl_trigger_0l2tauDown" ]
  triggerSF_split = triggerSF_2lss + triggerSF_3l + triggerSF_1l1tau + triggerSF_0l2tau

  JES_HEM = "CMS_ttHl_JESHEMDown" # addresses HEM15/16, see https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/2000.html

  JEC_regrouped = [
    "CMS_ttHl_JESAbsoluteUp",           "CMS_ttHl_JESAbsoluteDown",
    "CMS_ttHl_JESAbsolute_EraUp",       "CMS_ttHl_JESAbsolute_EraDown",
    "CMS_ttHl_JESBBEC1Up",              "CMS_ttHl_JESBBEC1Down",
    "CMS_ttHl_JESBBEC1_EraUp",          "CMS_ttHl_JESBBEC1_EraDown",
    "CMS_ttHl_JESEC2Up",                "CMS_ttHl_JESEC2Down",
    "CMS_ttHl_JESEC2_EraUp",            "CMS_ttHl_JESEC2_EraDown",
    "CMS_ttHl_JESFlavorQCDUp",          "CMS_ttHl_JESFlavorQCDDown",
    "CMS_ttHl_JESHFUp",                 "CMS_ttHl_JESHFDown",
    "CMS_ttHl_JESHF_EraUp",             "CMS_ttHl_JESHF_EraDown",
    "CMS_ttHl_JESRelativeBalUp",        "CMS_ttHl_JESRelativeBalDown",
    "CMS_ttHl_JESRelativeSample_EraUp", "CMS_ttHl_JESRelativeSample_EraDown",
    JES_HEM,
  ]

  JER_split = [
    "CMS_ttHl_JERBarrelUp",        "CMS_ttHl_JERBarrelDown",
    "CMS_ttHl_JEREndcap1Up",       "CMS_ttHl_JEREndcap1Down",
    "CMS_ttHl_JEREndcap2LowPtUp",  "CMS_ttHl_JEREndcap2LowPtDown",
    "CMS_ttHl_JEREndcap2HighPtUp", "CMS_ttHl_JEREndcap2HighPtDown",
    "CMS_ttHl_JERForwardLowPtUp",  "CMS_ttHl_JERForwardLowPtDown",
    "CMS_ttHl_JERForwardHighPtUp", "CMS_ttHl_JERForwardHighPtDown",
  ]

  AK8_JES = [ "CMS_ttHl_AK8JESUp", "CMS_ttHl_AK8JESDown" ]
  AK8_JER = [ "CMS_ttHl_AK8JERUp", "CMS_ttHl_AK8JERDown" ]
  AK8_JMS = [ "CMS_ttHl_AK8JMSUp", "CMS_ttHl_AK8JMSDown" ]
  AK8_JMR = [ "CMS_ttHl_AK8JMRUp", "CMS_ttHl_AK8JMRDown" ]
  AK8 = AK8_JES + AK8_JER + AK8_JMS + AK8_JMR

  MEM_3l        = [ "CMS_ttHl_MEM_3l_LRUp",        "CMS_ttHl_MEM_3l_LRDown"        ]
  MEM_2lss_1tau = [ "CMS_ttHl_MEM_2lss_1tau_LRUp", "CMS_ttHl_MEM_2lss_1tau_LRDown" ]
  MEM_3l_1tau   = [ "CMS_ttHl_MEM_3l_1tau_LRUp",   "CMS_ttHl_MEM_3l_1tau_LRDown"   ]
  MEM = MEM_3l + MEM_2lss_1tau + MEM_3l_1tau

  class TTbar(object):
    hdamp = [ "TT_hdampUp", "TT_hdampDown" ]
    ue    = [ "TT_ueUp",    "TT_ueDown"    ]
    cr    = [ "TT_QCDbased", "TT_GluonMove", "TT_erdON" ]
    mtop  = [
      "TT_mtop166p5", "TT_mtop169p5", "TT_mtop171p5", "TT_mtop173p5", "TT_mtop175p5", "TT_mtop178p5",
    ]
    width = [
      "TT_widthx0p7", "TT_widthx0p85", "TT_widthx1p15", "TT_widthx1p3",
    ]
    full = hdamp + ue + cr + mtop + width

  class L1PreFiring_(object):
    up   = "CMS_ttHl_l1PreFireUp"
    down = "CMS_ttHl_l1PreFireDown"
    full = [ up, down ]

  L1PreFiring = L1PreFiring_().full

  class PU_(object):
    up   = "CMS_ttHl_pileupUp"
    down = "CMS_ttHl_pileupDown"
    full = [ up, down ]

  PU = PU_().full

  class topPtReweighting_(object):
    up   = "CMS_ttHl_topPtReweightingUp"
    down = "CMS_ttHl_topPtReweightingDown"
    full = [ up, down ]

  topPtReweighting = topPtReweighting_().full

  class PartonShower(object):

    class TT(object):

      class ISR_(object):
        up   = "CMS_ttHl_PS_TT_ISRUp"
        down = "CMS_ttHl_PS_TT_ISRDown"
        full = [ up, down ]

      class FSR_(object):
        up   = "CMS_ttHl_PS_TT_FSRUp"
        down = "CMS_ttHl_PS_TT_FSRDown"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_PS_TTUp"
        down = "CMS_ttHl_PS_TTDown"
        full = [ up, down ]

      full = ISR_().full + FSR_().full + env_().full
    
    class DY(object):

      class ISR_(object):
        up   = "CMS_ttHl_PS_DY_ISRUp"
        down = "CMS_ttHl_PS_DY_ISRDown"
        full = [ up, down ]

      class FSR_(object):
        up   = "CMS_ttHl_PS_DY_FSRUp"
        down = "CMS_ttHl_PS_DY_FSRDown"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_PS_DYUp"
        down = "CMS_ttHl_PS_DYDown"
        full = [ up, down ]

      full = ISR_().full + FSR_().full + env_().full
    
    class WJets(object):

      class ISR_(object):
        up   = "CMS_ttHl_PS_W_ISRUp"
        down = "CMS_ttHl_PS_W_ISRDown"
        full = [ up, down ]

      class FSR_(object):
        up   = "CMS_ttHl_PS_W_FSRUp"
        down = "CMS_ttHl_PS_W_FSRDown"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_PS_WUp"
        down = "CMS_ttHl_PS_WDown"
        full = [ up, down ]

      full = ISR_().full + FSR_().full + env_().full

    ttbar = TT().full
    dy    = DY().full
    wjets = WJets().full

    procs = [ TT, DY, WJets ]

    isr_up   = [ proc.ISR_().up   for proc in procs ]
    isr_down = [ proc.ISR_().down for proc in procs ]
    fsr_up   = [ proc.FSR_().up   for proc in procs ]
    fsr_down = [ proc.FSR_().down for proc in procs ]
    env_up   = [ proc.env_().up   for proc in procs ]
    env_down = [ proc.env_().down for proc in procs ]

    full = ttbar + dy + wjets

  class LHE(object):

    class TTH(object):

      class x1_(object):
        up   = "CMS_ttHl_thu_shape_ttH_x1Up"
        down = "CMS_ttHl_thu_shape_ttH_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_ttH_y1Up"
        down = "CMS_ttHl_thu_shape_ttH_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_ttHUp"
        down = "CMS_ttHl_thu_shape_ttHDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class THQ(object):

      class x1_(object):
        up   = "CMS_ttHl_thu_shape_tHq_x1Up"
        down = "CMS_ttHl_thu_shape_tHq_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_tHq_y1Up"
        down = "CMS_ttHl_thu_shape_tHq_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_tHqUp"
        down = "CMS_ttHl_thu_shape_tHqDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class THW(object):

      class x1_(object):
        up   = "CMS_ttHl_thu_shape_tHW_x1Up"
        down = "CMS_ttHl_thu_shape_tHW_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_tHW_y1Up"
        down = "CMS_ttHl_thu_shape_tHW_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_tHWUp"
        down = "CMS_ttHl_thu_shape_tHWDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class TTW(object):

      class x1_(object):
        up   = "CMS_ttHl_thu_shape_ttW_x1Up"
        down = "CMS_ttHl_thu_shape_ttW_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_ttW_y1Up"
        down = "CMS_ttHl_thu_shape_ttW_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_ttWUp"
        down = "CMS_ttHl_thu_shape_ttWDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class TTZ(object):

      class x1_(object):
        up   = "CMS_ttHl_thu_shape_ttZ_x1Up"
        down = "CMS_ttHl_thu_shape_ttZ_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_ttZ_y1Up"
        down = "CMS_ttHl_thu_shape_ttZ_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_ttZUp"
        down = "CMS_ttHl_thu_shape_ttZDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class HH(object):
      class x1_(object):

        up   = "CMS_ttHl_thu_shape_HH_x1Up"
        down = "CMS_ttHl_thu_shape_HH_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_HH_y1Up"
        down = "CMS_ttHl_thu_shape_HH_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_HHUp"
        down = "CMS_ttHl_thu_shape_HHDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class DY(object):
      class x1_(object):

        up   = "CMS_ttHl_thu_shape_DY_x1Up"
        down = "CMS_ttHl_thu_shape_DY_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_DY_y1Up"
        down = "CMS_ttHl_thu_shape_DY_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_DYUp"
        down = "CMS_ttHl_thu_shape_DYDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class TT(object):
      class x1_(object):

        up   = "CMS_ttHl_thu_shape_TT_x1Up"
        down = "CMS_ttHl_thu_shape_TT_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_TT_y1Up"
        down = "CMS_ttHl_thu_shape_TT_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_TTUp"
        down = "CMS_ttHl_thu_shape_TTDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    class WZ(object):
      class x1_(object):

        up   = "CMS_ttHl_thu_shape_WZ_x1Up"
        down = "CMS_ttHl_thu_shape_WZ_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_WZ_y1Up"
        down = "CMS_ttHl_thu_shape_WZ_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_WZUp"
        down = "CMS_ttHl_thu_shape_WZDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full
    
    class ZZ(object):
      class x1_(object):

        up   = "CMS_ttHl_thu_shape_ZZ_x1Up"
        down = "CMS_ttHl_thu_shape_ZZ_x1Down"
        full = [ up, down ]

      class y1_(object):
        up   = "CMS_ttHl_thu_shape_ZZ_y1Up"
        down = "CMS_ttHl_thu_shape_ZZ_y1Down"
        full = [ up, down ]

      class env_(object):
        up   = "CMS_ttHl_thu_shape_ZZUp"
        down = "CMS_ttHl_thu_shape_ZZDown"
        full = [ up, down ]

      full = x1_().full + y1_().full + env_().full

    ttH   = TTH().full
    tHq   = THQ().full
    tHW   = THW().full
    ttW   = TTW().full
    ttZ   = TTZ().full
    hh    = HH().full
    dy    = DY().full
    ttbar = TT().full
    wz    = WZ().full
    zz    = ZZ().full

    procs = [ TTH, THQ, THW, TTW, TTZ, HH, DY, TT, WZ, ZZ ]

    x1_up    = [ proc.x1_().up    for proc in procs ]
    x1_down  = [ proc.x1_().down  for proc in procs ]
    y1_down  = [ proc.y1_().down  for proc in procs ]
    y1_up    = [ proc.y1_().up    for proc in procs ]
    env_up   = [ proc.env_().up   for proc in procs ]
    env_down = [ proc.env_().down for proc in procs ]

    full = ttH + tHq + tHW + ttW + ttZ + dy + hh + ttbar + wz + zz

  class LeptonIDSF(object):
    el_tight = [ "CMS_ttHl_lepEff_eltightUp", "CMS_ttHl_lepEff_eltightDown" ]
    mu_tight = [ "CMS_ttHl_lepEff_mutightUp", "CMS_ttHl_lepEff_mutightDown" ]
    el_loose = [ "CMS_ttHl_lepEff_ellooseUp", "CMS_ttHl_lepEff_ellooseDown" ]
    mu_loose = [ "CMS_ttHl_lepEff_mulooseUp", "CMS_ttHl_lepEff_mulooseDown" ]

    full = el_tight + mu_tight + el_loose + mu_loose

  class Btag(object):

    HF       = [ "CMS_ttHl_btag_HFUp",       "CMS_ttHl_btag_HFDown"       ]
    HFStats1 = [ "CMS_ttHl_btag_HFStats1Up", "CMS_ttHl_btag_HFStats1Down" ]
    HFStats2 = [ "CMS_ttHl_btag_HFStats2Up", "CMS_ttHl_btag_HFStats2Down" ]

    LF       = [ "CMS_ttHl_btag_LFUp",       "CMS_ttHl_btag_LFDown"       ]
    LFStats1 = [ "CMS_ttHl_btag_LFStats1Up", "CMS_ttHl_btag_LFStats1Down" ]
    LFStats2 = [ "CMS_ttHl_btag_LFStats2Up", "CMS_ttHl_btag_LFStats2Down" ]

    cErr1 = [ "CMS_ttHl_btag_cErr1Up", "CMS_ttHl_btag_cErr1Down" ]
    cErr2 = [ "CMS_ttHl_btag_cErr2Up", "CMS_ttHl_btag_cErr2Down" ]

    full = HF + HFStats1 + HFStats2 + LF + LFStats1 + LFStats2 + cErr1 + cErr2

  class FakeRate_e_shape(object):
    pt         = [ "CMS_ttHl_FRe_shape_ptUp",         "CMS_ttHl_FRe_shape_ptDown"         ]
    norm       = [ "CMS_ttHl_FRe_shape_normUp",       "CMS_ttHl_FRe_shape_normDown"       ]
    eta_barrel = [ "CMS_ttHl_FRe_shape_eta_barrelUp", "CMS_ttHl_FRe_shape_eta_barrelDown" ]
    corr       = [ "CMS_ttHl_FRe_shape_corrUp",       "CMS_ttHl_FRe_shape_corrDown"       ]

    full = pt + norm + eta_barrel + corr

  class FakeRate_m_shape(object):
    pt         = [ "CMS_ttHl_FRm_shape_ptUp",         "CMS_ttHl_FRm_shape_ptDown"         ]
    norm       = [ "CMS_ttHl_FRm_shape_normUp",       "CMS_ttHl_FRm_shape_normDown"       ]
    eta_barrel = [ "CMS_ttHl_FRm_shape_eta_barrelUp", "CMS_ttHl_FRm_shape_eta_barrelDown" ]
    corr       = [ "CMS_ttHl_FRm_shape_corrUp",       "CMS_ttHl_FRm_shape_corrDown"       ]

    full = pt + norm + eta_barrel + corr

  class FakeRate_t(object):
    jt_norm  = [ "CMS_ttHl_FRjt_normUp",  "CMS_ttHl_FRjt_normDown"  ]
    jt_shape = [ "CMS_ttHl_FRjt_shapeUp", "CMS_ttHl_FRjt_shapeDown" ]
    et_shift = [ "CMS_ttHl_FRet_shiftUp", "CMS_ttHl_FRet_shiftDown" ]
    mt_shift = [ "CMS_ttHl_FRmt_shiftUp", "CMS_ttHl_FRmt_shiftDown" ]

    jt   = jt_norm + jt_shape
    full = jt + et_shift + mt_shift

  class Electron_energy(object):
    ER       = [ "CMS_ttHl_electronERUp",       "CMS_ttHl_electronERDown"       ]
    ESEndcap = [ "CMS_ttHl_electronESEndcapUp", "CMS_ttHl_electronESEndcapDown" ]
    ESBarrel = [ "CMS_ttHl_electronESBarrelUp", "CMS_ttHl_electronESBarrelDown" ]

    full = ER + ESEndcap + ESBarrel

  class Muon_energy(object):
    ER        = [ "CMS_ttHl_muonERUp",        "CMS_ttHl_muonERDown"        ]
    ESBarrel1 = [ "CMS_ttHl_muonESBarrel1Up", "CMS_ttHl_muonESBarrel1Down" ]
    ESBarrel2 = [ "CMS_ttHl_muonESBarrel2Up", "CMS_ttHl_muonESBarrel2Down" ]
    ESEndcap1 = [ "CMS_ttHl_muonESEndcap1Up", "CMS_ttHl_muonESEndcap1Down" ]
    ESEndcap2 = [ "CMS_ttHl_muonESEndcap2Up", "CMS_ttHl_muonESEndcap2Down" ]

    full = ER + ESBarrel1 + ESBarrel2 + ESEndcap1 + ESEndcap2

  lhe          = LHE().full
  btag         = Btag().full
  leptonIDSF   = LeptonIDSF().full
  FRe_shape    = FakeRate_e_shape().full
  FRm_shape    = FakeRate_m_shape().full
  FR_t         = FakeRate_t().full
  electron_E   = Electron_energy().full
  muon_E       = Muon_energy().full
  FR_all       = FRe_shape + FRm_shape + FR_t
  partonShower = PartonShower().full
  ttbar        = TTbar().full

  # Analysis-specific definitions
  an_leptonFR        =    central +  JES +  JER          + UnclusteredEn  + MET_ResponseSyst +  MET_ResolutionSyst
  an_leptonFR_opts   = [ "central", "JES", "JER",         "UnclusteredEn", "MET_ResponseSyst", "MET_ResolutionSyst" ]
  an_jetToTauFR      =    central +  JES         + tauES                   + tauIDSF
  an_jetToTauFR_opts = [ "central", "JES", "JER",          "UnclusteredEn", "tauIDSF" ]
  an_addMEM          =    central +  JES +  JER +  tauES +  UnclusteredEn
  an_addMEM_opts     = [ "central", "JES", "JER", "tauES", "UnclusteredEn" ]

  an_chargeFlip_e       =    central +  electron_E
  an_chargeFlip_e_opts  = [ "central", "electron_E" ]
  an_chargeFlip_mu      =    central +  muon_E
  an_chargeFlip_mu_opts = [ "central", "muon_E" ]

  an_tth = central + JES + JER + tauES + leptonIDSF + tauIDSF + UnclusteredEn + btag + FR_t + FRe_shape + FRm_shape + \
           lhe + triggerSF + PU + DYMCReweighting + DYMCNormScaleFactors + L1PreFiring + EWK_jet + EWK_bjet + partonShower
  an_tth_opts = [
    "central", "JES", "JER", "tauES", "leptonIDSF", "tauIDSF", "UnclusteredEn", "btag", "FR_t", "FRe_shape", "FRm_shape",
    "lhe", "triggerSF", "PU", "DYMCReweighting", "DYMCNormScaleFactors", "L1PreFiring", "EWK_jet", "EWK_bjet", "partonShower",
  ]

  an_inclusive      =    central +  JES +  JER +  tauES +  UnclusteredEn +  btag
  an_inclusive_opts = [ "central", "JES", "JER", "tauES", "UnclusteredEn", "btag" ]

  an_hh_multilepton = an_tth
  an_opts_hh_multilepton = an_tth_opts
  # TODO uncomment ttbar and/or AK8 once the Ntuples are ready
  an_hh_bbww = an_tth # + ttbar + AK8
  an_opts_hh_bbww = an_tth_opts # + "ttbar" + "AK8"

  an_internal_no_mem = central + leptonIDSF + tauIDSF + btag + FR_t + lhe + triggerSF + PU + L1PreFiring + \
                       FRe_shape + FRm_shape + DYMCReweighting + DYMCNormScaleFactors + topPtReweighting + \
                       EWK_jet + EWK_bjet + partonShower
  an_internal = an_internal_no_mem + MEM + triggerSF_split
