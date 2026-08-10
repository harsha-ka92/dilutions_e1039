#ifndef _ANA_SIM_DST__H_
#define _ANA_SIM_DST__H_
#include <map>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <ktracker/SRecEvent.h>
#include <fun4all/SubsysReco.h>
#include "TreeData.h"
class TFile;
class TTree;
class SQEvent;
class SRecEvent;
class SQMCEvent;
class SQTrackVector;
class SQDimuonVector;

/// An example class to analyze the simulated uDST file.
class AnaSimDst: public SubsysReco {
  /// Input
  SQEvent       * mi_evt;
  SRecEvent     * mi_srec;
  SQMCEvent     * mi_evt_true;
  SQTrackVector * mi_vec_trk;
  SQDimuonVector* mi_vec_dim;

  /// Output
  TFile*     mo_file;
  TTree*     mo_tree;
  EventData  mo_evt;
  TrackList  mo_trk_true;
  TrackList  mo_trk_reco;
  DimuonList mo_dim_true;
  DimuonList mo_dim_reco;

 public:
  AnaSimDst();
  virtual ~AnaSimDst() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

 private:
  typedef std::map<int, int> IdMap_t; // For now the key is not ID but index.
  void FindTrackRelation (IdMap_t& id_map);
  void FindDimuonRelation(IdMap_t& id_map);
  int dim_origin(SRecTrack trk_pos, SRecTrack trk_neg, double z1, double z2);
};

#endif /* _ANA_SIM_DST__H_ */
