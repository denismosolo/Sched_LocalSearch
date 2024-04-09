// File Sched_Helpers.cc
#include "Sched_Helpers.hh"

namespace {
  // Greedy V6_1 help function
  // Creates a list of randomly chosen possibile compatible hours for a professor checking free hours in professor and class schedule, 
  // taking in account maximum hours per day and, optionally, hours not compatible with professor day off.
  vector<pair<unsigned, unsigned>> find_randomized_day_ordered_compatible_hours(const Sched_Input& in, Sched_Output& out, unsigned c, unsigned p, const unsigned& extra_daily_hours, const bool& check_unavailability_day)
  {
    unsigned d, h;
    vector<unsigned> random_days(in.N_Days());
    unsigned hours_per_day = 0;
    vector<pair<unsigned, unsigned>> compatible_hours(0);

    // Creates the vector containing days scanning order
    iota(random_days.begin(), random_days.end(), 0);

    // Shuffles the vector containing days scanning order
    shuffle(random_days.begin(), random_days.end(), Random::GetGenerator());

    for (d = 0; d < in.N_Days(); d++)
    {
      hours_per_day = 0;

      for (h = 0; h < in.N_HoursXDay(); h++)
      {
        if ((check_unavailability_day) && in.ProfUnavailability(p) == random_days[d])
          break;

        if (out.Prof_Schedule(p, random_days[d], h) == -1 && out.Class_Schedule(c, random_days[d], h) == -1)
        {
          compatible_hours.push_back(pair<unsigned, unsigned>(random_days[d], h));
          hours_per_day++;
        }

        if (hours_per_day >= in.SubjectMaxHoursXDay() + extra_daily_hours)
          break;
      }
    }
    return compatible_hours;
  }
}

/***************************************************************************
 * Solution Manager Code
 ***************************************************************************/

Sched_SolutionManager::Sched_SolutionManager(const Sched_Input & pin) 
  : SolutionManager<Sched_Input,Sched_Output>(pin, "SchedSolutionManager")  {} 

void Sched_SolutionManager::RandomState(Sched_Output& out) 
{
  // to be implemented -- deve instrinsecamente rispettare il vincolo del singolo prof per materia
  unsigned c, s, p, d, h, i;
  vector<pair<unsigned, unsigned>> all_d_h_permutations;
  vector<unsigned> subjects(in.N_Subjects());

  iota(subjects.begin(), subjects.end(), 0);

  //load all day-hour permutations
  for (d = 0; d < in.N_Days(); d++)
    for (h = 0; h < in.N_HoursXDay(); h++)
      all_d_h_permutations.push_back(make_pair(d,h));

  for (c = 0; c < in.N_Classes(); c++)
  {
    // mescolo vettore materie
    shuffle(subjects.begin(), subjects.end(), Random::GetGenerator());

    for (s = 0; s < in.N_Subjects(); s++)
    {
      //mescolo il vettore delle coppie giorno-ora
      shuffle(all_d_h_permutations.begin(), all_d_h_permutations.end(), Random::GetGenerator());

      //seleziono a caso un prof della materia che non sia già pieno
      do
      {
        p = in.SubjectProf(s, Random::Uniform<unsigned>(0, in.N_ProfsXSubject(s)-1));
      } while (out.ProfWeeklyAssignedHours(p) >= (in.N_Days()*in.N_HoursXDay()));

      h = 0;

      for (i = 0; i < all_d_h_permutations.size(); i++)
      {
        if (out.AssignHour(c, all_d_h_permutations[i].first, all_d_h_permutations[i].second, p))
          h++;
        
        if (h == in.N_HoursXSubject(s))
          break;  // ho assegnato tutte le ore
      }
    }  
  }
}

void Sched_SolutionManager::GreedyState(Sched_Output& out) 
{
  // Greedy_V6_1 algorithm

  unsigned c, s, p, i;
  vector<vector<unsigned>> profs_assignation_order_per_subject;
  vector<unsigned> subjects_assignation_order(in.N_Subjects());

  vector<pair<unsigned, unsigned>> compatible_hours;

  bool prof_assigned;
  unsigned extra_daily_hours;
  bool take_in_account_unavailability;

  profs_assignation_order_per_subject = in.GetSubjectProfs();

  // Fills subjects_assignation_order vector with subjects number
  iota(subjects_assignation_order.begin(), subjects_assignation_order.end(), 0);

  // Sorts subjects_assignation_order vector descending using as metric the weekly hours of each subject
  sort(subjects_assignation_order.begin(), subjects_assignation_order.end(), [&](const unsigned& s1, const unsigned& s2) { return (in.N_HoursXSubject(s1) != in.N_HoursXSubject(s2)) ? in.N_HoursXSubject(s1) > in.N_HoursXSubject(s2) : Random::Uniform<int>(0,1) ; });

  // Reset output
  out.Reset();

  // Create a schedule for each class, for each subject
  for (c = 0; c < in.N_Classes(); c++)
    for (s = 0; s < in.N_Subjects(); s++)
    {
      // To shuffle order of profs with same load
      shuffle(profs_assignation_order_per_subject[subjects_assignation_order[s]].begin(), profs_assignation_order_per_subject[subjects_assignation_order[s]].end(), Random::GetGenerator());

      // Order available professors, divided by subjects, ascending by load
      sort(profs_assignation_order_per_subject[subjects_assignation_order[s]].begin(), profs_assignation_order_per_subject[subjects_assignation_order[s]].end(), [&](const unsigned a, const unsigned b) { return (out.ProfWeeklyAssignedHours(a) < out.ProfWeeklyAssignedHours(b)); });// ? (out.ProfWeeklyAssignedHours(a) < out.ProfWeeklyAssignedHours(b)) : Random(); });

      prof_assigned = false;
      extra_daily_hours = 0;
      take_in_account_unavailability = true;

      // Cycle until a professor is assigned for the current subject or maximum daily hours for the subject constrain
      // is relaxed reaching maximum daily hours of the schedule
      while (!prof_assigned && in.SubjectMaxHoursXDay() + extra_daily_hours <= in.N_HoursXDay())
      {

        for (p = 0; p < in.N_ProfsXSubject(subjects_assignation_order[s]) && !prof_assigned; p++)
        {
          compatible_hours = find_randomized_day_ordered_compatible_hours(in, out, c, profs_assignation_order_per_subject[subjects_assignation_order[s]][p], extra_daily_hours, take_in_account_unavailability);

          // If compatible hours are enough to cover weekly subject hours, assign that professor to the class
          if (compatible_hours.size() >= in.N_HoursXSubject(subjects_assignation_order[s]))
          {
            for (i = 0; i < in.N_HoursXSubject(subjects_assignation_order[s]); i++)
              out.AssignHour(c, compatible_hours[i].first, compatible_hours[i].second, profs_assignation_order_per_subject[subjects_assignation_order[s]][p]);

            prof_assigned = true;
          }
        }

        // If no professor has been found suitable to class schedule, relax constraint for maximum subject hours per day or professor unavailability
        // depending on costs
        if (!prof_assigned)
        {
          if ((in.UnavailabilityViolationCost() < (extra_daily_hours + 1) * in.MaxSubjectHoursXDayViolationCost() || in.SubjectMaxHoursXDay() + extra_daily_hours == in.N_HoursXDay()) && take_in_account_unavailability)
            take_in_account_unavailability = false;
          else
            extra_daily_hours++;
        }
      }
    }
}

bool Sched_SolutionManager::CheckConsistency(const Sched_Output& out) const
{
  return true; //TBC if necessary or already handled by AssignHour method
}

/***************************************************************************
 * Cost Components Code
 ***************************************************************************/

int Sched_ProfUnavailability_CC::ComputeCost(const Sched_Output& out) const
{
  unsigned p;
  unsigned violations = 0;
  
  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfAssignedDayOff(p) == -1 || (unsigned)out.ProfAssignedDayOff(p) != in.ProfUnavailability(p))
      violations++;

  return violations * in.UnavailabilityViolationCost();
}

void Sched_ProfUnavailability_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned p;
  
  for (p = 0; p < in.N_Profs(); p++)
  {
    if (out.ProfAssignedDayOff(p) == -1)
      os << "Prof " << in.Prof_Name(p) << " has no day off" << endl;
    else if ((unsigned)out.ProfAssignedDayOff(p) != in.ProfUnavailability(p))
    {
      os << "To prof " << in.Prof_Name(p) << " assigned day off on " << in.Day_Name((days)out.ProfAssignedDayOff(p))
        << " instead of " << in.Day_Name((days)in.ProfUnavailability(p))
        << endl;
    }
  }
}

int Sched_MaxSubjectHoursXDay_CC::ComputeCost(const Sched_Output& out) const
{
  unsigned c, s, d;
  unsigned violations = 0;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (s = 0; s < in.N_Subjects(); s++)
        if (out.DailySubjectAssignedHours(c, d, s) > in.SubjectMaxHoursXDay())
          violations += out.DailySubjectAssignedHours(c, d, s) - in.SubjectMaxHoursXDay();

  return violations * in.MaxSubjectHoursXDayViolationCost();
}

void Sched_MaxSubjectHoursXDay_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned c, s, d;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (s = 0; s < in.N_Subjects(); s++)
        if (out.DailySubjectAssignedHours(c, d, s) > in.SubjectMaxHoursXDay())
          os << "To subject " << in.Subject_Name(s) << " in class " << in.Class_Name(c)
            << " assigned " << out.DailySubjectAssignedHours(c, d, s) << " hours instead of " << in.SubjectMaxHoursXDay()
            << " on " << in.Day_Name((days)d)
            << endl;
}

int Sched_ProfMaxWeeklyHours_CC::ComputeCost(const Sched_Output& out) const
{
  unsigned p;
  unsigned violations = 0;

  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      violations += out.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours();

  return violations * in.MaxProfWeeklyHoursViolationCost();
}

void Sched_ProfMaxWeeklyHours_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned p;

  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      os << "To prof " << in.Prof_Name(p) << " assigned "
        << out.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours() << " extra hours"
        << endl;
}

int Sched_ScheduleContiguity_CC::ComputeCost(const Sched_Output& out) const
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
          if (out.Class_Schedule(c, d, h) != -1 && (unsigned)out.Class_Schedule(c, d, h) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              violations++;

            subject_last_position = h;
          }
      }

  return violations * in.ScheduleContiguityViolationCost();
}

void Sched_ScheduleContiguity_CC::PrintViolations(const Sched_Output& out, ostream& os) const
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
          if (out.Class_Schedule(c, d, h) != -1 && (unsigned)out.Class_Schedule(c, d, h) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              os << "Contiguity violation in class " << in.Class_Name(c)
                << " for subject " << in.Subject_Name(s) << " on " << in.Day_Name((days)d)
                << " between hours " << h+1 << " - " << subject_last_position+1
                << endl;

            subject_last_position = h;
          }
      }
}

int Sched_SolutionComplete_CC::ComputeCost(const Sched_Output& out) const
{
  unsigned c, d, h;
  unsigned violations = 0;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (h = 0; h < in.N_HoursXDay(); h++)
        if(out.IsClassHourFree(c, d, h))
          violations++;

  return violations;
}

void Sched_SolutionComplete_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned c, d, h;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (h = 0; h < in.N_HoursXDay(); h++)
        if(out.IsClassHourFree(c, d, h))
          os << "Free hour for class " << in.Class_Name(c)
            << " on " << in.Day_Name(days(d))
            << " at hour " << h+1
            << endl;
}

/***************************************************************************
 * Moves Code
 ***************************************************************************/

//Sched_Change::Sched_Change()
//{
//  _class = -1;
//  new_day = -1;
//  new_hour = -1;
//  new_prof = -1;
//  new_free = false;
//  old_day = -1;
//  old_hour = -1;
//  old_prof = -1;
//  old_free = false;
//  count = -1;
//}


bool operator==(const  Sched_SwapHours_M& mv1, const  Sched_SwapHours_M& mv2)
{
  return mv1._class == mv2._class && mv1.day_1 == mv2.day_1 && mv1.hour_1 == mv2.hour_1 && mv1.day_2 == mv2.day_2 && mv1.hour_2 == mv2.hour_2;
}

bool operator!=(const  Sched_SwapHours_M& mv1, const  Sched_SwapHours_M& mv2)
{
  return mv1._class != mv2._class || mv1.day_1 != mv2.day_1 || mv1.hour_1 != mv2.hour_1 || mv1.day_2 != mv2.day_2 || mv1.hour_2 != mv2.hour_2;
}

bool operator<(const  Sched_SwapHours_M& mv1, const  Sched_SwapHours_M& mv2)
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

istream& operator>>(istream& is,  Sched_SwapHours_M& mv)
{
  char ch;  //se è vuoto funziona come spazio?
  is >> mv._class >> ch >> mv.day_1 >> ch >> mv.hour_1 >> ch >> ch >> mv.day_2 >> ch >> mv.hour_2;
  return is;
}

ostream& operator<<(ostream& os, const  Sched_SwapHours_M& mv)
{
  os << "(class " << mv._class << ") : (" << mv.day_1 << ", " << mv.hour_1 << ") <-> (" << mv.day_2 << ", " << mv.hour_2 << ")";
  return os;
}

/***************************************************************************
 * Sched_Change Neighborhood Explorer Code
 ***************************************************************************/

void Sched_ChangeNeighborhoodExplorer::RandomMove(const Sched_Output& out,  Sched_SwapHours_M& mv) const
{
  mv._class = Random::Uniform<unsigned>(0, in.N_Classes()-1);

  do
  {
    mv.day_1 = Random::Uniform<unsigned>(0, in.N_Days()-1);
    mv.hour_1 = Random::Uniform<unsigned>(0, in.N_HoursXDay()-1);

    mv.day_2 = Random::Uniform<unsigned>(0, in.N_Days()-1);
    mv.hour_2 = Random::Uniform<unsigned>(0, in.N_HoursXDay()-1);
    
  } while (!FeasibleMove(out, mv));
  
} 

bool Sched_ChangeNeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
  int prof_1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  int prof_2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  // If the prof is the same (either the case that both are -1)
  if (prof_1 == prof_2)
    return false;
  else
  {
    // If there is only one class the move is always feasible
    if (in.N_Classes() != 1)
    {
      // If at least one of the profs has their "new" hour already assigned
      if ((prof_1 != -1 && !out.IsProfHourFree(prof_1, mv.day_2, mv.hour_2)) || (prof_2 != -1 && !out.IsProfHourFree(prof_2, mv.day_1, mv.hour_1)))
        return false;
    }

    return mv.day_1 != mv.day_2 || mv.hour_1 != mv.hour_2;
  }
} 

void Sched_ChangeNeighborhoodExplorer::MakeMove(Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
  out.SwapHours(mv._class, mv.day_1, mv.hour_1, mv._class, mv.day_2, mv.hour_2); //troppi check, sarebbero da rimuovere perché il codice deve essere efficiente non robusto -- valutare
}  

void Sched_ChangeNeighborhoodExplorer::FirstMove(const Sched_Output& out,  Sched_SwapHours_M& mv) const
{
  mv._class = 0;

  mv.day_1 = 0;
  mv.hour_1 = 0;

  mv.day_2 = 0;
  mv.hour_2 = 1;
}

bool Sched_ChangeNeighborhoodExplorer::NextMove(const Sched_Output& out,  Sched_SwapHours_M& mv) const
{
  do
  {
    if (!AnyNextMove(out, mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_ChangeNeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_SwapHours_M& mv) const
{
  // Debug
  // cout << "Enters: " << "(" << mv.day_1 << ", " << mv.hour_1 << ") <-> (" << mv.day_2 << ", " << mv.hour_2 << ")";

  // Incremento l'ora
  mv.hour_2++;

  // Se l'ora supera il numero massimo di ore giornaliere
  if (mv.hour_2 == in.N_HoursXDay())
  {
    mv.hour_2 = 0;  // Resetto l'ora a 0
    mv.day_2++;     // Passo al giorno successivo

    // Se il giorno supera il numero massimo di giorni settimanali
    if (mv.day_2 == in.N_Days())
    {
      mv.hour_1++;  // Incremento l'ora della prima ora

      // Se anche l'ora supera il numero massimo di ore giornaliere
      if (mv.hour_1 == in.N_HoursXDay())
      {
        mv.hour_1 = 0;  // Resetto l'ora a 0
        mv.day_1++;     // Passo al giorno successivo
      }

      // Controllo se l'ora non è l'ultima dell'ultimo giorno della settimana
      if (mv.hour_1 != in.N_HoursXDay() - 1)
      {
        mv.day_2 = mv.day_1;       // Imposto il secondo giorno uguale al primo
        mv.hour_2 = mv.hour_1 + 1; // Imposto la seconda ora una successiva alla prima
      }
      else
      {
        mv.day_2 = mv.day_1 + 1; // Passo al giorno successivo
        mv.hour_2 = 0;           // Resetto l'ora a 0
      }

      // Se siamo all'ultimo giorno e all'ultima ora disponibili
      if (mv.day_2 == in.N_Days())
      {
        mv._class++;  // Incremento la classe

        // Se la classe supera il numero massimo di classi
        if (mv._class == in.N_Classes())
          return false;  // Termina l'esplorazione

        // Resetto le variabili per la nuova classe
        mv.day_1 = 0;
        mv.hour_1 = 0;
        mv.day_2 = 0;
        mv.hour_2 = 1;

        // Debug
        // cout << "  Exits: " << "(" << mv.day_1 << ", " << mv.hour_1 << ") <-> (" << mv.day_2 << ", " << mv.hour_2 << ")" << endl;

        return true;  // Continua l'esplorazione
      }
    }
  }

  // Debug
  // cout << "  Exits: " << "(" << mv.day_1 << ", " << mv.hour_1 << ") <-> (" << mv.day_2 << ", " << mv.hour_2 << ")" << endl;

  return true;  // Continua l'esplorazione
}

int Sched_ChangeDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaCompleteSolution::ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}