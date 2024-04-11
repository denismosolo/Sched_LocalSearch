// File Sched_SwapProf_NHE.cc
#include "Sched_Helpers.hh"

/***************************************************************************
 * Moves Code
 ***************************************************************************/

Sched_SwapProf::Sched_SwapProf()
{
  class_1 = -1;
  class_2 = -1;
  prof_subject = -1;
}

bool operator==(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2)
{
  return mv1.class_1 == mv2.class_1 && mv1.class_2 == mv2.class_2 && mv1.prof_subject == mv2.prof_subject;
}

bool operator!=(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2)
{
  return mv1.class_1 != mv2.class_1 || mv1.class_2 != mv2.class_2 || mv1.prof_subject != mv2.prof_subject;
}

bool operator<(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2)
{
  if (mv1.prof_subject != mv2.prof_subject)
    return mv1.prof_subject < mv2.prof_subject;
  else if (mv1.class_1 != mv2.class_1)
    return mv1.class_1 < mv2.class_1;
  else
    return mv1.class_2 < mv2.class_2;
}

istream& operator>>(istream& is, Sched_SwapProf& mv)
{
  char ch;
  is >> mv.prof_subject >> ch >> mv.class_1 >> ch >> mv.class_2;
  return is;
}

ostream& operator<<(ostream& os, const Sched_SwapProf& mv)
{
  os << mv.prof_subject << " " << mv.class_1 << " " << mv.class_2;
  return os;
}

/***************************************************************************
 * Sched_SwapProf Neighborhood Explorer Code
 ***************************************************************************/

void Sched_SwapProf_NeighborhoodExplorer::RandomMove(const Sched_Output& out, Sched_SwapProf& mv) const
{
  do
  {
    mv.prof_subject = Random::Uniform<int>(0, in.N_Subjects() - 1);

    mv.class_1 = Random::Uniform<int>(0, in.N_Classes() - 1);
    mv.class_2 = Random::Uniform<int>(0, in.N_Classes() - 1);

  } while (!FeasibleMove(out, mv));
  
} 

bool Sched_SwapProf_NeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned d, h;

  if (mv.class_1 == mv.class_2)
    return false;

  // Se non è stato assegnato ancora un prof per quella materia per una delle due classi,
  // la mossa non è valida (non esiste scambio)
  if (out.Subject_Prof(mv.class_1, mv.prof_subject) == -1 || out.Subject_Prof(mv.class_2, mv.prof_subject) == -1)
    return false;

  // Controlla incompatibilità di orario
  for(d = 0; d < in.N_Days(); d++)
    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      // Il prof non è impegnato nella classe 1 e l'ora non è un ora buca (del prof):
      // il prof è quindi impengato con una classe terza non coinvolta nello scambio.
      if (out.Prof_Schedule(out.Subject_Prof(mv.class_1, mv.prof_subject), d, h) != mv.class_1 && out.Prof_Schedule(out.Subject_Prof(mv.class_1, mv.prof_subject), d, h) != -1)
        return false;

      if (out.Prof_Schedule(out.Subject_Prof(mv.class_2, mv.prof_subject), d, h) != mv.class_2 && out.Prof_Schedule(out.Subject_Prof(mv.class_2, mv.prof_subject), d, h) != -1)
        return false;
    }

  return true;
} 

void Sched_SwapProf_NeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned d, h, i;

  int prof_1 = out.Subject_Prof(mv.class_1, mv.prof_subject);
  int prof_2 = out.Subject_Prof(mv.class_2, mv.prof_subject);
  vector<pair<unsigned, unsigned>> hours_prof_1(0);
  vector<pair<unsigned, unsigned>> hours_prof_2(0);

  for (d = 0; d < in.N_Days(); d++)
    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      if (out.Class_Schedule(mv.class_1, d, h) == prof_1)
      {
        hours_prof_1.push_back(pair<unsigned, unsigned>(d, h));
        out.FreeHour(mv.class_1, d, h);
      }
      
      if (out.Class_Schedule(mv.class_2, d, h) == prof_2)
      {
        hours_prof_2.push_back(pair<unsigned, unsigned>(d, h));
        out.FreeHour(mv.class_2, d, h);
      }
    }

  for (i = 0; i < hours_prof_1.size(); i++)
    out.AssignHour(mv.class_2, hours_prof_1[i].first, hours_prof_1[i].second, prof_2);

  for (i = 0; i < hours_prof_2.size(); i++)
    out.AssignHour(mv.class_1, hours_prof_2[i].first, hours_prof_2[i].second, prof_1);

}  

void Sched_SwapProf_NeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_SwapProf& mv) const
{
  mv.prof_subject = 0;

  mv.class_1 = 0;
  mv.class_2 = 1;
}

bool Sched_SwapProf_NeighborhoodExplorer::NextMove(const Sched_Output& out, Sched_SwapProf& mv) const
{
  do
  {
    if (!AnyNextMove(out,mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_SwapProf_NeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_SwapProf& mv) const
{
  // Ultimo scambio possibile tra due classi (a parità di materia)
  if (mv.class_1 == in.N_Classes() - 2 && mv.class_2 == in.N_HoursXDay() - 1)
  {
    // Le materie sono finite
    if (mv.prof_subject == in.N_Subjects() - 1)
      return false;
    
    mv.prof_subject++;

    mv.class_1 = 0;
    mv.class_2 = 1;

    return true;
  }

  // Incrementa la classe 2
  mv.class_2++;
  
  // Se la classe 2 è arrivata in fondo
  if (mv.class_2 == in.N_Classes())
  {
    mv.class_1++;                 // Incrementa la classe 1
    mv.class_2 = mv.class_1 + 1;  // Imposta la classe 2 a quella successiva alla classe 1
  }

  return true;
}