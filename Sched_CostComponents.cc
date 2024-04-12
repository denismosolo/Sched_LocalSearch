// File Sched_CostComponents.cc
#include "Sched_Helpers.hh"

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

  for(c = 0; c < in.N_Classes(); c++)
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
 * Delta Cost Components Code - Sched_SwapHours
 ***************************************************************************/

int Sched_SwapHoursDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  unsigned h;
  int p1, p2;
  int cost = 0;

  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  // sottraggo i vecchi costi
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day_1, h) == p1 && mv.hour_1 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost--;  //  Il costo lo setta in automatico (weight)
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day_2, h) == p2 && mv.hour_2 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost--;  //  Il costo lo setta in automatico (weight)
  }

  // sommo i nuovi costi
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day_2, h) == p1 && mv.hour_2 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost++;  //  Il costo lo setta in automatico (weight)
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day_1, h) == p2 && mv.hour_1 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost++;  //  Il costo lo setta in automatico (weight)
  }

  return cost;
}

int Sched_SwapHoursDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  int p1, p2;
  int cost = 0;

  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);

  // sottraggo vecchi costi
  if (p1 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_1, in.ProfSubject(p1)) > in.SubjectMaxHoursXDay())
    cost--;
  
  if (p2 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_2, in.ProfSubject(p2)) > in.SubjectMaxHoursXDay())
    cost--;

  // sommo nuovi costi
  if (p2 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_1, in.ProfSubject(p2)) >= in.SubjectMaxHoursXDay()) // aggiungo il caso = perché se sono al limite e ne aggiungo un'altra causo una violazione
    cost++;

  if (p1 != -1 && out.DailySubjectAssignedHours(mv._class, mv.day_2, in.ProfSubject(p1)) >= in.SubjectMaxHoursXDay())
    cost++;

  return cost;
}

int Sched_SwapHoursDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  // Questa componente di costo non viene mai intaccata dalla mossa perché le ore settimanali assegnate ai prof non cambiano
  return 0;
}

int Sched_SwapHoursDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  unsigned h, violations_old, violations_new;
  int p1, p2, last_old_1, last_old_2, last_new_1, last_new_2;
  int last_1, last_2;

  violations_old = 0;
  violations_new = 0;
  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);
  last_old_1 = -1;
  last_old_2 = -1;
  last_new_1 = -1;
  last_new_2 = -1;
  last_1 = -1;
  last_2 = -1;

  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    // Devo verificare le nuove violazioni sia per la materia precedente che per quella nuova, per entrambe le giornate
    // Violazioni attuali materia precedente
    if (out.Class_Schedule(mv._class, mv.day_1, h) != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1)
    {
      if (last_old_1 != -1 && h - last_old_1 > 1)
        violations_old++;

      last_1 = h;
    }

    // Violazioni attuali materia precedente
    if (out.Class_Schedule(mv._class, mv.day_2, h) != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        violations_old++;

      last_2 = h;
    }

    // Violazioni attuali nuova materia
    if (out.Class_Schedule(mv._class, mv.day_1, h) != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1 && h != mv.hour_1)
    {
      if (last_old_1 != -1 && h - last_old_1 > 1)
        violations_old++;

      last_1 = h;
    }

    // Violazioni attuali nuova materia
    if (out.Class_Schedule(mv._class, mv.day_2, h) != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2 && h != mv.hour_2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        violations_old++;

      last_2 = h;
    }
    
    // Violazioni dovute alla rimozione della materia precedente
    if (out.Class_Schedule(mv._class, mv.day_1, h) != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1 && h != mv.hour_1)
    {
      if (last_old_1 != -1 && h - last_old_1 > 1)
        violations_new++;

      last_old_1 = h;
    }

    // Violazioni dovute alla rimozione della materia precedente
    if (out.Class_Schedule(mv._class, mv.day_2, h) != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2 && h != mv.hour_2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        violations_new++;

      last_old_2 = h;
    }

    // Violazioni dovute all'inserimento della nuova materia
    if (out.Class_Schedule(mv._class, mv.day_1, h) != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p2)
    {
      if (last_new_1 != -1 && h - last_new_1 > 1)
        violations_new++;

      last_new_1 = h;
    }

    // Violazioni dovute all'inserimento della nuova materia
    if (out.Class_Schedule(mv._class, mv.day_2, h) != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p1)
    {
      if (last_new_2 != -1 && h - last_new_2 > 1)
        violations_new++;

      last_new_2 = h;
    }
  }

  return violations_new - violations_old;
}

int Sched_SwapHoursDeltaCompleteSolution::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  // Questa componente di costo non viene mai intaccata dalla mossa perché le ore settimanali assegnate ai prof non cambiano
  return 0;
}