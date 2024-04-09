// File Sched_SolutionManager.cc
#include "Sched_Helpers.hh"

namespace
{
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
    // shuffle subject vector
    shuffle(subjects.begin(), subjects.end(), Random::GetGenerator());

    for (s = 0; s < in.N_Subjects(); s++)
    {
      // shuffle d_h pairs
      shuffle(all_d_h_permutations.begin(), all_d_h_permutations.end(), Random::GetGenerator());

      // choose random a professor
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
          break;  // all hours assigned
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
