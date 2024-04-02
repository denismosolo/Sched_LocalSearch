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
  unsigned p;
  unsigned violations = 0;
  
  for (p = 0; p < in.N_Profs(); p++)
    if (st.ProfAssignedDayOff(p) == -1 || (unsigned)st.ProfAssignedDayOff(p) != in.ProfUnavailability(p))
      violations++;

  return violations * in.UnavailabilityViolationCost();
}

void Sched_ProfUnavailability::PrintViolations(const Sched_Output& st, ostream& os) const
{
  unsigned p;
  
  for (p = 0; p < in.N_Profs(); p++)
    if (st.ProfAssignedDayOff(p) == -1 || (unsigned)st.ProfAssignedDayOff(p) != in.ProfUnavailability(p))
      os << "To prof " << in.Prof_Name(p) << " assigned day off on " << st.ProfAssignedDayOff(p)
        << " instead of " << in.ProfUnavailability(p) 
        << endl;
}

int Sched_MaxSubjectHoursXDay::ComputeCost(const Sched_Output& st) const
{
  unsigned c, s, d;
  unsigned violations = 0;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (s = 0; s < in.N_Subjects(); s++)
        if (st.DailySubjectAssignedHours(c, d, s) > in.SubjectMaxHoursXDay())
          violations += st.DailySubjectAssignedHours(c, d, s) - in.SubjectMaxHoursXDay();

  return violations * in.MaxSubjectHoursXDayViolationCost();
}

void Sched_MaxSubjectHoursXDay::PrintViolations(const Sched_Output& st, ostream& os) const
{
  unsigned c, s, d;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (s = 0; s < in.N_Subjects(); s++)
        if (st.DailySubjectAssignedHours(c, d, s) > in.SubjectMaxHoursXDay())
          os << "To subject " << in.Subject_Name(s) << " in class " << in.Class_Name(c)
            << " assigned " << st.DailySubjectAssignedHours(c, d, s) << " instead of " << in.SubjectMaxHoursXDay()
            << " on " << days(d)
            << endl;
}

int Sched_ProfMaxWeeklyHours::ComputeCost(const Sched_Output& st) const
{
  unsigned p;
  unsigned violations = 0;

  for (p = 0; p < in.N_Profs(); p++)
    if (st.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      violations += st.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours();

  return violations * in.MaxProfWeeklyHoursViolationCost();
}

void Sched_ProfMaxWeeklyHours::PrintViolations(const Sched_Output& st, ostream& os) const
{
  unsigned p;

  for (p = 0; p < in.N_Profs(); p++)
    if (st.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      os << "To prof " << in.Prof_Name(p) << " assigned "
        << st.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours() << " extra hours"
        << endl;
}

int Sched_ScheduleContiguity::ComputeCost(const Sched_Output& st) const
{
  unsigned c, s, d, h;
  int subject_last_position;
  unsigned violations = 0;

  if (in.N_HoursXDay() < 3)
    return 0;

  for(c = 0; c < in.N_Classes(); c++)
    for (s = 0; s < in.N_Subjects(); s++)
      for (d = 0; d < in.N_Days(); d++)
      {
        subject_last_position = -1;

        for (h = 0; h < in.N_HoursXDay(); h++)
          if (st.Class_Schedule(c, d, h) != -1 && (unsigned)st.Class_Schedule(c, d, h) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              violations++;

            subject_last_position = h;
          }
      }

  return violations * in.ScheduleContiguityViolationCost();
}

void Sched_ScheduleContiguity::PrintViolations(const Sched_Output& st, ostream& os) const
{
  unsigned c, s, d, h;
  int subject_last_position;

  if (in.N_HoursXDay() < 3)
    return;

  for(c = 0; c < in.N_Classes(); c++)
    for (s = 0; s < in.N_Subjects(); s++)
      for (d = 0; d < in.N_Days(); d++)
      {
        subject_last_position = -1;

        for (h = 0; h < in.N_HoursXDay(); h++)
          if (st.Class_Schedule(c, d, h) != -1 && (unsigned)st.Class_Schedule(c, d, h) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              os << "Contiguity violation in class " << in.Class_Name(c)
                << " for subject " << in.Subject_Name(s) << " on " << days(d)
                << " between hours " << h+1 << " - " << subject_last_position+1
                << endl;

            subject_last_position = h;
          }
      }
}


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
  // to be implemented
  //return mv1.store == mv2.store && mv1.new_w == mv2.new_w;
  return true;
}

bool operator!=(const Sched_Change& mv1, const Sched_Change& mv2)
{
  // to be implemented
  //return mv1.store != mv2.store || mv1.new_w != mv2.new_w;
  return true;
}

bool operator<(const Sched_Change& mv1, const Sched_Change& mv2)
{
  // to be implemented
  //return (mv1.store < mv2.store) || (mv1.store == mv2.store && mv1.new_w < mv2.new_w);
  return true;
}

istream& operator>>(istream& is, Sched_Change& mv)
{
  // to be implemented
  //char ch;
  //is >> mv.store >> ch >> mv.old_w >> ch >> ch >> mv.new_w;
  return is;
}

ostream& operator<<(ostream& os, const Sched_Change& mv)
{
  //to be implemented
  //os << mv.store << ':' << mv.old_w << "->" << mv.new_w;
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
  // to be implemented
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