#include <iomanip>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQMCEvent.h>
#include <interface_main/SQTrackVector.h>
#include <interface_main/SQDimuonVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include "TracePHG4Particle.h"
using namespace std;

TracePHG4Particle::TracePHG4Particle()
  : SubsysReco("TracePHG4Particle")
  , m_evt      (0)
  , m_mcevt    (0)
  , m_vec_trk  (0)
  , m_vec_dim  (0)
  , m_g4true   (0)
{
  ;
}

int TracePHG4Particle::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int TracePHG4Particle::InitRun(PHCompositeNode* topNode)
{
  m_evt     = findNode::getClass<SQEvent>(topNode, "SQEvent");
  m_mcevt   = findNode::getClass<SQMCEvent>(topNode, "SQMCEvent");
  m_vec_trk = findNode::getClass<SQTrackVector>(topNode, "SQTruthTrackVector");
  m_vec_dim = findNode::getClass<SQDimuonVector>(topNode, "SQTruthDimuonVector");
  m_g4true  = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  return Fun4AllReturnCodes::EVENT_OK;
}

int TracePHG4Particle::process_event(PHCompositeNode* topNode)
{
  static unsigned int n_evt = 0;
  cout << "TracePHG4: Event " << ++n_evt << "\n";
  if (!m_g4true) return Fun4AllReturnCodes::EVENT_OK;

  if (m_mcevt) {
    cout << "  MCE " << m_mcevt->get_process_id();
    for (int ii = 0; ii < 4; ii++) {
      cout << " " << m_mcevt->get_particle_id(ii);
      //m_mcevt->get_particle_momentum(ii);
    }
    cout << " " << m_mcevt->get_weight() << endl;
  }

  //PrintPrimaryParticles();
  PrintParticles();
  PrintTraces(-13);
  PrintTraces(+13);

  if (m_vec_trk) {
    for (unsigned int ii = 0; ii < m_vec_trk->size(); ii++) {
      SQTrack* trk = m_vec_trk->at(ii);
      cout << "  SQT " << trk->get_charge() << endl;
      //trk->get_pos_vtx();
      //trk->get_mom_vtx();
    }
  }

  if (m_vec_dim) {
    for (unsigned int ii = 0; ii < m_vec_dim->size(); ii++) {
      SQDimuon* dim = m_vec_dim->at(ii);
      cout << "  SQD " << dim->get_pdg_id() << endl;
      //dim->get_pos();
      //dim->get_mom();
      //dim->get_mom_pos();
      //dim->get_mom_neg();
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int TracePHG4Particle::End(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

void TracePHG4Particle::PrintPrimaryParticles()
{
  cout << "  PHG4P Primary:";
  for (auto it = m_g4true->GetPrimaryParticleRange().first; it != m_g4true->GetPrimaryParticleRange().second; ++it) {
    PHG4Particle* par = it->second;
    int pid = par->get_pid();
    cout << " " << pid;
  }
  cout << endl;
}

void TracePHG4Particle::PrintParticles()
{
  cout << "  PHG4P All:";
  for (auto it = m_g4true->GetParticleRange().first; it != m_g4true->GetParticleRange().second; ++it) {
    PHG4Particle* par = it->second;
    int pid = par->get_pid();
    cout << " " << pid;
    //PHG4VtxPoint* vtx = m_g4true->GetVtx(vtx_id);
  }
  cout << endl;
}

void TracePHG4Particle::PrintTraces(const int pdg_id)
{
  for (auto it = m_g4true->GetParticleRange().first; it != m_g4true->GetParticleRange().second; ++it) {
    PHG4Particle* par = it->second;
    if (par->get_pid() == pdg_id) TraceParent(par);
  }
}

void TracePHG4Particle::TraceParent(const PHG4Particle* par, const int depth)
{
  if (depth == 1) cout << "  Trace ";
  int pdg_id = par->get_pid();
  int trk_id = par->get_track_id(); // ID of this particle
  //int vtx_id = par->get_vtx_id();
  int par_id = par->get_parent_id();
  //int pri_id = par->get_primary_id();
  //cout << pdg_id << ":" << trk_id << ":" << par_id;
  cout << pdg_id;
  if (par_id == 0) {
    cout << ".\n";
    return;
  }
  cout << " <= ";
  for (auto it = m_g4true->GetParticleRange().first; it != m_g4true->GetParticleRange().second; ++it) {
    PHG4Particle* par = it->second;
    if (par->get_track_id() == par_id) {
      TraceParent(par, depth + 1);
      return;
    }
  }
  cout << "[Unexpected Case]" << endl;
  exit(1);
}
