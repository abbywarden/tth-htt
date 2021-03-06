from collections import OrderedDict as OD

# file generated at 2020-03-02 21:31:41 with the following command:
# create_dictionary.py -m python/samples/metaDict_2018_sync.py -p /hdfs/local/karl/addMEM/2018/2019Dec15_sync_nonNom_high/final_ntuples/2lss_1tau -N samples_2018 -E 2018 -o python/samples -g tthAnalyzeSamples_2018_addMEM_2lss1tau_deepVSjVLoose_sync.py -M

samples_2018 = OD()
samples_2018["/ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"] = OD([
  ("type",                            "mc"),
  ("sample_category",                 "ttH"),
  ("process_name_specific",           "ttHToNonbb_M125_powheg"),
  ("nof_files",                       1),
  ("nof_db_files",                    224),
  ("nof_events",                      {
    'Count'                                                      : [        63000, ],
    'CountWeighted'                                              : [        61655,        61672,        61641, ],
    'CountWeightedLHEWeightScale'                                : [        89708,        87079,        85066,        63180,        61655,        60535,        51752,        50390,        49352, ],
  }),
  ("nof_tree_events",                 13715),
  ("nof_db_events",                   7525991),
  ("fsize_local",                     70691008), # 70.69MB, avg file size 70.69MB
  ("fsize_db",                        469867184231), # 469.87GB, avg file size 2.10GB
  ("use_it",                          True),
  ("xsection",                        0.2118),
  ("genWeight",                       True),
  ("triggers",                        ['1e', '1mu', '2e', '2mu', '1e1mu', '3e', '3mu', '2e1mu', '1e2mu', '1e1tau', '1mu1tau', '2tau']),
  ("has_LHE",                         True),
  ("nof_PSweights",                   1),
  ("LHE_set",                         "LHA IDs 91400 - 91432 -> PDF4LHC15_nnlo_30_pdfas PDF set, expecting 33 weights (counted 33 weights)"),
  ("nof_reweighting",                 0),
  ("local_paths",
    [
      OD([
        ("path",      "/hdfs/local/karl/addMEM/2018/2019Dec15_sync_nonNom_high/final_ntuples/2lss_1tau/ttHToNonbb_M125_powheg"),
        ("selection", "*"),
        ("blacklist", []),
      ]),
    ]
  ),
  ("missing_completely",           [
    # not computed
  ]),
  ("missing_from_superset",        [
    # not computed
  ]),
  ("missing_hlt_paths",            [

  ]),
  ("hlt_paths",                    [
    # not computed
  ]),
])

samples_2018["sum_events"] = [
]

