// File Sched_AssignProf_NHE.cc
#include "Sched_Headers.hh"

namespace
{
  // Help function: for the selected class gets the professor of the subjects not completly assigned
  //                if a subject doesn't have assigned hours gets all the profs teaching that subject
  vector<unsigned> GetAvailableProfs(const Sched_Input& in, const Sched_Output& out, const int c)
  {
    unsigned s, p;
    vector<unsigned> available_profs;

    available_profs.clear();
    
    for (s = 0; s < in.N_Subjects(); s++)
    {
      if (out.WeeklySubjectResidualHours(c, s) > 0)
      {
        if (out.WeeklySubjectAssignedHours(c, s) > 0)
          available_profs.push_back(out.Subject_Prof(c, s));
        else
          for (p = 0; p < in.N_ProfsXSubject(s); p++)
            available_profs.push_back(in.SubjectProf(s, p));
      }
    }
    return available_profs;
  }
}

/***************************************************************************
 * Moves Code
 ***************************************************************************/

Sched_AssignProf::Sched_AssignProf()
{
  _class = -1;
  day = -1;
  hour = -1;
  prof = -1;
  index = -1;
}

bool operator==(const Sched_AssignProf& mv1, const Sched_AssignProf& mv2)
{
  return mv1._class == mv2._class && mv1.day == mv2.day && mv1.hour == mv2.hour;
}

bool operator!=(const Sched_AssignProf& mv1, const Sched_AssignProf& mv2)
{
  return mv1._class != mv2._class || mv1.day != mv2.day || mv1.hour != mv2.hour;
}

bool operator<(const Sched_AssignProf& mv1, const Sched_AssignProf& mv2)
{
  if (mv1._class != mv2._class)
    return mv1._class < mv2._class;
  else if (mv1.day != mv2.day)
    return mv1.day < mv2.day;
  else 
    return mv1.hour < mv2.hour;
}

istream& operator>>(istream& is, Sched_AssignProf& mv)
{
  char ch;
  is >> mv._class >> ch >> ch >> mv.day >> ch >> mv.hour >> ch >> ch >> ch >> mv.prof;
  return is;
}

ostream& operator<<(ostream& os, const Sched_AssignProf& mv)
{
  os << mv._class << ": (" << mv.day << ", " << mv.hour << ") -> " << mv.prof;
  return os;
}

/***************************************************************************
 * Sched_AssignProf Neighborhood Explorer Code
 ***************************************************************************/

void Sched_AssignProf_NeighborhoodExplorer::RandomMove(const Sched_Output& out, Sched_AssignProf& mv) const
{
  unsigned c;
  vector<unsigned> available_profs;
  vector<unsigned> class_with_moves;
  unsigned max_iterations = 1000000;
  unsigned iterations = 0;

  for (c = 0; c < in.N_Classes(); c++)
    if (GetAvailableProfs(in, out, c).size() > 0)
      class_with_moves.push_back(c);

  if (class_with_moves.size() == 0)
    throw EmptyNeighborhood();

  do
  {
    mv._class = class_with_moves[Random::Uniform<int>(0, class_with_moves.size()-1)];

    // Get all profs of the class with not all hours already assigned
    available_profs.clear();
    available_profs = GetAvailableProfs(in, out, mv._class);

    mv.day = Random::Uniform<int>(0, in.N_Days()-1);
    mv.hour = Random::Uniform<int>(0, in.N_HoursXDay()-1);
    mv.prof = available_profs[Random::Uniform<int>(0, available_profs.size()-1)];

    iterations++;
    if (iterations > max_iterations)
      throw EmptyNeighborhood();

  } while (!FeasibleMove(out, mv));
} 

bool Sched_AssignProf_NeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  // If both Class and Prof are free
  return out.IsClassHourFree(mv._class, mv.day, mv.hour) && out.IsProfHourFree(mv.prof, mv.day, mv.hour);
} 

void Sched_AssignProf_NeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_AssignProf& mv) const
{
  out.AssignHour(mv._class, mv.day, mv.hour, mv.prof);
}

void Sched_AssignProf_NeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_AssignProf& mv) const
{
  unsigned c;
  vector<unsigned> available_profs;

  for (c = 0; c < in.N_Classes(); c++)
    if (GetAvailableProfs(in, out, c).size() > 0)
      break;

  if (c == in.N_Classes())  // There are no moves
    throw EmptyNeighborhood();
  
  mv._class = c;

  available_profs = GetAvailableProfs(in, out, mv._class);

  mv.day = 0;
  mv.hour = 0;
  mv.index = 0;
  mv.prof = available_profs[mv.index];

  while (!FeasibleMove(out, mv))
    if (!AnyNextMove(out, mv))
      throw EmptyNeighborhood();
} 

bool Sched_AssignProf_NeighborhoodExplorer::NextMove(const Sched_Output& out, Sched_AssignProf& mv) const
{
  do
  {
    if (!AnyNextMove(out,mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_AssignProf_NeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_AssignProf& mv) const
{
  vector<unsigned> available_profs;

  available_profs = GetAvailableProfs(in, out, mv._class);

  while (available_profs.size() == 0)
  {
    mv._class++;

    if (mv._class == in.N_Classes())
      return false;

    mv.index = -1;
    available_profs.clear();
    available_profs = GetAvailableProfs(in, out, mv._class);
  }

  if (mv.index < available_profs.size() - 1)
    mv.index++;
  else
  {
    mv.hour++;
    mv.index = 0;

    if (mv.hour == in.N_HoursXDay())
    {
      mv.day++;
      mv.hour = 0;

      if (mv.day == in.N_Days())
      {
        do
        {
          mv._class++;

          if (mv._class == in.N_Classes())
            return false;
        
          available_profs.clear();
          available_profs = GetAvailableProfs(in, out, mv._class);
        } while (available_profs.size() == 0);

        mv.day = 0;
      }
    }
  }

  mv.prof = available_profs[mv.index];

  return true;
}