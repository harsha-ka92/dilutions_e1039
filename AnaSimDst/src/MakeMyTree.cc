#include <iomanip>
#include <TFile.h>
#include <TTree.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQSpillMap.h>
#include <interface_main/SQHitVector.h>
#include <interface_main/SQMCEvent.h>
#include <interface_main/SQTrackVector.h>
#include <interface_main/SQDimuonVector.h>
#include <ktracker/SRecEvent.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <ktracker/UtilSRawEvent.h>
#include <UtilAna/UtilDimuon.h>
#include "MakeMyTree.h"
using namespace std;

MakeMyTree::MakeMyTree()
  : SubsysReco("MakeMyTree")
  , m_file_name("my_tree.root")
  , m_tree_name("tree")
  , m_evt         (0)
  , m_sp_map      (0)
  , m_hit_vec     (0)
  , m_trig_hit_vec(0)
  , m_srec    (0)
  , m_evt_true(0)
  , m_vec_trk (0)
  , m_vec_dim (0)
  , m_file (0)
  , m_tree (0)
  , m_sraw  (0)
{
  ;
}

int MakeMyTree::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int MakeMyTree::InitRun(PHCompositeNode* topNode)
{
  m_evt = findNode::getClass<SQEvent    >(topNode, "SQEvent");
  if (!m_evt) return Fun4AllReturnCodes::ABORTEVENT;

  m_sp_map       = findNode::getClass<SQSpillMap >(topNode, "SQSpillMap");
  m_hit_vec      = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
  m_trig_hit_vec = findNode::getClass<SQHitVector>(topNode, "SQTriggerHitVector");

  m_evt_true = findNode::getClass<SQMCEvent     >(topNode, "SQMCEvent");
  m_vec_trk  = findNode::getClass<SQTrackVector >(topNode, "SQTruthTrackVector");
  m_vec_dim  = findNode::getClass<SQDimuonVector>(topNode, "SQTruthDimuonVector");
  m_srec     = findNode::getClass<SRecEvent     >(topNode, "SRecEvent");

  m_file = new TFile(m_file_name.c_str(), "RECREATE");
  m_tree = new TTree(m_tree_name.c_str(), "Created by MakeMyTree");
  m_sraw  = new SRawEvent();

  m_tree->Branch("SQEvent"    , &m_evt);
  m_tree->Branch("SRawEvent"  , &m_sraw);
  if (m_evt_true) m_tree->Branch("SQMCEvent"          , &m_evt_true);
  if (m_vec_trk ) m_tree->Branch("SQTruthTrackVector" , &m_vec_trk_true);
  if (m_vec_dim ) m_tree->Branch("SQTruthDimuonVector", &m_vec_dim_true);
  if (m_srec    ) m_tree->Branch("SRecEvent"          , &m_srec);

  return Fun4AllReturnCodes::EVENT_OK;
}

int MakeMyTree::process_event(PHCompositeNode* topNode)
{
  static unsigned int n_evt = 0;
  if    (++n_evt % 100000 == 0) cout << n_evt << endl;
  else if (n_evt %  10000 == 0) cout << " . " << flush;

  //int run_id = m_evt->get_run_id();
  int sp_id  = m_evt->get_spill_id();
  //int evt_id = m_evt->get_event_id();

  SQSpill* sp = m_sp_map ? m_sp_map->get(sp_id) : 0;

  UtilSRawEvent::SetEvent     (m_sraw, m_evt);
  UtilSRawEvent::SetSpill     (m_sraw, sp);
  UtilSRawEvent::SetHit       (m_sraw, m_hit_vec);
  UtilSRawEvent::SetTriggerHit(m_sraw, m_trig_hit_vec);

  m_vec_trk_true.clear();
  if (m_vec_trk) {
    for (unsigned int ii = 0; ii < m_vec_trk->size(); ii++) {
      SQTrack_v1* trk = dynamic_cast<SQTrack_v1*>(m_vec_trk->at(ii));
      m_vec_trk_true.push_back(*trk);
    }
  }

  m_vec_dim_true.clear();
  if (m_vec_dim) {
    for (unsigned int ii = 0; ii < m_vec_dim->size(); ii++) {
      SQDimuon_v1* dim = dynamic_cast<SQDimuon_v1*>(m_vec_dim->at(ii));
      m_vec_dim_true.push_back(*dim);
    }
  }

  m_tree->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

int MakeMyTree::End(PHCompositeNode* topNode)
{
  m_file->cd();
  m_file->Write();
  m_file->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
