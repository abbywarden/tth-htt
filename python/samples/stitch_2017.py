
samples_to_stitch_2017 = [
  {
    'inclusive' : {
      'samples'   : [ 'DYJetsToLL_M-50', 'DYJetsToLL_M-50_ext1' ],
      'LHE_Njets' : [ 0, 5 ],
      'LHE_HT'    : [ 0., 100000. ],
    },
    'LHE_Njets' : [
      { 'value' : [ 1, 2 ], 'samples' : [ 'DY1JetsToLL_M-50', 'DY1JetsToLL_M-50_ext1' ] },
      { 'value' : [ 2, 3 ], 'samples' : [ 'DY2JetsToLL_M-50', 'DY2JetsToLL_M-50_ext1' ] },
      { 'value' : [ 3, 4 ], 'samples' : [ 'DY3JetsToLL_M-50', 'DY3JetsToLL_M-50_ext1' ] },
      { 'value' : [ 4, 5 ], 'samples' : [ 'DY4JetsToLL_M-50'                          ] },
    ],
    'LHE_HT'    : [
      { 'value' : [  100.,    200. ], 'samples' : [ 'DYJetsToLL_M50_HT100to200', 'DYJetsToLL_M50_HT100to200_ext1' ] },
      { 'value' : [  200.,    400. ], 'samples' : [ 'DYJetsToLL_M50_HT200to400', 'DYJetsToLL_M50_HT200to400_ext1' ] },
      { 'value' : [  400.,    600. ], 'samples' : [ 'DYJetsToLL_M50_HT400to600', 'DYJetsToLL_M50_HT400to600_ext1' ] },
      { 'value' : [  600.,    800. ], 'samples' : [ 'DYJetsToLL_M50_HT600to800'                                   ] },
      { 'value' : [  800.,   1200. ], 'samples' : [ 'DYJetsToLL_M50_HT800to1200'                                  ] },
      { 'value' : [ 1200.,   2500. ], 'samples' : [ 'DYJetsToLL_M50_HT1200to2500'                                 ] },
      { 'value' : [ 2500., 100000. ], 'samples' : [ 'DYJetsToLL_M50_HT2500toInf'                                  ] },
    ],
  },
  {
    'inclusive' : {
      'samples' : [ 'DYJetsToLL_M-10to50' ],
      'LHE_HT'  : [ 0., 100000. ]
    },
    'LHE_HT'    : [
#      { 'value' : [  70.,    100. ], 'samples' : [ 'DYJetsToLL_M-4to50_HT-70to100',  'DYJetsToLL_M-4to50_HT-70to100_ext1'  ] }, # [*]
      { 'value' : [ 100.,    200. ], 'samples' : [ 'DYJetsToLL_M-4to50_HT-100to200', 'DYJetsToLL_M-4to50_HT-100to200_ext1' ] },
      { 'value' : [ 200.,    400. ], 'samples' : [ 'DYJetsToLL_M-4to50_HT-200to400', 'DYJetsToLL_M-4to50_HT-200to400_ext1' ] },
      { 'value' : [ 400.,    600. ], 'samples' : [ 'DYJetsToLL_M-4to50_HT-400to600', 'DYJetsToLL_M-4to50_HT-400to600_ext1' ] },
      { 'value' : [ 600., 100000. ], 'samples' : [ 'DYJetsToLL_M-4to50_HT-600toInf',                                       ] },
    ],
  },
  {
    'inclusive' : {
      'samples'   : [ 'WJetsToLNu' ],
      'LHE_Njets' : [ 0, 5 ],
    },
    'LHE_Njets' : [
      { 'value' : [ 1, 2 ], 'samples' : [ 'W1JetsToLNu' ] },
      { 'value' : [ 2, 3 ], 'samples' : [ 'W2JetsToLNu' ] },
      { 'value' : [ 3, 4 ], 'samples' : [ 'W3JetsToLNu' ] },
      { 'value' : [ 4, 5 ], 'samples' : [ 'W4JetsToLNu' ] },
    ],
  },
]

# [*] not used b/c the sample was produced w/ incorrect gridpack (or, to be precise, w/ incorrect MG cards)
# see https://hypernews.cern.ch/HyperNews/CMS/get/generators/4005.html for more information