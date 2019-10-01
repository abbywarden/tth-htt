#!/usr/bin/env python

from tthAnalysis.HiggsToTauTau.configs.analyzeConfig_1l_1tau import analyzeConfig_1l_1tau
from tthAnalysis.HiggsToTauTau.jobTools import query_yes_no
from tthAnalysis.HiggsToTauTau.analysisSettings import systematics, get_lumi
from tthAnalysis.HiggsToTauTau.runConfig import tthAnalyzeParser, filter_samples
from tthAnalysis.HiggsToTauTau.common import logging, load_samples

import os
import sys
import getpass

# E.g.: ./test/tthAnalyzeRun_1l_1tau.py -v 2017Dec13 -m default -e 2017

mode_choices     = [ 'default', 'forBDTtraining', 'sync' ]
sys_choices      = [ 'full' ] + systematics.an_extended_opts
systematics.full = systematics.an_extended

parser = tthAnalyzeParser()
parser.add_modes(mode_choices)
parser.add_sys(sys_choices)
parser.add_preselect()
parser.add_rle_select()
parser.add_nonnominal()
parser.add_tau_id_wp()
parser.add_tau_id()
parser.add_hlt_filter()
parser.add_files_per_job(files_per_job = 2)
parser.add_use_home()
parser.add_jet_cleaning()
parser.add_gen_matching()
args = parser.parse_args()

# Common arguments
era                = args.era
version            = args.version
dry_run            = args.dry_run
no_exec            = args.no_exec
auto_exec          = args.auto_exec
check_output_files = not args.not_check_input_files
debug              = args.debug
sample_filter      = args.filter
num_parallel_jobs  = args.num_parallel_jobs
running_method     = args.running_method

# Additional arguments
mode              = args.mode
systematics_label = args.systematics
use_preselected   = args.use_preselected
rle_select        = os.path.expanduser(args.rle_select)
use_nonnominal    = args.original_central
hlt_filter        = args.hlt_filter
files_per_job     = args.files_per_job
use_home          = args.use_home
jet_cleaning      = args.jet_cleaning
gen_matching      = args.gen_matching
tau_id            = args.tau_id

# Use the arguments
central_or_shifts = []
for systematic_label in systematics_label:
  for central_or_shift in getattr(systematics, systematic_label):
    if central_or_shift not in central_or_shifts:
      central_or_shifts.append(central_or_shift)
do_sync = mode.startswith('sync')
lumi = get_lumi(era)
jet_cleaning_by_index = (jet_cleaning == 'by_index')
gen_matching_by_index = (gen_matching == 'by_index')

hadTauWP_map = {
  'dR03mva' : 'Medium',
  'deepVSj' : 'Medium',
}
hadTau_selection = tau_id + hadTauWP_map[tau_id]

hadTauWP_veto_map = {
  'dR03mva' : 'Medium',
  'deepVSj' : 'Medium',
}
hadTau_selection_veto = tau_id + hadTauWP_veto_map[tau_id]

if mode == "default":
   samples = load_samples(era, suffix = "preselected" if use_preselected else "")
   for sample_name, sample_info in samples.items():
    if sample_name == 'sum_events': continue
    if sample_info["sample_category"] in [
      "data_obs"
    ]:
      sample_info["use_it"] = False
elif mode == "test":
   samples = load_samples(era, suffix = "preselected" if use_preselected else "")
   for sample_name, sample_info in samples.items():
    if sample_name == 'sum_events': continue
    if not sample_info["sample_category"] in [
      "signal",
      "TTWH",
      "TTZH",
      "HH",
      "ggH",
      "qqH",
      "VH",
      "tHq",
      "tHW"
    ]:
      sample_info["use_it"] = False
elif mode == "forBDTtraining":
  if use_preselected:
    raise ValueError("Makes no sense to use preselected samples w/ BDT training mode")
  samples = load_samples(era, suffix = "BDT_DY")
  hadTauWP_map_relaxed = {
    'dR03mva' : 'Loose',
    'deepVSj' : 'Loose',
  }
  if args.tau_id_wp:
    tau_id = args.tau_id[:7]
  hadTau_selection_relaxed = tau_id + hadTauWP_map_relaxed[tau_id]
elif mode == "sync":
  sample_suffix = "sync" if use_nonnominal else "sync_nom"
  if use_preselected:
    sample_suffix = "preselected_{}".format(sample_suffix)
  samples = load_samples(era, suffix = sample_suffix)
else:
  raise ValueError("Invalid mode: %s" % mode)

evtCategories = None
if mode == "default" and len(central_or_shifts) <= 1:
  evtCategories = [
    "1l_1tau" , "1l_1tau_OS", "1l_1tau_SS", "1l_1tau_OS_wChargeFlipWeights"
  ]
else:
  evtCategories = [
    "1l_1tau" , "1l_1tau_OS", "1l_1tau_SS", "1l_1tau_OS_wChargeFlipWeights"
  ]

for sample_name, sample_info in samples.items():
  if sample_name == 'sum_events': continue
  if sample_name.startswith("/DY"):
    sample_info["sample_category"] = "DY"

if __name__ == '__main__':
  logging.info(
    "Running the jobs with the following systematic uncertainties enabled: %s" % \
    ', '.join(central_or_shifts)
  )

  if sample_filter:
    samples = filter_samples(samples, sample_filter)

  if args.tau_id_wp:
    logging.info("Changing tau ID working point: %s -> %s" % (hadTau_selection, args.tau_id_wp))
    hadTau_selection = args.tau_id_wp

  analysis = analyzeConfig_1l_1tau(
    configDir = os.path.join("/home",       getpass.getuser(), "ttHAnalysis", era, version),
    outputDir = os.path.join("/hdfs/local", getpass.getuser(), "ttHAnalysis", era, version),
    executable_analyze                    = "analyze_1l_1tau",
    cfgFile_analyze                       = "analyze_1l_1tau_cfg.py",
    samples                               = samples,
    hadTau_selection                      = hadTau_selection,
    hadTau_selection_veto                 = hadTau_selection_veto,
    chargeSumSelections                   = [ "disabled" ], # CV: setting chargeSumSelections to 'disabled' will run OS and SS lepton+tau charge selections as separate categories
    applyFakeRateWeights                  = "2L",
    jet_cleaning_by_index                 = jet_cleaning_by_index,
    gen_matching_by_index                 = gen_matching_by_index,
    central_or_shifts                     = central_or_shifts,
    evtCategories                         = evtCategories,
    max_files_per_job                     = files_per_job,
    era                                   = era,
    use_lumi                              = True,
    lumi                                  = lumi,
    check_output_files                    = check_output_files,
    running_method                        = running_method,
    num_parallel_jobs                     = num_parallel_jobs,
    executable_addBackgrounds             = "addBackgrounds",
    # CV: use common executable for estimating jet->lepton and jet->tau_h fake background
    executable_addFakes                   = "addBackgroundLeptonFakes",
    executable_addFlips                   = "addBackgroundLeptonFlips",
    histograms_to_fit                     = {
      "EventCounter"                      : {},
      "numJets"                           : {},
      "mTauTauVis"                        : {},
      "mTauTau"                           : {},
      "mva_16Var"                         : {},
    },
    select_rle_output                     = True,
    dry_run                               = dry_run,
    do_sync                               = do_sync,
    isDebug                               = debug,
    rle_select                            = rle_select,
    use_nonnominal                        = use_nonnominal,
    hlt_filter                            = hlt_filter,
    use_home                              = use_home,
  )

  if mode == "forBDTtraining" :
    analysis.set_BDT_training(hadTau_selection_relaxed)

  job_statistics = analysis.create()
  for job_type, num_jobs in job_statistics.items():
    logging.info(" #jobs of type '%s' = %i" % (job_type, num_jobs))

  if auto_exec:
    run_analysis = True
  elif no_exec:
    run_analysis = False
  else:
    run_analysis = query_yes_no("Start jobs ?")
  if run_analysis:
    analysis.run()
  else:
    sys.exit(0)
