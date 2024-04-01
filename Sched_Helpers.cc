// File Sched_Helpers.cc
#include "Sched_Helpers.hh"

Sched_SolutionManager::Sched_SolutionManager(const Sched_Input & pin) 
  : SolutionManager<Sched_Input,Sched_Output>(pin, "SchedSolutionManager")  {} 

void Sched_SolutionManager::RandomState(Sched_Output& out) 
{
  // to be implemented
}

void Sched_SolutionManager::GreedyState(Sched_Output& out) 
{
  // insert Greedy_V6_1
}

bool Sched_SolutionManager::CheckConsistency(const Sched_Output& st) const
{ // not necessary, as state modifications are managed by the single method Assign
  return true; //TBC if necessary
}

/***************************************************************************
 * State Manager 
 ***************************************************************************/

int Sched_ProfUnavailability::ComputeCost(const Sched_Output& st) const
{
  return 0;
}

void Sched_ProfUnavailability::PrintViolations(const Sched_Output& st, ostream& os) const
{}

int Sched_MaxSubjectHoursXDay::ComputeCost(const Sched_Output& st) const
{
  return 0;
}

void Sched_MaxSubjectHoursXDay::PrintViolations(const Sched_Output& st, ostream& os) const
{}

int Sched_ProfMaxWeeklyHours::ComputeCost(const Sched_Output& st) const
{
  return 0;
}

void Sched_ProfMaxWeeklyHours::PrintViolations(const Sched_Output& st, ostream& os) const
{}

int Sched_ScheduleContiguity::ComputeCost(const Sched_Output& st) const
{
  return 0;
}

void Sched_ScheduleContiguity::PrintViolations(const Sched_Output& st, ostream& os) const
{}


/***************************************************************************
 * Sched_Change Neighborhood Explorer:
 ***************************************************************************/

Sched_Change::Sched_Change()
{
   store = -1;  //da sostituire con i nostri
   old_w = -1;
   new_w = -1;
}

bool operator==(const Sched_Change& mv1, const Sched_Change& mv2)
{
  return mv1.store == mv2.store && mv1.new_w == mv2.new_w;
}

bool operator!=(const Sched_Change& mv1, const Sched_Change& mv2)
{
  return mv1.store != mv2.store || mv1.new_w != mv2.new_w;
}

bool operator<(const Sched_Change& mv1, const Sched_Change& mv2)
{
  return (mv1.store < mv2.store)
  || (mv1.store == mv2.store && mv1.new_w < mv2.new_w);
}

istream& operator>>(istream& is, Sched_Change& mv)
{
  char ch;
  is >> mv.store >> ch >> mv.old_w >> ch >> ch >> mv.new_w;
  return is;
}

ostream& operator<<(ostream& os, const Sched_Change& mv)
{
  os << mv.store << ':' << mv.old_w << "->" << mv.new_w;
  return os;
}

void Sched_ChangeNeighborhoodExplorer::RandomMove(const Sched_Output& st, Sched_Change& mv) const
{
// to be implemented
} 

bool Sched_ChangeNeighborhoodExplorer::FeasibleMove(const Sched_Output& st, const Sched_Change& mv) const
{
  // to be implemented
  return true;
} 

void Sched_ChangeNeighborhoodExplorer::MakeMove(Sched_Output& st, const Sched_Change& mv) const
{
// to be implemented
}  

void Sched_ChangeNeighborhoodExplorer::FirstMove(const Sched_Output& st, Sched_Change& mv) const
{
// to be implemented
}

bool Sched_ChangeNeighborhoodExplorer::NextMove(const Sched_Output& st, Sched_Change& mv) const
{
  do
  {
    if (!AnyNextMove(st,mv))
      return false;
  }  
  while (!FeasibleMove(st,mv));
  
  return true;
}

bool Sched_ChangeNeighborhoodExplorer::AnyNextMove(const Sched_Output& st, Sched_Change& mv) const
{
// to be implemented
  return true;
}

int Sched_ChangeDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}