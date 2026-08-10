#ifndef _ANA_SIM_HIT__H_
#define _ANA_SIM_HIT__H_
#include <map>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <fun4all/SubsysReco.h>
class PHG4TruthInfoContainer;
class PHG4Particle;
class SQEvent;
class SQMCEvent;
class SQHitVector;

/// A SubsysReco module to look into simulated hits.
class AnaSimHit: public SubsysReco {
  /// Input
  SQEvent* m_evt;
  SQMCEvent* m_evt_true;
  SQHitVector* m_hit_vec;
  PHG4TruthInfoContainer* m_g4true;

 public:
  AnaSimHit();
  virtual ~AnaSimHit() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

 private:
  int GetParticleID(const int track_id);
  void PrintHits(const std::string name, SQHitVector* hv, std::ostream& os=std::cout);
  void CountHits(SQHitVector* hv, int& n_pri, int& n_sec);
};

#endif // _ANA_SIM_HIT__H_
