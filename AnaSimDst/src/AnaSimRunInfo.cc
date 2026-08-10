#include <sstream>
#include <interface_main/SQEvent.h>
#include <phhepmc/PHGenIntegral.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include "AnaSimRunInfo.h"
using namespace std;

AnaSimRunInfo::AnaSimRunInfo(const char* fn_lumi_tot, const char* fn_lumi_info, const char* fn_lumi_list)
  : SubsysReco("AnaSimRunInfo")
  , m_fn_lumi_tot (fn_lumi_tot )
  , m_fn_lumi_info(fn_lumi_info)
  , m_fn_lumi_list(fn_lumi_list)
  , lumi_inte_tot(0)
  , n_evt_gen_tot(0)
  , n_evt_pro_tot(0)
  , n_run        (0)
{
  ;
}

int AnaSimRunInfo::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * The run info cannot be parsed here.  This function is not called per DST file
 * since the run ID is constant over simulated DST files in our present framework.
 */
int AnaSimRunInfo::InitRun(PHCompositeNode* topNode)
{
  mi_evt      = findNode::getClass<SQEvent      >(topNode, "SQEvent");
  mi_gen_inte = findNode::getClass<PHGenIntegral>(topNode, "PHGenIntegral");
  if (mi_evt && mi_gen_inte) {
    if (m_fn_lumi_list.length() > 0) ofs_lumi_list.open(m_fn_lumi_list.c_str());
  } else {
    cout << "AnaSimRunInfo will do nothing since cannot find SQEvent/PHGenIntegral." << endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * This function tries to detect a new DST file by a change of run ID, but it never happens at present.
 * Instead this function also uses a decrease of event ID to assume that a new DST file starts.
 */
int AnaSimRunInfo::process_event(PHCompositeNode* topNode)
{
  if (!mi_evt || !mi_gen_inte) return Fun4AllReturnCodes::EVENT_OK;

  static int run_id_pre = -1;
  static int evt_id_pre = -1;
  int run_id = mi_evt->get_run_id();
  int evt_id = mi_evt->get_event_id();
  if (run_id != run_id_pre || evt_id < evt_id_pre) {
    double lumi_inte = mi_gen_inte->get_Integrated_Lumi();
    long   n_evt_gen = mi_gen_inte->get_N_Generator_Accepted_Event();
    long   n_evt_pro = mi_gen_inte->get_N_Processed_Event();
    if (Verbosity() > 0) {
      cout << "AnaSimRunInfo: Integrated luminosity          = " << lumi_inte << " /pb\n"
           << "               N of generator-accepted events = " << n_evt_gen << "\n"
           << "               N of processed events          = " << n_evt_pro << endl;
    }
    if (ofs_lumi_list.is_open()) ofs_lumi_list << lumi_inte << "\t" << n_evt_gen << "\t" << n_evt_pro << "\n";
    lumi_inte_tot += lumi_inte;
    n_evt_gen_tot += n_evt_gen;
    n_evt_pro_tot += n_evt_pro;
    n_run++;
  }
  run_id_pre = run_id;
  evt_id_pre = evt_id;
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaSimRunInfo::End(PHCompositeNode* topNode)
{
  if (!mi_evt || !mi_gen_inte) return Fun4AllReturnCodes::EVENT_OK;

  if (ofs_lumi_list.is_open()) ofs_lumi_list.close();

  ostringstream oss;
  oss << "Integrated luminosity          = " << lumi_inte_tot << " /pb\n"
      << "N of generator-accepted events = " << n_evt_gen_tot << "\n"
      << "N of processed events          = " << n_evt_pro_tot << "\n"
      << "N of input runs                = " << n_run         << endl;
  if (Verbosity() > 0) {
    cout << "AnaSimRunInfo::End():\n"
         << oss.str();
  }

  if (m_fn_lumi_info.length() > 0) {
    ofstream ofs(m_fn_lumi_info);
    ofs << oss.str();
    ofs.close();
  }

  ofstream ofs(m_fn_lumi_tot);
  ofs << lumi_inte_tot << "\n";
  ofs.close();

  return Fun4AllReturnCodes::EVENT_OK;
}
