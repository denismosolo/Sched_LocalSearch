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

  // RandomMove and AnyNextMovehelp function
  vector<unsigned> GetAvailableProfs(const Sched_Input& in, const Sched_Output& out, const int c)
  {
    unsigned s;
    vector<unsigned> available_profs;
    
    for (s = 0; s < in.N_Subjects(); s++)
      if (out.WeeklySubjectResidualHours(c, s) != 0)
        available_profs.push_back(out.Subject_Prof(c, s));
    return available_profs;
  }
}

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
 * State Manager 
 ***************************************************************************/

int Sched_ProfUnavailability::ComputeCost(const Sched_Output& out) const
{
  unsigned p;
  unsigned violations = 0;
  
  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfAssignedDayOff(p) == -1 || (unsigned)out.ProfAssignedDayOff(p) != in.ProfUnavailability(p))
      violations++;

  return violations * in.UnavailabilityViolationCost();
}

void Sched_ProfUnavailability::PrintViolations(const Sched_Output& out, ostream& os) const
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

int Sched_MaxSubjectHoursXDay::ComputeCost(const Sched_Output& out) const
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

void Sched_MaxSubjectHoursXDay::PrintViolations(const Sched_Output& out, ostream& os) const
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

int Sched_ProfMaxWeeklyHours::ComputeCost(const Sched_Output& out) const
{
  unsigned p;
  unsigned violations = 0;

  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      violations += out.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours();

  return violations * in.MaxProfWeeklyHoursViolationCost();
}

void Sched_ProfMaxWeeklyHours::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned p;

  for (p = 0; p < in.N_Profs(); p++)
    if (out.ProfWeeklyAssignedHours(p) > in.ProfMaxWeeklyHours())
      os << "To prof " << in.Prof_Name(p) << " assigned "
        << out.ProfWeeklyAssignedHours(p) - in.ProfMaxWeeklyHours() << " extra hours"
        << endl;
}

int Sched_ScheduleContiguity::ComputeCost(const Sched_Output& out) const
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

void Sched_ScheduleContiguity::PrintViolations(const Sched_Output& out, ostream& os) const
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

int Sched_CompleteSolution::ComputeCost(const Sched_Output& out) const
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

void Sched_CompleteSolution::PrintViolations(const Sched_Output& out, ostream& os) const
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
 * Sched_Change Neighborhood Explorer:
 ***************************************************************************/

Sched_Change::Sched_Change()
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
  count = -1;
}

bool operator==(const Sched_Change& mv1, const Sched_Change& mv2)
{
  return mv1._class == mv2._class && mv1.new_day == mv2.new_day && mv1.new_hour == mv2.new_hour && mv1.old_day == mv2.old_day && mv1.old_hour == mv2.old_hour;
}

bool operator!=(const Sched_Change& mv1, const Sched_Change& mv2)
{
  return mv1._class != mv2._class || mv1.new_day != mv2.new_day || mv1.new_hour != mv2.new_hour || mv1.old_day != mv2.old_day || mv1.old_hour != mv2.old_hour;
}

bool operator<(const Sched_Change& mv1, const Sched_Change& mv2)
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

istream& operator>>(istream& is, Sched_Change& mv)
{
  char ch;  //se è vuoto funziona come spazio?
  is >> mv._class >> ch >> mv.old_day >> ch >> mv.old_hour >> ch >> ch >> mv.new_day >> ch >> mv.new_hour;
  return is;
}

ostream& operator<<(ostream& os, const Sched_Change& mv)
{
  os << mv._class << ": (" << mv.old_day << ", " << mv.old_hour << ") -> (" << mv.new_day << ", " << mv.old_hour << ")";
  return os;
}

void Sched_ChangeNeighborhoodExplorer::RandomMove(const Sched_Output& out, Sched_Change& mv) const
{
  vector<unsigned> available_profs;

  mv._class = Random::Uniform<int>(0, in.N_Classes()-1);

  // Get all profs of the class with not all hours already assigned
  available_profs = GetAvailableProfs(in, out, mv._class);

  do
  {
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

bool Sched_ChangeNeighborhoodExplorer::FeasibleMove(const Sched_Output& out, const Sched_Change& mv) const
{
  // If there is only one class the move is always feasible
  if (in.N_Classes() == 1)
    return mv.old_day != mv.new_day || mv.old_hour != mv.new_hour;

  // If at least one of the profs has their "new" hour already assigned
  if (out.IsProfHourFree(mv.old_prof, mv.new_day, mv.new_hour) || out.IsProfHourFree(mv.new_prof, mv.old_day, mv.old_hour))
    return false;
  else
    return mv.old_day != mv.new_day || mv.old_hour != mv.new_hour;
} 

void Sched_ChangeNeighborhoodExplorer::MakeMove(Sched_Output& out, const Sched_Change& mv) const
{
  out.SwapHours(mv._class, mv.old_day, mv.old_hour, mv._class, mv.new_day, mv.new_hour); //troppi check, sarebbero da rimuovere perché il codice deve essere efficiente non robusto -- valutare
  cerr << mv << endl;
}  

void Sched_ChangeNeighborhoodExplorer::FirstMove(const Sched_Output& out, Sched_Change& mv) const
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
    mv.count++;
  }
  else
    mv.old_prof = out.Class_Schedule(mv._class, mv.old_day, mv.old_hour);

  mv.new_day = 1;
  mv.new_hour = 1;
  if (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour))
  {
    mv.new_free = true;
    mv.new_prof = available_profs[0];
    mv.count++;
  }
  else
    mv.new_prof = out.Class_Schedule(mv._class, mv.new_day, mv.new_hour);
}

bool Sched_ChangeNeighborhoodExplorer::NextMove(const Sched_Output& out, Sched_Change& mv) const
{
  do
  {
    if (!AnyNextMove(out,mv))
      return false;
  }  
  while (!FeasibleMove(out,mv));
  
  return true;
}

bool Sched_ChangeNeighborhoodExplorer::AnyNextMove(const Sched_Output& out, Sched_Change& mv) const
{
  vector<unsigned> available_profs;

  available_profs = GetAvailableProfs(in, out, mv._class);

  if ((mv.new_free ^ mv.old_free) && mv.count < available_profs.size())  // NOTE: ^ is XOR in C++
  {
    if (mv.count == available_profs.size() - 1)
    {
      mv.count++;
      if (mv.new_free)
        mv.new_prof = -1;
      else
        mv.old_prof = -1;
    }
    else if (mv.new_free)
      mv.new_prof = available_profs[++mv.count];
    else
      mv.old_prof = available_profs[++mv.count];
  }
  else
  {
    // aumento la nuova ora di 1
    mv.new_hour++;

    mv.count = -1;
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

            available_profs = GetAvailableProfs(in, out, mv._class);
          };
        }
        // se arrivo qui ho sicuro cambiato old quindi devo ricalcolarmi il professore
        if (out.IsClassHourFree(mv._class, mv.old_day, mv.old_hour))
        {
          mv.count++;
          mv.old_prof = available_profs[mv.count];
          mv.old_free = true;
        }
        else
          mv.old_prof = out.Class_Schedule(mv._class, mv.old_day, mv.old_hour);
      }
    }
    if (out.IsClassHourFree(mv._class, mv.new_day, mv.new_hour))
    {
      mv.count++;
      mv.new_prof = available_profs[mv.count];
      mv.new_free = true;
    }
    else
      mv.new_prof = out.Class_Schedule(mv._class, mv.new_day, mv.new_hour);
  }

  return true;
}

int Sched_ChangeDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}

int Sched_ChangeDeltaCompleteSolution::ComputeDeltaCost(const Sched_Output& out, const Sched_Change& mv) const
{
// to be implemented
// sottrarre il costo della mossa che vado a togliere e sommare quello della nuova
  return 0;
}