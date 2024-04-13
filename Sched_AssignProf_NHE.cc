// File Sched_AssignProf_NHE.cc
#include "Sched_Helpers.hh"

namespace
{
  // RandomMove and AnyNextMovehelp function
  vector<unsigned> GetAvailableProfs(const Sched_Input& in, const Sched_Output& out, const int c)
  {
    unsigned s, p;
    vector<unsigned> available_profs;
    
    for (s = 0; s < in.N_Subjects(); s++)
    {
      if (out.WeeklySubjectResidualHours(c, s) != 0)
        available_profs.push_back(out.Subject_Prof(c, s));
      
      if (out.WeeklySubjectAssignedHours(c, s) == 0)
          for (p = 0; p < in.N_ProfsXSubject(s); p++)
            available_profs.push_back(in.SubjectProf(s, p));
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
  char ch;  //se è vuoto funziona come spazio?
  is >> mv._class >> ch >> ch >> ch >> mv.day >> ch >> ch >> mv.hour >> ch >> ch >> ch >> ch >> ch >> mv.prof;
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
  vector<unsigned> available_profs;

  do
  {
    mv._class = Random::Uniform<int>(0, in.N_Classes()-1);

    // Get all profs of the class with not all hours already assigned
    available_profs.clear();
    available_profs = GetAvailableProfs(in, out, mv._class);

    mv.day = Random::Uniform<int>(0, in.N_Days()-1);
    mv.hour = Random::Uniform<int>(0, in.N_HoursXDay()-1);
    if (available_profs.size() > 0)
      mv.prof = available_profs[Random::Uniform<int>(0, available_profs.size()-1)];
    else
      mv.prof = -1;
  } while (!FeasibleMove(out, mv));
  
} 

bool Sched_AssignProf_NeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  // If both Class and Prof are free  devo accettare il prof = -1 altrimenti RandomMove potrebbe trovarsi in un ciclo infinito se non esiste una mossa, tanto lo escludo il make move
  return mv.prof == -1 || (out.IsClassHourFree(mv._class, mv.day, mv.hour) && out.IsProfHourFree(mv.prof, mv.day, mv.hour));
} 

void Sched_AssignProf_NeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_AssignProf& mv) const
{
  if ( mv.prof != -1) //per risolvere il problema che potrebbe non esistere una mossa
    out.AssignHour(mv._class, mv.day, mv.hour, mv.prof);
}  

void Sched_AssignProf_NeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_AssignProf& mv) const
{
  vector<unsigned> available_profs;

  mv._class = 0;

  available_profs = GetAvailableProfs(in, out, mv._class);

  mv.day = 0;
  mv.hour = 0;
  mv.index = 0;
  if (available_profs.size() > 0)
    mv.prof = available_profs[mv.index];
  else
    mv.prof = -1;
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
  {
    mv.index++;
    mv.prof = available_profs[mv.index];
  }
  else
  {
    // Move to next hour
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