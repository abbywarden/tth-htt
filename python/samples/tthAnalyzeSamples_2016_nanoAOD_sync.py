from collections import OrderedDict as OD

# file generated at 2019-04-21 13:29:36 with the following command:
# create_dictionary.py -m python/samples/metaDict_2016_sync.py -p /hdfs/local/karl/sync_ntuples/2016/nanoAODproduction/2019Apr21 -N samples_2016 -E 2016 -o python/samples -g tthAnalyzeSamples_2016_nanoAOD_sync.py -M

samples_2016 = OD()
samples_2016["/ttHToNonbb_M125_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"] = OD([
  ("type",                            "mc"),
  ("sample_category",                 "signal"),
  ("process_name_specific",           "ttHToNonbb_M125_powheg"),
  ("nof_files",                       1),
  ("nof_db_files",                    35),
  ("nof_events",                      {
  }),
  ("nof_tree_events",                 77700),
  ("nof_db_events",                   3981250),
  ("fsize_local",                     258256493), # 258.26MB, avg file size 258.26MB
  ("fsize_db",                        208884155497), # 208.88GB, avg file size 5.97GB
  ("use_it",                          True),
  ("xsection",                        0.2118),
  ("genWeight",                       True),
  ("triggers",                        ['1e', '1mu', '2e', '2mu', '1e1mu', '3e', '3mu', '2e1mu', '1e2mu', '1e1tau', '1mu1tau', '2tau']),
  ("has_LHE",                         True),
  ("local_paths",
    [
      OD([
        ("path",      "/hdfs/local/karl/sync_ntuples/2016/nanoAODproduction/2019Apr21/ttHToNonbb_M125_powheg"),
        ("selection", "*"),
        ("blacklist", []),
      ]),
    ]
  ),
  ("missing_from_superset",           [
    # not computed
  ]),
  ("missing_hlt_paths",               [

  ]),
  ("hlt_paths",               [
    # not computed
  ]),
])

samples_2016["sum_events"] = [
]
