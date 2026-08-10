#ifndef _ANA_SIM_RUN_INFO__H_
#define _ANA_SIM_RUN_INFO__H_
#include <fstream>
#include <fun4all/SubsysReco.h>
class SQEvent;
class PHGenIntegral;

/// A SubsysReco module to analyze the simulated run (=DST) info.
/**
 * This module adds up the integrated luminosities of all DST files and prints them out at the end.
 */
class AnaSimRunInfo: public SubsysReco {
  std::string m_fn_lumi_tot;
  std::string m_fn_lumi_info;
  std::string m_fn_lumi_list;
  double lumi_inte_tot;
  long   n_evt_gen_tot;
  long   n_evt_pro_tot;
  long   n_run;

  SQEvent      * mi_evt;
  PHGenIntegral* mi_gen_inte;

  std::ofstream ofs_lumi_list;

 public:
  AnaSimRunInfo(const char* fn_lumi_tot="lumi_tot.txt", const char* fn_lumi_info="lumi_info.txt", const char* fn_lumi_list="lumi_list.txt");
  virtual ~AnaSimRunInfo() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
};

#endif // _ANA_SIM_RUN_INFO__H_
