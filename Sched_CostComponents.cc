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

int Sched_SwapHoursDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const
{
  unsigned h, violations_old, violations_new;
  int p1, p2, last_old_1, last_old_2, last_new_1, last_new_2, last_1, last_2, last_prev_1, last_prev_2;

  violations_old = 0;
  violations_new = 0;
  p1 = out.Class_Schedule(mv._class, mv.day_1, mv.hour_1);
  p2 = out.Class_Schedule(mv._class, mv.day_2, mv.hour_2);
  last_1 = -1;
  last_2 = -1;
  last_old_1 = -1;
  last_old_2 = -1;
  last_new_1 = -1;
  last_new_2 = -1;
  last_prev_1 = -1;
  last_prev_2 = -1;

  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    // Devo verificare le nuove violazioni sia per la materia precedente che per quella nuova, per entrambe le giornate
    // Violazioni attuali materia precedente
    if (out.Class_Schedule(mv._class, mv.day_1, h) != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1)
    {
      if (last_old_1 != -1 && h - last_old_1 > 1)
        violations_old++;

      last_prev_1 = h;
    }

    // Violazioni attuali materia precedente
    if (out.Class_Schedule(mv._class, mv.day_2, h) != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        violations_old++;

      last_prev_2 = h;
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


/***************************************************************************
 * Delta Cost Components Code - Sched_AssignProf
 ***************************************************************************/

int Sched_AssignProfDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;
  int cost = 0;

  // Non ho vecchi costi di indisponibilità perché sto aggiungendo il professore
  if (mv.prof != -1 && in.ProfUnavailability(mv.prof) == mv.day)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day, h) == mv.prof && mv.hour != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata --> introduco una violazione
      cost = 1;  //  Il costo lo setta in automatico (weight)
  }

  return cost;
}

int Sched_AssignProfDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  int cost = 0;

  // Non ho vecchi costi da sottrarre

  // sommo nuovi costi
  if (mv.prof != -1 && out.DailySubjectAssignedHours(mv._class, mv.day, in.ProfSubject(mv.prof)) >= in.SubjectMaxHoursXDay())
    cost = 1;

  return cost;
}

int Sched_AssignProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  int cost = 0;

  if (mv.prof != -1 && out.ProfWeeklyAssignedHours(mv.prof) >= in.ProfMaxWeeklyHours())
    cost = 1;

  return cost;
}

int Sched_AssignProfDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;
  int cost = 0;

  // se non è la prima o l'ultima ora
  if (mv.hour > 0 && mv.hour < in.N_HoursXDay()-1)
    if (out.Class_Schedule(mv._class, mv.day, mv.hour-1) == mv.prof && out.Class_Schedule(mv._class, mv.day, mv.hour+1) == mv.prof)
      cost--;

  for (h = 0; h < in.N_HoursXDay(); h++)
    if (out.Class_Schedule(mv._class, mv.day, h) == mv.prof && abs((int)(h - mv.hour)) > 1)
      cost++;

  return cost;
}

int Sched_AssignProfDeltaCompleteSolution::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  int cost = 0;

  if (mv.prof != -1)
    cost = -1;

  return cost;
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

  if (out.ProfAssignedDayOff(prof_1) != in.ProfUnavailability(prof_1))
    cost--;

  if (out.ProfAssignedDayOff(prof_2) != in.ProfUnavailability(prof_2))
    cost--;

  for (h = 0; h < in.N_HoursXDay(); h++)
    if (!out.IsProfHourFree(prof_1, out.ProfAssignedDayOff(prof_2), h))
    {
      cost++;
      break;
    }
    
  for (h = 0; h < in.N_HoursXDay(); h++)
    if (!out.IsProfHourFree(prof_2, out.ProfAssignedDayOff(prof_1), h))
    {
      cost++;
      break;
    }
  return cost;
}

int Sched_SwapProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned prof_1, prof_2;
  int viol_1, viol_2;

  prof_1 = out.Subject_Prof(mv.class_1, mv.subject);
  prof_2 = out.Subject_Prof(mv.class_2, mv.subject);

  viol_1 = out.ProfWeeklyAssignedHours(prof_1) - in.ProfMaxWeeklyHours();
  viol_2 = out.ProfWeeklyAssignedHours(prof_2) - in.ProfMaxWeeklyHours();

  viol_1 -= out.WeeklySubjectAssignedHours(mv.class_1, mv.subject);
  viol_2 -= out.WeeklySubjectAssignedHours(mv.class_2, mv.subject);

  viol_1 += out.WeeklySubjectAssignedHours(mv.class_2, mv.subject);
  viol_2 += out.WeeklySubjectAssignedHours(mv.class_1, mv.subject);
  
  return viol_1 + viol_2;
}