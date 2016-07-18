import codecs, os

from tthAnalysis.HiggsToTauTau.jobTools import create_if_not_exists, run_cmd, generate_file_ids, generate_input_list

DKEY_CFGS = "cfgs"       # dir for python configuration and batch script files for each job
DKEY_HIST = "histograms" # dir for histograms = output of the jobs
DKEY_LOGS = "logs"       # dir for log files (stdout/stderr of jobs)
DKEY_DCRD = "datacards"  # dir for the datacard

def initDict(dictionary, keys):
  """Auxiliary function to initialize dictionary for access with multiple keys
  """
  dictionary_at_keylevel = dictionary
  numKeys = len(keys)
  for idxKey in range(numKeys - 1):
    key = keys[idxKey]
    if key not in dictionary_at_keylevel.keys():
      dictionary_at_keylevel[key] = {}
    dictionary_at_keylevel = dictionary_at_keylevel[key]

def append_to_key(key, part):
  new_key = key
  if len(new_key) > 0:
    new_key += "_"
  new_key += str(part)
  return new_key

def getKey(*args):
  key = ""
  for part in args:
    key = append_to_key(key, part)
  return key

def create_cfg(cfgFile_original, cfgFile_modified, lines):
  """Auxiliary function to clone config file.
     The lines given as argument are added to the end of the cloned file,
     overwriting some of the configuration parameters defined in the original file.
  """
  f_original = open(cfgFile_original, "r")
  cfg_original = f_original.read()
  f_original.close()
  cfg_modified = cfg_original
  cfg_modified += "\n"
  for line in lines:
    cfg_modified += "%s\n" % line
  cfg_modified += "\n"
  f_modified = open(cfgFile_modified, "w")
  f_modified.write(cfg_modified)
  f_modified.close()

def createFile(fileName, lines):
  ##print "<createFile>:"
  ##print " lines = ", lines
  content = ""
  for line in lines:
    content += "%s\n" % line
  content += "\n"
  f = open(fileName, "w")
  f.write(content)
  f.close()
  
class analyzeConfig:
  """Configuration metadata needed to run analysis in a single go.
  
  Sets up a folder structure by defining full path names; no directory creation is delegated here.
  
  Args:
    outputDir: The root output dir -- all configuration, log and output files are stored in its subdirectories
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
    histogram_files_exists: flags indicating if histogram files already exist from a previous execution of 'tthAnalyzeRun_1l_2tau.py', so that 'analyze_1l_2tau' jobs do not have to be submitted again	
    histogram_file_hadd_stage1: the histogram file obtained by hadding the output of all jobs
    histogram_file_hadd_stage2: the final histogram file with data-driven background estimates added
    datacardFile: the datacard -- final output file of this execution flow
    cfgFile_prep_dcard: python configuration file for datacard preparation executable
    histogramDir_prep_dcard: directory in final histogram file that is used for building datacard
  """
  def __init__(self, outputDir, executable_analyze, channel, central_or_shifts,
               max_files_per_job, use_lumi, lumi, debug, running_method, num_parallel_jobs, 
               histograms_to_fit, executable_prep_dcard = "prepareDatacards"):
    self.outputDir = outputDir    
    self.executable_analyze = executable_analyze
    self.channel = channel    
    self.central_or_shifts = central_or_shifts
    self.max_files_per_job = max_files_per_job
    self.use_lumi = use_lumi
    self.lumi = lumi
    self.debug = debug
    assert(running_method.lower() in [ "sbatch", "makefile" ]), "Invalid running method: %s" % running_method
    self.running_method = running_method
    self.is_sbatch = False
    self.is_makefile = False    
    if self.running_method.lower() == "sbatch":
      self.is_sbatch = True
    else:
      self.is_makefile = True
    self.makefile = os.path.join(self.outputDir, "Makefile_%s" % self.channel)
    self.num_parallel_jobs = num_parallel_jobs
    self.histograms_to_fit = histograms_to_fit
    self.executable_prep_dcard = executable_prep_dcard
    self.prep_dcard_processesToCopy = [ "data_obs", "TT", "TTW", "TTZ", "EWK", "Rares" ]
    self.prep_dcard_signals = [ "ttH_hww", "ttH_hzz", "ttH_htt" ]

    self.workingDir = os.getcwd()
    create_if_not_exists(self.outputDir)
    self.stdout_file = codecs.open(os.path.join(self.outputDir, "stdout_%s.log" % self.channel), 'w', 'utf-8')
    self.stderr_file = codecs.open(os.path.join(self.outputDir, "stderr_%s.log" % self.channel), 'w', 'utf-8')
    self.dirs = {}
    self.samples = {}
    self.cfgFiles_analyze_modified = {}
    self.logFiles_analyze = {}
    self.sbatchFile_analyze = os.path.join(self.outputDir, "sbatch_analyze_%s.py" % self.channel)
    self.histogramFiles = {}    
    self.histogramFile_hadd_stage1 = os.path.join(self.outputDir, DKEY_HIST, "histograms_harvested_stage1_%s.root" % self.channel)
    self.histogramFile_hadd_stage2 = os.path.join(self.outputDir, DKEY_HIST, "histograms_harvested_stage2_%s.root" % self.channel)    
    self.datacardFiles = {}
    self.cfgFile_prep_dcard_original = os.path.join(self.workingDir, "prepareDatacards_cfg.py")
    self.cfgFile_prep_dcard_modified = {}
    self.histogramDir_prep_dcard = None

  def createCfg_analyze(self, *args):
    raise ValueError("Function 'createCfg_analyze' not implemented in derrived class !!")      

  def createCfg_prep_dcard(self, histogramToFit):
    """Fills the template of python configuration file for datacard preparation

    Args:
      histogramFile: name of the input ROOT file 
      histogramDir: name of the directory in the ROOT file containing the histograms 
      channel: name of the channel in the datacard
      histogramToFit: name of the histogram used for signal extraction
      outputFile: name of the datacard file
    """
    self.datacardFiles[histogramToFit] = os.path.join(self.outputDir, DKEY_DCRD, "prepareDatacards_%s_%s.root" % (self.channel, histogramToFit))
    lines = []
    lines.append("process.fwliteInput.fileNames = cms.vstring('%s')" % self.histogramFile_hadd_stage2)
    lines.append("process.fwliteOutput.fileName = cms.string('%s')" % self.datacardFiles[histogramToFit])
    lines.append("process.prepareDatacards.processesToCopy = cms.vstring(%s)" % self.prep_dcard_processesToCopy)
    lines.append("process.prepareDatacards.signals = cms.vstring(%s)" % self.prep_dcard_signals)
    lines.append("process.prepareDatacards.categories = cms.VPSet(")
    lines.append("    cms.PSet(")
    lines.append("        input = cms.string('%s/sel/evt')," % self.histogramDir_prep_dcard)
    lines.append("        output = cms.string('ttH_%s')" % self.channel)
    lines.append("    )")
    lines.append(")")
    lines.append("process.prepareDatacards.histogramToFit = cms.string('%s')" % histogramToFit)
    
    self.cfgFile_prep_dcard_modified[histogramToFit] = os.path.join(self.outputDir, DKEY_CFGS, "prepareDatacards_%s_%s_cfg.py" % (self.channel, histogramToFit))
    create_cfg(self.cfgFile_prep_dcard_original, self.cfgFile_prep_dcard_modified[histogramToFit], lines)

  def initializeInputFileIds(self, sample_name, sample_info):
    """Retrieves the number of input ROOT files (Ntuples) corresponding to a given sample
       and fills the number into the dictionary 'self.inputFileIds', with the name of the sample as key
    """
    nof_inputFiles = sample_info["nof_files"]
    store_dirs = sample_info["local_paths"]
    assert(len(store_dirs) <= 2), "There is more than one secondary dir!"
    primary_store, secondary_store = "", ""
    secondary_files = []
    for store_dir in store_dirs:
      if store_dir["selection"] == "*":
        primary_store = store_dir["path"]
      else:
        secondary_store = store_dir["path"]
        secondary_files = map(lambda x: int(x), store_dir["selection"].split(","))
    self.inputFileIds[sample_name] = generate_file_ids(nof_inputFiles, self.max_files_per_job)
    return ( secondary_files, primary_store, secondary_store )

  def createScript_sbatch(self):
    """Creates the python script necessary to submit the analysis jobs to the batch system
    """
    lines_sbatch = []
    lines_sbatch.append("from tthAnalysis.HiggsToTauTau.sbatchManager import sbatchManager")
    lines_sbatch.append("")
    lines_sbatch.append("m = sbatchManager()")
    lines_sbatch.append("m.setWorkingDir('%s')" % self.workingDir)
    for key_file, cfgFile in self.cfgFiles_analyze_modified.items():
      histogramFile = self.histogramFiles[key_file]
      logFile = self.logFiles_analyze[key_file]
      if os.path.exists(histogramFile):
        print "output file %s already exists" % histogramFile
        continue
      lines_sbatch.append("m.submitJob('%s', '%s', '%s', True, '%s')" % (self.executable_analyze, cfgFile, logFile, histogramFile))
    lines_sbatch.append("m.waitForJobs()")
    createFile(self.sbatchFile_analyze, lines_sbatch)

  def addToMakefile_analyze(self, lines_makefile):
    """Adds the commands to Makefile that are necessary for running the analysis code on the Ntuple and filling the histograms
    """
    if self.is_sbatch:
      lines_makefile.append("sbatch:")
      lines_makefile.append("\t%s %s" % ("python", self.sbatchFile_analyze))
      lines_makefile.append("")
    for key_file, histogramFile in self.histogramFiles.items():
      cfgFile_analyze_modified = self.cfgFiles_analyze_modified[key_file]
      if self.is_makefile:
        lines_makefile.append("%s:" % histogramFile)
        cfgFile = self.cfgFiles[key_file]
        logFile = self.logFiles[key_file]
        lines_makefile.append("\t%s %s &> %s" % (self.executable_analyze, cfgFile, logFile))
        lines_makefile.append("")
      elif self.is_sbatch:
        lines_makefile.append("%s: %s" % (histogramFile, "sbatch"))
        lines_makefile.append("\t%s" % ":") # CV: null command
        lines_makefile.append("")

  def addToMakefile_hadd_stage1(self, lines_makefile):
    raise ValueError("Method 'addToMakefile_hadd_stage1' not implemented in derrived class !!")
  
  def addToMakefile_backgrounds_from_data(self, lines_makefile):
    """Adds the commands to Makefile that are necessary for adding the data-driven background estimates.
       Default implementation is a dummy and does not actually add any histograms.
    """
    return

  def addToMakefile_hadd_stage2(self, lines_makefile):
    """Adds the commands to Makefile that are necessary for building the final histogram file.
       Default implementation is a dummy and assumes that 'addToMakefile_backgrounds_from_data' method does not actually add any histograms,
       so that the hadd stage2 file is simply a copy of the hadd stage1 file.
    """
    lines_makefile.append("%s: %s" % (self.histogramFile_hadd_stage2, self.histogramFile_hadd_stage1))
    lines_makefile.append("\tln -s %s %s" % (self.histogramFile_hadd_stage1, self.histogramFile_hadd_stage2))
    lines_makefile.append("")

  def addToMakefile_prep_dcard(self, lines_makefile):
    """Adds the commands to Makefile that are necessary for building the datacards.
    """
    for histogramToFit in self.histograms_to_fit:
      lines_makefile.append("%s: %s" % (self.datacardFiles[histogramToFit], self.histogramFile_hadd_stage2))
      lines_makefile.append("\t%s %s" % (self.executable_prep_dcard, self.cfgFile_prep_dcard_modified[histogramToFit]))
    lines_makefile.append("")
    lines_makefile.append("all: %s" % " ".join(self.datacardFiles.values()))
    lines_makefile.append("")

  def createMakefile(self, lines_makefile):
    """Creates Makefile that runs the complete analysis workfow
    """
    lines_makefile_with_header = []
    lines_makefile_with_header.append(".DEFAULT_GOAL := all")
    lines_makefile_with_header.append("")
    lines_makefile_with_header.extend(lines_makefile)
    createFile(self.makefile, lines_makefile_with_header)

  def create(self):
    """Creates all config files necessary for runing the complete analysis workfow
    """
    raise ValueError("Method 'create' not implemented in derrived class !!")

  def run(self):
    """Runs the complete analysis workfow -- either locally or on the batch system
    """
    run_cmd("make -f %s -j %i " % (self.makefile, self.num_parallel_jobs), False, self.stdout_file, self.stderr_file)
