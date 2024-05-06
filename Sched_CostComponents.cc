// File Sched_CostComponents.cc
#include "Sched_Headers.hh"

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

  return violations;
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

  return violations;
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

  return violations;
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

  for (c = 0; c < in.N_Classes(); c++)
    for (s = 0; s < in.N_Subjects(); s++)
      for (d = 0; d < in.N_Days(); d++)
      {
        subject_last_position = -1;

        for (h = 0; h < in.N_HoursXDay(); h++)
          if (out.Class_Schedule(c, d, h) != -1 && in.ProfSubject(out.Class_Schedule(c, d, h)) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              violations++;

            subject_last_position = h;
          }
      }

  return violations;
}

void Sched_ScheduleContiguity_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned c, s, d, h;
  int subject_last_position;

  if (in.N_HoursXDay() < 3)
    return;

  for (c = 0; c < in.N_Classes(); c++)
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
              << " between hours " << h + 1 << " - " << subject_last_position + 1
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
        if (out.IsClassHourFree(c, d, h))
          violations++;

  return violations;
}

void Sched_SolutionComplete_CC::PrintViolations(const Sched_Output& out, ostream& os) const
{
  unsigned c, d, h;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (h = 0; h < in.N_HoursXDay(); h++)
        if (out.IsClassHourFree(c, d, h))
          os << "Free hour for class " << in.Class_Name(c)
          << " on " << in.Day_Name(days(d))
          << " at hour " << h + 1
          << endl;
}

/***************************************************************************
 * Delta Cost Components Code - Sched_SwapHours
 ***************************************************************************/

int Sched_SwapHoursDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  unsigned h;
  int p1, p2;
  int cost = 0;

  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  if (mv.day_1 == mv.day_2)
    return 0;

  // Subtract old costs
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p1, mv.day_1, h) && mv.hour_1 != h) // if I find the professor in another hour of the same day
        break;
    if (h == in.N_HoursXDay()) // The professor is present only 1 hour in the day
      cost--;  
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p2, mv.day_2, h) && mv.hour_2 != h)
        break;
    if (h == in.N_HoursXDay())
      cost--;  
  }

  // Add new costs
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p1, mv.day_2, h) && mv.hour_2 != h)
        break;
    if (h == in.N_HoursXDay())
      cost++;
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p2, mv.day_1, h) && mv.hour_1 != h)
        break;
    if (h == in.N_HoursXDay())
      cost++;
  }

  return cost;
}

int Sched_SwapHoursDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  int p1, p2;
  int cost = 0;

  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  if (mv.day_1 == mv.day_2)
    return 0;

  // Subtract old costs
  if (p1 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_1, in.ProfSubject(p1)) > in.SubjectMaxHoursXDay())
    cost--;

  if (p2 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_2, in.ProfSubject(p2)) > in.SubjectMaxHoursXDay())
    cost--;

  // Add new costs
  if (p2 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_1, in.ProfSubject(p2)) >= in.SubjectMaxHoursXDay()) // add the case "=" because if I am at the limit adding another one cause a violation
    cost++;

  if (p1 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_2, in.ProfSubject(p1)) >= in.SubjectMaxHoursXDay())
    cost++;

  return cost;
}

int Sched_SwapHoursDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  unsigned h;
  int cost = 0;

  int p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  int p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  int last_old_1 = -1;
  int last_old_2 = -1;
  int last_res_1 = -1;
  int last_res_2 = -1;
  int last_new_1 = -1;
  int last_new_2 = -1;
  int last_arr_1 = -1;
  int last_arr_2 = -1;

  // Iterating over the schedule hour by hour, considering only the days involved in the hour swap
  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    // Current violations of "subject 1" on the day involved in the swap
    if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1)
    {
      if (last_old_1 != -1 && h - last_old_1 > 1)
        cost--;
      last_old_1 = h;
    }

    // Current violations of "subject 2" on the day involved in the swap
    if (p2 != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        cost--;
      last_old_2 = h;
    }

    // Violations already present on the arrival day - but only if different days
    if (mv.day_1 != mv.day_2)
    {
      if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p1)
      {
        if (last_arr_1 != -1 && h - last_arr_1 > 1)
          cost--;
        last_arr_1 = h;
      }

      if (p2 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p2)
      {
        if (last_arr_2 != -1 && h - last_arr_2 > 1)
          cost--;
        last_arr_2 = h;
      }
    }

    // Violations of "subject 1" on the day currently occupied by "subject 2" if the swap is made.
    // NOTE: the hour on day 2 is currently occupied by subject 1, or it's the hour that will be occupied by "subject 1" if the swap is made.
    if (p1 != -1 && (out.Class_Schedule(mv._class, mv.day_2, h) == p1 || h == mv.hour_2) && !(mv.day_1 == mv.day_2 && h == mv.hour_1))
    {
      if (last_new_1 != -1 && h - last_new_1 > 1)
        cost++;
      last_new_1 = h;
    }

    // Violations of "subject 2" on the day currently occupied by "subject 1" if the swap is made.
    if (p2 != -1 && (out.Class_Schedule(mv._class, mv.day_1, h) == p2 || h == mv.hour_1) && !(mv.day_1 == mv.day_2 && h == mv.hour_2))
    {
      if (last_new_2 != -1 && h - last_new_2 > 1)
        cost++;
      last_new_2 = h;
    }

    // Residual violations on the departure day. only if different days
    if (mv.day_1 != mv.day_2)
    {
      if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1 && h != mv.hour_1)
      {
        if (last_res_1 != -1 && h - last_res_1 > 1)
          cost++;
        last_res_1 = h;
      }

      if (p2 != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2 && h != mv.hour_2)
      {
        if (last_res_2 != -1 && h - last_res_2 > 1)
          cost++;
        last_res_2 = h;
      }
    }
  }
  return cost;
}


/***************************************************************************
 * Delta Cost Components Code - Sched_AssignProf
 ***************************************************************************/

int Sched_AssignProfDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;

  // There's no old costs to subtract

  if (in.ProfUnavailability(mv.prof) == mv.day)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day, h) == mv.prof && h != mv.hour) // if I find the professor at another hour of the same day
        return 0;

    // I reached the end of the loop so the professor is present only 1 hour in the day --> I get one (1) violation
    return 1;
  }

  // The day where the professor is added is not his requested day off
  return 0;
}

int Sched_AssignProfDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  // There's no old costs to subtract

  if (out.DailySubjectAssignedHours(mv._class, mv.day, in.ProfSubject(mv.prof)) >= in.SubjectMaxHoursXDay())
    return 1;

  return 0;
}

int Sched_AssignProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  // There's no old costs to subtract

  if (out.ProfWeeklyAssignedHours(mv.prof) >= in.ProfMaxWeeklyHours())
    return 1;

  return 0;
}

int Sched_AssignProfDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;
  int last_hour = -1;

  // There's no old costs to subtract

  // If I place the new hour next to one of the same subject, the cost remains unchanged
  if (mv.hour > 0)
    if (out.Class_Schedule(mv._class, mv.day, mv.hour - 1) == mv.prof)
      return 0;

  // If I place the new hour before one of the same subject, the cost remains unchanged
  if (mv.hour < in.N_HoursXDay() - 1)
    if (out.Class_Schedule(mv._class, mv.day, mv.hour + 1) == mv.prof)
      return 0;

  // Case in which I place the new hour far from the other hours of the same subject -> in any case I introduce a delta in the cost equal to 1
  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    if (out.Class_Schedule(mv._class, mv.day, h) == mv.prof || h == mv.hour)
    {
      if (last_hour != -1 && h - last_hour > 1)
        return 1;

      last_hour = h;
    }
  }
  return 0;
}



/***************************************************************************
 * Delta Cost Components Code - Sched_SwapProf
 ***************************************************************************/

int Sched_SwapProfDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned prof_1, prof_2, h;
  int cost = 0;

  prof_1 = out.Subject_Prof(mv.class_1, mv.subject);
  prof_2 = out.Subject_Prof(mv.class_2, mv.subject);

  for (h = 0; h < in.N_HoursXDay(); h++)
    if (!out.IsProfHourFree(prof_1, in.ProfUnavailability(prof_1), h) && out.Prof_Schedule(prof_1, in.ProfUnavailability(prof_1), h) != mv.class_1)
      break; // prof_1 is engaged with another class on his day off => I don't modify the violations for prof_1
  if (h == in.N_HoursXDay()) // prof_1 on the free day has at most only class_1 => there may be changes to violations
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Prof_Schedule(prof_2, in.ProfUnavailability(prof_1), h) == mv.class_2)
        break; // a lesson is introduced on prof_1's day off
    // No lessons are introduced on prof_1's day off AND there is a violation (I already know it's only due to class_1) => I resolve it
    if (h == in.N_HoursXDay() && out.ProfAssignedDayOff(prof_1) != in.ProfUnavailability(prof_1))
      cost--;
    // Lessons are introduced on prof_1's day off AND there was no violation already => I introduce it
    else if (h < in.N_HoursXDay() && out.ProfAssignedDayOff(prof_1) == in.ProfUnavailability(prof_1))
      cost++;
    // else => I don't change anything
  }

  for (h = 0; h < in.N_HoursXDay(); h++)
    if (!out.IsProfHourFree(prof_2, in.ProfUnavailability(prof_2), h) && out.Prof_Schedule(prof_2, in.ProfUnavailability(prof_2), h) != mv.class_2)
      break; // prof_2 is engaged with another class on their day off
  if (h == in.N_HoursXDay()) // prof_2 on the free day has only class_2
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Prof_Schedule(prof_1, in.ProfUnavailability(prof_2), h) == mv.class_1)
        break; // a lesson is introduced on prof_2's day off
    // No lessons are introduced on prof_2's day off AND there is a violation (I already know it's only due to class_2) => I resolve it
    if (h == in.N_HoursXDay() && out.ProfAssignedDayOff(prof_2) != in.ProfUnavailability(prof_2))
      cost--;
    // Lessons are introduced on prof_2's day off AND there was no violation already => I introduce it
    else if (h < in.N_HoursXDay() && out.ProfAssignedDayOff(prof_2) == in.ProfUnavailability(prof_2))
      cost++;
    // else => I don't change anything
  }
  return cost;
}

// the cost is not 0 only if the number of assigned hours for that subject to the two classes is different
int Sched_SwapProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned p1, p2;
  int p1_extra_hours, p2_extra_hours, delta;
  int cost = 0;

  p1 = out.Subject_Prof(mv.class_1, mv.subject);
  p2 = out.Subject_Prof(mv.class_2, mv.subject);

  p1_extra_hours = out.ProfWeeklyAssignedHours(p1) - in.ProfMaxWeeklyHours();
  p2_extra_hours = out.ProfWeeklyAssignedHours(p2) - in.ProfMaxWeeklyHours();

  if (out.WeeklySubjectAssignedHours(mv.class_1, mv.subject) > out.WeeklySubjectAssignedHours(mv.class_2, mv.subject))
  {
    delta = out.WeeklySubjectAssignedHours(mv.class_1, mv.subject) - out.WeeklySubjectAssignedHours(mv.class_2, mv.subject);
    if (p1_extra_hours > 0)
    {
      if (p1_extra_hours > delta)
        cost -= delta;
      else
        cost -= p1_extra_hours;
    }

    if (p2_extra_hours > 0)
      cost += delta;
    else if (p2_extra_hours + delta > 0)
      cost += p2_extra_hours + delta;
  }

  if (out.WeeklySubjectAssignedHours(mv.class_1, mv.subject) < out.WeeklySubjectAssignedHours(mv.class_2, mv.subject))
  {
    delta = out.WeeklySubjectAssignedHours(mv.class_2, mv.subject) - out.WeeklySubjectAssignedHours(mv.class_1, mv.subject);
    if (p2_extra_hours > 0)
    {
      if (p2_extra_hours > delta)
        cost -= delta;
      else
        cost -= p2_extra_hours;
    }

    if (p1_extra_hours > 0)
      cost += delta;
    else if (p1_extra_hours + delta > 0)
      cost += p1_extra_hours + delta;
  }

  return cost;
}
