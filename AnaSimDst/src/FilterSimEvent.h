#ifndef _FILTER_SIM_EVENT__H_
#define _FILTER_SIM_EVENT__H_
#include <fun4all/SubsysReco.h>
class SRecEvent;

/// An example class to filter (simulated) events.
/** 
 * By default it requires at least one reconstructed dimuon per event.
 */
class FilterSimEvent: public SubsysReco {
  SRecEvent* srec;

 public:
  FilterSimEvent() {;}
  virtual ~FilterSimEvent() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
};

#endif /* _FILTER_SIM_EVENT__H_ */
