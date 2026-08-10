#include <iomanip>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include "TraceHepMCParticle.h"
using namespace std;

TraceHepMCParticle::TraceHepMCParticle()
  : SubsysReco("TraceHepMCParticle")
  , m_mcgem(0)
{
  ;
}

int TraceHepMCParticle::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int TraceHepMCParticle::InitRun(PHCompositeNode* topNode)
{
  m_mcgem = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  return Fun4AllReturnCodes::EVENT_OK;
}

int TraceHepMCParticle::process_event(PHCompositeNode* topNode)
{
  static unsigned int n_evt = 0;
  cout << "TraceHepMC: Event " << ++n_evt << "\n";
  if (!m_mcgem) return Fun4AllReturnCodes::EVENT_OK;
  PrintParticles();
  PrintTraces(-13);
  PrintTraces(+13);
  return Fun4AllReturnCodes::EVENT_OK;
}

int TraceHepMCParticle::End(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

void TraceHepMCParticle::PrintParticles()
{
  cout << "  HepMC:";
  for (PHHepMCGenEventMap::Iter iter = m_mcgem->begin(); iter != m_mcgem->end(); ++iter) {
    PHHepMCGenEvent *genevt = iter->second;
    HepMC::GenEvent *evt = genevt->getEvent();
    for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin();
         it != evt->particles_end(); it++) {
      const HepMC::GenParticle* par = *it;
      int pdg_id = par->pdg_id();
      cout << " " << pdg_id;

      //if (abs(pdg_id) == 13) TraceParent(par);
      //const HepMC::GenVertex* vtx_pro = par->production_vertex();
      //if (vtx_pro) {
      //  for (HepMC::GenVertex::particles_in_const_iterator it2 = vtx_pro->particles_in_const_begin();
      //       it2 != vtx_pro->particles_in_const_end(); it2++) {
      //    const HepMC::GenParticle* par2 = *it2;
      //    cout << " " << par2->pdg_id();
      //  }
      //}
      //cout << endl;
    }
  }
  cout << endl;
}

void TraceHepMCParticle::PrintTraces(const int pdg_id)
{
  for (PHHepMCGenEventMap::Iter iter = m_mcgem->begin(); iter != m_mcgem->end(); ++iter) {
    PHHepMCGenEvent *genevt = iter->second;
    HepMC::GenEvent *evt = genevt->getEvent();
    for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin();
         it != evt->particles_end(); it++) {
      const HepMC::GenParticle* par = *it;
      if (par->pdg_id() == pdg_id) TraceParent(par);
    }
  }
  cout << endl;
}

void TraceHepMCParticle::TraceParent(const HepMC::GenParticle* par, const int depth)
{
  if (depth == 1) cout << "  Trace ";
  cout << setw(5) << par->pdg_id();
  const HepMC::GenVertex* vtx = par->production_vertex();
  if (! vtx) {
    cout << ".\n";
    return;
  }
  cout << " <= ";
  //cout << ":" << vtx->id() << " <= ";
  bool line_1st = true;
  for (HepMC::GenVertex::particles_in_const_iterator it = vtx->particles_in_const_begin(); it != vtx->particles_in_const_end(); it++) {
    if (! line_1st) { // Fill spaces
      cout << "        "; // "  Trace "
      for (int dd = 0; dd < depth; dd++) cout << "         "; // "***** <= "
    }
    TraceParent(*it, depth + 1);
    line_1st = false;
  }
}
