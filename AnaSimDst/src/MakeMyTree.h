#ifndef _MAKE_MY_TREE__H_
#define _MAKE_MY_TREE__H_
#include <fun4all/SubsysReco.h>
#include <interface_main/SQTrack_v1.h>
#include <interface_main/SQDimuon_v1.h>
class TFile;
class TTree;
class SQEvent;
class SQSpillMap;
class SQHitVector;
class SRecEvent;
class SQMCEvent;
class SQTrackVector;
class SQDimuonVector;
class SRawEvent;
typedef std::vector<SQTrack_v1> SQTrackVec;
typedef std::vector<SQDimuon_v1> SQDimuonVec;

/// An example class to output a tree file in your own data structure.
/**
 * The output tree structure, i.e. the type of each branch variable, has been selected
 * so that the output file can be parsed without the E1039 software.
 */
class MakeMyTree: public SubsysReco {
  std::string m_file_name;
  std::string m_tree_name;

  /// Input
  SQEvent       * m_evt;
  SQSpillMap    * m_sp_map;
  SQHitVector   * m_hit_vec;
  SQHitVector   * m_trig_hit_vec;
  SRecEvent     * m_srec;
  SQMCEvent     * m_evt_true;
  SQTrackVector * m_vec_trk;
  SQDimuonVector* m_vec_dim;

  /// Output
  TFile*     m_file;
  TTree*     m_tree;
  SRawEvent* m_sraw;
  SQTrackVec  m_vec_trk_true;
  SQDimuonVec m_vec_dim_true;

 public:
  MakeMyTree();
  virtual ~MakeMyTree() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void SetFileName(const std::string name) { m_file_name = name; }
  void SetTreeName(const std::string name) { m_tree_name = name; }
};

#endif /* _MAKE_MY_TREE__H_ */
