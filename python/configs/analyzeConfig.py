from tthAnalysis.HiggsToTauTau.jobTools import create_if_not_exists, run_cmd, generate_file_ids, get_log_version, check_submission_cmd, record_software_state
from tthAnalysis.HiggsToTauTau.analysisTools import initDict, getKey, create_cfg, createFile, is_dymc_reweighting, is_dymc_normalization, check_sample_pairs
from tthAnalysis.HiggsToTauTau.analysisTools import createMakefile as tools_createMakefile, get_tH_weight_str, get_tH_SM_str
from tthAnalysis.HiggsToTauTau.sbatchManagerTools import createScript_sbatch as tools_createScript_sbatch
from tthAnalysis.HiggsToTauTau.sbatchManagerTools import createScript_sbatch_hadd_nonBlocking as tools_createScript_sbatch_hadd_nonBlocking
from tthAnalysis.HiggsToTauTau.analysisSettings import Triggers, systematics
from tthAnalysis.HiggsToTauTau.common import logging, DEPENDENCIES
from tthAnalysis.HiggsToTauTau.samples.stitch import get_branch_type

from tthAnalysis.NanoAODTools.tHweights_cfi import tHweights, thIdxsNoCP, find_tHweight

import FWCore.ParameterSet.Config as cms

import os
import uuid
import inspect
import copy
import collections

LEP_MVA_WPS = {
  'default' : 'mu=0.85;e=0.80',
  'ttZctrl' : 'mu=0.85;e=0.50',
}

DKEY_CFGS    = "cfgs"        # dir for python configuration and batch script files for each analysis job
DKEY_HIST    = "histograms"  # dir for ROOT files containing histograms = output of the anaysis jobs
DKEY_PLOT    = "plots"       # dir for control plots (prefit)
DKEY_SCRIPTS = "scripts"  # dir for sbatchManagers scripts that submit analysis and hadd jobs to batch system
DKEY_LOGS    = "logs"        # dir for log files (stdout/stderr of jobs)
DKEY_DCRD    = "datacards"   # dir for the datacard
DKEY_RLES    = "output_rle"  # dir for the selected run:lumi:event numbers
DKEY_HADD_RT = "hadd_cfg_rt" # dir for hadd cfg files generated during the runtime
DKEY_SYNC    = 'sync_ntuple' # dir for storing sync Ntuples

DKEY_COMBINE_OUTPUT = "combine_output" # dir for storing post-fit results (of auxiliary analyses)

DIRLIST = [
    DKEY_CFGS, DKEY_DCRD, DKEY_HIST, DKEY_PLOT, DKEY_SCRIPTS, DKEY_LOGS, DKEY_RLES,
    DKEY_HADD_RT, DKEY_SYNC
]

def convert_lep_wp(float_str):
  return float_str.replace('.', '')

class analyzeConfig(object):
    """Configuration metadata needed to run analysis in a single go.

       Sets up a folder structure by defining full path names; no directory creation is delegated here.

       Args:
         configDir: The root config dir -- all configuration files are stored in its subdirectories
         outputDir: The root output dir -- all log and output files are stored in its subdirectories
         executable_analyze: Name of the executable that runs the analysis; possible values are `analyze_2lss_1tau`, `analyze_2los_1tau`, `analyze_1l_2tau`,...
         max_files_per_job: maximum number of input ROOT files (Ntuples) are allowed to chain together per job
         use_lumi: if True, use lumiSection aka event weight ( = xsection * luminosity / nof events), otherwise uses plain event count
         debug: if True, checks each input root file (Ntuple) before creating the python configuration files
         running_method: either `sbatch` (uses SLURM) or `Makefile`
         num_parallel_jobs: number of jobs that can be run in parallel on local machine (does not limit number of analysis jobs running in parallel on batch system)
         poll_interval: the interval of checking whether all sbatch jobs are completed (matters only if `running_method` is set to `sbatch`)
         histograms_to_fit: what histograms are filtered in datacard preparation
         executable_prep_dcard: executable name for preparing the datacards

       Other:
         is_sbatch: boolean that is True if the `running_method` is set to `sbatch`; False otherwise
         is_makefile: boolean that is True if the `running_method` is set to `Makefile`; False otherwise
         channel: name of the channel in the datacard
         workindgDir: path to 'tthAnalysis/HiggsToTauTau/test' directory in CMSSW area
         dirs: list of subdirectories under `subdir` -- jobs, cfgs, histograms, logs, datacards
         makefile: full path to the Makefile
         histogram_files: the histogram files produced by 'analyze_1l_2tau' jobs
         histogram_files_exists: flags indicating if histogram files already exist from a previous execution of 'tthAnalyzeRun_1l_2tau.py',
                                 so that 'analyze_1l_2tau' jobs do not have to be submitted again
         histogramFile_hadd_stage1: the histogram file obtained by hadding the output of all jobs
         histogramFile_hadd_stage2: the final histogram file with data-driven background estimates added
         datacardFile: the datacard -- final output file of this execution flow
         cfg_file_prep_dcard: python configuration file for datacard preparation executable
         histogramDir_prep_dcard: directory in final histogram file that is used for building datacard
    """

    def __init__(self,
          configDir,
          outputDir,
          executable_analyze,
          channel,
          samples,
          jet_cleaning_by_index,
          gen_matching_by_index,
          central_or_shifts,
          max_files_per_job,
          era,
          use_lumi,
          lumi,
          check_output_files,
          running_method,
          num_parallel_jobs,
          histograms_to_fit,
          triggers,
          lep_mva_wp                      = "default",
          executable_prep_dcard           = "prepareDatacards",
          executable_add_syst_dcard       = "addSystDatacards",
          executable_add_syst_fakerate    = "addSystFakeRates",
          executable_make_plots           = "makePlots",
          executable_make_plots_mcClosure = "makePlots",
          do_sync                         = False,
          verbose                         = False,
          dry_run                         = False,
          use_home                        = False,
          isDebug                         = False,
          template_dir                    = None,
          submission_cmd                  = None,
      ):

        self.configDir = configDir
        self.outputDir = outputDir
        self.executable_analyze = executable_analyze
        self.channel = channel

        # sum the event counts for samples which cover the same phase space only if
        # there are multiple such samples
        event_sums = copy.deepcopy(samples['sum_events'])
        del samples['sum_events']
        dbs_list_to_sum = []
        for sample_list_to_sum in event_sums:
          dbs_list = []
          for sample_to_sum in sample_list_to_sum:
            for dbs_key, sample_entry in samples.items():
              if sample_entry['process_name_specific'] == sample_to_sum and sample_entry['use_it']:
                dbs_list.append(dbs_key)
          if len(dbs_list) > 1:
            dbs_list_to_sum.append(dbs_list)
            dbs_names = [ samples[dbs_key]['process_name_specific'] for dbs_key in dbs_list ]
            missing_dbs_names = list(sorted(list(set(sample_list_to_sum) - set(dbs_names)), key = lambda k: k.lower()))
            if missing_dbs_names:
              logging.info(
                'Summing the effective counts of {} but NOT the effective counts of {} because the latter '
                'is not processed'.format(', '.join(dbs_names), ', '.join(missing_dbs_names))
              )
            else:
              logging.info('Summing the effective counts of {}'.format(', '.join(dbs_names)))
          elif len(dbs_list) == 1:
            logging.info('NOT summing the effective event counts of {} as only {} is processed'.format(
              ', '.join(sample_list_to_sum), samples[dbs_list[0]]['process_name_specific']
            ))
          else:
            logging.info('NOT summing the effective event counts of {} as none of the samples are needed'.format(
              ', '.join(sample_list_to_sum)
            ))
        for dbs_list in dbs_list_to_sum:
          dbs_list_human = ', '.join(samples[dbs_key]['process_name_specific'] for dbs_key in dbs_list)
          nof_events = {}
          for dbs_key in dbs_list:
            if len(nof_events) == 0:
              nof_events = copy.deepcopy(samples[dbs_key]['nof_events'])
            else:
              sample_nof_events_set = set(evt_key for evt_key in samples[dbs_key]['nof_events'] if 'PSWeight' not in evt_key)
              nof_events_set = set(evt_key for evt_key in nof_events.keys() if 'PSWeight' not in evt_key)
              if sample_nof_events_set != nof_events_set:
                raise ValueError('Mismatching event counts for samples: %s' % dbs_list_human)
              for count_type, count_array in samples[dbs_key]['nof_events'].items():
                if count_type not in nof_events and 'PSWeight' in count_type:
                  nof_events[count_type] = [ 0 ] * len(count_array)
                if len(nof_events[count_type]) != len(count_array):
                  raise ValueError(
                    'Mismatching array length of %s for samples: %s' % (count_type, dbs_list_human)
                  )
                for count_idx, count_val in enumerate(count_array):
                  nof_events[count_type][count_idx] += count_val
          for dbs_key in dbs_list:
            samples[dbs_key]['nof_events'] = copy.deepcopy(nof_events)

        self.samples = copy.deepcopy(samples)
        for sample_key, sample_info in self.samples.items():
          if sample_key == 'sum_events': continue
          sample_info["dbs_name"] = sample_key
          sample_info["apply_toppt_rwgt"] = sample_key.startswith('/TTTo')
          if any('CountWeightedPSWeight' in event_count for event_count in sample_info['nof_events']):
            nof_events_default = sample_info['nof_events']['CountWeighted'][0]
            nof_events_psweight = sample_info['nof_events']['CountWeightedPSWeight'][0]
            nof_events_psweight_lhenom = sample_info['nof_events']['CountWeightedPSWeightOriginalXWGTUP'][0]
            apply_lhe_nom = abs(nof_events_default - nof_events_psweight_lhenom) < abs(nof_events_default - nof_events_psweight)
            if apply_lhe_nom:
              logging.warning("Applying nominal LHE weight for PS weights in sample {}".format(sample_info["process_name_specific"]))
            else:
              logging.warning("Not applying nominal LHE weight for PS weights in sample {}".format(sample_info["process_name_specific"]))
            sample_info["apply_lhe_nom"] = apply_lhe_nom
            event_counts_remove = []
            for event_count in sample_info["nof_events"]:
              if 'PSWeight' in event_count and (
                    (apply_lhe_nom and 'PSWeightOriginalXWGTUP' not in event_count) or
                    (not apply_lhe_nom and 'PSWeightOriginalXWGTUP' in event_count)
                  ):
                event_counts_remove.append(event_count)
            for event_count in event_counts_remove:
              logging.warning("Removing event yield {} from sample {}".format(event_count, sample_info["process_name_specific"]))
              del sample_info['nof_events'][event_count]

        self.lep_mva_wp = lep_mva_wp
        self.central_or_shifts = central_or_shifts
        if not 'central' in self.central_or_shifts:
            logging.warning('Running with systematic uncertainties, but without central value, is not supported --> adding central value.')
            self.central_or_shifts.append('central')
        #------------------------------------------------------------------------
        # CV: make sure that 'central' is always first entry in self.central_or_shifts
        #    (logic for building dependencies between analysis, 'hadd', and 'addBackgrounds' jobs in derived classes may abort with KeyError otherwise)
        if len(self.central_or_shifts) > 1:
            self.central_or_shifts.remove('central')
            self.central_or_shifts = [ 'central' ] + self.central_or_shifts
        self.central_or_shifts_fr = []
        self.central_or_shifts_internal = []
        self.central_or_shifts_external = []
        #------------------------------------------------------------------------
        self.era = era
        self.do_l1prefiring = self.era != "2018"
        if (set(systematics.L1PreFiring) & set(self.central_or_shifts)) == set(systematics.L1PreFiring) and not self.do_l1prefiring:
          logging.warning('Removing systematics from {} era: {}'.format(self.era, ', '.join(systematics.L1PreFiring)))
          for central_or_shift in systematics.L1PreFiring:
            self.central_or_shifts.remove(central_or_shift)
        # ------------------------------------------------------------------------
        if self.era != "2018":
          if systematics.JES_HEM in self.central_or_shifts:
            logging.warning('Removing systematics {} from {} era'.format(systematics.JES_HEM, self.era))
            self.central_or_shifts.remove(systematics.JES_HEM)
        # ------------------------------------------------------------------------
        for central_or_shift in self.central_or_shifts:
          if central_or_shift in systematics.TTbar().full:
            for sample_key, sample_info in self.samples.items():
              if sample_info["sample_category"] == central_or_shift:
                logging.info("Enabling sample {} because systematics {} was requested".format(
                  sample_info["process_name_specific"], central_or_shift,
                ))
                sample_info["use_it"] = True
        # ------------------------------------------------------------------------
        self.ttHProcs = [ "ttH" ]# , "ttH_ctcvcp" ]
        self.prep_dcard_processesToCopy = [  ]
        self.decayModes = [ "htt", "hww", "hzz", "hmm", "hzg" ]
        self.decayModes_HH = [ "tttt",  "zzzz",  "wwww",  "ttzz",  "ttww",  "zzww", "bbtt", "bbww", "bbzz" ]
        self.procsWithDecayModes = self.ttHProcs + [ "WH", "ZH", "tHW", "tHq", "ggH", "qqH", "TTWH", "TTZH" ]
        self.prep_dcard_signals = self.ttHProcs + [
          "{}_{}".format(proc, decMode) for proc in self.ttHProcs for decMode in self.decayModes + [ 'fake' ]
        ] + [
          "HH_{}".format(decMode)  for decMode in self.decayModes_HH + [ 'fake' ]
        ]
        self.convs_backgrounds = [ "XGamma" ]
        # ------------------------------------------------------------------------
        central_or_shifts_remove = []
        for central_or_shift in self.central_or_shifts:
          is_central_or_shift_selected = False
          for sample_key, sample_info in self.samples.items():
            if not sample_info["use_it"]:
              continue
            if self.accept_central_or_shift(central_or_shift, sample_info):
              is_central_or_shift_selected = True
              break
          if not is_central_or_shift_selected:
            central_or_shifts_remove.append(central_or_shift)
        for central_or_shift in central_or_shifts_remove:
          logging.warning("Removing systematics {} because it's never used".format(central_or_shift))
          self.central_or_shifts.remove(central_or_shift)

        self.jet_cleaning_by_index = jet_cleaning_by_index
        self.gen_matching_by_index = gen_matching_by_index
        self.max_files_per_job = max_files_per_job
        self.max_num_jobs = -1
        self.use_lumi = use_lumi
        self.lumi = lumi
        self.check_output_files = check_output_files
        assert(running_method.lower() in [ "sbatch", "makefile" ]), "Invalid running method: %s" % running_method
        self.running_method = running_method
        self.is_sbatch = False
        self.is_makefile = False
        if self.running_method.lower() == "sbatch":
            self.is_sbatch = True
        else:
            self.is_makefile = True
        self.makefile = os.path.join(
            self.configDir, "Makefile_%s" % self.channel)
        self.num_parallel_jobs = num_parallel_jobs
        self.histograms_to_fit = histograms_to_fit
        self.executable_prep_dcard = executable_prep_dcard
        self.executable_add_syst_dcard = executable_add_syst_dcard
        self.executable_add_syst_fakerate = executable_add_syst_fakerate
        self.executable_make_plots = executable_make_plots
        self.executable_make_plots_mcClosure = executable_make_plots_mcClosure
        self.verbose = verbose
        self.dry_run = dry_run
        self.use_home = use_home
        self.isDebug = isDebug
        self.triggers = triggers
        self.triggerTable = Triggers(self.era)
        self.do_sync = do_sync
        self.topPtRwgtChoice = "Quadratic" # alternatives: "TOP16011", "Linear"

        samples_to_stitch = []
        if self.era == '2016':
          from tthAnalysis.HiggsToTauTau.samples.stitch import samples_to_stitch_2016 as samples_to_stitch
        elif self.era == '2017':
          from tthAnalysis.HiggsToTauTau.samples.stitch import samples_to_stitch_2017 as samples_to_stitch
        elif self.era == '2018':
          from tthAnalysis.HiggsToTauTau.samples.stitch import samples_to_stitch_2018 as samples_to_stitch
        else:
          raise ValueError('Invalid era: %s' % self.era)

        self.stitched_weights = "tthAnalysis/HiggsToTauTau/data/stitched_weights_{}.root".format(self.era)
        #assert(os.path.isfile(os.path.join(os.environ['CMSSW_BASE'], 'src', self.stitched_weights)))

        # we do not need to stitch anything when running the analysis on the sync Ntuple
        self.stitching_args = {}
        if not self.do_sync:
          # create temporary LUT
          samples_lut = {}
          for sample_key, sample_entry in self.samples.items():
            if sample_key == 'sum_events': continue
            process_name = sample_entry['process_name_specific']
            assert(process_name not in samples_lut)
            samples_lut[process_name] = sample_key

          # loop over the list of binned samples to determine the coverage of phase spaces
          for samples_to_stitch_entry in samples_to_stitch:
            # loop over the inclusive samples
            inclusive_samples = []
            for inclusive_sample in samples_to_stitch_entry['inclusive']['samples']:
              if inclusive_sample not in samples_lut:
                logging.error("Stitched inclusive sample %s not found in the sample dictionary" % inclusive_sample)
                continue
              if not self.samples[samples_lut[inclusive_sample]]['use_it']:
                logging.warning('Sample {} not enabled'.format(inclusive_sample))
                continue
              inclusive_samples.append(inclusive_sample)

            # loop over the binned samples
            all_present = { key : True for key in samples_to_stitch_entry.keys() if key != 'inclusive' }
            for binning_key in all_present:
              for binned_samples in samples_to_stitch_entry[binning_key]:
                for binned_sample in binned_samples['samples']:
                  # if at least one sample is not enabled, disable all other samples that
                  # are binned by the same variable
                  if binned_sample not in samples_lut:
                    logging.error("Stitched binned sample %s not found in the sample dictionary" % binned_sample)
                    continue
                  if not self.samples[samples_lut[binned_sample]]['use_it']:
                    logging.warning('Sample %s not used' % binned_sample)
                  all_present[binning_key] &= self.samples[samples_lut[binned_sample]]['use_it']
            for binning_key in all_present:
              if not all_present[binning_key]:
                logging.info(
                  'Disabling %s-binned samples that are complementary to: %s' % (
                    binning_key, ', '.join(inclusive_samples)
                  )
                )
                for binned_samples in samples_to_stitch_entry[binning_key]:
                  for binned_sample in binned_samples['samples']:
                    logging.warning('Disabling sample %s' % binned_sample)
                    self.samples[samples_lut[binned_sample]]['use_it'] = False
                del samples_to_stitch_entry[binning_key]

          # construct the list of arguments that need to be propagated to the config files
          # these parameters specify how to access additional weights at the analysis level such
          # that the phase space is modelled accurately
          for samples_to_stitch_entry in samples_to_stitch:
            binning_vars = [var for var in samples_to_stitch_entry if var != 'inclusive']
            histogram_path = ''
            branch_name_xaxis = ''
            branch_name_yaxis = ''
            if len(binning_vars) == 0:
              # only inclusive sample is being used
              continue
            elif len(binning_vars) == 1:
              histogram_path = binning_vars[0]
              branch_name_xaxis = binning_vars[0]
            elif len(binning_vars) == 2:
              histogram_path = '%s_v_%s' % (binning_vars[0], binning_vars[1])
              branch_name_xaxis = binning_vars[0]
              branch_name_yaxis = binning_vars[1]
            else:
              raise ValueError(
                'More than 2 variables by which the samples are binned: %s' % ', '.join(binning_vars)
              )
            assert(histogram_path != '' and branch_name_xaxis != '')
            # loop over the inclusive samples:
            inclusive_samples_disabled = False
            for inclusive_sample in samples_to_stitch_entry['inclusive']['samples']:
              assert(inclusive_sample not in self.stitching_args)
              if inclusive_sample not in samples_lut:
                continue
              if not self.samples[samples_lut[inclusive_sample]]['use_it']:
                inclusive_samples_disabled = True
              self.stitching_args[inclusive_sample] = {
                'histogram_path'    : '%s/%s' % (inclusive_sample, histogram_path),
                'branch_name_xaxis' : branch_name_xaxis,
                'branch_name_yaxis' : branch_name_yaxis,
                'branch_type_xaxis' : get_branch_type(branch_name_xaxis),
                'branch_type_yaxis' : get_branch_type(branch_name_yaxis),
              }

            if inclusive_samples_disabled:
              if len(binning_vars) == 2:
                histogram_path += "_wo_inclusive"
              else:
                # inclusive sample not used => no need for stitching weights
                continue
            # loop over the binned samples
            for binning_key in binning_vars:
              for binned_samples in samples_to_stitch_entry[binning_key]:
                for binned_sample in binned_samples['samples']:
                  assert(binned_sample not in self.stitching_args)
                  self.stitching_args[binned_sample] = {
                    'histogram_path'    : '%s/%s' % (binned_sample, histogram_path),
                    'branch_name_xaxis' : branch_name_xaxis,
                    'branch_name_yaxis' : branch_name_yaxis,
                    'branch_type_xaxis' : get_branch_type(branch_name_xaxis),
                    'branch_type_yaxis' : get_branch_type(branch_name_yaxis),
                  }

        assert(check_sample_pairs(samples))
        self.workingDir = os.getcwd()
        logging.info("Working directory is: %s" % self.workingDir)
        if template_dir:
            self.template_dir = template_dir
        else:
            self.template_dir = os.path.join(
                os.getenv('CMSSW_BASE'), 'src', 'tthAnalysis', 'HiggsToTauTau', 'test', 'templates'
            )
        logging.info("Templates directory is: %s" % self.template_dir)

        create_if_not_exists(self.configDir)
        create_if_not_exists(self.outputDir)

        self.stdout_file_path = os.path.join(self.configDir, "stdout_%s.log" % self.channel)
        self.stderr_file_path = os.path.join(self.configDir, "stderr_%s.log" % self.channel)
        self.sw_ver_file_cfg  = os.path.join(self.configDir, "VERSION_%s.log" % self.channel)
        self.sw_ver_file_out  = os.path.join(self.outputDir, "VERSION_%s.log" % self.channel)
        self.validation_out   = os.path.join(self.configDir, "VALIDATION_%s.log" % self.channel)
        self.submission_out   = os.path.join(self.configDir, "SUBMISSION_%s.log" % self.channel)
        self.stdout_file_path, self.stderr_file_path, self.sw_ver_file_cfg, self.sw_ver_file_out, \
        self.validation_out, self.submission_out = get_log_version((
            self.stdout_file_path, self.stderr_file_path, self.sw_ver_file_cfg, self.sw_ver_file_out,
            self.validation_out, self.submission_out,
        ))
        check_submission_cmd(self.submission_out, submission_cmd, self.do_sync)

        self.dirs = {}

        tH_SM_str = get_tH_SM_str()
        self.thIdxs = thIdxsNoCP
        self.thcouplings = list(filter(
          lambda tH_str: tH_str != tH_SM_str,
          map(
            lambda couplings: get_tH_weight_str(
              float(couplings.kt.configValue()),
              float(couplings.kv.configValue()),
              float(couplings.cosa.configValue()) if hasattr(couplings, 'cosa') else None
            ),
            [ copy.deepcopy(find_tHweight(tHweights, thIdx)) for thIdx in self.thIdxs ]
          )
        ))
        self.kt_weights = []
        self.kt_scan_file = "hhAnalysis/multilepton/data/kt_scan.dat"
        with open(os.path.join(os.environ["CMSSW_BASE"], "src", self.kt_scan_file), "r") as kt_file:
          for line in kt_file:
            kt_value = float(line.split()[1])
            if kt_value == 1.0:
              # SM
              continue
            self.kt_weights += [
              "kt_" + str("{:3.2f}".format(kt_value)).replace(".", "p").replace("-", "m")
            ]

        self.jobOptions_analyze = {}
        self.inputFiles_hadd_stage1 = {}
        self.outputFile_hadd_stage1 = {}
        self.cfgFile_copyHistograms = os.path.join(self.template_dir, "copyHistograms_cfg.py")
        self.jobOptions_copyHistograms = {}
        self.executable_copyHistograms = 'copyHistograms_recursively'
        self.cfgFile_addBackgrounds = os.path.join(self.template_dir, "addBackgrounds_cfg.py")
        self.jobOptions_addBackgrounds = {}
        self.jobOptions_addBackgrounds_sum = {}
        self.inputFiles_hadd_stage1_5 = {}
        self.outputFile_hadd_stage1_5 = {}
        self.cfgFile_addFakes = os.path.join(self.template_dir, "addBackgroundLeptonFakes_cfg.py")
        self.jobOptions_addFakes = {}
        self.inputFiles_hadd_stage2 = {}
        self.outputFile_hadd_stage2 = {}
        self.make_dependency_hadd_stage2 = None
        self.cfgFile_prep_dcard = os.path.join(self.template_dir, "prepareDatacards_cfg.py")
        self.jobOptions_prep_dcard = {}
        self.histogramDir_prep_dcard = None
        self.cfgFile_add_syst_dcard = os.path.join(self.template_dir, "addSystDatacards_cfg.py")
        self.jobOptions_add_syst_dcard = {}
        self.cfgFile_add_syst_fakerate = os.path.join(self.template_dir, "addSystFakeRates_cfg.py")
        self.jobOptions_add_syst_fakerate = {}
        self.make_plots_backgrounds = [ ]
        self.make_plots_signal = "ttH"
        self.cfgFile_make_plots = os.path.join(self.template_dir, "makePlots_cfg.py")
        self.jobOptions_make_plots = {}
        self.filesToClean = []
        self.phoniesToAdd = []
        self.rleOutputFiles = {}

        self.inputFiles_sync = {}
        self.outputFile_sync = {}
        if self.do_sync:
            self.inputFiles_sync['sync'] = []

        self.targets = []
        self.cvmfs_error_log = {}

        self.num_jobs = {}
        self.num_jobs['analyze'] = 0
        self.num_jobs['hadd'] = 0
        self.num_jobs['copyHistograms'] = 0
        self.num_jobs['addBackgrounds'] = 0
        self.num_jobs['addFakes'] = 0

        self.leptonFakeRateWeight_histogramName_e = None
        self.leptonFakeRateWeight_histogramName_mu = None

        self.lep_mva_cut_map = dict(map(
          lambda lep_mva_cut: lep_mva_cut.split('='), LEP_MVA_WPS[self.lep_mva_wp].split(';')
        ))
        self.lep_mva_cut_mu = self.lep_mva_cut_map['mu']
        self.lep_mva_cut_e = self.lep_mva_cut_map['e']

        self.leptonFakeRateWeight_inputFile = "tthAnalysis/HiggsToTauTau/data/FR_lep_ttH_mva_{}_CERN_2019Jul08.root".format(self.era)
        if not os.path.isfile(os.path.join(os.environ['CMSSW_BASE'], 'src', self.leptonFakeRateWeight_inputFile)):
            raise ValueError("No such file: 'leptonFakeRateWeight_inputFile' = %s" % self.leptonFakeRateWeight_inputFile)

        self.hadTau_selection_relaxed = None
        if self.era in [ '2016', '2017', '2018' ]:
            self.hadTauFakeRateWeight_inputFile = "tthAnalysis/HiggsToTauTau/data/FR_deeptau_{}_v6.root".format(era)
        else:
            raise ValueError('Invalid era: %s' % self.era)
        assert(os.path.isfile(os.path.join(os.environ['CMSSW_BASE'], 'src', self.hadTauFakeRateWeight_inputFile)))
        self.isBDTtraining = False
        self.mcClosure_dir = {}
        self.cfgFile_make_plots_mcClosure = ''

    def __del__(self):
        if hasattr(self, "cvmfs_error_log"):
            for hostname, times in self.cvmfs_error_log.items():
                logging.error("Problem with cvmfs access: host = %s (%i jobs)" % (hostname, len(times)))
                for time in times:
                    logging.error(str(time))

    def set_leptonFakeRateWeightHistogramNames(self, central_or_shift, lepton_and_hadTau_selection):
        suffix = 'QCD' if 'mcClosure' in lepton_and_hadTau_selection else 'data_comb'

        # e.g. FR_mva080_el_QCD_NC, FR_mva085_mu_data_comb
        self.leptonFakeRateWeight_histogramName_e = "FR_mva%s_el_%s_NC" % (convert_lep_wp(self.lep_mva_cut_e), suffix)
        self.leptonFakeRateWeight_histogramName_mu = "FR_mva%s_mu_%s" % (convert_lep_wp(self.lep_mva_cut_mu), suffix)

    def set_BDT_training(self, hadTau_selection_relaxed):
        """Run analysis with loose selection criteria for leptons and hadronic taus,
           for the purpose of preparing event list files for BDT training.
        """
        self.hadTau_selection_relaxed = hadTau_selection_relaxed
        assert(self.hadTau_selection_relaxed.startswith("deepVSj"))
        if self.hadTau_selection_relaxed == "deepVSjVVVLoose":
            self.hadTauFakeRateWeight_inputFile = "tthAnalysis/HiggsToTauTau/data/FR_deeptau_BDT_{}_v4.root".format(self.era)
        assert(os.path.isfile(os.path.join(os.environ['CMSSW_BASE'], 'src', self.hadTauFakeRateWeight_inputFile)))
        self.isBDTtraining = True

    def get_addMEM_systematics(self, central_or_shift):
        #if central_or_shift in systematics.an_addMEM:
        #    return central_or_shift
        return "central"

    def accept_central_or_shift(self, central_or_shift, sample_info):
      sample_name = sample_info["dbs_name"]
      sample_category = sample_info["sample_category"]
      has_LHE = sample_info["has_LHE"]
      enable_toppt_rwgt = sample_info["apply_toppt_rwgt"] if "apply_toppt_rwgt" in sample_info else False
      run_ps = sample_info["nof_PSweights"] == 4
      is_HHmc = sample_category.startswith("signal") or sample_category == "HH"
      is_ttbar_sys = sample_info["sample_category"] in systematics.TTbar().full
      ttHProcs = self.ttHProcs + [ "TTH" ]

      if central_or_shift in systematics.LHE().full           and not has_LHE:                                 return False
      if central_or_shift in systematics.LHE().ttH            and sample_category not in ttHProcs:             return False
      if central_or_shift in systematics.LHE().tHq            and sample_category not in [ "tHq", "TH" ]:      return False
      if central_or_shift in systematics.LHE().tHW            and sample_category not in [ "tHW", "TH" ]:      return False
      if central_or_shift in systematics.LHE().ttW            and sample_category not in [ "TTW", "TTWW" ]:    return False
      if central_or_shift in systematics.LHE().ttZ            and sample_category != "TTZ":                    return False
      if central_or_shift in systematics.LHE().ttbar          and sample_category != "TT":                     return False
      if central_or_shift in systematics.LHE().dy             and sample_category != "DY":                     return False
      if central_or_shift in systematics.LHE().wz             and sample_category != "WZ":                     return False
      if central_or_shift in systematics.LHE().zz             and sample_category != "ZZ":                     return False
      if central_or_shift in systematics.DYMCReweighting      and not is_dymc_reweighting(sample_name):        return False
      if central_or_shift in systematics.DYMCNormScaleFactors and not is_dymc_normalization(sample_name):      return False
      if central_or_shift in systematics.tauIDSF              and 'tau' not in self.channel.lower():           return False
      if central_or_shift in systematics.leptonIDSF           and '0l' in self.channel.lower():                return False
      if central_or_shift in systematics.topPtReweighting     and not enable_toppt_rwgt:                       return False
      if central_or_shift in systematics.LHE().hh             and not is_HHmc:                                 return False
      if central_or_shift in systematics.EWK_jet              and sample_category not in [ "WZ", "ZZ" ]:       return False
      if central_or_shift in systematics.PartonShower().ttbar and not (sample_category == "TT" and run_ps):    return False
      if central_or_shift in systematics.PartonShower().dy    and not (sample_category == "DY" and run_ps):    return False
      if central_or_shift in systematics.PartonShower().wjets and not (sample_category == "W" and run_ps):     return False
      if central_or_shift != "central"                        and is_ttbar_sys:                                return False
      return True

    def createCfg_analyze(self, jobOptions, sample_info, additionalJobOptions = [], isLeptonFR = False, isHTT = False, dropCtrl = False):
        channel_str = self.channel
        if dropCtrl:
          channel_str = channel_str.replace('ctrl', '')
        process_string = 'process.analyze_{}'.format(channel_str)
        current_function_name = inspect.stack()[0][3]

        stitch_histogram_names = {}
        is_mc = (sample_info["type"] == "mc")
        use_th_weights = sample_info["type"] == "mc" and \
           sample_info['sample_category'] in [ 'tHq', 'tHW', 'ttH_ctcvcp', 'TH', 'TTH' ] and \
           sample_info['nof_reweighting'] > 0

        is_hh_channel = 'hh' in self.channel
        if (is_hh_channel and sample_info["sample_category"].startswith('signal_') and not "spin" in sample_info["sample_category"]) or \
           (not is_hh_channel and sample_info["sample_category"] == "HH"):
          sample_category_to_check = 'sample_category_hh' if not is_hh_channel else 'sample_category'
          assert(sample_category_to_check in sample_info)
          hhWeight_base = ''
          if any(decayMode in sample_info[sample_category_to_check] for decayMode in [ 'bbvv', 'bbtt' ]):
            hhWeight_base = 'bbww'
          elif any(decayMode in sample_info[sample_category_to_check] for decayMode in [ 'tttt', 'wwtt', 'wwww' ]):
            hhWeight_base = 'multilepton'
          else:
            raise ValueError("Uncrecongizable sample category: %s" % sample_info[sample_category_to_check])
          jobOptions['hhWeight_cfg.denominator_file'] = 'hhAnalysis/{}/data/denom_{}.root'.format(hhWeight_base, self.era)
          #jobOptions['hhWeight_cfg.histtitle'] = sample_info["sample_category_hh"]
          jobOptions['hhWeight_cfg.histtitle'] = sample_info[sample_category_to_check]
          jobOptions['hhWeight_cfg.ktScan_file'] = self.kt_scan_file
          jobOptions['hhWeight_cfg.do_ktscan'] = 'hh' in self.channel
          jobOptions['hhWeight_cfg.apply_rwgt'] = 'hh' in self.channel

        if 'process' not in jobOptions:
          jobOptions['process'] = sample_info["sample_category"]
        if 'isMC' not in jobOptions:
          jobOptions['isMC'] = is_mc
        if 'jetCleaningByIndex' not in jobOptions:
          jobOptions['jetCleaningByIndex'] = self.jet_cleaning_by_index
        if 'genMatchingByIndex' not in jobOptions:
          jobOptions['genMatchingByIndex'] = self.gen_matching_by_index
        if 'apply_genWeight' not in jobOptions:
          jobOptions['apply_genWeight'] = sample_info["genWeight"] if is_mc else False
        if 'apply_DYMCReweighting' not in jobOptions:
          jobOptions['apply_DYMCReweighting'] = is_dymc_reweighting(sample_info["dbs_name"])
        if 'apply_DYMCNormScaleFactors' not in jobOptions:
          jobOptions['apply_DYMCNormScaleFactors'] =  is_dymc_normalization(sample_info["dbs_name"])
        if 'apply_l1PreFireWeight' not in jobOptions:
          jobOptions['apply_l1PreFireWeight'] = self.do_l1prefiring if is_mc else False
        if 'central_or_shift' not in jobOptions:
          jobOptions['central_or_shift'] = 'central'
        if 'apply_topPtReweighting' not in jobOptions:
          if 'apply_toppt_rwgt' in sample_info and sample_info['apply_toppt_rwgt']:
            jobOptions['apply_topPtReweighting'] = self.topPtRwgtChoice
          else:
            jobOptions['apply_topPtReweighting'] = ''
        if 'hasPS' not in jobOptions:
          jobOptions['hasPS'] = sample_info["nof_PSweights"] == 4 and any(
            central_or_shift in systematics.PartonShower().full \
            for central_or_shift in jobOptions['central_or_shifts_local']
          )
          if jobOptions['hasPS']:
            if 'apply_lhe_nom' not in sample_info:
              logging.warning('Sample {} has parton shower weights, but is not consiered in ISR/FSR variation'.format(
                sample_info["process_name_specific"]
              ))
              jobOptions['hasPS'] = False
              central_or_shift_to_remove = []
              for central_or_shift in jobOptions['central_or_shifts_local']:
                if central_or_shift in systematics.PartonShower().full:
                  central_or_shift_to_remove.append(central_or_shift)
              for central_or_shift in central_or_shift_to_remove:
                jobOptions['central_or_shifts_local'].remove(central_or_shift)
            else:
              jobOptions['apply_LHE_nom'] = sample_info['apply_lhe_nom']
        if 'lumiScale' not in jobOptions:

          nof_reweighting = sample_info['nof_reweighting']
          nof_events = collections.OrderedDict()
          tH_weights_map = {}

          if is_mc:
            if jobOptions['central_or_shift'] == "central":# and not use_th_weights:
              central_or_shifts = self.central_or_shifts_internal + [ jobOptions['central_or_shift'] ]
            else:
              central_or_shifts = [ jobOptions['central_or_shift'] ]
            for central_or_shift in central_or_shifts:
              if not self.accept_central_or_shift(central_or_shift, sample_info):
                continue
              nof_events_label = ''
              nof_events_idx = -1

              # Convention: CountWeighted includes the sign of genWeight, CountFullWeighted includes the full genWeight
              # If L1 prefiring weights are enabled, then L1PrefireNom suffix is added
              count_suffix = "L1PrefireNom" if self.do_l1prefiring else ""
              if central_or_shift == systematics.PU_().up:
                nof_events_label = 'CountWeighted{}'.format(count_suffix)
                nof_events_idx = 1 # PU weight up
              elif central_or_shift == systematics.PU_().down:
                nof_events_label = 'CountWeighted{}'.format(count_suffix)
                nof_events_idx = 2 # PU weight down
              elif central_or_shift in systematics.LHE().x1_up:
                nof_events_label = 'CountWeightedLHEWeightScale{}'.format(count_suffix)
                nof_events_idx = 5 # muR=1   muF=2
              elif central_or_shift in systematics.LHE().y1_up:
                nof_events_label = 'CountWeightedLHEWeightScale{}'.format(count_suffix)
                nof_events_idx = 7 # muR=2   muF=1
              elif central_or_shift in systematics.LHE().x1_down:
                nof_events_label = 'CountWeightedLHEWeightScale{}'.format(count_suffix)
                nof_events_idx = 3 # muR=1   muF=0.5
              elif central_or_shift in systematics.LHE().y1_down:
                nof_events_label = 'CountWeightedLHEWeightScale{}'.format(count_suffix)
                nof_events_idx = 1 # muR=0.5 muF=1
              elif central_or_shift in systematics.LHE().env_up:
                nof_events_label = 'CountWeightedLHEEnvelope{}'.format(count_suffix)
                nof_events_idx = 0
              elif central_or_shift in systematics.LHE().env_down:
                nof_events_label = 'CountWeightedLHEEnvelope{}'.format(count_suffix)
                nof_events_idx = 1
              elif central_or_shift in systematics.L1PreFiring_().up:
                nof_events_label = 'CountWeightedL1Prefire'
                nof_events_idx = 1  # L1 prefiring weight up
              elif central_or_shift in systematics.L1PreFiring_().down:
                nof_events_label = 'CountWeightedL1Prefire'
                nof_events_idx = 2  # L1 prefiring weight down
              elif central_or_shift == jobOptions['central_or_shift']:
                nof_events_label = 'CountWeighted{}'.format(count_suffix)
                nof_events_idx = 0 # central

              if jobOptions['hasPS'] and central_or_shift in systematics.PartonShower().full:
                assert(is_mc)
                psWeights_str = 'PSWeight'
                if jobOptions['apply_LHE_nom']:
                  psWeights_str += 'OriginalXWGTUP'
                if central_or_shift in systematics.PartonShower().isr_up:
                  nof_events_idx = 0
                elif central_or_shift in systematics.PartonShower().fsr_up:
                  nof_events_idx = 1
                elif central_or_shift in systematics.PartonShower().env_up:
                  nof_events_idx = 2
                elif central_or_shift in systematics.PartonShower().isr_down:
                  nof_events_idx = 3
                elif central_or_shift in systematics.PartonShower().fsr_down:
                  nof_events_idx = 4
                elif central_or_shift in systematics.PartonShower().env_down:
                  nof_events_idx = 5
                nof_events_label = "CountWeighted{}{}".format(psWeights_str, count_suffix)

              if jobOptions['apply_topPtReweighting']:
                assert(is_mc)
                toppt_str = "{}TopPtRwgtSF".format(jobOptions['apply_topPtReweighting'])
                if central_or_shift not in systematics.topPtReweighting:
                  nof_events_label += toppt_str
                elif central_or_shift == systematics.topPtReweighting_().down:
                  # no SF is applied
                  nof_events_label = "CountWeighted{}".format(count_suffix)
                  nof_events_idx = 0
                elif central_or_shift == systematics.topPtReweighting_().up:
                  nof_events_label = "CountWeighted{}{}Squared".format(count_suffix, toppt_str)
                  nof_events_idx = 0

              if nof_events_idx >= 0 and nof_events_label:
                nof_events[central_or_shift] = sample_info["nof_events"][nof_events_label][nof_events_idx]
                assert(nof_events[central_or_shift] > 0)
                stitch_histogram_names[central_or_shift] = '{}_{}'.format(nof_events_label, nof_events_idx)

                if use_th_weights and central_or_shift not in tH_weights_map:
                  missing_reweighting =  set(self.thIdxs) - set(range(-1, nof_reweighting))

                  if missing_reweighting:
                    logging.warning("Could not find the following weights for {}: {}".format(
                      sample_info["process_name_specific"],
                      ", ".join(map(str, missing_reweighting))
                    ))
                  else:
                    # record the weight for the default case (corresponds to no reweighting weight, i.e. idx of -1)
                    tHweight_default = copy.deepcopy(find_tHweight(tHweights, -1))
                    tHweight_default.weight = cms.double(1.)
                    tHweight_default.central_or_shift = cms.string(central_or_shift)
                    tH_weights_map[central_or_shift] = [ tHweight_default ]

                    for idx in self.thIdxs:
                      if idx < 0:
                        # we've already recorded the weight for the default case
                        continue
                      if idx in missing_reweighting:
                        continue

                      nof_events_rwgt = sample_info["nof_events"]["{}_rwgt{}".format(nof_events_label, idx)][nof_events_idx]
                      tHweight = copy.deepcopy(find_tHweight(tHweights, idx))
                      assert(nof_events_rwgt >= 0)
                      if nof_events_rwgt == 0:
                        assert(float(tHweight.kt.configValue()) == 0. and sample_info['sample_category'] == 'ttH_ctcvcp')
                      final_reweighting = (float(nof_events[central_or_shift]) / nof_events_rwgt) if nof_events_rwgt > 0. else 0.
                      tHweight.weight = cms.double(final_reweighting)
                      tHweight.central_or_shift = cms.string(central_or_shift)
                      tH_weights_map[central_or_shift].append(tHweight)

          if is_mc and self.use_lumi:
            jobOptions['lumiScale'] = [
              cms.PSet(
                central_or_shift = cms.string(central_or_shift),
                lumi             = cms.double(sample_info["xsection"] * self.lumi / nof_events[central_or_shift]),
              ) for central_or_shift in nof_events
            ]
          if use_th_weights:
            tH_weights = []
            for central_or_shift in tH_weights_map:
              tH_weights.extend(tH_weights_map[central_or_shift])
            jobOptions['tHweights'] = tH_weights

        if 'hasLHE' not in jobOptions:
            jobOptions['hasLHE'] = sample_info['has_LHE']
        if 'skipEvery' in sample_info:
            assert('skipEvery' not in jobOptions)
            jobOptions['skipEvery'] = sample_info['skipEvery']
        if 'useObjectMultiplicity' not in jobOptions:
            jobOptions['useObjectMultiplicity'] = False
        if 'useAssocJetBtag' not in jobOptions:
            jobOptions['useAssocJetBtag'] = False

        jobOptions_local = [
            'process',
            'isMC',
            'hasLHE',
            'hasPS',
            'apply_LHE_nom',
            'central_or_shift',
            'central_or_shifts_local',
            'evtCategories',
            'leptonSelection',
            'electronSelection',
            'muonSelection',
            'lep_mva_cut_mu',
            'lep_mva_cut_e',
            'chargeSumSelection',
            'histogramDir',
            'isControlRegion',
            'lumiScale',
            'leptonChargeSelection',
            'hadTauChargeSelection',
            'hadTauSelection',
            'hadTauAk8Clean_tauIdWP',
            'apply_hadTauVeto',
            'hadTauSelection_veto',
            'apply_leptonGenMatching',
            'apply_hadTauGenMatching',
            'applyFakeRateWeights',
            'apply_genWeight',
            'apply_DYMCReweighting',
            'apply_DYMCNormScaleFactors',
            'selEventsFileName_output',
            'fillGenEvtHistograms',
            'selectBDT',
            'useNonNominal',
            'apply_hlt_filter',
            'branchName_memOutput',
            'leptonFakeRateWeight.inputFileName',
            'leptonFakeRateWeight.histogramName_e',
            'leptonFakeRateWeight.histogramName_mu',
            'hadTauFakeRateWeight.inputFileName',
            'hadTauFakeRateWeight.lead.fitFunctionName',
            'hadTauFakeRateWeight.sublead.fitFunctionName',
            'hadTauFakeRateWeight.third.fitFunctionName',
            'hadTauFakeRateWeight.fourth.fitFunctionName',
            'hadTauFakeRateWeight.applyFitFunction_lead',
            'hadTauFakeRateWeight.applyFitFunction_sublead',
            'hadTauFakeRateWeight.applyFitFunction_third',
            'hadTauFakeRateWeight.applyFitFunction_fourth',
            'hadTauFakeRateWeight.applyGraph_lead',
            'hadTauFakeRateWeight.applyGraph_sublead',
            'hadTauFakeRateWeight.applyGraph_third',
            'hadTauFakeRateWeight.applyGraph_fourth',
            'hadTauFakeRateWeight.lead.graphName',
            'hadTauFakeRateWeight.sublead.graphName',
            'hadTauFakeRateWeight.third.graphName',
            'hadTauFakeRateWeight.fourth.graphName',
            'apply_hadTauFakeRateSF',
            'jetCleaningByIndex',
            'genMatchingByIndex',
            'branchName_fatJetsLS',
            'branchName_subJetsLS',
            'tHweights',
            'useObjectMultiplicity',
            'hhWeight_cfg.denominator_file',
            'hhWeight_cfg.histtitle',
            'hhWeight_cfg.do_ktscan',
            'hhWeight_cfg.ktScan_file',
            'hhWeight_cfg.apply_rwgt',
            'minNumJets',
            'skipEvery',
            'apply_topPtReweighting',
            'useAssocJetBtag',
        ]
        jobOptions_typeMapping = {
          'central_or_shifts_local' : 'cms.vstring(%s)',
          'evtCategories'           : 'cms.vstring(%s)',
          'skipEvery'               : 'cms.uint32(%s)',
        }
        jobOptions_keys = jobOptions_local + additionalJobOptions
        max_option_len = max(map(len, [ key for key in jobOptions_keys if key in jobOptions ]))

        lines = [
          "# Filled in %s" % current_function_name,
          "process.fwliteInput.fileNames = cms.vstring(%s)" % jobOptions['ntupleFiles'],
          "process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(jobOptions['histogramFile']),
          "{}.{:<{len}} = cms.string('{}')".format(process_string, 'era',             self.era,                       len = max_option_len),
          "{}.{:<{len}} = cms.bool({})".format    (process_string, 'redoGenMatching', not self.gen_matching_by_index, len = max_option_len),
          "{}.{:<{len}} = cms.bool({})".format    (process_string, 'isDEBUG',         self.isDebug,                   len = max_option_len),
        ]
        if (not isHTT and not self.do_sync) or self.do_sync:
          lines.extend([
            "{}.{:<{len}} = EvtYieldHistManager_{}".format  (process_string, 'cfgEvtYieldHistManager', self.era, len = max_option_len),
            "{}.{:<{len}} = recommendedMEtFilters_{}".format(process_string, 'cfgMEtFilter',           self.era, len = max_option_len),
          ])
        if not is_mc:
          lines.append("{}.{:<{len}} = trigger_runs_{}".format(process_string, 'triggerWhiteList', self.era, len = max_option_len))
        for jobOptions_key in jobOptions_keys:
            if jobOptions_key not in jobOptions:
              continue
            jobOptions_val = jobOptions[jobOptions_key]
            if self.do_sync: # TEMPORARY !!
                if jobOptions_key == 'applyFakeRateWeights':
                    jobOptions_val = 'disabled'
                elif jobOptions_key == 'apply_hadTauFakeRateSF':
                    jobOptions_val = False
            jobOptions_expr = ""
            if jobOptions_key in jobOptions_typeMapping:
              jobOptions_expr = jobOptions_typeMapping[jobOptions_key]
            elif type(jobOptions_val) == bool:
                jobOptions_expr = "cms.bool(%s)"
            elif type(jobOptions_val) == int:
                jobOptions_expr = "cms.int32(%s)"
            elif type(jobOptions_val) == float:
                jobOptions_expr = "cms.double(%s)"
            elif type(jobOptions_val) == str:
                jobOptions_expr = "cms.string('%s')"
            elif type(jobOptions_val) == list:
                if all(map(lambda x: type(x) == int, jobOptions_val)):
                    jobOptions_expr = "cms.vint32(%s)"
                elif all(map(lambda x: type(x) == float, jobOptions_val)):
                    jobOptions_expr = "cms.vdouble(%s)"
                elif all(map(lambda x: type(x) == str, jobOptions_val)):
                    jobOptions_expr = "cms.vstring(%s)"
                elif all(map(lambda x: type(x) == cms.PSet, jobOptions_val)):
                    jobOptions_expr = "cms.VPSet(%s)"
                else:
                    raise ValueError(
                        "Cannot find correct cms vector type for value %s: %s" % \
                        (jobOptions_key, str(jobOptions_val))
                    )
            else:
                raise ValueError(
                    "Cannot find correct cms type for value %s: %s" % \
                    (jobOptions_key, str(jobOptions_val))
                )
            assert(jobOptions_expr)
            if jobOptions_key.startswith('apply_') and jobOptions_key.endswith('GenMatching'):
                jobOptions_val = jobOptions_val and is_mc and not self.isBDTtraining
            if jobOptions_key not in [ 'triggers_mu_cfg', 'triggers_e_cfg' ]:
              jobOptions_val = jobOptions_expr % str(jobOptions_val)
            lines.append("{}.{:<{len}} = {}".format(process_string, jobOptions_key, jobOptions_val, len = max_option_len))

        blacklist = set()#set(sample_info["missing_hlt_paths"]) | set(sample_info["missing_from_superset"])
        for trigger in self.triggers:
            trigger_string     = '%s.triggers_%s'     % (process_string, trigger)
            trigger_use_string = '%s.use_triggers_%s' % (process_string, trigger)
            if isLeptonFR:
                available_triggers = list(self.triggerTable.triggers_leptonFR[trigger] - blacklist)
            else:
                available_triggers = list(set(trigger_stat['name'] for trigger_stat in self.triggerTable.triggers_analysis[trigger]) - blacklist)
            use_trigger = bool(trigger in sample_info['triggers'])
            lines.extend([
                "{:<{len}} = cms.vstring({})".format(trigger_string,     available_triggers, len = max_option_len + len(process_string) + 1),
                "{:<{len}} = cms.bool({})".format   (trigger_use_string, use_trigger,        len = max_option_len + len(process_string) + 1),
            ])

        if self.do_sync:
            lines.extend([
                "{}.{:<{len}} = cms.string('{}')".format(process_string, 'syncNtuple.output', os.path.basename(jobOptions['syncOutput']), len = max_option_len),
                "{}.{:<{len}} = cms.string('{}')".format(process_string, 'selEventsFileName_input', jobOptions['syncRLE'], len = max_option_len),
            ])
            if 'syncOpts' in jobOptions:
              sync_opts = "{}.{:<{len}} = cms.VPSet(\n".format(process_string, "syncNtuple.options", len = max_option_len)
              for treeName, genMatch in jobOptions['syncOpts']:
                  sync_opts += "  cms.PSet(tree = cms.string('{}'), genMatch = cms.vstring({})),\n".format(treeName, genMatch)
              sync_opts += ")"
              lines.append(sync_opts)
            elif 'syncTree' in jobOptions:
              lines.append(
                "{}.{:<{len}} = cms.string('{}')".format(process_string, 'syncNtuple.tree', jobOptions['syncTree'], len = max_option_len)
              )
              if 'syncGenMatch' in jobOptions:
                lines.append(
                  "{}.{:<{len}} = cms.vstring({})".format(process_string, 'syncNtuple.genMatch', jobOptions['syncGenMatch'], len = max_option_len)
                )
            else:
              raise RuntimeError("Not enough information available to preapre jobs for sync Ntuple production")

        if sample_info['process_name_specific'] in self.stitching_args:
          assert(stitch_histogram_names)
          process_stitching_args = self.stitching_args[sample_info['process_name_specific']]
          branch_name_xaxis = process_stitching_args['branch_name_xaxis']
          branch_name_yaxis = process_stitching_args['branch_name_yaxis']
          branch_type_xaxis = process_stitching_args['branch_type_xaxis']
          branch_type_yaxis = process_stitching_args['branch_type_yaxis']
          lines.extend([
            "{}.{:<{len}} = cms.bool({})".format    (process_string, 'evtWeight.apply',           True,                  len = max_option_len),
            "{}.{:<{len}} = cms.string('{}')".format(process_string, 'evtWeight.histogramFile',   self.stitched_weights, len = max_option_len),
            "{}.{:<{len}} = cms.string('{}')".format(process_string, 'evtWeight.branchNameXaxis', branch_name_xaxis,     len = max_option_len),
            "{}.{:<{len}} = cms.string('{}')".format(process_string, 'evtWeight.branchNameYaxis', branch_name_yaxis,     len = max_option_len),
            "{}.{:<{len}} = cms.string('{}')".format(process_string, 'evtWeight.branchTypeXaxis', branch_type_xaxis,     len = max_option_len),
            "{}.{:<{len}} = cms.string('{}')".format(process_string, 'evtWeight.branchTypeYaxis', branch_type_yaxis,     len = max_option_len),
            "{}.{:<{len}} = cms.VPSet([".format     (process_string, 'evtWeight.histograms',      branch_type_yaxis,     len = max_option_len),
          ])
          for central_or_shift in stitch_histogram_names:
            lines.extend([
              "  cms.PSet(",
              "    central_or_shift = cms.string('{}'),".format(central_or_shift),
              "    histogramName = cms.string('{}/{}'),".format(process_stitching_args['histogram_path'], stitch_histogram_names[central_or_shift]),
              "  ),",
            ])
          lines.extend("])")

        return lines

    def pruneSystematics(self):
        assert(self.central_or_shifts_fr)
        central_or_shifts_fr_remove = [
            central_or_shift for central_or_shift in systematics.FR_all if central_or_shift not in self.central_or_shifts_fr
        ]
        if central_or_shifts_fr_remove:
            logging.warning("Disabling the following systematics: {}".format(', '.join(central_or_shifts_fr_remove)))
            self.central_or_shifts = [
               central_or_shift for central_or_shift in self.central_or_shifts if central_or_shift not in central_or_shifts_fr_remove
            ]

    def internalizeSystematics(self):
      assert(self.central_or_shifts)
      self.central_or_shifts_internal = [
        central_or_shift for central_or_shift in self.central_or_shifts if central_or_shift in systematics.an_internal
      ]
      self.central_or_shifts_external = [
        central_or_shift for central_or_shift in self.central_or_shifts if central_or_shift not in systematics.an_internal
      ]
      if "central" not in self.central_or_shifts_internal:
        self.central_or_shifts_internal = [ "central" ] + self.central_or_shifts_internal
      if "central" not in self.central_or_shifts_external:
        self.central_or_shifts_external = [ "central" ] + self.central_or_shifts_external
      logging.info("Separated internal systematics: {}".format(', '.join(self.central_or_shifts_internal)))
      logging.info("Separated external systematics: {}".format(', '.join(self.central_or_shifts_external)))

    def runTHweights(self, sample_info):
        return False

    def get_samples_categories_MC(self, nonfake_backgrounds):
        samples_categories_MC = []
        for sample_category in nonfake_backgrounds + self.ttHProcs:
            decays = [""]
            if sample_category in self.procsWithDecayModes:
                decays += self.decayModes
            if "HH" in sample_category:
                decays += self.decayModes_HH

            couplings = [""]
            if sample_category in ["tHq", "tHW"]:
                couplings += self.thcouplings
            if sample_category in ["HH"] :
                couplings += self.kt_weights

            for decayMode in decays:
                for coupling in couplings:
                  if sample_category not in self.ttHProcs and decayMode in ["hmm", "hzg"]:
                      continue
                  if sample_category in ["tHq", "tHW"] and not coupling == "" and decayMode == "":
                      continue
                  if coupling == "" and decayMode == "":
                      samples_categories_MC.append("%s" % sample_category)
                  elif coupling == "":
                      samples_categories_MC.append("%s_%s" % (sample_category, decayMode))
                  else:
                      samples_categories_MC.append("%s_%s_%s" % (sample_category, coupling, decayMode))
        return samples_categories_MC

    def get_processes_input_base(self, sample_categories):
        processes_input_base = []
        for sample_category in sample_categories:
            if sample_category == "WH" or sample_category == "ZH":
              continue  # in fakes we do not care about separation
            decays = [""]
            couplings = [""]
            for decayMode in decays:
                for coupling in couplings:
                    if sample_category not in self.ttHProcs and decayMode in ["hmm", "hzg"]:
                      continue
                    if sample_category in ["tHq", "tHW"] and not coupling == "" and decayMode == "":
                      continue
                    if coupling == "" and decayMode == "":
                        processes_input_base.append("%s" % sample_category)
                    elif coupling == "":
                        processes_input_base.append("%s_%s" % (sample_category, decayMode))
                    else:
                        processes_input_base.append("%s_%s_%s" % (sample_category, coupling, decayMode))
        return processes_input_base

    def createCfg_copyHistograms(self, jobOptions):
        """Create python configuration file for the copyHistograms executable (split the ROOT files produced by hadd_stage1 into separate ROOT files, one for each event category)

           Args:
             inputFiles: input file (the ROOT file produced by hadd_stage1)
             outputFile: output file of the job
        """
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(jobOptions['outputFile']))
        lines.append("process.copyHistograms.categories = cms.vstring(%s)" % jobOptions['categories'])
        create_cfg(self.cfgFile_copyHistograms, jobOptions['cfgFile_modified'], lines)

    def createCfg_addBackgrounds(self, jobOptions):
        """Create python configuration file for the addBackgrounds executable (sum either all "fake" or all "non-fake" contributions)

           Args:
             inputFiles: input file (the ROOT file produced by hadd_stage1)
             outputFile: output file of the job
        """
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(jobOptions['outputFile']))
        lines.append("process.addBackgrounds.categories = cms.vstring(%s)" % jobOptions['categories'])
        lines.append("process.addBackgrounds.processes_input = cms.vstring(%s)" % jobOptions['processes_input'])
        lines.append("process.addBackgrounds.process_output = cms.string('%s')" % jobOptions['process_output'])
        if 'histogramsToCopy' in jobOptions.keys():
            lines.append("process.addBackgrounds.histogramsToCopy = cms.vstring(%s)" % jobOptions['histogramsToCopy'])
        if 'sysShifts' in jobOptions.keys():
            lines.append("process.addBackgrounds.sysShifts = cms.vstring(%s)" % jobOptions['sysShifts'])
        else:
            lines.append("process.addBackgrounds.sysShifts = cms.vstring(%s)" % self.central_or_shifts)
        create_cfg(self.cfgFile_addBackgrounds, jobOptions['cfgFile_modified'], lines)

    def createCfg_addFakes(self, jobOptions):
        """Create python configuration file for the addBackgroundLeptonFakes executable (data-driven estimation of 'Fakes' backgrounds)

           Args:
             inputFiles: input file (the ROOT file produced by hadd_stage1)
             outputFile: output file of the job
        """
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(jobOptions['outputFile']))
        lines.append("process.addBackgroundLeptonFakes.categories = cms.VPSet(")
        lines.append("    cms.PSet(")
        lines.append("        signal = cms.string('%s')," % jobOptions['category_signal'])
        lines.append("        sideband = cms.string('%s')" % jobOptions['category_sideband'])
        lines.append("    )")
        lines.append(")")
        nonfake_backgrounds = [ category for category in self.nonfake_backgrounds if category not in [ "WH", "ZH" ] ]
        processesToSubtract = []
        processesToSubtract.extend(nonfake_backgrounds)
        if '0l' not in self.channel:
            processesToSubtract.extend([ "%s_Convs" % conv_background for conv_background in self.convs_backgrounds])
        lines.append("process.addBackgroundLeptonFakes.processesToSubtract = cms.vstring(%s)" % processesToSubtract)
        lines.append("process.addBackgroundLeptonFakes.sysShifts = cms.vstring(%s)" % self.central_or_shifts)
        create_cfg(self.cfgFile_addFakes, jobOptions['cfgFile_modified'], lines)

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
        processesToSubtract = [ "data_fakes" ]
        processesToSubtract.extend([ "%s_Convs" % conv_background for conv_background in self.convs_backgrounds ])
        lines.append("process.addBackgroundLeptonFlips.processesToSubtract = cms.vstring(%s)" % processesToSubtract)
        lines.append("process.addBackgroundLeptonFlips.sysShifts = cms.vstring(%s)" % self.central_or_shifts)
        create_cfg(self.cfgFile_addFlips, jobOptions['cfgFile_modified'], lines)

    def createCfg_prep_dcard(self, jobOptions):
        """Fills the template of python configuration file for datacard preparation

           Args:
             histogramToFit: name of the histogram used for signal extraction
        """
        assert(self.prep_dcard_processesToCopy)
        category_output = self.channel
        if 'label' in jobOptions.keys() and jobOptions['label']:
            category_output += "_%s" % jobOptions['label']
        histogramToFit = jobOptions['histogramToFit']
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % jobOptions['datacardFile'])
        lines.append("process.prepareDatacards.processesToCopy = cms.vstring(%s)" % self.prep_dcard_processesToCopy)
        lines.append("process.prepareDatacards.signals = cms.vstring(%s)" % self.prep_dcard_signals)
        lines.append("process.prepareDatacards.makeSubDir = cms.bool(False)")
        lines.append("process.prepareDatacards.categories = cms.VPSet(")
        lines.append("    cms.PSet(")
        lines.append("        input = cms.string('%s/sel/evt')," % jobOptions['histogramDir'])
        lines.append("        output = cms.string('ttH_%s')" % category_output)
        lines.append("    )")
        lines.append(")")
        lines.append("process.prepareDatacards.histogramToFit = cms.string('%s')" % histogramToFit)
        lines.append("process.prepareDatacards.sysShifts = cms.vstring(%s)" % self.central_or_shifts)

        # If the user has specified the binning options for a particular histogram, we expect to see
        # a dictionary instead of a list of histogram names that's been passed to this class as histograms_to_fit
        assert(histogramToFit in self.histograms_to_fit)
        histogramToFit_options = self.histograms_to_fit[histogramToFit]

        # Check the binning options
        if not histogramToFit_options:
            # Use whatever the default setting are in the original prepareDatacards template
            pass
        else:
            # Expected syntax:
            # {
            #   "EventCounter"    : { 'auto_rebin' : True, 'min_auto_rebin' = 0.05 }, # no quantile
            #   "numJets"         : { 'quantile_rebin' : 5 }, # also enables quantile rebinning, no auto
            #   "mTauTauVis1_sel" : {}, # default settings (no auto or quantile rebinning)
            # }
            if 'auto_rebin' in histogramToFit_options:
                lines.append("process.prepareDatacards.apply_automatic_rebinning = cms.bool(%s)" % \
                             histogramToFit_options['auto_rebin'])
            if 'min_auto_rebin' in histogramToFit_options:
                lines.append("process.prepareDatacards.minEvents_automatic_rebinning = cms.double(%.3f)" % \
                             histogramToFit_options['min_auto_rebin'])
            if 'quantile_rebin' in histogramToFit_options:
                lines.append("process.prepareDatacards.nbin_quantile_rebinning = cms.int32(%d)" % \
                             histogramToFit_options['quantile_rebin'])
                if 'quantile_in_fakes' in histogramToFit_options:
                    lines.append("process.prepareDatacards.quantile_rebinning_in_fakes = cms.bool(%s)" % \
                                 histogramToFit_options['quantile_in_fakes'])
            if 'explicit_binning' in histogramToFit_options:
                explicit_binning = histogramToFit_options['explicit_binning']
                assert(type(explicit_binning) == list and sorted(explicit_binning) == explicit_binning)
                lines.append("process.prepareDatacards.explicit_binning = cms.vdouble(%s)" % explicit_binning)

        create_cfg(self.cfgFile_prep_dcard, jobOptions['cfgFile_modified'], lines)

    def createCfg_add_syst_dcard(self, jobOptions):
        """Fills the template of python configuration file for adding specific shape systematics to the datacard,
           e.g. CMS_ttHl_Clos_e_shapeUp/Down, CMS_ttHl_Clos_m_shapeUp/Down

           Args:
             histogramToFit: name of the histogram used for signal extraction
        """
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % jobOptions['outputFile'])
        lines.append("process.addSystDatacards.category = cms.string('%s')" % jobOptions['category'])
        lines.append("process.addSystDatacards.histogramToFit = cms.string('%s')" % jobOptions['histogramToFit'])
        create_cfg(self.cfgFile_add_syst_dcard, jobOptions['cfgFile_modified'], lines)

    def createCfg_add_syst_fakerate(self, jobOptions):
        """Fills the template of python configuration file for adding the following shape systematics to the datacard:
            - 'CMS_ttHl_Clos_norm_e'
            - 'CMS_ttHl_Clos_shape_e'
            - 'CMS_ttHl_Clos_norm_m'
            - 'CMS_ttHl_Clos_shape_m'
            - 'CMS_ttHl_Clos_norm_t'
            - 'CMS_ttHl_Clos_shape_t'

           Args:
             histogramToFit: name of the histogram used for signal extraction
        """
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % jobOptions['outputFile'])
        lines.append("process.addSystFakeRates.category = cms.string('%s')" % jobOptions['category'])
        lines.append("process.addSystFakeRates.histogramToFit = cms.string('%s')" % jobOptions['histogramToFit'])
        xAxisTitle = None
        yAxisTitle = None
        if jobOptions['histogramToFit'].find("mva") != -1:
            xAxisTitle = "MVA Discriminant"
            yAxisTitle = "dN/dMVA"
        elif jobOptions['histogramToFit'].find("mTauTauVis") != -1:
            xAxisTitle = "m_{#tau#tau}^{vis} [GeV]"
            yAxisTitle = "dN/dm_{#tau#tau}^{vis} [1/GeV]"
        else:
            xAxisTitle = ""
            yAxisTitle = ""
        lines.append("process.addSystFakeRates.xAxisTitle = cms.string('%s')" % xAxisTitle)
        lines.append("process.addSystFakeRates.yAxisTitle = cms.string('%s')" % yAxisTitle)
        lines.append("process.addSystFakeRates.addSyst = cms.VPSet(")
        for lepton_and_hadTau_type in [ 'e', 'm', 't' ]:
            if ('add_Clos_%s' % lepton_and_hadTau_type) in jobOptions:
                lines.append("    cms.PSet(")
                lines.append("        name = cms.string('CMS_ttHl_Clos_%s')," % lepton_and_hadTau_type)
                lines.append("        fakes_mc = cms.PSet(")
                lines.append("            inputFileName = cms.string('%s')," % jobOptions['inputFile_nominal_%s' % lepton_and_hadTau_type])
                lines.append("            histogramName = cms.string('%s')," % jobOptions['histogramName_nominal_%s' % lepton_and_hadTau_type])
                lines.append("        ),")
                lines.append("        mcClosure = cms.PSet(")
                lines.append("            inputFileName = cms.string('%s')," % jobOptions['inputFile_mcClosure_%s' % lepton_and_hadTau_type])
                lines.append("            histogramName = cms.string('%s')," % jobOptions['histogramName_mcClosure_%s' % lepton_and_hadTau_type])
                lines.append("        ),")
                lines.append("    ),")
        lines.append(")")
        lines.append("process.addSystFakeRates.outputFileName = cms.string('%s')" % jobOptions['plots_outputFileName'])
        create_cfg(self.cfgFile_add_syst_fakerate, jobOptions['cfgFile_modified'], lines)

    def createCfg_makePlots_addShapes(self, lines):
        central_or_shifts_prefix = []
        for central_or_shift in self.central_or_shifts:
            if central_or_shift in systematics.JEC_regrouped or central_or_shift in systematics.JER_split:
                continue
            if central_or_shift.startswith('CMS_ttHl_FR'):
                continue
            central_or_shift_prefix = ''
            if central_or_shift.endswith('Up'):
                central_or_shift_prefix = central_or_shift[:-2]
            elif central_or_shift.endswith('Down'):
                central_or_shift_prefix = central_or_shift[:-4]
            else:
                continue
            if central_or_shift_prefix not in central_or_shifts_prefix:
                central_or_shifts_prefix.append(central_or_shift_prefix)
        if central_or_shifts_prefix:
            lines.append("process.makePlots.showUncertainty = cms.bool(True)")
            lines.append("process.makePlots.shape = cms.PSet(")
            for central_or_shift_prefix in central_or_shifts_prefix:
                lines.append("  {} = cms.string('0.00 +/- 1.00'),".format(central_or_shift_prefix))
            lines.append(")")

    def createCfg_makePlots(self, jobOptions):
        """Fills the template of python configuration file for making control plots

           Args:
             histogram_file: name of the input ROOT file
        """
        assert(jobOptions['make_plots_backgrounds'])
        category_label = self.channel
        if jobOptions['label']:
            category_label += " (%s)" % jobOptions['label']
        lines = []
        lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
        lines.append("process.makePlots.outputFileName = cms.string('%s')" % jobOptions['outputFile'])
        lines.append("process.makePlots.processesBackground = cms.vstring(%s)" % jobOptions['make_plots_backgrounds'])
        lines.append("process.makePlots.processSignal = cms.string('%s')" % self.make_plots_signal)
        lines.append("process.makePlots.categories = cms.VPSet(")
        lines.append("  cms.PSet(")
        lines.append("    name = cms.string('%s')," % jobOptions['histogramDir'])
        lines.append("    label = cms.string('%s')" % category_label)
        lines.append("  )")
        lines.append(")")
        lines.append("process.makePlots.intLumiData = cms.double(%.1f)" % (self.lumi / 1000))
        if 'extra_params' in jobOptions:
          lines.append(jobOptions['extra_params'])
        self.createCfg_makePlots_addShapes(lines)
        if hasattr(self, 'isControlRegion'):
          lines.append("extend({})".format(self.isControlRegion))
        create_cfg(self.cfgFile_make_plots, jobOptions['cfgFile_modified'], lines)

    def createCfg_makePlots_mcClosure(self, jobOptions): #TODO
      """Fills the template of python configuration file for making control plots

      Args:
        histogramFile: name of the input ROOT file
      """
      assert(self.make_plots_backgrounds)
      lines = []
      lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % jobOptions['inputFile'])
      lines.append("process.makePlots.outputFileName = cms.string('%s')" % jobOptions['outputFile'])
      lines.append("process.makePlots.processesBackground = cms.vstring(%s)" % self.make_plots_backgrounds)
      lines.append("process.makePlots.processSignal = cms.string('%s')" % self.make_plots_signal)
      lines.append("process.makePlots.categories = cms.VPSet(")
      lines.append("  cms.PSet(")
      lines.append("    signal = cms.string('%s')," % self.histogramDir_prep_dcard)
      lines.append("    sideband = cms.string('%s')," % self.histogramDir_prep_dcard.replace("Tight", "Fakeable_mcClosure_wFakeRateWeights"))
      lines.append("    label = cms.string('%s')" % self.channel)
      lines.append("  )")
      lines.append(")")
      lines.append("process.makePlots.intLumiData = cms.double(%.1f)" % self.lumi)
      self.createCfg_makePlots_addShapes(lines)
      if hasattr(self, 'isControlRegion'):
        lines.append("extend({})".format(self.isControlRegion))
      create_cfg(self.cfgFile_make_plots_mcClosure, jobOptions['cfgFile_modified'], lines)

    def createScript_sbatch(self, executable, sbatchFile, jobOptions,
                            key_cfg_file = 'cfgFile_modified', key_input_file = 'inputFile',
                            key_output_file = 'outputFile', key_log_file = 'logFile',
                            min_file_size = 20000):
        """Creates the python script necessary to submit 'generic' (addBackgrounds, addBackgroundFakes/addBackgroundFlips) jobs to the batch system
        """
        num_jobs = tools_createScript_sbatch(
            sbatch_script_file_name = sbatchFile,
            executable = executable,
            command_line_parameters = { key: value[key_cfg_file] for key, value in jobOptions.items() },
            input_file_names = { key: value[key_input_file] for key, value in jobOptions.items() },
            output_file_names = { key: value[key_output_file] for key, value in jobOptions.items() },
            script_file_names = { key: value[key_cfg_file].replace(".py", ".sh").replace("_cfg", "") for key, value in jobOptions.items() },
            log_file_names = { key: value[key_log_file] for key, value in jobOptions.items() },
            working_dir = self.workingDir,
            max_num_jobs = self.max_num_jobs,
            cvmfs_error_log = self.cvmfs_error_log,
            pool_id = uuid.uuid4(),
            verbose = self.verbose,
            dry_run = self.dry_run,
            validate_outputs = self.check_output_files,
            use_home = self.use_home,
            min_file_size = min_file_size,
        )
        return num_jobs

    def createScript_sbatch_syncNtuple(self, executable, sbatchFile, jobOptions):
        """Creates the python script necessary to submit the syncNtuple production jobs to the batch system
        """
        self.num_jobs['analyze'] += self.createScript_sbatch(
            executable, sbatchFile, jobOptions, 'cfgFile_modified', 'ntupleFiles', 'syncOutput',
            'logFile', min_file_size = -1,
        )

    def createScript_sbatch_analyze(self, executable, sbatchFile, jobOptions):
        """Creates the python script necessary to submit the analysis jobs to the batch system
        """
        self.num_jobs['analyze'] += self.createScript_sbatch(
            executable, sbatchFile, jobOptions, 'cfgFile_modified', 'ntupleFiles', 'histogramFile',
            'logFile')

    def createScript_sbatch_copyHistograms(self, executable, sbatchFile, jobOptions):
        """Creates the python script necessary to submit the 'copyHistograms' jobs to the batch system
        """
        self.num_jobs['copyHistograms'] += self.createScript_sbatch(executable, sbatchFile, jobOptions, min_file_size = 5000)

    def createScript_sbatch_addBackgrounds(self, executable, sbatchFile, jobOptions):
        """Creates the python script necessary to submit the 'addBackgrounds' jobs to the batch system
        """
        self.num_jobs['addBackgrounds'] += self.createScript_sbatch(executable, sbatchFile, jobOptions, min_file_size = 5000)

    def createScript_sbatch_addFakes(self, executable, sbatchFile, jobOptions):
        """Creates the python script necessary to submit the 'addBackgroundLeptonFakes' jobs to the batch system
        """
        self.num_jobs['addFakes'] += self.createScript_sbatch(executable, sbatchFile, jobOptions)

    def create_hadd_python_file(self, inputFiles, outputFiles, hadd_stage_name, max_input_files_per_job = 10, max_mem = ''):
        sbatch_hadd_file = os.path.join(self.dirs[DKEY_SCRIPTS], "sbatch_hadd_%s_%s.py" % (self.channel, hadd_stage_name)).replace(".root", "")
        scriptFile       = os.path.join(self.dirs[DKEY_SCRIPTS], os.path.basename(sbatch_hadd_file).replace(".py", ".sh"))
        logFile          = os.path.join(self.dirs[DKEY_LOGS],    os.path.basename(sbatch_hadd_file).replace(".py", ".log"))
        sbatch_hadd_dir  = os.path.join(self.dirs[DKEY_HADD_RT], self.channel, hadd_stage_name) if self.dirs[DKEY_HADD_RT] else ''
        self.num_jobs['hadd'] += tools_createScript_sbatch_hadd_nonBlocking(
            sbatch_script_file_name = sbatch_hadd_file,
            inputFiles              = inputFiles,
            outputFiles             = outputFiles,
            script_file_name        = scriptFile,
            log_file_name           = logFile,
            working_dir             = self.workingDir,
            auxDirName              = sbatch_hadd_dir,
            verbose                 = self.verbose,
            dry_run                 = self.dry_run,
            max_input_files_per_job = max_input_files_per_job,
            use_home                = self.use_home,
            max_mem                 = max_mem,
        )
        return sbatch_hadd_file

    def addToMakefile_analyze(self, lines_makefile, make_target = "phony_analyze", make_dependency = ""):
        """Adds the commands to Makefile that are necessary for running the analysis code on the Ntuple and filling the histograms
        """
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_analyze))
        else:
            for job in self.jobOptions_analyze.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_analyze, job['cfgFile_modified'], job['logFile']))
        lines_makefile.append("")
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        for job in self.jobOptions_analyze.values():
            self.filesToClean.append(job['histogramFile'])

    def addToMakefile_syncNtuple(self, lines_makefile, make_target = "phony_analyze", make_dependency = ""):
        """Adds the commands to Makefile that are necessary for running the analysis code on the Ntuple and filling the histograms
        """
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_analyze))
        else:
            for job in self.jobOptions_analyze.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_analyze, job['cfgFile_modified'], job['logFile']))
                lines_makefile.append("\tmv %s %s" % (os.path.basename(job['syncOutput']), job['syncOutput']))
                lines_makefile.append("\tsleep 60")  # sleep 60 seconds for hadoop to catch up
        lines_makefile.append("")
        for job in self.jobOptions_analyze.values():
            self.filesToClean.append(job['syncOutput'])

    def addToMakefile_hadd(self, lines_makefile, make_target, make_dependency, inputFiles, outputFiles, 
                           max_input_files_per_job = 10, max_mem = ''):
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        if self.is_sbatch:
            # do not remove the output file -> maybe it's valid
            # the sbatch job checks the existance of the file anyways
            #lines_makefile.append("\t%s %s" % ("rm -f", outputFiles[key]))
            scriptFile = self.create_hadd_python_file(
              inputFiles, outputFiles, "_".join([ make_target, "ClusterHistogramAggregator" ]), max_input_files_per_job, max_mem
            )
            lines_makefile.append("%s: %s" % (make_target, make_dependency))
            lines_makefile.append("\t%s %s" % ("python", scriptFile))
        else:
            numOutputFiles = len(outputFiles.keys())
            numBatches = min(100, numOutputFiles)
            make_target_batches = []
            idxBatch = 0
            for idxKey, key in enumerate(outputFiles.keys()):
                if idxKey*numBatches >= idxBatch*numOutputFiles:
                    if idxKey > 0:
                        lines_makefile.append("")
                    make_target_batch = "%s_part%i" % (make_target, idxBatch)
                    lines_makefile.append("%s: %s" % (make_target_batch, make_dependency))
                    make_target_batches.append(make_target_batch)
                    idxBatch = idxBatch + 1
                    if make_target_batch not in self.phoniesToAdd:
                        self.phoniesToAdd.append(make_target_batch)
                outputFile_base = os.path.basename(outputFiles[key])
                lines_makefile.append("\thadd -f %s %s" % (outputFile_base, ' '.join(inputFiles[key])))
                if outputFile_base != outputFiles[key]:
                    lines_makefile.append("\tmv %s %s" % (outputFile_base, outputFiles[key]))
            lines_makefile.append("")
            lines_makefile.append("%s: %s" % (make_target, " ".join(make_target_batches)))
        lines_makefile.append("")
        for outputFile in outputFiles.values():
            self.filesToClean.append(outputFile)

    def addToMakefile_hadd_stage1(self, lines_makefile, make_target = "phony_hadd_stage1", make_dependency = "phony_analyze", 
                                  max_input_files_per_job = 10, max_mem = ''):
        self.addToMakefile_hadd(lines_makefile, make_target, make_dependency, self.inputFiles_hadd_stage1, self.outputFile_hadd_stage1)

    def addToMakefile_hadd_sync(self, lines_makefile, make_target = "phony_hadd_sync", make_dependency = "phony_analyze"):
        self.addToMakefile_hadd(lines_makefile, make_target, make_dependency, self.inputFiles_sync, self.outputFile_sync)

    def addToMakefile_copyHistograms(self, lines_makefile, make_target = "phony_copyHistograms", make_dependency = "phony_hadd_stage1"):
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_copyHistograms))
        else:
            for job in self.jobOptions_copyHistograms.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_copyHistograms, job['cfgFile_modified'], job['logFile']))
        lines_makefile.append("")
        for job in self.jobOptions_copyHistograms.values():
            self.filesToClean.append(job['outputFile'])

    def addToMakefile_addBackgrounds(self, lines_makefile, make_target, make_dependency, sbatchFile, jobOptions):
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", sbatchFile))
        else:
            for job in jobOptions.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_addBackgrounds, job['cfgFile_modified'], job['logFile']))
        lines_makefile.append("")
        for job in jobOptions.values():
            self.filesToClean.append(job['outputFile'])

    def addToMakefile_hadd_stage1_5(self, lines_makefile, make_target, make_dependency, max_input_files_per_job = 2, max_mem = ''):
        """Adds the commands to Makefile that are necessary for building the intermediate histogram file
           that is used as input for data-driven background estimation.
        """
        self.addToMakefile_hadd(lines_makefile, make_target, make_dependency, 
          self.inputFiles_hadd_stage1_5, self.outputFile_hadd_stage1_5, max_input_files_per_job, max_mem)

    def addToMakefile_hadd_stage1_6(self, lines_makefile, make_target, make_dependency, max_input_files_per_job = 2, max_mem = ''):
        """Adds the commands to Makefile that are necessary for building the intermediate histogram file
           that is used as input for data-driven background estimation.
        """
        self.addToMakefile_hadd(lines_makefile, make_target, make_dependency, 
          self.inputFiles_hadd_stage1_6, self.outputFile_hadd_stage1_6, max_input_files_per_job, max_mem)

    def addToMakefile_addFakes(self, lines_makefile, make_target, make_dependency):
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_addFakes))
        else:
            for job in self.jobOptions_addFakes.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_addFakes, job['cfgFile_modified'], job['logFile']))
        lines_makefile.append("")
        for job in self.jobOptions_addFakes.values():
            self.filesToClean.append(job['outputFile'])

    def addToMakefile_addFlips(self, lines_makefile, make_target, make_dependency):
        if make_target not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target)
        lines_makefile.append("%s: %s" % (make_target, make_dependency))
        if self.is_sbatch:
            lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_addFlips))
        else:
            for job in self.jobOptions_addFlips.values():
                lines_makefile.append("\t%s %s &> %s" % (self.executable_addFlips, job['cfgFile_modified'], job['logFile']))
        lines_makefile.append("")
        for job in self.jobOptions_addFlips.values():
            self.filesToClean.append(job['outputFile'])

    def addToMakefile_backgrounds_from_data(self, lines_makefile, make_target = "phony_addFakes", make_dependency = "phony_hadd_stage1", 
                                            hadd_max_input_files_per_job = 2, hadd_max_mem = ''):
        self.addToMakefile_addBackgrounds(lines_makefile, "phony_addBackgrounds", make_dependency, 
          self.sbatchFile_addBackgrounds, self.jobOptions_addBackgrounds)
        self.addToMakefile_hadd_stage1_5(lines_makefile, "phony_hadd_stage1_5", "phony_addBackgrounds", 
          max_input_files_per_job = hadd_max_input_files_per_job, max_mem = hadd_max_mem)
        self.addToMakefile_addBackgrounds(lines_makefile, "phony_addBackgrounds_sum", "phony_hadd_stage1_5", 
          self.sbatchFile_addBackgrounds_sum, self.jobOptions_addBackgrounds_sum)
        self.addToMakefile_addFakes(lines_makefile, "phony_addFakes", "phony_hadd_stage1_5")
        if make_target != "phony_addFakes":
            lines_makefile.append("%s: %s" % (make_target, "phony_addFakes"))
            lines_makefile.append("")
        self.make_dependency_hadd_stage2 = " ".join([ "phony_addBackgrounds_sum", make_target ])

    def addToMakefile_backgrounds_from_data_withFlips(self, lines_makefile, make_target = "phony_addFlips", 
                                                      hadd_max_input_files_per_job = 2, hadd_max_mem = ''):
        self.addToMakefile_addBackgrounds(lines_makefile, "phony_addBackgrounds", "phony_hadd_stage1", 
          self.sbatchFile_addBackgrounds, self.jobOptions_addBackgrounds)
        self.addToMakefile_hadd_stage1_5(lines_makefile, "phony_hadd_stage1_5", "phony_addBackgrounds", 
          max_input_files_per_job = hadd_max_input_files_per_job, max_mem = hadd_max_mem)
        self.addToMakefile_addBackgrounds(lines_makefile, "phony_addBackgrounds_sum", "phony_hadd_stage1_5", 
          self.sbatchFile_addBackgrounds_sum, self.jobOptions_addBackgrounds_sum)
        self.addToMakefile_addFakes(lines_makefile, "phony_addFakes", "phony_hadd_stage1_5")
        self.addToMakefile_hadd_stage1_6(lines_makefile, "phony_hadd_stage1_6", "phony_addFakes", 
          max_input_files_per_job = hadd_max_input_files_per_job, max_mem = hadd_max_mem)
        self.addToMakefile_addFlips(lines_makefile, "phony_addFlips", "phony_hadd_stage1_6")
        if make_target != "phony_addFlips":
            lines_makefile.append("%s: %s" % (make_target, "phony_addFlips"))
            lines_makefile.append("")
        self.make_dependency_hadd_stage2 = " ".join([ "phony_addBackgrounds_sum", make_target ])

    def addToMakefile_hadd_stage2(self, lines_makefile, make_target = "phony_hadd_stage2", make_dependency = None, 
                                  max_input_files_per_job = 2, max_mem = ''):
        """Adds the commands to Makefile that are necessary for building the final histogram file.
        """
        if make_dependency is None:
            make_dependency = self.make_dependency_hadd_stage2
        self.addToMakefile_hadd(lines_makefile, make_target, make_dependency, self.inputFiles_hadd_stage2, self.outputFile_hadd_stage2, 
          max_input_files_per_job, max_mem)
        lines_makefile.append("")
        for outputFile in self.outputFile_hadd_stage2.values():
            lines_makefile.append("%s: %s" % (outputFile, make_target))
        lines_makefile.append("")

    def addToMakefile_prep_dcard(self, lines_makefile):
        """Adds the commands to Makefile that are necessary for building the datacards.
        """
        for job in self.jobOptions_prep_dcard.values():
            lines_makefile.append("%s: %s" % (job['datacardFile'], job['inputFile']))
            lines_makefile.append("\t%s %s" % (self.executable_prep_dcard, job['cfgFile_modified']))
            self.filesToClean.append(job['datacardFile'])
            lines_makefile.append("")

    def addToMakefile_add_syst_dcard(self, lines_makefile):
        """Adds the commands to Makefile that are necessary for including additional systematic uncertainties into the datacards.
        """
        for job in self.jobOptions_add_syst_dcard.values():
            lines_makefile.append("%s: %s" % (job['outputFile'], job['inputFile']))
            lines_makefile.append("\t%s %s" % (self.executable_add_syst_dcard, job['cfgFile_modified']))
            self.filesToClean.append(job['outputFile'])
            lines_makefile.append("")

    def addToMakefile_add_syst_fakerate(self, lines_makefile):
        """Adds the commands to Makefile that are necessary for including additional systematic uncertainties,
           related to the non-closure of the fake-rates for electrons, muons, and taus, into the datacards.
        """
        for job in self.jobOptions_add_syst_fakerate.values():
            lines_makefile.append("%s: %s" % (job['outputFile'], job['inputFile']))
            lines_makefile.append("\t%s %s" % (self.executable_add_syst_fakerate, job['cfgFile_modified']))
            self.filesToClean.append(job['outputFile'])
            lines_makefile.append("")

    def addToMakefile_make_plots(self, lines_makefile):
        """Adds the commands to Makefile that are necessary for making control plots of the jet->tau fake background estimation procedure.
        """
        for idxJob, job in enumerate(self.jobOptions_make_plots.values()):
            make_target_plot = "phony_makePlots%i" % idxJob
            lines_makefile.append("%s: %s" % (make_target_plot, job['inputFile']))
            lines_makefile.append("\t%s %s" % (job['executable'], job['cfgFile_modified']))
            lines_makefile.append("")
            if make_target_plot not in self.phoniesToAdd:
                self.phoniesToAdd.append(make_target_plot)

    def addToMakefile_validate(self, lines_makefile, make_dependency = "phony_analyze", single_channel = True):
        """Validates the results
        """
        make_target_validate = "phony_validate"
        lines_makefile.append("%s: %s" % (make_target_validate, make_dependency))
        inspect_argument = '-w {}'.format(self.channel) if single_channel else ''
        lines_makefile.append("\tinspect_rle_numbers.py -v -i %s %s &> %s" % (self.outputDir, inspect_argument, self.validation_out))
        lines_makefile.append("")
        if make_target_validate not in self.phoniesToAdd:
            self.phoniesToAdd.append(make_target_validate)
        if make_target_validate not in self.targets:
            self.targets.append(make_target_validate)

    def createMakefile(self, lines_makefile):
        """Creates Makefile that runs the complete analysis workfow.
        """
        self.targets.extend([ jobOptions['datacardFile'] for jobOptions in self.jobOptions_prep_dcard.values() ])
        self.targets.extend([ jobOptions['outputFile'] for jobOptions in self.jobOptions_add_syst_dcard.values() ])
        self.targets.extend([ jobOptions['outputFile'] for jobOptions in self.jobOptions_add_syst_fakerate.values() ])
        for idxJob, jobOptions in enumerate(self.jobOptions_make_plots.values()):
            make_target_plot = "phony_makePlots%i" % idxJob
            self.targets.append(make_target_plot)
            if make_target_plot not in self.phoniesToAdd:
                  self.phoniesToAdd.append(make_target_plot)
        if len(self.targets) == 0:
            self.targets.append("phony_analyze")
        tools_createMakefile(self.makefile, self.targets, lines_makefile, self.filesToClean, self.is_sbatch, self.phoniesToAdd)
        logging.info("Run it with:\tmake -f %s -j %i " % (self.makefile, self.num_parallel_jobs))

    def initializeInputFileIds(self, sample_name, sample_info):
        """Retrieves the number of input ROOT files (Ntuples) corresponding to a given sample
           and fills the number into the dictionary 'self.inputFileIds', with the name of the sample as key

           TODO: add blacklist to the secondary storage as well
        """
        logging.warning("Function <initializeInputFileIds> is deprecated and should not be used anymore !!")
        logging.warning("Please have a look at 'analyzeConfig_2lss_1tau.py' to see " \
                        "how to migrate your python scripts to the new syntax.")
        nof_inputFiles = sample_info["nof_files"]
        store_dirs = sample_info["local_paths"]
        assert(len(store_dirs) <= 2), "There is more than one secondary dir!"
        primary_store, secondary_store = "", ""
        secondary_files, blacklist = [], []
        for store_dir in store_dirs:
            if store_dir["selection"] == "*":
                primary_store = store_dir["path"]
                if "blacklist" in store_dir:
                    blacklist = store_dir["blacklist"]
            else:
                secondary_store = store_dir["path"]
                secondary_files = map(lambda x: int(x), store_dir[
                                      "selection"].split(","))
        self.inputFileIds[sample_name] = generate_file_ids(
            nof_inputFiles, self.max_files_per_job, blacklist)
        return (secondary_files, primary_store, secondary_store)

    def create(self):
        """Creates all config files necessary for runing the complete analysis workfow.
        """
        raise ValueError("Method 'create' not implemented in derrived class !!")

    def run(self):
        """Runs the complete analysis workfow -- either locally or on the batch system.
        """
        record_software_state(self.sw_ver_file_cfg, self.sw_ver_file_out, DEPENDENCIES)
        run_cmd(
            "make -f %s -j %i 2>%s 1>%s" % \
            (self.makefile, self.num_parallel_jobs, self.stderr_file_path, self.stdout_file_path),
            False
        )
