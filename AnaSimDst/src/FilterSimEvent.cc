#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <ktracker/SRecEvent.h>
#include "FilterSimEvent.h"
using namespace std;

int FilterSimEvent::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int FilterSimEvent::InitRun(PHCompositeNode* topNode)
{
  srec = findNode::getClass<SRecEvent>(topNode, "SRecEvent");
  if (!srec) return Fun4AllReturnCodes::ABORTEVENT;
  return Fun4AllReturnCodes::EVENT_OK;
}

int FilterSimEvent::process_event(PHCompositeNode* topNode)
{
  if (srec->getNDimuons() == 0) return Fun4AllReturnCodes::ABORTEVENT;
  return Fun4AllReturnCodes::EVENT_OK;
}

int FilterSimEvent::End(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}
