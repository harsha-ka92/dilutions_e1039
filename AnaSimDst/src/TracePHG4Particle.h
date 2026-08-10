#ifndef _TRACE_PHG4_PARTICLE__H_
#define _TRACE_PHG4_PARTICLE__H_
#include <fun4all/SubsysReco.h>
class PHG4TruthInfoContainer;
class PHG4Particle;
class SQEvent;
class SQMCEvent;
class SQTrackVector;
class SQDimuonVector;

/// A SubsysReco module to trace particle productions and decays using the PHG4 container.
class TracePHG4Particle: public SubsysReco {
  SQEvent* m_evt;
  SQMCEvent* m_mcevt;
  SQTrackVector* m_vec_trk;
  SQDimuonVector* m_vec_dim;
  PHG4TruthInfoContainer* m_g4true;

 public:
  TracePHG4Particle();
  virtual ~TracePHG4Particle() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

 private:
  void PrintPrimaryParticles();
  void PrintParticles();
  void PrintTraces(const int pdg_id);
  void TraceParent(const PHG4Particle* par, const int depth=1);
};

#endif // _TRACE_PHG4_PARTICLE__H_
