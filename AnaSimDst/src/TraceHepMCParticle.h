#ifndef _TRACE_HEPMC_PARTICLE__H_
#define _TRACE_HEPMC_PARTICLE__H_
#include <fun4all/SubsysReco.h>
namespace HepMC {
  class GenParticle;
};
class PHHepMCGenEventMap;

/// A SubsysReco module to trace particle productions and decays using the HepMC container.
class TraceHepMCParticle: public SubsysReco {
  PHHepMCGenEventMap* m_mcgem;

 public:
  TraceHepMCParticle();
  virtual ~TraceHepMCParticle() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

 private:
  void PrintParticles();
  void PrintTraces(const int pdg_id);
  void TraceParent(const HepMC::GenParticle* par, const int depth=1);
};

#endif // _TRACE_HEPMC_PARTICLE__H_
