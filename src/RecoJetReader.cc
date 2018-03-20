#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReader.h" // RecoJetReader

#include "tthAnalysis/HiggsToTauTau/interface/GenLeptonReader.h" // GenLeptonReader
#include "tthAnalysis/HiggsToTauTau/interface/GenHadTauReader.h" // GenHadTauReader
#include "tthAnalysis/HiggsToTauTau/interface/GenJetReader.h" // GenJetReader

#include "tthAnalysis/HiggsToTauTau/interface/cmsException.h" // cmsException()
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h" // kBtag_*
#include "tthAnalysis/HiggsToTauTau/interface/BranchAddressInitializer.h" // BranchAddressInitializer, TTree, Form()

std::map<std::string, int> RecoJetReader::numInstances_;
std::map<std::string, RecoJetReader *> RecoJetReader::instances_;

RecoJetReader::RecoJetReader(int era,
                             bool isMC,
                             bool readGenMatching)
  : RecoJetReader(era, isMC, "Jet", readGenMatching)
{}

RecoJetReader::RecoJetReader(int era,
                             bool isMC,
                             const std::string & branchName_obj,
                             bool readGenMatching)
  : era_(era)
  , isMC_(isMC)
  , max_nJets_(256)
  , branchName_num_(Form("n%s", branchName_obj.data()))
  , branchName_obj_(branchName_obj)
  , branchName_btag_(! RecoJet::useDeepCSV ? "CSVV2" : "DeepB")
  , genLeptonReader_(nullptr)
  , genHadTauReader_(nullptr)
  , genJetReader_(nullptr)
  , readGenMatching_(readGenMatching)
  , jetPt_option_(RecoJetReader::kJetPt_central)
  , read_BtagWeight_systematics_(false)
  , jet_pt_(nullptr)
  , jet_eta_(nullptr)
  , jet_phi_(nullptr)
  , jet_mass_(nullptr)
  , jet_charge_(nullptr)  
  , jet_jecUncertTotal_(nullptr)
  , jet_BtagCSV_(nullptr)
  , jet_BtagWeight_(nullptr)
  , jet_QGDiscr_(nullptr)
  , jet_pullEta_(nullptr)
  , jet_pullPhi_(nullptr)
  , jet_pullMag_(nullptr)
{
  if(readGenMatching_)
  {
    genLeptonReader_ = new GenLeptonReader(Form("%s_genLepton", branchName_obj_.data()));
    genHadTauReader_ = new GenHadTauReader(Form("%s_genTau",    branchName_obj_.data()));
    genJetReader_    = new GenJetReader   (Form("%s_genJet",    branchName_obj_.data()));
  }
  setBranchNames();
}

RecoJetReader::~RecoJetReader()
{
  --numInstances_[branchName_obj_];
  assert(numInstances_[branchName_obj_] >= 0);
  if(numInstances_[branchName_obj_] == 0)
  {
    RecoJetReader * const gInstance = instances_[branchName_obj_];
    assert(gInstance);
    delete gInstance->genLeptonReader_;
    delete gInstance->genHadTauReader_;
    delete gInstance->genJetReader_;
    delete[] gInstance->jet_pt_;
    delete[] gInstance->jet_eta_;
    delete[] gInstance->jet_phi_;
    delete[] gInstance->jet_mass_;
    delete[] gInstance->jet_charge_;
    delete[] gInstance->jet_jecUncertTotal_;
    delete[] gInstance->jet_BtagCSV_;
    delete[] gInstance->jet_BtagWeight_;
    delete[] gInstance->jet_QGDiscr_;
    delete[] gInstance->jet_pullEta_;
    delete[] gInstance->jet_pullPhi_;
    delete[] gInstance->jet_pullMag_;
    for(auto & kv: gInstance->jet_BtagWeights_systematics_)
    {
      delete[] kv.second;
    }
    instances_[branchName_obj_] = nullptr;
  }
}

void
RecoJetReader::setJetPt_central_or_shift(int jetPt_option)
{
  jetPt_option_ = jetPt_option;
}

void
RecoJetReader::setBranchName_BtagWeight(int central_or_shift)
{
  branchName_BtagWeight_ = getBranchName_bTagWeight(branchName_obj_, era_, central_or_shift);
}

void
RecoJetReader::read_BtagWeight_systematics(bool flag)
{
  read_BtagWeight_systematics_ = flag;
}

void
RecoJetReader::setBranchNames()
{
  if(numInstances_[branchName_obj_] == 0)
  {
    branchName_pt_ = Form("%s_%s", branchName_obj_.data(), "pt");
    branchName_eta_ = Form("%s_%s", branchName_obj_.data(), "eta");
    branchName_phi_ = Form("%s_%s", branchName_obj_.data(), "phi");
    branchName_mass_ = Form("%s_%s", branchName_obj_.data(), "mass");
    branchName_jetCharge_ = Form("%s_%s", branchName_obj_.data(), "jetCharge");
    branchName_jecUncertTotal_ = Form("%s_%s", branchName_obj_.data(), "jecUncertTotal");
    branchName_BtagCSV_ = Form("%s_%s", branchName_obj_.data(), Form("btag%s", branchName_btag_.data()));
    branchName_QGDiscr_ = Form("%s_%s", branchName_obj_.data(), "qgl");
    branchName_BtagWeight_ = getBranchName_bTagWeight(branchName_obj_, era_, kBtag_central);
    for(int idxShift = kBtag_hfUp; idxShift <= kBtag_jesDown; ++idxShift)
    {
      branchNames_BtagWeight_systematics_[idxShift] = getBranchName_bTagWeight(branchName_obj_, era_, idxShift);
    }
    branchName_pullEta_ = Form("%s_%s", branchName_obj_.data(), "pullEta");
    branchName_pullPhi_ = Form("%s_%s", branchName_obj_.data(), "pullPhi");
    branchName_pullMag_ = Form("%s_%s", branchName_obj_.data(), "pullMag");
    instances_[branchName_obj_] = this;
  }
  else
  {
    if(branchName_num_ != instances_[branchName_obj_]->branchName_num_)
    {
      throw cmsException(this)
        << "Association between configuration parameters 'branchName_num' and 'branchName_obj' must be unique:"
        << " present association 'branchName_num' = " << branchName_num_ << " with 'branchName_obj' = " << branchName_obj_
        << " does not match previous association 'branchName_num' = " << instances_[branchName_obj_]->branchName_num_
        << " with 'branchName_obj' = " << instances_[branchName_obj_]->branchName_obj_ << " !!\n";
    }
  }
  ++numInstances_[branchName_obj_];
}

void
RecoJetReader::setBranchAddresses(TTree * tree)
{
  if(instances_[branchName_obj_] == this)
  {
    BranchAddressInitializer bai(tree, max_nJets_);
    if(readGenMatching_)
    {
      genLeptonReader_->setBranchAddresses(tree);
      genHadTauReader_->setBranchAddresses(tree);
      genJetReader_->setBranchAddresses(tree);
    }
    bai.setBranchAddress(nJets_, branchName_num_);
    bai.setBranchAddress(jet_pt_, branchName_pt_);
    bai.setBranchAddress(jet_eta_, branchName_eta_);
    bai.setBranchAddress(jet_phi_, branchName_phi_);
    bai.setBranchAddress(jet_mass_, branchName_mass_);
    bai.setBranchAddress(jet_charge_, branchName_jetCharge_);
    bai.setBranchAddress(jet_jecUncertTotal_, isMC_ ? branchName_jecUncertTotal_ : "", 0.);
    bai.setBranchAddress(jet_BtagCSV_, branchName_BtagCSV_);
    bai.setBranchAddress(jet_BtagWeight_, isMC_ ? branchName_BtagWeight_ : "", 1.);
    if(read_BtagWeight_systematics_)
    {
      for(int idxShift = kBtag_hfUp; idxShift <= kBtag_jesDown; ++idxShift)
      {
        bai.setBranchAddress(jet_BtagWeights_systematics_[idxShift], isMC_ ? branchNames_BtagWeight_systematics_[idxShift] : "", 1.);
      }
    }
    bai.setBranchAddress(jet_QGDiscr_, branchName_QGDiscr_, 1.);
    bai.setBranchAddress(jet_pullEta_, branchName_pullEta_);
    bai.setBranchAddress(jet_pullPhi_, branchName_pullPhi_);
    bai.setBranchAddress(jet_pullMag_, branchName_pullMag_);
  }
}

std::vector<RecoJet>
RecoJetReader::read() const
{
  const RecoJetReader * const gInstance = instances_[branchName_obj_];
  assert(gInstance);

  std::vector<RecoJet> jets;
  const UInt_t nJets = gInstance->nJets_;
  if(nJets > max_nJets_)
  {
    throw cmsException(this)
      << "Number of jets stored in Ntuple = " << nJets << ", exceeds max_nJets = " << max_nJets_ << " !!\n";
  }

  if(nJets > 0)
  {
    jets.reserve(nJets);
    for(UInt_t idxJet = 0; idxJet < nJets; ++idxJet)
    {
      Float_t jet_pt = -1.;
      switch(jetPt_option_)
      {
        case RecoJetReader::kJetPt_central: jet_pt = gInstance->jet_pt_[idxJet]; break;
        case RecoJetReader::kJetPt_jecUp:   jet_pt = gInstance->jet_pt_[idxJet]*(1. + jet_jecUncertTotal_[idxJet]); break;
        case RecoJetReader::kJetPt_jecDown: jet_pt = gInstance->jet_pt_[idxJet]*(1. - jet_jecUncertTotal_[idxJet]); break;
        default: throw cmsException(this) << "Invalid JEC option: " << jetPt_option_;
      }

      jets.push_back({
        {
          jet_pt,
          gInstance->jet_eta_[idxJet],
          gInstance->jet_phi_[idxJet],
          gInstance->jet_mass_[idxJet]
        },
        gInstance->jet_charge_[idxJet],
        gInstance->jet_jecUncertTotal_[idxJet],
        gInstance->jet_BtagCSV_[idxJet],
        gInstance->jet_BtagWeight_[idxJet],
        gInstance->jet_QGDiscr_[idxJet],
        gInstance->jet_pullEta_[idxJet], 
        gInstance->jet_pullPhi_[idxJet], 
        gInstance->jet_pullMag_[idxJet], 
        static_cast<Int_t>(idxJet)
      });

      RecoJet & jet = jets.back();

      if(read_BtagWeight_systematics_)
      {
        for(int idxShift = kBtag_hfUp; idxShift <= kBtag_jesDown; ++idxShift)
        {
          if(jet_BtagWeights_systematics_.count(idxShift))
          {
            jet.BtagWeight_systematics_[idxShift] = jet_BtagWeights_systematics_.at(idxShift)[idxJet];
          }
        } // idxShift
      } // read_BtagWeight_systematics_
    } // idxJet

    readGenMatching(jets);
  } // nJets > 0
  return jets;
}

void
RecoJetReader::readGenMatching(std::vector<RecoJet> & jets) const
{
  if(readGenMatching_)
  {
    assert(genLeptonReader_ && genHadTauReader_ && genJetReader_);
    std::size_t nJets = jets.size();

    std::vector<GenLepton> matched_genLeptons = genLeptonReader_->read();
    assert(matched_genLeptons.size() == nJets);

    std::vector<GenHadTau> matched_genHadTaus = genHadTauReader_->read();
    assert(matched_genHadTaus.size() == nJets);

    std::vector<GenJet> matched_genJets = genJetReader_->read();
    assert(matched_genJets.size() == nJets);

    for(std::size_t idxJet = 0; idxJet < nJets; ++idxJet)
    {
      RecoJet & jet = jets[idxJet];

      const GenLepton & matched_genLepton = matched_genLeptons[idxJet];
      if(matched_genLepton.isValid()) jet.set_genLepton(new GenLepton(matched_genLepton));

      const GenHadTau & matched_genHadTau = matched_genHadTaus[idxJet];
      if(matched_genHadTau.isValid()) jet.set_genHadTau(new GenHadTau(matched_genHadTau));

      const GenJet & matched_genJet = matched_genJets[idxJet];
      if(matched_genJet.isValid()) jet.set_genJet(new GenJet(matched_genJet));
    }
  }
}
