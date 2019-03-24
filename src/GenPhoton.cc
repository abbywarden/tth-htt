#include "tthAnalysis/HiggsToTauTau/interface/GenPhoton.h" // GenPhoton, GenParticle

GenPhoton::GenPhoton(Double_t pt,
                     Double_t eta,
                     Double_t phi,
                     Double_t mass,
                     Int_t pdgId,
                     Int_t status,
                     Int_t statusFlags)
  : GenParticle(pt, eta, phi, mass, pdgId, 0, status, statusFlags)
{}

std::ostream &
operator<<(std::ostream & stream,
           const GenPhoton & genPhoton)
{
  stream << static_cast<const GenParticle &>(genPhoton);
  return stream;
}
