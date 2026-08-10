#include <iomanip>
#include <TFile.h>
#include <TTree.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQMCEvent.h>
#include <interface_main/SQTrackVector.h>
#include <interface_main/SQDimuonVector.h>
#include <ktracker/SRecEvent.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <UtilAna/UtilDimuon.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "AnaSimDst.h"
using namespace std;

AnaSimDst::AnaSimDst() : SubsysReco("AnaSimDst")
{
  ;
}

int AnaSimDst::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimDst::InitRun(PHCompositeNode* topNode)
{
  mi_evt      = findNode::getClass<SQEvent       >(topNode, "SQEvent");
  mi_evt_true = findNode::getClass<SQMCEvent     >(topNode, "SQMCEvent");
  mi_vec_trk  = findNode::getClass<SQTrackVector >(topNode, "SQTruthTrackVector");
  mi_vec_dim  = findNode::getClass<SQDimuonVector>(topNode, "SQTruthDimuonVector");
  mi_srec     = findNode::getClass<SRecEvent     >(topNode, "SRecEvent");
  if (!mi_evt || !mi_evt_true || !mi_vec_trk || !mi_vec_dim) return Fun4AllReturnCodes::ABORTEVENT;
  if (!mi_srec) {
    cout << "The SRecEvent node cannot be found in DST and thus won't be analyzed." << endl;
  }

  mo_file = new TFile("sim_tree.root", "RECREATE");
  mo_tree = new TTree("tree", "Created by AnaSimDst");
  mo_tree->Branch("evt"     , &mo_evt);
  mo_tree->Branch("trk_true", &mo_trk_true);
  mo_tree->Branch("trk_reco", &mo_trk_reco);
  mo_tree->Branch("dim_true", &mo_dim_true);
  mo_tree->Branch("dim_reco", &mo_dim_reco);

  TH1* h1_dz  = new TH1D("h1_dz" , ";Dimuon z (cm);", 100, -700, 300);

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimDst::process_event(PHCompositeNode* topNode)
{
  static unsigned int n_evt = 0;
  if    (++n_evt % 100000 == 0) cout << n_evt << endl;
  else if (n_evt %  10000 == 0) cout << " . " << flush;

  ///
  /// Event info
  ///
  mo_evt.proc_id = mi_evt_true->get_process_id();
  for (int ii = 0; ii < 4; ii++) {
    mo_evt.par_id [ii] = mi_evt_true->get_particle_id      (ii);
    mo_evt.par_mom[ii] = mi_evt_true->get_particle_momentum(ii);
  }
  mo_evt.weight     = mi_evt_true->get_weight();
  mo_evt.trig_bits  = mi_evt->get_trigger();
  mo_evt.n_dim_true = mi_vec_dim->size();

  if (mi_srec) {
    mo_evt.rec_stat   = mi_srec->getRecStatus();
    mo_evt.n_dim_reco = mi_srec->getNDimuons();
  } else {
    mo_evt.rec_stat   = 0;
    mo_evt.n_dim_reco = 0;
  }

  ///
  /// Track info
  ///
  IdMap_t id_trk_t2r;
  if (mi_srec) FindTrackRelation(id_trk_t2r);
  mo_trk_true.clear();
  mo_trk_reco.clear();
  for (unsigned int ii = 0; ii < mi_vec_trk->size(); ii++) {
    SQTrack* trk = mi_vec_trk->at(ii);
    TrackData td;
    td.charge  = trk->get_charge();
    td.pos_vtx = trk->get_pos_vtx();
    td.mom_vtx = trk->get_mom_vtx();
    mo_trk_true.push_back(td);
  
    if (mi_srec) {
      TrackData tdr;
      if (id_trk_t2r[ii] >= 0) {
        SRecTrack* trk_reco = &mi_srec->getTrack(id_trk_t2r[ii]);
        tdr.charge  = trk_reco->getCharge();
        tdr.pos_vtx = trk_reco->getVertex();
        tdr.mom_vtx = trk_reco->getMomentumVertex();
      }
      mo_trk_reco.push_back(tdr);
    }
  }

  ///
  /// Dimuon info
  ///
  //
  bool target_cut = true;
  bool dump_cut = false;

  IdMap_t id_dim_t2r;
  if (mi_srec) FindDimuonRelation(id_dim_t2r);
  mo_dim_true.clear();
  mo_dim_reco.clear();
  for (unsigned int ii = 0; ii < mi_vec_dim->size(); ii++) {
    SQDimuon* dim = mi_vec_dim->at(ii);
    DimuonData dd;
    dd.pdg_id  = dim->get_pdg_id();
    dd.pos     = dim->get_pos();
    dd.mom     = dim->get_mom();
    dd.mom_pos = dim->get_mom_pos();
    dd.mom_neg = dim->get_mom_neg();
    UtilDimuon::CalcVar(dim, dd.mass, dd.pT, dd.x1, dd.x2, dd.xF, dd.costh, dd.phi);
    mo_dim_true.push_back(dd);

    if (mi_srec) {
      DimuonData ddr;
      if (id_dim_t2r[ii] >= 0) {
        SRecDimuon dim_reco = mi_srec->getDimuon(id_dim_t2r[ii]);
        SRecTrack trk_pos = mi_srec->getTrack(dim_reco.get_track_id_pos());
    	SRecTrack trk_neg = mi_srec->getTrack(dim_reco.get_track_id_neg());
	double z1 = trk_pos.get_pos_vtx().Z();
	double z2 = trk_neg.get_pos_vtx().Z();
	
	int origin = dim_origin(trk_pos, trk_neg, z1, z2);
	if (target_cut && origin == 0) dim_reco.calcVariables(1);
	if (target_cut && origin != 0) continue;
	
	//if (dump_cut && origin == 1) dim_reco.calcVariables(2);
        //if (dump_cut && origin != 1) continue;

	ddr.pos     = dim_reco.vtx;
	ddr.mass    = dim_reco.get_mass();
        ddr.mom_pos = dim_reco.p_pos;
        ddr.mom_neg = dim_reco.p_neg;
        ddr.mom     = dim_reco.p_pos + dim_reco.p_neg;
	/*
	if (target_cut){
		ddr.mom_pos = dim_reco.p_pos_target;
        	ddr.mom_neg = dim_reco.p_neg_target;
		ddr.mom     = dim_reco.p_pos_target + dim_reco.p_neg_target;
	}

	if (dump_cut){
                ddr.mom_pos = dim_reco.p_pos_dump;
                ddr.mom_neg = dim_reco.p_neg_dump;
                ddr.mom     = dim_reco.p_pos_dump + dim_reco.p_neg_dump;
        }
	*/
        ddr.x1      = dim_reco.x1;
        ddr.x2      = dim_reco.x2;
        ddr.chisq_pos          = trk_pos.get_chisq();
    	ddr.chisq_target_pos   = trk_pos.getChisqTarget();//get_chisq_target();
    	ddr.chisq_dump_pos     = trk_pos.get_chisq_dump();
    	ddr.chisq_upstream_pos = trk_pos.get_chsiq_upstream();
    	ddr.pos_pos            = trk_pos.get_pos_vtx();
	ddr.chisq_neg          = trk_neg.get_chisq();
    	ddr.chisq_target_neg   = trk_neg.getChisqTarget();//get_chisq_target();
    	ddr.chisq_dump_neg     = trk_neg.get_chisq_dump();
    	ddr.chisq_upstream_neg = trk_neg.get_chsiq_upstream(); // not chisq
    	ddr.pos_neg            = trk_neg.get_pos_vtx();

      }
      mo_dim_reco.push_back(ddr);
    }
  }

  mo_tree->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimDst::End(PHCompositeNode* topNode)
{
  mo_file->cd();
  mo_file->Write();
  mo_file->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

void AnaSimDst::FindTrackRelation(IdMap_t& id_map)
{
  id_map.clear();
  for (unsigned int i_true = 0; i_true < mi_vec_trk->size(); i_true++) {
    SQTrack* trk_true = mi_vec_trk->at(i_true);
    int     ch_true = trk_true->get_charge();
    double mom_true = trk_true->get_mom_vtx().Mag();

    int i_reco_best = -1;
    double mom_diff_best;
    for (int i_reco = 0; i_reco < mi_srec->getNTracks(); i_reco++) {
      SRecTrack* trk_reco = &mi_srec->getTrack(i_reco);
      if (trk_reco->getCharge() != ch_true) continue;
      double mom_diff = fabs(trk_reco->getMomentumVertex().Mag() - mom_true);
      if (i_reco_best < 0 || mom_diff < mom_diff_best) {
        i_reco_best   = i_reco;
        mom_diff_best = mom_diff;
      }
    }
    id_map[i_true] = i_reco_best;
  }
}

void AnaSimDst::FindDimuonRelation(IdMap_t& id_map)
{
  id_map.clear();
  for (unsigned int i_true = 0; i_true < mi_vec_dim->size(); i_true++) {
    SQDimuon* dim_true = mi_vec_dim->at(i_true);
    double mass_true = dim_true->get_mom().M();

    int i_reco_best = -1;
    double mass_diff_best;
    for (int i_reco = 0; i_reco < mi_srec->getNDimuons(); i_reco++) {
      SRecDimuon dim_reco = mi_srec->getDimuon(i_reco);
      double mass_diff = fabs(dim_reco.mass - mass_true);
      if (i_reco_best < 0 || mass_diff < mass_diff_best) {
        i_reco_best   = i_reco;
        mass_diff_best = mass_diff;
      }
    }
    id_map[i_true] = i_reco_best;
  }
}

int AnaSimDst::dim_origin(SRecTrack trk_pos, SRecTrack trk_neg, double z1, double z2){

        double pos_chisq_t = trk_pos.getChisqTarget();
        double pos_chisq_d = trk_pos.getChisqDump();
        double pos_chisq_us = trk_pos.get_chsiq_upstream();

        double neg_chisq_t = trk_neg.getChisqTarget();
        double neg_chisq_d = trk_neg.getChisqDump();
        double neg_chisq_us = trk_neg.get_chsiq_upstream();

        double pos_chi_td = pos_chisq_t - pos_chisq_d;
        double pos_chi_tus = pos_chisq_t - pos_chisq_us;
        double neg_chi_td = neg_chisq_t - neg_chisq_d;
        double neg_chi_tus = neg_chisq_t - neg_chisq_us;

        //
        //Target_Like:- 0
        //
        bool tgt_pos_ok = pos_chisq_t >=0 && pos_chisq_d >=0 && pos_chisq_us >=0 &&
                pos_chi_td <0 && pos_chi_tus <0;
        bool tgt_neg_ok = neg_chisq_t >=0 && neg_chisq_d >=0 && neg_chisq_us >=0 &&
                neg_chi_td <0 && neg_chi_tus <0;

        if (tgt_pos_ok && tgt_neg_ok && z1 > -690 && z2 > -690) return 0;

        //
        //Dump_Like:- 1
        //
        bool d_pos_ok = pos_chisq_t >=0. && pos_chisq_d >=0. && -(pos_chi_td)<0.;
        bool d_neg_ok = neg_chisq_t >=0. && neg_chisq_d >=0. && -(neg_chi_td)<0.;

        if (d_pos_ok && d_neg_ok) return 1;

        //
        //Upstream_Like:- 2
        //
        bool us_pos_ok = pos_chisq_t >=0. && pos_chisq_us >=0. && -(pos_chi_tus)<0.;
        bool us_neg_ok = neg_chisq_t >=0. && neg_chisq_us >=0. && -(neg_chi_tus)<0.;

        if (us_pos_ok && us_neg_ok) return 2;

        return -999;
}
