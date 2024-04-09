// File Sched_SwapHours_NHE.cc
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

Sched_SwapHours::Sched_SwapHours()
{
  _class = -1;
  new_day = -1;
  new_hour = -1;
  new_prof = -1;
  new_free = false;
  old_day = -1;
  old_hour = -1;
  old_prof = -1;
  old_free = false;
  index = -1;
}

bool operator==(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  return mv1._class == mv2._class && mv1.new_day == mv2.new_day && mv1.new_hour == mv2.new_hour && mv1.old_day == mv2.old_day && mv1.old_hour == mv2.old_hour;
}

bool operator!=(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  return mv1._class != mv2._class || mv1.new_day != mv2.new_day || mv1.new_hour != mv2.new_hour || mv1.old_day != mv2.old_day || mv1.old_hour != mv2.old_hour;
}

bool operator<(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  if (mv1._class != mv2._class)
    return mv1._class < mv2._class;
  else if (mv1.old_day != mv2.old_day)
    return mv1.old_day < mv2.old_day;
  else if (mv1.old_hour != mv2.old_hour)
    return mv1.old_hour < mv2.old_hour;
  else if (mv1.new_day != mv2.new_day)
    return mv1.new_day < mv2.new_day;
  else
    return mv1.new_hour < mv2.new_hour;
}

istream& operator>>(istream& is, Sched_SwapHours& mv)
{
  char ch;
  is >> mv._class >> ch >> ch >> ch >> mv.old_day >> ch >> ch >> mv.old_hour >> ch >> ch >> ch >> ch >> ch >> ch >> mv.new_day >> ch >> ch >> mv.new_hour >> ch;
  return is;
}

ostream& operator<<(ostream& os, const Sched_SwapHours& mv)
{
  os << mv._class << ": (" << mv.old_day << ", " << mv.old_hour << ") -> (" << mv.new_day << ", " << mv.old_hour << ")";
  return os;
}

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer Code
 ***************************************************************************/

void Sched_SwapHoursNeighborhoodExplorer::RandomMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  vector<unsigned> available_profs;

  do
  {
    mv._class = Random::Uniform<int>(0, in.N_Classes()-1);

    // Get all profs of the class with not all hours already assigned
    available_profs = GetAvailableProfs(in, out, mv._class);

    mv.old_day = Random::Uniform<int>(0, in.N_Days()-1);
    mv.old_hour = Random::Uniform<int>(0, in.N_HoursXDay()-1);
    if (out.IsClassHourFree(mv._class, mv.old_day, mv.old_hour)) // è un'ora buca
      mv.old_prof = Random::Uniform<int>(0, available_profs.size()-1);
    else
      mv.old_prof = out.Class_Schedule(mv._class, mv.old_day, mv.old_hour);

    mv.new_day = Random::Uniform<int>(0, in.N_Days()-1);
    mv.new_hour = Random::Uniform<int>(0, in.N_HoursXDay()-1);
    if (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour))
      mv.new_prof = Random::Uniform<int>(0, available_profs.size()-1);
    else
      mv.new_prof = out.Class_Schedule(mv._class, mv.new_day, mv.new_hour);
  } while (!FeasibleMove(out, mv));
  
} 

bool Sched_SwapHoursNeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  // If the prof is the same (either the case that both are -1)
  if (mv.old_prof == mv.new_prof)
    return false;
  else
  {
    // If there is only one class the move is always feasible
    if (in.N_Classes() == 1)
      return mv.old_day != mv.new_day || mv.old_hour != mv.new_hour;

    // If at least one of the profs has their "new" hour already assigned
    if ((mv.old_prof != -1 && !out.IsProfHourFree(mv.old_prof, mv.new_day, mv.new_hour)) || (mv.new_prof != -1 && !out.IsProfHourFree(mv.new_prof, mv.old_day, mv.old_hour)))
      return false;
    else
      return mv.old_day != mv.new_day || mv.old_hour != mv.new_hour;
  }
} 

void Sched_SwapHoursNeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_SwapHours& mv) const
{
  out.SwapHours(mv._class, mv.old_day, mv.old_hour, mv._class, mv.new_day, mv.new_hour); //troppi check, sarebbero da rimuovere perché il codice deve essere efficiente non robusto -- valutare
}  

void Sched_SwapHoursNeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  vector<unsigned> available_profs;

  mv._class = 0;

  available_profs = GetAvailableProfs(in, out, mv._class);

  mv.old_day = 0;
  mv.old_hour = 0;
  if (out.IsClassHourFree(mv._class, mv.old_day, mv.old_hour))
  {
    mv.old_free = true;
    mv.old_prof = available_profs[0];
    mv.index++;
  }
  else
    mv.old_prof = out.Class_Schedule(mv._class, mv.old_day, mv.old_hour);

  mv.new_day = 1;
  mv.new_hour = 1;
  if (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour))
  {
    mv.new_free = true;
    mv.new_prof = available_profs[0];
    mv.index++;
  }
  else
    mv.new_prof = out.Class_Schedule(mv._class, mv.new_day, mv.new_hour);
}

bool Sched_SwapHoursNeighborhoodExplorer::NextMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  do
  {
    if (!AnyNextMove(out,mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_SwapHoursNeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  vector<unsigned> available_profs;

  available_profs = GetAvailableProfs(in, out, mv._class);

  if ((mv.new_free ^ mv.old_free) && mv.index < available_profs.size() && (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour) ^ out.IsClassHourFree(mv._class, mv.old_day, mv.old_hour)))  // NOTE: ^ is XOR in C++
  {
    if (mv.index == available_profs.size() - 1)
    {
      mv.index++;
      if (mv.new_free)
        mv.new_prof = -1;
      else
        mv.old_prof = -1;
    }
    else if (mv.new_free)
      mv.new_prof = available_profs[++mv.index];
    else
      mv.old_prof = available_profs[++mv.index];
  }
  else
  {
    // Move to next hour
    mv.new_hour++;

    mv.index = -1;
    mv.new_free = false;
    mv.old_free = false;
    
    if (mv.new_hour == in.N_HoursXDay())
    {
      mv.new_day++;
      mv.new_hour = 0;

      if (mv.new_day == in.N_Days())
      {
        mv.old_hour++;
        mv.new_day = mv.old_day;
        mv.new_hour = mv.old_hour + 1; // Il confronto con le ore precedenti l'ho già eseguito

        if (mv.old_hour == in.N_HoursXDay() - 1) // Il confronto con l'ultima ora l'ho già eseguito
        {
          mv.old_day++;
          mv.old_hour = 0;
          mv.new_day = mv.old_day;
          mv.new_hour = 1;

          if (mv.old_day == in.N_Days() - 1) // Il confronto con l'ultimo giorno l'ho già eseguito
          {
            mv._class++;
            mv.old_day = 0;
            mv.old_hour = 0;
            mv.new_day = 0;
            mv.new_hour = 1;

            if (mv._class == in.N_Classes())
              return false;

            available_profs.clear();
            available_profs = GetAvailableProfs(in, out, mv._class);
          };
        }
        // se arrivo qui ho sicuro cambiato old quindi devo ricalcolarmi il professore
        if (out.IsClassHourFree(mv._class, mv.old_day, mv.old_hour))
        {
          mv.index++;
          mv.old_prof = available_profs[mv.index];
          mv.old_free = true;
        }
        else
          mv.old_prof = out.Class_Schedule(mv._class, mv.old_day, mv.old_hour);
      }
    }
    if (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour))
    {
      mv.index++;
      mv.new_prof = available_profs[mv.index];
      mv.new_free = true;
    }
    else
      mv.new_prof = out.Class_Schedule(mv._class, mv.new_day, mv.new_hour);
  }

  return true;
}