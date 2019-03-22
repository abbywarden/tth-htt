from tthAnalysis.HiggsToTauTau.configs.analyzeConfig import *
from tthAnalysis.HiggsToTauTau.jobTools import create_if_not_exists
from tthAnalysis.HiggsToTauTau.analysisTools import initDict, getKey, create_cfg, createFile, generateInputFileList, is_dymc_reweighting
from tthAnalysis.HiggsToTauTau.common import logging

import re

def get_lepton_and_hadTau_selection_and_frWeight(lepton_and_hadTau_selection, lepton_and_hadTau_frWeight):
  lepton_and_hadTau_selection_and_frWeight = lepton_and_hadTau_selection
  if lepton_and_hadTau_selection.startswith("Fakeable"):
    if lepton_and_hadTau_frWeight == "enabled":
      lepton_and_hadTau_selection_and_frWeight += "_wFakeRateWeights"
    elif lepton_and_hadTau_frWeight == "disabled":
      lepton_and_hadTau_selection_and_frWeight += "_woFakeRateWeights"
  lepton_and_hadTau_selection_and_frWeight = lepton_and_hadTau_selection_and_frWeight.replace("|", "_")
  return lepton_and_hadTau_selection_and_frWeight

def getHistogramDir(lepton_selection, hadTau_selection, lepton_and_hadTau_frWeight, lepton_charge_selection, chargeSumSelection):
  histogramDir = "2lss_1tau_lep%s_sum%s_%s" % (lepton_charge_selection, chargeSumSelection, lepton_selection)
  if lepton_selection.find("Fakeable") != -1 or hadTau_selection.find("Fakeable") != -1:
    if lepton_and_hadTau_frWeight == "enabled":
      histogramDir += "_wFakeRateWeights"
    elif lepton_and_hadTau_frWeight == "disabled":
      histogramDir += "_woFakeRateWeights"
  return histogramDir

class analyzeConfig_2lss_1tau(analyzeConfig):
  """Configuration metadata needed to run analysis in a single go.

     Sets up a folder structure by defining full path names; no directory creation is delegated here.

     Args specific to analyzeConfig_2lss_1tau:
       lepton_selections: either `Tight`, `Loose` or `Fakeable`
       hadTau_selection: either `dR05iso[Loose|Medium|Tight]` or `dR03mva[Medium|Tight|VTight|VVTight]`

     See $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/python/analyzeConfig.py
     for documentation of further Args.

  """
  def __init__(self,
        configDir,
        outputDir,
        executable_analyze,
        cfgFile_analyze,
        samples,
        MEMbranch,
        lep_mva_wp,
        lepton_charge_selections,
        hadTau_selection,
        hadTau_selection_veto,
        applyFakeRateWeights,
        chargeSumSelections,
        central_or_shifts,
        max_files_per_job,
        era,
        use_lumi,
        lumi,
        check_output_files,
        running_method,
        num_parallel_jobs,
        executable_addBackgrounds,
        executable_addFakes,
        executable_addFlips,
        histograms_to_fit,
        select_rle_output         = False,
        executable_prep_dcard     = "prepareDatacards",
        executable_add_syst_dcard = "addSystDatacards",
        do_sync                   = False,
        verbose                   = False,
        dry_run                   = False,
        isDebug                   = False,
        rle_select                = '',
        use_nonnominal            = False,
        hlt_filter                = False,
        use_home                  = True,
      ):
    analyzeConfig.__init__(self,
      configDir                 = configDir,
      outputDir                 = outputDir,
      executable_analyze        = executable_analyze,
      channel                   = "2lss_1tau",
      samples                   = samples,
      lep_mva_wp                = lep_mva_wp,
      central_or_shifts         = central_or_shifts,
      max_files_per_job         = max_files_per_job,
      era                       = era,
      use_lumi                  = use_lumi,
      lumi                      = lumi,
      check_output_files        = check_output_files,
      running_method            = running_method,
      num_parallel_jobs         = num_parallel_jobs,
      histograms_to_fit         = histograms_to_fit,
      triggers                  = [ '1e', '1mu', '2e', '2mu', '1e1mu' ],
      executable_prep_dcard     = executable_prep_dcard,
      executable_add_syst_dcard = executable_add_syst_dcard,
      do_sync                   = do_sync,
      verbose                   = verbose,
      dry_run                   = dry_run,
      isDebug                   = isDebug,
      use_home                  = use_home,
    )

    self.MEMbranch = MEMbranch

    self.lepton_and_hadTau_selections = [ "Tight", "Fakeable" ]
    self.lepton_and_hadTau_frWeights = [ "enabled", "disabled" ]
    self.lepton_charge_selections = lepton_charge_selections
    self.hadTau_selection_part2 = hadTau_selection
    self.hadTau_selection_veto = hadTau_selection_veto
    self.applyFakeRateWeights = applyFakeRateWeights
    run_mcClosure = 'central' not in self.central_or_shifts or len(central_or_shifts) > 1 or self.do_sync
    if self.era != '2017':
      logging.warning('mcClosure for lepton FR not possible for era %s' % self.era)
      run_mcClosure = False
    if run_mcClosure:
      # Run MC closure jobs only if the analysis is run w/ (at least some) systematic uncertainties
      #self.lepton_and_hadTau_selections.extend([ "Fakeable_mcClosure_all" ]) #TODO
      pass

    self.lepton_genMatches = [ "2l0g0j", "1l1g0j", "1l0g1j", "0l2g0j", "0l1g1j", "0l0g2j" ]
    self.hadTau_genMatches = [ "1t0e0m0j", "0t1e0m0j", "0t0e1m0j", "0t0e0m1j" ]

    self.apply_leptonGenMatching = None
    self.apply_hadTauGenMatching = None
    self.lepton_and_hadTau_genMatches_nonfakes = []
    self.lepton_and_hadTau_genMatches_conversions = []
    self.lepton_and_hadTau_genMatches_fakes = []
    self.lepton_and_hadTau_genMatches_gentau = []
    self.lepton_and_hadTau_genMatches_faketau = []
    if self.applyFakeRateWeights == "3L":
      self.apply_leptonGenMatching = True
      self.apply_hadTauGenMatching = True
      for lepton_genMatch in self.lepton_genMatches:
        for hadTau_genMatch in self.hadTau_genMatches:
          lepton_and_hadTau_genMatch = "&".join([ lepton_genMatch, hadTau_genMatch ])
          if lepton_genMatch.endswith("0g0j") and hadTau_genMatch.endswith("0j"):
            self.lepton_and_hadTau_genMatches_nonfakes.append(lepton_and_hadTau_genMatch)
          elif lepton_genMatch.endswith("0j") and hadTau_genMatch.endswith("0j"):
            self.lepton_and_hadTau_genMatches_conversions.append(lepton_and_hadTau_genMatch)
          else:
            self.lepton_and_hadTau_genMatches_fakes.append(lepton_and_hadTau_genMatch)
      if run_mcClosure:
        self.lepton_and_hadTau_selections.extend([ "Fakeable_mcClosure_e", "Fakeable_mcClosure_m", "Fakeable_mcClosure_t" ])
    elif applyFakeRateWeights == "2lepton":
      self.apply_leptonGenMatching = True
      self.apply_hadTauGenMatching = True
      for lepton_genMatch in self.lepton_genMatches:
        for hadTau_genMatch in self.hadTau_genMatches:
          lepton_and_hadTau_genMatch = "&".join([ lepton_genMatch, hadTau_genMatch ])
          if lepton_genMatch.endswith("0g0j"):
            self.lepton_and_hadTau_genMatches_nonfakes.append(lepton_and_hadTau_genMatch)
            if hadTau_genMatch.endswith("0j"):
              self.lepton_and_hadTau_genMatches_gentau.append(lepton_and_hadTau_genMatch)
            else:
              self.lepton_and_hadTau_genMatches_faketau.append(lepton_and_hadTau_genMatch)
          elif lepton_genMatch.endswith("0j"):
            self.lepton_and_hadTau_genMatches_conversions.append(lepton_and_hadTau_genMatch)
          else:
            self.lepton_and_hadTau_genMatches_fakes.append(lepton_and_hadTau_genMatch)
      if run_mcClosure:
        self.lepton_and_hadTau_selections.extend([ "Fakeable_mcClosure_e", "Fakeable_mcClosure_m" ])
    elif applyFakeRateWeights == "1tau":
      self.apply_leptonGenMatching = True
      self.apply_hadTauGenMatching = True
      for lepton_genMatch in self.lepton_genMatches:
        for hadTau_genMatch in self.hadTau_genMatches:
          if lepton_genMatch.find("0g") != -1 and hadTau_genMatch.endswith("0j"):
            self.lepton_and_hadTau_genMatches_nonfakes.append(hadTau_genMatch)
          elif hadTau_genMatch.endswith("0j"):
            self.lepton_and_hadTau_genMatches_conversions.append(hadTau_genMatch)
          else:
            self.lepton_and_hadTau_genMatches_fakes.append(hadTau_genMatch)
      if run_mcClosure:
        self.lepton_and_hadTau_selections.extend([ "Fakeable_mcClosure_t" ])
    else:
      raise ValueError("Invalid Configuration parameter 'applyFakeRateWeights' = %s !!" % applyFakeRateWeights)

    self.chargeSumSelections = chargeSumSelections

    self.executable_addBackgrounds = executable_addBackgrounds
    self.executable_addFakes = executable_addFakes
    self.executable_addFlips = executable_addFlips

    self.nonfake_backgrounds = [ "TT", "TTW", "TTWW", "TTZ", "EWK", "Rares", "tHq", "tHW", "VH" ]

    self.prep_dcard_processesToCopy = [ "data_obs" ] + self.nonfake_backgrounds + [ "conversions", "fakes_data", "fakes_mc", "flips_data" ]
    self.make_plots_backgrounds = [ "TTW", "TTZ", "TTWW", "EWK", "Rares", "tHq", "tHW" ] + [ "conversions", "fakes_data", "flips_data" ]

    self.cfgFile_analyze = os.path.join(self.template_dir, cfgFile_analyze)
    self.inputFiles_hadd_stage1_6 = {}
    self.outputFile_hadd_stage1_6 = {}
    self.cfgFile_addFlips = os.path.join(self.template_dir, "addBackgroundLeptonFlips_cfg.py")
    self.jobOptions_addFlips = {}
    self.histogramDir_prep_dcard = "2lss_1tau_lepSS_sum%s_Tight"
    self.histogramDir_prep_dcard_OS = "2lss_1tau_lepOS_sum%s_Tight"
    self.cfgFile_make_plots = os.path.join(self.template_dir, "makePlots_2lss_1tau_cfg.py")
    self.cfgFile_make_plots_mcClosure = os.path.join(self.template_dir, "makePlots_mcClosure_2lss_1tau_cfg.py")  #TODO

    self.select_rle_output = select_rle_output
    self.rle_select = rle_select
    self.use_nonnominal = use_nonnominal
    self.hlt_filter = hlt_filter

  def set_BDT_training(self, hadTau_selection_relaxed):
    """Run analysis with loose selection criteria for leptons and hadronic taus,
       for the purpose of preparing event list files for BDT training.
    """
    self.lepton_and_hadTau_selections = [ "forBDTtraining" ]
    self.lepton_and_hadTau_frWeights  = [ "disabled" ]
    super(analyzeConfig_2lss_1tau, self).set_BDT_training(hadTau_selection_relaxed)

  def createCfg_analyze(self, jobOptions, sample_info, lepton_and_hadTau_selection):
    """Create python configuration file for the analyze_2lss_1tau executable (analysis code)

       Args:
         inputFiles: list of input files (Ntuples)
         outputFile: output file of the job -- a ROOT file containing histogram
         process: either `TTW`, `TTZ`, `Rares`, `data_obs`, `ttH_hww`, 'ttH_hzg', 'ttH_hmm', `ttH_hzz` or `ttH_htt`
         is_mc: flag indicating whether job runs on MC (True) or data (False)
         lumi_scale: event weight (= xsection * luminosity / number of events)
         central_or_shift: either 'central' or one of the systematic uncertainties defined in $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/bin/analyze_2lss_1tau.cc
    """
    lepton_and_hadTau_frWeight = "disabled" if jobOptions['applyFakeRateWeights'] == "disabled" else "enabled"
    histogramDir = getHistogramDir(
      lepton_and_hadTau_selection, jobOptions['hadTauSelection'], lepton_and_hadTau_frWeight,
      jobOptions['leptonChargeSelection'], jobOptions['chargeSumSelection']
    )
    jobOptions['histogramDir'] = histogramDir
    if 'mcClosure' in lepton_and_hadTau_selection:
      self.mcClosure_dir['%s_%s_%s' % (lepton_and_hadTau_selection, jobOptions['leptonChargeSelection'], jobOptions['chargeSumSelection'] )] = histogramDir

    self.set_leptonFakeRateWeightHistogramNames(jobOptions['central_or_shift'], lepton_and_hadTau_selection)
    jobOptions['leptonFakeRateWeight.inputFileName'] = self.leptonFakeRateWeight_inputFile
    jobOptions['leptonFakeRateWeight.histogramName_e'] = self.leptonFakeRateWeight_histogramName_e
    jobOptions['leptonFakeRateWeight.histogramName_mu'] = self.leptonFakeRateWeight_histogramName_mu

    jobOptions['hadTauFakeRateWeight.inputFileName'] = self.hadTauFakeRateWeight_inputFile
    jobOptions['hadTauFakeRateWeight.lead.graphName'] = 'jetToTauFakeRate/%s/$etaBin/jetToTauFakeRate_mc_hadTaus_pt' % self.hadTau_selection_part2
    jobOptions['hadTauFakeRateWeight.lead.fitFunctionName'] = 'jetToTauFakeRate/%s/$etaBin/fitFunction_data_div_mc_hadTaus_pt' % self.hadTau_selection_part2
    if 'mcClosure' in lepton_and_hadTau_selection:
      jobOptions['hadTauFakeRateWeight.applyGraph_lead'] = True
      jobOptions['hadTauFakeRateWeight.applyGraph_sublead'] = True
      jobOptions['hadTauFakeRateWeight.applyFitFunction_lead'] = False
      jobOptions['hadTauFakeRateWeight.applyFitFunction_sublead'] = False
      if self.applyFakeRateWeights not in [ "3L", "1tau" ] and not self.isBDTtraining:
        # We want to preserve the same logic as running in SR and applying the FF method only to leptons [*]
        jobOptions['hadTauFakeRateWeight.applyFitFunction_lead'] = True
        jobOptions['hadTauFakeRateWeight.applyFitFunction_sublead'] = True
    if jobOptions['hadTauSelection'].find("Tight") != -1 and self.applyFakeRateWeights not in [ "3L", "1tau" ] and not self.isBDTtraining:
      # [*] SR and applying the FF method only to leptons
      jobOptions['hadTauFakeRateWeight.applyGraph_lead'] = False # FR in MC for the leading tau
      jobOptions['hadTauFakeRateWeight.applyGraph_sublead'] = False
      jobOptions['hadTauFakeRateWeight.applyFitFunction_lead'] = True # data-to-MC SF for the leading tau
      jobOptions['hadTauFakeRateWeight.applyFitFunction_sublead'] = True
      jobOptions['apply_hadTauFakeRateSF'] = True

    lines = super(analyzeConfig_2lss_1tau, self).createCfg_analyze(jobOptions, sample_info)
    create_cfg(self.cfgFile_analyze, jobOptions['cfgFile_modified'], lines)

  def createCfg_addFlips(self, jobOptions):
    """Create python configuration file for the addBackgroundLeptonFlips executable (data-driven estimation of 'Flips' backgrounds)

       Args:
         inputFiles: input file (the ROOT file produced by hadd_stage1)
         outputFile: output file of the job
    """
    lines = []
    lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
    lines.append("process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(jobOptions['outputFile']))
    lines.append("process.addBackgroundLeptonFlips.categories = cms.VPSet(")
    lines.append("    cms.PSet(")
    lines.append("        signal = cms.string('%s')," % jobOptions['category_signal'])
    lines.append("        sideband = cms.string('%s')" % jobOptions['category_sideband'])
    lines.append("    )")
    lines.append(")")
    processesToSubtract = [ "fakes_data" ]
    processesToSubtract.extend([ "%s_conversion" % nonfake_background for nonfake_background in self.nonfake_backgrounds ])
    lines.append("process.addBackgroundLeptonFlips.processesToSubtract = cms.vstring(%s)" % processesToSubtract)
    lines.append("process.addBackgroundLeptonFlips.sysShifts = cms.vstring(%s)" % self.central_or_shifts)
    create_cfg(self.cfgFile_addFlips, jobOptions['cfgFile_modified'], lines)

  def addToMakefile_hadd_stage1_6(self, lines_makefile):
    """Adds the commands to Makefile that are necessary for building the intermediate histogram file
       that is used as input for data-driven background estimation.
    """
    for chargeSumSelection in self.chargeSumSelections:
      key_hadd_stage1_6 = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "OS", chargeSumSelection)
      self.addToMakefile_hadd(lines_makefile, { key_hadd_stage1_6 : self.inputFiles_hadd_stage1_6[key_hadd_stage1_6] },
                                              { key_hadd_stage1_6 : self.outputFile_hadd_stage1_6[key_hadd_stage1_6] }, "stage1_6")

  def addToMakefile_addFlips(self, lines_makefile):
    if self.is_sbatch:
      lines_makefile.append("sbatch_addFlips: %s" % " ".join([ jobOptions['inputFile'] for jobOptions in self.jobOptions_addFlips.values() ]))
      lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_addFlips))
      lines_makefile.append("")
    for jobOptions in self.jobOptions_addFlips.values():
      if self.is_makefile:
        lines_makefile.append("%s: %s" % (jobOptions['outputFile'], jobOptions['inputFile']))
        lines_makefile.append("\t%s %s &> %s" % (self.executable_addFlips, jobOptions['cfgFile_modified'], jobOptions['logFile']))
        lines_makefile.append("")
      elif self.is_sbatch:
        lines_makefile.append("%s: %s" % (jobOptions['outputFile'], "sbatch_addFlips"))
        lines_makefile.append("\t%s" % ":") # CV: null command
        lines_makefile.append("")
      self.filesToClean.append(jobOptions['outputFile'])

  def addToMakefile_backgrounds_from_data(self, lines_makefile):
    self.addToMakefile_addBackgrounds(lines_makefile, "sbatch_addBackgrounds", self.sbatchFile_addBackgrounds, self.jobOptions_addBackgrounds)
    self.addToMakefile_addBackgrounds(lines_makefile, "sbatch_addBackgrounds_sum", self.sbatchFile_addBackgrounds_sum, self.jobOptions_addBackgrounds_sum)
    self.addToMakefile_hadd_stage1_5(lines_makefile)
    self.addToMakefile_addFakes(lines_makefile)
    self.addToMakefile_hadd_stage1_6(lines_makefile)
    self.addToMakefile_addFlips(lines_makefile)

  def create(self):
    """Creates all necessary config files and runs the complete analysis workfow -- either locally or on the batch system
    """

    for sample_name, sample_info in self.samples.items():
      if not sample_info["use_it"] or sample_info["sample_category"] in [ "additional_signal_overlap", "background_data_estimate" ]:
        continue
      process_name = sample_info["process_name_specific"]
      logging.info("Building dictionaries for sample %s..." % process_name)
      for lepton_and_hadTau_selection in self.lepton_and_hadTau_selections:
        for lepton_and_hadTau_frWeight in self.lepton_and_hadTau_frWeights:
          if lepton_and_hadTau_frWeight == "enabled" and not lepton_and_hadTau_selection.startswith("Fakeable"):
            continue
          lepton_and_hadTau_selection_and_frWeight = get_lepton_and_hadTau_selection_and_frWeight(lepton_and_hadTau_selection, lepton_and_hadTau_frWeight)
          for lepton_charge_selection in self.lepton_charge_selections:
            for chargeSumSelection in self.chargeSumSelections:
              central_or_shifts_extended = [ "" ]
              central_or_shifts_extended.extend(self.central_or_shifts)
              for central_or_shift_or_dummy in central_or_shifts_extended:
                process_name_extended = [ process_name, "hadd", "addBackgrounds" ]
                for process_name_or_dummy in process_name_extended:
                  key_dir = getKey(process_name_or_dummy, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection, central_or_shift_or_dummy)
                  for dir_type in [ DKEY_CFGS, DKEY_HIST, DKEY_LOGS, DKEY_RLES, DKEY_SYNC ]:
                    initDict(self.dirs, [ key_dir, dir_type ])
                    if dir_type in [ DKEY_CFGS, DKEY_LOGS ]:
                      self.dirs[key_dir][dir_type] = os.path.join(self.configDir, dir_type, self.channel,
                        "_".join([ lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection ]), process_name_or_dummy, central_or_shift_or_dummy)
                    else:
                      self.dirs[key_dir][dir_type] = os.path.join(self.outputDir, dir_type, self.channel,
                        "_".join([ lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection ]), process_name_or_dummy, central_or_shift_or_dummy)
    for dir_type in [ DKEY_CFGS, DKEY_SCRIPTS, DKEY_HIST, DKEY_LOGS, DKEY_DCRD, DKEY_PLOT, DKEY_HADD_RT, DKEY_SYNC ]:
      initDict(self.dirs, [ dir_type ])
      if dir_type in [ DKEY_CFGS, DKEY_SCRIPTS, DKEY_LOGS, DKEY_DCRD, DKEY_PLOT, DKEY_HADD_RT ]:
        self.dirs[dir_type] = os.path.join(self.configDir, dir_type, self.channel)
      else:
        self.dirs[dir_type] = os.path.join(self.outputDir, dir_type, self.channel)

    numDirectories = len(self.dirs.keys())
    logging.info("Creating directory structure (numDirectories = %i)" % numDirectories)
    numDirectories_created = 0;
    for key in self.dirs.keys():
      if type(self.dirs[key]) == dict:
        for dir_type in self.dirs[key].keys():
          create_if_not_exists(self.dirs[key][dir_type])
      else:
        create_if_not_exists(self.dirs[key])
      numDirectories_created = numDirectories_created + 1
      if (numDirectories_created % (numDirectories / 100)) == 0:
        logging.info(" %i%% completed" % (numDirectories_created / (numDirectories / 100)))
    logging.info("done.")

    inputFileLists = {}
    for sample_name, sample_info in self.samples.items():
      if not sample_info["use_it"] or sample_info["sample_category"] in [ "additional_signal_overlap", "background_data_estimate" ]:
        continue
      logging.info("Checking input files for sample %s" % sample_info["process_name_specific"])
      inputFileLists[sample_name] = generateInputFileList(sample_info, self.max_files_per_job)

    mcClosure_regex = re.compile('Fakeable_mcClosure_(?P<type>m|e|t)_wFakeRateWeights')
    for lepton_and_hadTau_selection in self.lepton_and_hadTau_selections:
      lepton_selection = lepton_and_hadTau_selection
      hadTau_selection = lepton_and_hadTau_selection
      electron_selection = lepton_selection
      muon_selection = lepton_selection

      if self.applyFakeRateWeights == "2lepton":
        hadTau_selection = "Tight"
        hadTau_selection = "|".join([ hadTau_selection, self.hadTau_selection_part2 ])

      if lepton_and_hadTau_selection == "forBDTtraining":
        electron_selection = "Loose"
        muon_selection = "Loose"
        if not self.applyFakeRateWeights == "2lepton":
            hadTau_selection = "Tight|%s" % self.hadTau_selection_relaxed
            # hadTau ID it is the same level of analysis if applyFakeRateWeights == "2lepton"
      elif lepton_and_hadTau_selection == "Fakeable_mcClosure_e":
        electron_selection = "Fakeable"
        muon_selection = "Tight"
        hadTau_selection = "Tight"
        hadTau_selection = "|".join([ hadTau_selection, self.hadTau_selection_part2 ])
      elif lepton_and_hadTau_selection == "Fakeable_mcClosure_m":
        electron_selection = "Tight"
        muon_selection = "Fakeable"
        hadTau_selection = "Tight"
        hadTau_selection = "|".join([ hadTau_selection, self.hadTau_selection_part2 ])
      elif lepton_and_hadTau_selection == "Fakeable_mcClosure_t":
        electron_selection = "Tight"
        muon_selection = "Tight"
        hadTau_selection = "Fakeable"
        hadTau_selection = "|".join([ hadTau_selection, self.hadTau_selection_part2 ])

      for lepton_and_hadTau_frWeight in self.lepton_and_hadTau_frWeights:
        if lepton_and_hadTau_frWeight == "enabled" and not lepton_and_hadTau_selection.startswith("Fakeable"):
          continue
        if lepton_and_hadTau_frWeight == "disabled" and not lepton_and_hadTau_selection in [ "Tight", "forBDTtraining" ]:
          continue
        lepton_and_hadTau_selection_and_frWeight = get_lepton_and_hadTau_selection_and_frWeight(lepton_and_hadTau_selection, lepton_and_hadTau_frWeight)

        for lepton_charge_selection in self.lepton_charge_selections:
          for chargeSumSelection in self.chargeSumSelections:

            if 'mcClosure' in lepton_and_hadTau_selection and lepton_charge_selection != 'SS':
              # Run MC closure only for the region that complements the SR
              continue

            for sample_name, sample_info in self.samples.items():
              if not sample_info["use_it"] or sample_info["sample_category"] in [ "additional_signal_overlap", "background_data_estimate" ]:
                continue
              process_name = sample_info["process_name_specific"]
              logging.info("Creating configuration files to run '%s' for sample %s" % (self.executable_analyze, process_name))

              sample_category = sample_info["sample_category"]
              is_mc = (sample_info["type"] == "mc")
              is_signal = (sample_category == "signal")

              for central_or_shift in self.central_or_shifts:

                inputFileList = inputFileLists[sample_name]
                for jobId in inputFileList.keys():
                  if central_or_shift != "central":
                    isFR_shape_shift = (central_or_shift in systematics.FR_all)
                    if not ((lepton_and_hadTau_selection == "Fakeable" and lepton_charge_selection == "SS" and isFR_shape_shift) or
                            (lepton_and_hadTau_selection == "Tight"    and lepton_charge_selection == "SS")):
                      continue
                    if not is_mc and not isFR_shape_shift:
                      continue

                  if central_or_shift in systematics.LHE().ttH and sample_category != "signal":
                    continue
                  if central_or_shift in systematics.LHE().ttW and sample_category != "TTW":
                    continue
                  if central_or_shift in systematics.LHE().ttZ and sample_category != "TTZ":
                    continue
                  if central_or_shift in systematics.DYMCReweighting and not is_dymc_reweighting(sample_name):
                    continue

                  logging.info(" ... for '%s' and systematic uncertainty option '%s'" % (lepton_and_hadTau_selection_and_frWeight, central_or_shift))

                  # build config files for executing analysis code
                  key_analyze_dir = getKey(process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection, central_or_shift)
                  analyze_job_tuple = (process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection, central_or_shift, jobId)
                  key_analyze_job = getKey(*analyze_job_tuple)
                  ntupleFiles = inputFileList[jobId]
                  if len(ntupleFiles) == 0:
                    logging.warning("No input ntuples for %s --> skipping job !!" % (key_analyze_job))
                    continue

                  syncOutput = ''
                  syncTree = ''
                  syncRequireGenMatching = True
                  if self.do_sync:
                    if chargeSumSelection != 'OS':
                      continue
                    mcClosure_match = mcClosure_regex.match(lepton_and_hadTau_selection_and_frWeight)
                    if lepton_and_hadTau_selection_and_frWeight == 'Tight':
                      if lepton_charge_selection == 'SS':
                        syncOutput = os.path.join(self.dirs[key_analyze_dir][DKEY_SYNC], '%s_%s_SR.root' % (self.channel, central_or_shift))
                        syncTree   = 'syncTree_%s_SR' % self.channel.replace('_', '').replace('ss', 'SS')
                      elif lepton_charge_selection == 'OS':
                        syncOutput = os.path.join(self.dirs[key_analyze_dir][DKEY_SYNC], '%s_%s_Flip.root' % (self.channel, central_or_shift))
                        syncTree   = 'syncTree_%s_Flip' % self.channel.replace('_', '').replace('ss', 'SS')
                      else:
                        continue
                      syncRequireGenMatching = True
                    elif lepton_and_hadTau_selection_and_frWeight == 'Fakeable_wFakeRateWeights' and lepton_charge_selection == 'SS':
                      syncOutput = os.path.join(self.dirs[key_analyze_dir][DKEY_SYNC], '%s_%s_Fake.root' % (self.channel, central_or_shift))
                      syncTree   = 'syncTree_%s_Fake' % self.channel.replace('_', '').replace('ss', 'SS')
                    elif mcClosure_match:
                      mcClosure_type = mcClosure_match.group('type')
                      syncOutput = os.path.join(self.dirs[key_analyze_dir][DKEY_SYNC], '%s_%s_mcClosure_%s.root' % (self.channel, central_or_shift, mcClosure_type))
                      syncTree = 'syncTree_%s_mcClosure_%s' % (self.channel.replace('_', '').replace('ss', 'SS'), mcClosure_type)
                    else:
                      continue
                  if syncTree and central_or_shift != "central":
                    syncTree = os.path.join(central_or_shift, syncTree)
                  syncRLE = ''
                  if self.do_sync and self.rle_select:
                    syncRLE = self.rle_select % syncTree
                    if not os.path.isfile(syncRLE):
                      logging.warning("Input RLE file for the sync is missing: %s; skipping the job" % syncRLE)
                      continue
                  if syncOutput:
                    self.inputFiles_sync['sync'].append(syncOutput)

                  cfgFile_modified_path = os.path.join(self.dirs[key_analyze_dir][DKEY_CFGS], "analyze_%s_%s_lep%s_sum%s_%s_%i_cfg.py" % analyze_job_tuple)
                  logFile_path = os.path.join(self.dirs[key_analyze_dir][DKEY_LOGS], "analyze_%s_%s_lep%s_sum%s_%s_%i.log" % analyze_job_tuple)
                  rleOutputFile_path = os.path.join(self.dirs[key_analyze_dir][DKEY_RLES], "rle_%s_%s_lep%s_sum%s_%s_%i.txt" % analyze_job_tuple) \
                                       if self.select_rle_output else ""
                  histogramFile_path = os.path.join(self.dirs[key_analyze_dir][DKEY_HIST], "analyze_%s_%s_lep%s_sum%s_%s_%i.root" % analyze_job_tuple)
                  applyFakeRateWeights = self.applyFakeRateWeights \
                    if self.isBDTtraining or not  (lepton_selection == "Tight" and hadTau_selection.find("Tight") != -1) \
                    else "disabled"
                  branchName_memOutput = '%s_%s' % (self.MEMbranch, self.get_addMEM_systematics(central_or_shift)) \
                                         if self.MEMbranch else ''

                  self.jobOptions_analyze[key_analyze_job] = {
                    'ntupleFiles'              : ntupleFiles,
                    'cfgFile_modified'         : cfgFile_modified_path,
                    'histogramFile'            : histogramFile_path,
                    'logFile'                  : logFile_path,
                    'selEventsFileName_output' : rleOutputFile_path,
                    'electronSelection'        : electron_selection,
                    'muonSelection'            : muon_selection,
                    'lep_mva_cut'              : self.lep_mva_cut,
                    'apply_leptonGenMatching'  : self.apply_leptonGenMatching,
                    'leptonChargeSelection'    : lepton_charge_selection,
                    'hadTauSelection'          : hadTau_selection,
                    'hadTauSelection_veto'     : self.hadTau_selection_veto,
                    'apply_hadTauGenMatching'  : self.apply_hadTauGenMatching,
                    'chargeSumSelection'       : chargeSumSelection,
                    'applyFakeRateWeights'     : applyFakeRateWeights,
                    'central_or_shift'         : central_or_shift,
                    'selectBDT'                : self.isBDTtraining,
                    'branchName_memOutput'     : branchName_memOutput,
                    'syncOutput'               : syncOutput,
                    'syncTree'                 : syncTree,
                    'syncRLE'                  : syncRLE,
                    'syncRequireGenMatching'   : syncRequireGenMatching,
                    'apply_hlt_filter'         : self.hlt_filter,
                    'useNonNominal'            : self.use_nonnominal,
                    'fillGenEvtHistograms'     : True,
                  }
                  self.createCfg_analyze(self.jobOptions_analyze[key_analyze_job], sample_info, lepton_and_hadTau_selection)

                  # initialize input and output file names for hadd_stage1
                  key_hadd_stage1_dir = getKey(process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                  hadd_stage1_job_tuple = (process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                  key_hadd_stage1_job = getKey(*hadd_stage1_job_tuple)
                  if not key_hadd_stage1_job in self.inputFiles_hadd_stage1:
                    self.inputFiles_hadd_stage1[key_hadd_stage1_job] = []
                  self.inputFiles_hadd_stage1[key_hadd_stage1_job].append(self.jobOptions_analyze[key_analyze_job]['histogramFile'])
                  self.outputFile_hadd_stage1[key_hadd_stage1_job] = os.path.join(self.dirs[key_hadd_stage1_dir][DKEY_HIST],
                                                                                  "hadd_stage1_%s_%s_lep%s_sum%s.root" % hadd_stage1_job_tuple)

                  if self.isBDTtraining:
                    self.targets.append(self.outputFile_hadd_stage1[key_hadd_stage1_job])

              if self.isBDTtraining or self.do_sync:
                continue

              if is_mc:
                logging.info("Creating configuration files to run 'addBackgrounds' for sample %s" % process_name)

                sample_categories = [ sample_category ]
                if is_signal:
                  sample_categories = [ "signal", "ttH", "ttH_htt", "ttH_hww", "ttH_hzz", "ttH_hmm", "ttH_hzg" ]
                for sample_category in sample_categories:
                  # sum non-fake and fake contributions for each MC sample separately
                  genMatch_categories = [ "nonfake", "conversions", "fake" ]

                  # in case fake background method is applied to leptons only,
                  # split events with genuine leptons (taken from MC) into "gentau" and "faketau" parts,
                  # so that different systematic uncertainties can be applied to both parts
                  if self.applyFakeRateWeights == "2lepton":
                    genMatch_categories.extend([ "gentau", "faketau" ])

                  for genMatch_category in genMatch_categories:
                    key_hadd_stage1_job = getKey(process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    key_addBackgrounds_dir = getKey(process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    addBackgrounds_job_tuple = None
                    processes_input = None
                    process_output = None
                    cfgFile_modified = None
                    outputFile = None
                    if genMatch_category == "nonfake":
                      # sum non-fake contributions for each MC sample separately
                      # input processes: TT2l0g0j; ...
                      # output processes: TT; ...
                      if sample_category in [ "signal" ]:
                        lepton_and_hadTau_genMatches = []
                        lepton_and_hadTau_genMatches.extend(self.lepton_and_hadTau_genMatches_nonfakes)
                        lepton_and_hadTau_genMatches.extend(self.lepton_and_hadTau_genMatches_conversions)
                        lepton_and_hadTau_genMatches.extend(self.lepton_and_hadTau_genMatches_fakes)
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in lepton_and_hadTau_genMatches ]
                      elif sample_category in [ "ttH" ]:
                        lepton_and_hadTau_genMatches = []
                        lepton_and_hadTau_genMatches.extend(self.lepton_and_hadTau_genMatches_nonfakes)
                        lepton_and_hadTau_genMatches.extend(self.lepton_and_hadTau_genMatches_conversions)
                        processes_input = []
                        processes_input.extend([ "%s%s" % ("ttH_htt", genMatch) for genMatch in lepton_and_hadTau_genMatches ])
                        processes_input.extend([ "%s%s" % ("ttH_hww", genMatch) for genMatch in lepton_and_hadTau_genMatches ])
                        processes_input.extend([ "%s%s" % ("ttH_hzz", genMatch) for genMatch in lepton_and_hadTau_genMatches ])
                        processes_input.extend([ "%s%s" % ("ttH_hzg", genMatch) for genMatch in lepton_and_hadTau_genMatches ])
                        processes_input.extend([ "%s%s" % ("ttH_hmm", genMatch) for genMatch in lepton_and_hadTau_genMatches ])
                      else:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_nonfakes ]
                      process_output = sample_category                     
                      addBackgrounds_job_tuple = (process_name, sample_category, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    elif genMatch_category == "conversions":
                      # sum conversion background contributions for each MC sample separately
                      # input processes: TT1l1g0j, TT0l2g0j; ...
                      # output processes: TT_conversion; ...
                      if sample_category in [ "signal" ]:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ]
                      elif sample_category in [ "ttH" ]:
                        processes_input = []
                        processes_input.extend([ "%s%s" % ("ttH_htt", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ])
                        processes_input.extend([ "%s%s" % ("ttH_hww", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ])
                        processes_input.extend([ "%s%s" % ("ttH_hzz", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ])
                        processes_input.extend([ "%s%s" % ("ttH_hzg", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ])
                        processes_input.extend([ "%s%s" % ("ttH_hmm", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ])
                      else:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_conversions ]
                      process_output = "%s_conversion" % sample_category
                      addBackgrounds_job_tuple = (process_name, "%s_conversion" % sample_category, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    elif genMatch_category == "fake":
                      # sum fake background contributions for each MC sample separately
                      # input processes: TT1l0g1j, TT0l1g1j, TT0l0g2j; ...
                      # output processes: TT_fake; ...
                      if sample_category in [ "signal" ]:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ]
                      elif sample_category in [ "ttH" ]:
                        processes_input = []
                        processes_input.extend([ "%s%s" % ("ttH_htt", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ])
                        processes_input.extend([ "%s%s" % ("ttH_hww", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ])
                        processes_input.extend([ "%s%s" % ("ttH_hzz", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ])
                        processes_input.extend([ "%s%s" % ("ttH_hzg", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ])
                        processes_input.extend([ "%s%s" % ("ttH_hmm", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ])
                      else:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_fakes ]
                      process_output = "%s_fake" % sample_category
                      addBackgrounds_job_tuple = (process_name, "%s_fake" % sample_category, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    elif genMatch_category == "gentau":
                      # sum contributions with genuine leptons and genuine taus
                      # input processes: TT2l0g0j1t0e0m0j, TT2l0j0t1e0m0j, TT2l0j0t0e1m0j; ...
                      # output processes: TT_gentau; ...
                      if sample_category in [ "signal" ]:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ]
                      elif sample_category in [ "ttH" ]:
                        processes_input = []
                        processes_input.extend([ "%s%s" % ("ttH_htt", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ])
                        processes_input.extend([ "%s%s" % ("ttH_hww", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ])
                        processes_input.extend([ "%s%s" % ("ttH_hzz", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ])
                        processes_input.extend([ "%s%s" % ("ttH_hzg", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ])
                        processes_input.extend([ "%s%s" % ("ttH_hmm", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ])
                      else:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_gentau ]
                      process_output = "%s_gentau" % sample_category
                      addBackgrounds_job_tuple = (process_name, "%s_gentau" % sample_category, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    elif genMatch_category == "faketau":
                      # sum contributions with genuine leptons and fake taus
                      # input processes: TT2l0g0j0t0e0m1j; ...
                      # output processes: TT_faketau; ...
                      if sample_category in [ "signal" ]:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ]
                      elif sample_category in [ "ttH" ]:
                        processes_input = []
                        processes_input.extend([ "%s%s" % ("ttH_htt", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ])
                        processes_input.extend([ "%s%s" % ("ttH_hww", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ])
                        processes_input.extend([ "%s%s" % ("ttH_hzz", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ])
                        processes_input.extend([ "%s%s" % ("ttH_hzg", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ])
                        processes_input.extend([ "%s%s" % ("ttH_hmm", genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ])
                      else:
                        processes_input = [ "%s%s" % (sample_category, genMatch) for genMatch in self.lepton_and_hadTau_genMatches_faketau ]
                      process_output = "%s_faketau" % sample_category
                      addBackgrounds_job_tuple = (process_name, "%s_faketau" % sample_category, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                    if processes_input:
                      logging.info(" ...for genMatch option = '%s'" % genMatch_category)
                      key_addBackgrounds_job = getKey(*addBackgrounds_job_tuple)
                      cfgFile_modified = os.path.join(self.dirs[key_addBackgrounds_dir][DKEY_CFGS], "addBackgrounds_%s_%s_%s_lep%s_sum%s_cfg.py" % addBackgrounds_job_tuple)
                      outputFile = os.path.join(self.dirs[key_addBackgrounds_dir][DKEY_HIST], "addBackgrounds_%s_%s_%s_lep%s_sum%s.root" % addBackgrounds_job_tuple)
                      self.jobOptions_addBackgrounds[key_addBackgrounds_job] = {
                        'inputFile' : self.outputFile_hadd_stage1[key_hadd_stage1_job],
                        'cfgFile_modified' : cfgFile_modified,
                        'outputFile' : outputFile,
                        'logFile' : os.path.join(self.dirs[key_addBackgrounds_dir][DKEY_LOGS], os.path.basename(cfgFile_modified).replace("_cfg.py", ".log")),
                        'categories' : [ getHistogramDir(lepton_selection, hadTau_selection, lepton_and_hadTau_frWeight, lepton_charge_selection, chargeSumSelection) ],
                        'processes_input' : processes_input,
                        'process_output' : process_output
                      }
                      self.createCfg_addBackgrounds(self.jobOptions_addBackgrounds[key_addBackgrounds_job])

                      # initialize input and output file names for hadd_stage1_5
                      key_hadd_stage1_5_dir = getKey("hadd", lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                      hadd_stage1_5_job_tuple = (lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                      key_hadd_stage1_5_job = getKey(*hadd_stage1_5_job_tuple)
                      if not key_hadd_stage1_5_job in self.inputFiles_hadd_stage1_5:
                        self.inputFiles_hadd_stage1_5[key_hadd_stage1_5_job] = []
                      self.inputFiles_hadd_stage1_5[key_hadd_stage1_5_job].append(self.jobOptions_addBackgrounds[key_addBackgrounds_job]['outputFile'])
                      self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job] = os.path.join(self.dirs[key_hadd_stage1_5_dir][DKEY_HIST],
                                                                                          "hadd_stage1_5_%s_lep%s_sum%s.root" % hadd_stage1_5_job_tuple)

              if self.isBDTtraining or self.do_sync:
                continue

              # add output files of hadd_stage1 for data to list of input files for hadd_stage1_5
              if not is_mc:
                key_hadd_stage1_job = getKey(process_name, lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                key_hadd_stage1_5_job = getKey(lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
                if not key_hadd_stage1_5_job in self.inputFiles_hadd_stage1_5:
                  self.inputFiles_hadd_stage1_5[key_hadd_stage1_5_job] = []
                self.inputFiles_hadd_stage1_5[key_hadd_stage1_5_job].append(self.outputFile_hadd_stage1[key_hadd_stage1_job])

            if self.isBDTtraining or self.do_sync:
              continue

            # sum fake background contributions for the total of all MC samples
            # input processes: TT1l0g1j, TT0l1g1j, TT0l0g2j; ...
            # output process: fakes_mc
            key_hadd_stage1_5_job = getKey(lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            key_addBackgrounds_dir = getKey("addBackgrounds", lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            addBackgrounds_job_fakes_tuple = ("fakes_mc", lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            key_addBackgrounds_job_fakes = getKey(*addBackgrounds_job_fakes_tuple)
            sample_categories = []
            sample_categories.extend(self.nonfake_backgrounds)
            sample_categories.extend([ "signal" ])
            processes_input = []
            for sample_category in sample_categories:
              processes_input.append("%s_fake" % sample_category)
            self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_fakes] = {
              'inputFile' : self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job],
              'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "addBackgrounds_%s_%s_lep%s_sum%s_cfg.py" % addBackgrounds_job_fakes_tuple),
              'outputFile' : os.path.join(self.dirs[DKEY_HIST], "addBackgrounds_%s_%s_lep%s_sum%s.root" % addBackgrounds_job_fakes_tuple),
              'logFile' : os.path.join(self.dirs[DKEY_LOGS], "addBackgrounds_%s_%s_lep%s_sum%s.log" % addBackgrounds_job_fakes_tuple),
              'categories' : [ getHistogramDir(lepton_selection, hadTau_selection, lepton_and_hadTau_frWeight, lepton_charge_selection, chargeSumSelection) ],
              'processes_input' : processes_input,
              'process_output' : "fakes_mc"
            }
            self.createCfg_addBackgrounds(self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_fakes])

            # sum conversion background contributions for the total of all MC sample
            # input processes: TT1l1g0j, TT0l2g0j; ...
            # output process: conversions
            addBackgrounds_job_conversions_tuple = ("conversions", lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            key_addBackgrounds_job_conversions = getKey(*addBackgrounds_job_conversions_tuple)
            sample_categories = []
            sample_categories.extend(self.nonfake_backgrounds)
            sample_categories.extend([ "signal" ])
            processes_input = []
            for sample_category in sample_categories:
              processes_input.append("%s_conversion" % sample_category)
            self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_conversions] = {
              'inputFile' : self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job],
              'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "addBackgrounds_%s_%s_lep%s_sum%s_cfg.py" % addBackgrounds_job_conversions_tuple),
              'outputFile' : os.path.join(self.dirs[DKEY_HIST], "addBackgrounds_%s_%s_lep%s_sum%s.root" % addBackgrounds_job_conversions_tuple),
              'logFile' : os.path.join(self.dirs[DKEY_LOGS], "addBackgrounds_%s_%s_lep%s_sum%s.log" % addBackgrounds_job_conversions_tuple),
              'categories' : [ getHistogramDir(lepton_selection, hadTau_selection, lepton_and_hadTau_frWeight, lepton_charge_selection, chargeSumSelection) ],
              'processes_input' : processes_input,
              'process_output' : "conversions"
            }
            self.createCfg_addBackgrounds(self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_conversions])

            # initialize input and output file names for hadd_stage2
            key_hadd_stage1_5_job = getKey(lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            key_hadd_stage2_dir = getKey("hadd", lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            hadd_stage2_job_tuple = (lepton_and_hadTau_selection_and_frWeight, lepton_charge_selection, chargeSumSelection)
            key_hadd_stage2_job = getKey(*hadd_stage2_job_tuple)
            if not key_hadd_stage2_job in self.inputFiles_hadd_stage2:
              self.inputFiles_hadd_stage2[key_hadd_stage2_job] = []
            if lepton_and_hadTau_selection == "Tight":
              self.inputFiles_hadd_stage2[key_hadd_stage2_job].append(self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_fakes]['outputFile'])
              self.inputFiles_hadd_stage2[key_hadd_stage2_job].append(self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_conversions]['outputFile'])            
            self.inputFiles_hadd_stage2[key_hadd_stage2_job].append(self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job])
            self.outputFile_hadd_stage2[key_hadd_stage2_job] = os.path.join(self.dirs[key_hadd_stage2_dir][DKEY_HIST], "hadd_stage2_%s_lep%s_sum%s.root" % hadd_stage2_job_tuple)

    if self.isBDTtraining or self.do_sync:
      if self.is_sbatch:
        logging.info("Creating script for submitting '%s' jobs to batch system" % self.executable_analyze)
        self.sbatchFile_analyze = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_analyze_%s.py" % self.channel)
        if self.isBDTtraining:
          self.createScript_sbatch_analyze(self.executable_analyze, self.sbatchFile_analyze, self.jobOptions_analyze)
        elif self.do_sync:
          self.createScript_sbatch_syncNtuple(self.executable_analyze, self.sbatchFile_analyze, self.jobOptions_analyze)
      logging.info("Creating Makefile")
      lines_makefile = []
      if self.isBDTtraining:
        self.addToMakefile_analyze(lines_makefile)
        self.addToMakefile_hadd_stage1(lines_makefile)
      elif self.do_sync:
        self.addToMakefile_syncNtuple(lines_makefile)
        outputFile_sync_path = os.path.join(self.outputDir, DKEY_SYNC, '%s.root' % self.channel)
        self.outputFile_sync['sync'] = outputFile_sync_path
        self.targets.append(outputFile_sync_path)
        self.addToMakefile_hadd_sync(lines_makefile)
      else:
        raise ValueError("Internal logic error")
      self.createMakefile(lines_makefile)
      logging.info("Done")
      return self.num_jobs

    logging.info("Creating configuration files to run 'addBackgroundFakes'")
    for lepton_charge_selection in self.lepton_charge_selections:
      for chargeSumSelection in self.chargeSumSelections:
        key_hadd_stage1_5_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Fakeable", "enabled"), lepton_charge_selection, chargeSumSelection)
        addFakes_job_tuple = (lepton_charge_selection, chargeSumSelection)
        key_addFakes_job = getKey("fakes_data", *addFakes_job_tuple)
        category_sideband = None
        if self.applyFakeRateWeights == "2lepton":
          category_sideband = "2lss_1tau_lep%s_sum%s_Fakeable_wFakeRateWeights" % (lepton_charge_selection, chargeSumSelection)
        elif self.applyFakeRateWeights == "3L":
          category_sideband = "2lss_1tau_lep%s_sum%s_Fakeable_wFakeRateWeights" % (lepton_charge_selection, chargeSumSelection)
        elif self.applyFakeRateWeights == "1tau":
          category_sideband = "2lss_1tau_lep%s_sum%s_Fakeable_wFakeRateWeights" % (lepton_charge_selection, chargeSumSelection)
        else:
          raise ValueError("Invalid Configuration parameter 'applyFakeRateWeights' = %s !!" % self.applyFakeRateWeights)
        self.jobOptions_addFakes[key_addFakes_job] = {
          'inputFile' : self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job],
          'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "addBackgroundLeptonFakes_lep%s_sum%s_cfg.py" % addFakes_job_tuple),
          'outputFile' : os.path.join(self.dirs[DKEY_HIST], "addBackgroundLeptonFakes_lep%s_sum%s.root" % addFakes_job_tuple),
          'logFile' : os.path.join(self.dirs[DKEY_LOGS], "addBackgroundLeptonFakes_lep%s_sum%s.log" % addFakes_job_tuple),
          'category_signal' : "2lss_1tau_lep%s_sum%s_Tight" % (lepton_charge_selection, chargeSumSelection),
          'category_sideband' : category_sideband
        }
        self.createCfg_addFakes(self.jobOptions_addFakes[key_addFakes_job])
        key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), lepton_charge_selection, chargeSumSelection)
        self.inputFiles_hadd_stage2[key_hadd_stage2_job].append(self.jobOptions_addFakes[key_addFakes_job]['outputFile'])

    #--------------------------------------------------------------------------
    # CV: add histograms in OS and SS regions,
    #     so that "fakes_data" background can be subtracted from OS control region used to estimate charge flip background
    for chargeSumSelection in self.chargeSumSelections:
      key_hadd_stage1_5_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "OS", chargeSumSelection)
      key_addFakes_job = getKey("fakes_data", "OS", chargeSumSelection)
      key_hadd_stage1_6_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "OS", chargeSumSelection)
      if key_hadd_stage1_6_job not in self.inputFiles_hadd_stage1_6:
        self.inputFiles_hadd_stage1_6[key_hadd_stage1_6_job] = []
      self.inputFiles_hadd_stage1_6[key_hadd_stage1_6_job].append(self.jobOptions_addFakes[key_addFakes_job]['outputFile'])
      self.inputFiles_hadd_stage1_6[key_hadd_stage1_6_job].append(self.outputFile_hadd_stage1_5[key_hadd_stage1_5_job])
      self.outputFile_hadd_stage1_6[key_hadd_stage1_6_job] = os.path.join(self.dirs[DKEY_HIST], "hadd_stage1_6_Tight_lepOS_sum%s.root" % chargeSumSelection)
    #--------------------------------------------------------------------------

    logging.info("Creating configuration files to run 'addBackgroundFlips'")
    for chargeSumSelection in self.chargeSumSelections:
      key_hadd_stage1_6_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "OS", chargeSumSelection)
      key_addFlips_job = getKey("flips_data", chargeSumSelection)
      self.jobOptions_addFlips[key_addFlips_job] = {
        'inputFile' : self.outputFile_hadd_stage1_6[key_hadd_stage1_6_job],
        'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "addBackgroundLeptonFlips_sum%s_cfg.py" % chargeSumSelection),
        'outputFile' : os.path.join(self.dirs[DKEY_HIST], "addBackgroundLeptonFlips_sum%s.root" % chargeSumSelection),
        'logFile' : os.path.join(self.dirs[DKEY_LOGS], "addBackgroundLeptonFlips_sum%s.log" % chargeSumSelection),
        'category_signal' : "2lss_1tau_lepSS_sum%s_Tight" % chargeSumSelection,
        'category_sideband' : "2lss_1tau_lepOS_sum%s_Tight" % chargeSumSelection
      }
      self.createCfg_addFlips(self.jobOptions_addFlips[key_addFlips_job])
      key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "SS", chargeSumSelection)
      self.inputFiles_hadd_stage2[key_hadd_stage2_job].append(self.jobOptions_addFlips[key_addFlips_job]['outputFile'])

    logging.info("Creating configuration files to run 'prepareDatacards'")
    for chargeSumSelection in self.chargeSumSelections:
      if self.applyFakeRateWeights == "2lepton":
        processesToCopy = []
        for process in self.prep_dcard_processesToCopy:
          processesToCopy.append(process)
          if not (process.find("data") != -1 or process.find("fakes") != -1 or process.find("conversions") != -1 or process.find("flips") != -1):
            processesToCopy.append("%s_gentau" % process)
            processesToCopy.append("%s_faketau" % process)
        self.prep_dcard_processesToCopy = processesToCopy
        processesToCopy = []
        for process in self.prep_dcard_signals:
          processesToCopy.append(process)
          processesToCopy.append("%s_gentau" % process)
          processesToCopy.append("%s_faketau" % process)
        self.prep_dcard_signals = processesToCopy
      for histogramToFit in self.histograms_to_fit:
        key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "SS", chargeSumSelection)
        prep_dcard_job_tuple = (self.channel, chargeSumSelection, histogramToFit)
        key_prep_dcard_job = getKey(chargeSumSelection, histogramToFit)
        self.jobOptions_prep_dcard[key_prep_dcard_job] = {
          'inputFile' : self.outputFile_hadd_stage2[key_hadd_stage2_job],
          'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "prepareDatacards_%s_sum%s_%s_cfg.py" % prep_dcard_job_tuple),
          'datacardFile' : os.path.join(self.dirs[DKEY_DCRD], "prepareDatacards_%s_sum%s_%s.root" % prep_dcard_job_tuple),
          'histogramDir' : (self.histogramDir_prep_dcard % chargeSumSelection),
          'histogramToFit' : histogramToFit,
          'label' : None
        }
        self.createCfg_prep_dcard(self.jobOptions_prep_dcard[key_prep_dcard_job])

        # add shape templates for the following systematic uncertainties:
        #  - 'CMS_ttHl_Clos_norm_e'
        #  - 'CMS_ttHl_Clos_shape_e'
        #  - 'CMS_ttHl_Clos_norm_m'
        #  - 'CMS_ttHl_Clos_shape_m'
        #  - 'CMS_ttHl_Clos_norm_t'
        #  - 'CMS_ttHl_Clos_shape_t'
        add_syst_fakerate_job_tuple = (self.channel, chargeSumSelection, histogramToFit)
        key_add_syst_fakerate_job = getKey(chargeSumSelection, histogramToFit)
        self.jobOptions_add_syst_fakerate[key_add_syst_fakerate_job] = {
          'inputFile' : self.jobOptions_prep_dcard[key_prep_dcard_job]['datacardFile'],
          'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "addSystFakeRates_%s_sum%s_%s_cfg.py" % add_syst_fakerate_job_tuple),
          'outputFile' : os.path.join(self.dirs[DKEY_DCRD], "addSystFakeRates_%s_sum%s_%s.root" % add_syst_fakerate_job_tuple),
          'category' : self.channel,
          'histogramToFit' : histogramToFit,
          'plots_outputFileName' : os.path.join(self.dirs[DKEY_PLOT], "addSystFakeRates.png")
        }
        for lepton_and_hadTau_type in [ 'e', 'm', 't' ]:
          lepton_and_hadTau_mcClosure = "Fakeable_mcClosure_%s" % lepton_and_hadTau_type
          if lepton_and_hadTau_mcClosure not in self.lepton_and_hadTau_selections:
            continue
          lepton_and_hadTau_selection_and_frWeight = get_lepton_and_hadTau_selection_and_frWeight(lepton_and_hadTau_mcClosure, "enabled")
          key_addBackgrounds_job_fakes = getKey("fakes_mc", lepton_and_hadTau_selection_and_frWeight, 'SS', chargeSumSelection)
          histogramDir_mcClosure = self.mcClosure_dir['%s_%s_%s' % (lepton_and_hadTau_mcClosure, 'SS', chargeSumSelection)]
          self.jobOptions_add_syst_fakerate[key_add_syst_fakerate_job].update({
            'add_Clos_%s' % lepton_and_hadTau_type : ("Fakeable_mcClosure_%s" % lepton_and_hadTau_type) in self.lepton_and_hadTau_selections,
            'inputFile_nominal_%s' % lepton_and_hadTau_type : self.outputFile_hadd_stage2[key_hadd_stage2_job],
            'histogramName_nominal_%s' % lepton_and_hadTau_type : "%s/sel/evt/fakes_mc/%s" % (self.histogramDir_prep_dcard % chargeSumSelection, histogramToFit),
            'inputFile_mcClosure_%s' % lepton_and_hadTau_type : self.jobOptions_addBackgrounds_sum[key_addBackgrounds_job_fakes]['outputFile'],
            'histogramName_mcClosure_%s' % lepton_and_hadTau_type : "%s/sel/evt/fakes_mc/%s" % (histogramDir_mcClosure, histogramToFit),
          })
        self.createCfg_add_syst_fakerate(self.jobOptions_add_syst_fakerate[key_add_syst_fakerate_job])

      logging.info("Creating configuration files to run 'makePlots'")
      for chargeSumSelection in self.chargeSumSelections:
        key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "SS", chargeSumSelection)
        key_makePlots_job = ("SS", chargeSumSelection)
        self.jobOptions_make_plots[key_makePlots_job] = {
          'executable' : self.executable_make_plots,
          'inputFile' : self.outputFile_hadd_stage2[key_hadd_stage2_job],
          'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "makePlots_%s_sum%s_cfg.py" % (self.channel, chargeSumSelection)),
          'outputFile' : os.path.join(self.dirs[DKEY_PLOT], "makePlots_%s_sum%s.png" % (self.channel, chargeSumSelection)),
          'histogramDir' : (self.histogramDir_prep_dcard % chargeSumSelection),
          'label' : "2lSS+1#tau_{h}",
          'make_plots_backgrounds' : self.make_plots_backgrounds
        }
        self.createCfg_makePlots(self.jobOptions_make_plots[key_makePlots_job])
        if "OS" in self.lepton_charge_selections and chargeSumSelection == "OS":
          make_plots_backgrounds = self.make_plots_backgrounds
          if "flips_data" in make_plots_backgrounds:
            make_plots_backgrounds.remove("flips_data")
          key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "OS", chargeSumSelection)
          key_makePlots_job = getKey("OS", chargeSumSelection)
          self.jobOptions_make_plots[key_makePlots_job] = {
            'executable' : self.executable_make_plots,
            'inputFile' : self.outputFile_hadd_stage2[key_hadd_stage2_job],
            'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "makePlots_%s_lepOS_sum%s_cfg.py" % (self.channel, chargeSumSelection)),
            'outputFile' : os.path.join(self.dirs[DKEY_PLOT], "makePlots_%s_lepOS_sum%s.png" % (self.channel, chargeSumSelection)),
            'histogramDir' : (self.histogramDir_prep_dcard_OS % chargeSumSelection),
            'label' : "2lSS+1#tau_{h} OS",
            'make_plots_backgrounds' : make_plots_backgrounds
          }
          self.createCfg_makePlots(self.jobOptions_make_plots[key_makePlots_job])
        if "Fakeable_mcClosure" in self.lepton_and_hadTau_selections and chargeSumSelection == "OS": #TODO
          key_hadd_stage2_job = getKey(get_lepton_and_hadTau_selection_and_frWeight("Tight", "disabled"), "SS", chargeSumSelection)
          key_makePlots_job = getKey("SS", chargeSumSelection)
          self.jobOptions_make_plots[key_makePlots_job] = {
            'executable' : self.executable_make_plots_mcClosure,
            'inputFile' : self.outputFile_hadd_stage2[key_hadd_stage2_job],
            'cfgFile_modified' : os.path.join(self.dirs[DKEY_CFGS], "makePlots_mcClosure_%s_sum%s_cfg.py" % (self.channel, chargeSumSelection)),
            'outputFile' : os.path.join(self.dirs[DKEY_PLOT], "makePlots_mcClosure_%s_sum%s.png" % (self.channel, chargeSumSelection)),
            'histogramDir_signal' : (self.histogramDir_prep_dcard % jobOptions['chargeSumSelection']),
            'histogramDir_sideband' : (self.histogramDir_prep_dcard.replace("Tight", "Fakeable_mcClosure_wFakeRateWeights") % jobOptions['chargeSumSelection'])
          }
          self.createCfg_makePlots_mcClosure(self.jobOptions_make_plots[key_makePlots_job])

    if self.is_sbatch:
      logging.info("Creating script for submitting '%s' jobs to batch system" % self.executable_analyze)
      self.sbatchFile_analyze = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_analyze_%s.py" % self.channel)
      self.createScript_sbatch_analyze(self.executable_analyze, self.sbatchFile_analyze, self.jobOptions_analyze)
      logging.info("Creating script for submitting '%s' jobs to batch system" % self.executable_addBackgrounds)
      self.sbatchFile_addBackgrounds = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_addBackgrounds_%s.py" % self.channel)
      self.createScript_sbatch(self.executable_addBackgrounds, self.sbatchFile_addBackgrounds, self.jobOptions_addBackgrounds)
      self.sbatchFile_addBackgrounds_sum = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_addBackgrounds_sum_%s.py" % self.channel)
      self.createScript_sbatch(self.executable_addBackgrounds, self.sbatchFile_addBackgrounds_sum, self.jobOptions_addBackgrounds_sum)
      logging.info("Creating script for submitting '%s' jobs to batch system" % self.executable_addFakes)
      self.sbatchFile_addFakes = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_addFakes_%s.py" % self.channel)
      self.createScript_sbatch(self.executable_addFakes, self.sbatchFile_addFakes, self.jobOptions_addFakes)
      logging.info("Creating script for submitting '%s' jobs to batch system" % self.executable_addFlips)
      self.sbatchFile_addFlips = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_addFlips_%s.py" % self.channel)
      self.createScript_sbatch(self.executable_addFlips, self.sbatchFile_addFlips, self.jobOptions_addFlips)

    logging.info("Creating Makefile")
    lines_makefile = []
    self.addToMakefile_analyze(lines_makefile)
    self.addToMakefile_hadd_stage1(lines_makefile)
    self.addToMakefile_backgrounds_from_data(lines_makefile)
    self.addToMakefile_hadd_stage2(lines_makefile)
    self.addToMakefile_prep_dcard(lines_makefile)
    self.addToMakefile_add_syst_fakerate(lines_makefile)
    self.addToMakefile_make_plots(lines_makefile)
    self.createMakefile(lines_makefile)

    logging.info("Done.")

    return self.num_jobs
