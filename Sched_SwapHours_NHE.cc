// File Sched_SwapHours_NHE.cc
#include "Sched_Helpers.hh"

/***************************************************************************
 * Moves Code
 ***************************************************************************/

Sched_SwapHours::Sched_SwapHours()
{
  _class = -1;
  day_1 = -1;
  hour_1 = -1;
  day_2 = -1;
  hour_2 = -1;
  first_move = false;
}

bool operator==(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  return mv1._class == mv2._class && mv1.day_2 == mv2.day_2 && mv1.hour_2 == mv2.hour_2 && mv1.day_1 == mv2.day_1 && mv1.hour_1 == mv2.hour_1;
}

bool operator!=(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  return mv1._class != mv2._class || mv1.day_2 != mv2.day_2 || mv1.hour_2 != mv2.hour_2 || mv1.day_1 != mv2.day_1 || mv1.hour_1 != mv2.hour_1;
}

bool operator<(const Sched_SwapHours& mv1, const Sched_SwapHours& mv2)
{
  if (mv1._class != mv2._class)
    return mv1._class < mv2._class;
  else if (mv1.day_1 != mv2.day_1)
    return mv1.day_1 < mv2.day_1;
  else if (mv1.hour_1 != mv2.hour_1)
    return mv1.hour_1 < mv2.hour_1;
  else if (mv1.day_2 != mv2.day_2)
    return mv1.day_2 < mv2.day_2;
  else
    return mv1.hour_2 < mv2.hour_2;
}

istream& operator>>(istream& is, Sched_SwapHours& mv)
{
  char ch;
  is >> mv._class >> ch >> ch >> mv.day_1 >> ch >> mv.hour_1 >> ch >> ch >> ch >> ch >> ch >> mv.day_2 >> ch >> mv.hour_2 >> ch;
  return is;
}

ostream& operator<<(ostream& os, const Sched_SwapHours& mv)
{
  os << mv._class << ": (" << mv.day_1 << ", " << mv.hour_1 << ") <-> (" << mv.day_2 << ", " << mv.hour_2 << ")";
  return os;
}

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer Code
 ***************************************************************************/

void Sched_SwapHours_NeighborhoodExplorer::RandomMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  do
  {
    mv._class = Random::Uniform<int>(0, in.N_Classes()-1);

    mv.day_1 = Random::Uniform<int>(0, in.N_Days()-1);
    mv.hour_1 = Random::Uniform<int>(0, in.N_HoursXDay()-1);

    mv.day_2 = Random::Uniform<int>(0, in.N_Days()-1);
    mv.hour_2 = Random::Uniform<int>(0, in.N_HoursXDay()-1);

  } while (!FeasibleMove(out, mv));
  
} 

bool Sched_SwapHours_NeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  int prof_1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  int prof_2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  // If the prof is the same (either the case that both are -1)
  if (prof_1 == prof_2)
    return false;

    // If there is only one class the move is always feasible
  if (in.N_Classes() == 1)
    return true;

  // If at least one of the profs has their "new" hour already assigned
  if ((prof_1 != -1 && !out.IsProfHourFree(prof_1, mv.day_2, mv.hour_2)) || (prof_2 != -1 && !out.IsProfHourFree(prof_2, mv.day_1, mv.hour_1)))
    return false;

  return mv.day_1 != mv.day_2 || mv.hour_1 != mv.hour_2;
} 

void Sched_SwapHours_NeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_SwapHours& mv) const
{
  if ((mv.first_move && FeasibleMove(out, mv)) || !mv.first_move)
    out.SwapHours(mv._class, mv.day_1, mv.hour_1, mv._class, mv.day_2, mv.hour_2);
}  

void Sched_SwapHours_NeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  mv._class = 0;

  mv.day_1 = 0;
  mv.hour_1 = 0;

  mv.day_2 = 0;
  mv.hour_2 = 1;

  mv.first_move = true;
}

bool Sched_SwapHours_NeighborhoodExplorer::NextMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  do
  {
    if (!AnyNextMove(out,mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_SwapHours_NeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_SwapHours& mv) const
{
  mv.first_move = false;

  // Condizione che determina la fine della scansione dell'orario di una classe: non esiste nessuna mossa successiva allo scambio
  // degli ultimi due slot orari.
  if (mv.day_1 == in.N_Days() - 1 && mv.hour_1 >= in.N_HoursXDay() - 2 && mv.day_2 == in.N_Days() - 1 && mv.hour_2 >= in.N_HoursXDay() - 1)
  {
    mv._class++;

    // Le classi sono finite
    if (mv._class >= in.N_Classes())
      return false;

    mv.day_1 = 0;
    mv.hour_1 = 0;
    mv.day_2 = 0;
    mv.hour_2 = 1;

    return true;
  }

  // Move to next hour
  mv.hour_2++;
  
  if (mv.hour_2 == in.N_HoursXDay())
  {
    mv.day_2++;
    mv.hour_2 = 0;

    if (mv.day_2 == in.N_Days())
    {
      mv.hour_1++;

      if (mv.hour_1 == in.N_HoursXDay())
      {
        mv.day_1++;
        mv.hour_1 = 0;
      }

      // Comincio a scorrere il secondo slot temporale a partire da quello
      // successivo al primo
      if (mv.hour_1 < in.N_HoursXDay() - 1)
      {
        mv.day_2 = mv.day_1;
        mv.hour_2 = mv.hour_1 + 1;
      }
      else
      {
        mv.day_2 = mv.day_1 + 1;  // Passo al giorno successivo
        mv.hour_2 = 0;            // Resetto l'ora a 0
      }
    }
  }

  return true;
}