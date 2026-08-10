#include <iomanip>
#include <memory>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQMCEvent.h>
#include <interface_main/SQHitVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <UtilAna/UtilSQHit.h>
#include "AnaSimHit.h"
using namespace std;

AnaSimHit::AnaSimHit()
  : SubsysReco("AnaSimHit")
  , m_evt     (0)
  , m_evt_true(0)
  , m_hit_vec (0)
  , m_g4true  (0)
{
  ;
}

int AnaSimHit::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimHit::InitRun(PHCompositeNode* topNode)
{
  //m_evt      = findNode::getClass<SQEvent  >(topNode, "SQEvent");
  //m_evt_true = findNode::getClass<SQMCEvent>(topNode, "SQMCEvent");
  m_hit_vec = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
  m_g4true  = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimHit::process_event(PHCompositeNode* topNode)
{
  static unsigned int n_evt = 0;
  cout << "AnaSimHit: Event " << ++n_evt << "\n";
  if (!m_hit_vec || !m_g4true) return Fun4AllReturnCodes::EVENT_OK;

  shared_ptr<SQHitVector> hv_h1t(UtilSQHit::FindHits(m_hit_vec, "H1T"));
  shared_ptr<SQHitVector> hv_h1b(UtilSQHit::FindHits(m_hit_vec, "H1B"));
  shared_ptr<SQHitVector> hv_h2t(UtilSQHit::FindHits(m_hit_vec, "H2T"));
  shared_ptr<SQHitVector> hv_h2b(UtilSQHit::FindHits(m_hit_vec, "H2B"));
  shared_ptr<SQHitVector> hv_h3t(UtilSQHit::FindHits(m_hit_vec, "H3T"));
  shared_ptr<SQHitVector> hv_h3b(UtilSQHit::FindHits(m_hit_vec, "H3B"));
  shared_ptr<SQHitVector> hv_h4t(UtilSQHit::FindHits(m_hit_vec, "H4T"));
  shared_ptr<SQHitVector> hv_h4b(UtilSQHit::FindHits(m_hit_vec, "H4B"));
  PrintHits("H1T", hv_h1t.get());
  PrintHits("H1B", hv_h1b.get());
  PrintHits("H2T", hv_h2t.get());
  PrintHits("H2B", hv_h2b.get());
  PrintHits("H3T", hv_h3t.get());
  PrintHits("H3B", hv_h3b.get());
  PrintHits("H4T", hv_h4t.get());
  PrintHits("H4B", hv_h4b.get());

  int n_h1_pri = 0;
  int n_h1_sec = 0;
  CountHits(hv_h1t.get(), n_h1_pri, n_h1_sec);
  CountHits(hv_h1b.get(), n_h1_pri, n_h1_sec);
  int n_h2_pri = 0;
  int n_h2_sec = 0;
  CountHits(hv_h2t.get(), n_h2_pri, n_h2_sec);
  CountHits(hv_h2b.get(), n_h2_pri, n_h2_sec);
  int n_h3_pri = 0;
  int n_h3_sec = 0;
  CountHits(hv_h3t.get(), n_h3_pri, n_h3_sec);
  CountHits(hv_h3b.get(), n_h3_pri, n_h3_sec);
  int n_h4_pri = 0;
  int n_h4_sec = 0;
  CountHits(hv_h4t.get(), n_h4_pri, n_h4_sec);
  CountHits(hv_h4b.get(), n_h4_pri, n_h4_sec);
  cout << "  Primary  : " << n_h1_pri << " " << n_h2_pri << " " << n_h3_pri << " " << n_h4_pri << "\n"
       << "  Secondary: " << n_h1_sec << " " << n_h2_sec << " " << n_h3_sec << " " << n_h4_sec << "\n";

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimHit::End(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimHit::GetParticleID(const int track_id)
{
  for (auto it = m_g4true->GetParticleRange().first; it != m_g4true->GetParticleRange().second; ++it) {
    PHG4Particle* par = it->second;
    if (par->get_track_id() == track_id) return par->get_pid();
  }
  cout << "!!ERROR!!  AnaSimHit::GetParticleID():  Failed for track_id = " << track_id << ".  Abort.\n";
  exit(1);
}

void AnaSimHit::PrintHits(const std::string name, SQHitVector* hv, std::ostream& os)
{
  os << "  " << name;
  for (SQHitVector::ConstIter it = hv->begin(); it != hv->end(); it++) {
    int trk_id = (*it)->get_track_id();
    os << " " << trk_id << ":" << GetParticleID(trk_id);
  }
  os << endl;
}

void AnaSimHit::CountHits(SQHitVector* hv, int& n_pri, int& n_sec)
{
  for (SQHitVector::ConstIter it = hv->begin(); it != hv->end(); it++) {
    if ((*it)->get_track_id() >= 0) n_pri++;
    else                            n_sec++;
  }
}
