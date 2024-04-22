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

  if (mv.day_1 == mv.day_2)
    return 0;

  // sottraggo i vecchi costi
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p1, mv.day_1, h) && mv.hour_1 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost--;  //  Il costo lo setta in automatico (weight)
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p2, mv.day_2, h) && mv.hour_2 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost--;  //  Il costo lo setta in automatico (weight)
  }

  // sommo i nuovi costi
  if (p1 != -1 && in.ProfUnavailability(p1) == mv.day_2)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p1, mv.day_2, h) && mv.hour_2 != h) // se trovo il prof in un'altra ora della stessa giornata
        break;
    if (h == in.N_HoursXDay()) // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata
      cost++;  //  Il costo lo setta in automatico (weight)
  }

  if (p2 != -1 && in.ProfUnavailability(p2) == mv.day_1)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (!out.IsProfHourFree(p2, mv.day_1, h) && mv.hour_1 != h) // se trovo il prof in un'altra ora della stessa giornata
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

  if (mv.day_1 == mv.day_2)
    return 0;

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

  // per escludere l'eventuale errore sulla first move nel caso la prima mossa non sia feasible
  // in realtà questo caso è escluso da feasible move, first_move la accetta ma non la esegue
  if ( p1 == p2)
    return 0;

  // Scorro l'orario per ora valutando solo le giornate coinvolte dallo scambio di ore
  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    // Violazioni attuali della "materia 1" nella giornata coinvolta nello scambio
    if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1)
    {
      if(last_old_1 != -1 && h - last_old_1 > 1)
        cost--;
      last_old_1 = h;
    }
    
    // Violazioni attuali della "materia 2" nella giornata coinvolta nello scambio
    if (p2 != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p2)
    {
      if (last_old_2 != -1 && h - last_old_2 > 1)
        cost--;
      last_old_2 = h;
    }

    // Violazioni della "materia 1" già presenti nella giornata di arrivo - ma solo se giorni diversi
    if (mv.day_1 != mv.day_2)
    {
      if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_2, h) == p1)
      {
        if(last_arr_1 != -1 && h - last_arr_1 > 1)
          cost--;
        last_arr_1 = h;
      }
      
      // Violazioni della "materia 2" già presenti nella giornata di arrivo
      if (p2 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p2)
      {
        if (last_arr_2 != -1 && h - last_arr_2 > 1)
          cost--;
        last_arr_2 = h;
      }
    }

    // Violazioni della "materia 1" nella giornata attualmente occupata dalla "materia 2" se lo scambio viene effettuato.
    // NOTA: l'ora nella giornata 2 è attualmente occupata dalla materia 1, oppure è l'ora che verrà occupata dalla "materia 1" 
    // se lo scambio viene effettuato.
    if (p1 != -1 && (out.Class_Schedule(mv._class, mv.day_2, h) == p1 || h == mv.hour_2) && !(mv.day_1 == mv.day_2 && h == mv.hour_1))
    {
      if (last_new_1 != -1 && h - last_new_1 > 1)
        cost++;
      last_new_1 = h;
    }

    // Violazioni della "materia 2" nella giornata attualmente occupata dalla "materia 1" se lo scambio viene effettuato.
    if (p2 != -1 && (out.Class_Schedule(mv._class, mv.day_1, h) == p2 || h == mv.hour_1) && !(mv.day_1 == mv.day_2 && h == mv.hour_2))
    {
      if (last_new_2 != -1 && h - last_new_2 > 1)
        cost++;
      last_new_2 = h;
    }

    // Violazioni della "materia 1" residue nella giornata di partenza. solo giorni diversi
    if (mv.day_1 != mv.day_2)
    {
      if (p1 != -1 && out.Class_Schedule(mv._class, mv.day_1, h) == p1 && h != mv.hour_1)
      {
        if (last_res_1 != -1 && h - last_res_1 > 1)
          cost++;
        last_res_1 = h;
      }

      // Violazioni della "materia 2" residue nella giornata di partenza.
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

// Se la mossa è feasible va a risolvere una violazione hard e ciò è sempre superiore a qualsiasi costo soft

int Sched_AssignProfDeltaProfUnavailability::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;

  // Non ho vecchi costi di indisponibilità perché sto aggiungendo il professore
  if (in.ProfUnavailability(mv.prof) == mv.day)
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Class_Schedule(mv._class, mv.day, h) == mv.prof && h != mv.hour) // se trovo il prof in un'altra ora della stessa giornata
        return 0;

    // sono arrivato in fondo al ciclo quindi il prof è presente 1 ora sola nella giornata --> ottengo una (1) violazione
    return 1;
  }

  // Il giorno in cui viene aggiunto il prof non è quello della sua giornata libera richiesta
  return 0;
}

int Sched_AssignProfDeltaMaxSubjectHoursXDay::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  // Non ho vecchi costi da sottrarre

  // sommo nuovi costi
  if (out.DailySubjectAssignedHours(mv._class, mv.day, in.ProfSubject(mv.prof)) >= in.SubjectMaxHoursXDay())
    return 1;

  return 0;
}

int Sched_AssignProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{

  if (out.ProfWeeklyAssignedHours(mv.prof) >= in.ProfMaxWeeklyHours())
    return 1;

  return 0;
}

int Sched_AssignProfDeltaScheduleContiguity::ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const
{
  unsigned h;
  int last_hour = -1;

  // Se pongo la nuova ora successiva ad una della stessa materia il costo resta invariato
  if (mv.hour > 0)
    if (out.Class_Schedule(mv._class, mv.day, mv.hour-1) == mv.prof)
      return 0;

  // Se pongo la nuova ora precedente ad una della stessa materia il costo resta invariato
  if (mv.hour < in.N_HoursXDay()-1)
    if (out.Class_Schedule(mv._class, mv.day, mv.hour+1) == mv.prof)
      return 0;

  // Caso in cui pongo la nuova ora lontano dalle altre ore della stessa materia -> in qualsiasi caso introduco una delta nel costo pari ad 1
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
    if (out.Prof_Schedule(prof_1, in.ProfUnavailability(prof_1), h) != -1 && out.Prof_Schedule(prof_1, in.ProfUnavailability(prof_1), h) != mv.class_1)
      break; // il prof_1 è impegnato con anche un'altra classe nel suo giorno libero => non modifico le violazioni per prof_1
  if (h == in.N_HoursXDay()) // il prof_1 nella giornata libera ha al limite solo la classe_1 => possono esserci modifiche alle violazioni
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Prof_Schedule(prof_2, in.ProfUnavailability(prof_1), h) == mv.class_2)
        break; //  viene introdotta una lezione nel giorno libero di prof_1
    // Non vengono introdotte lezioni nel giorno libero di prof_1 E c'è una violazione (so già che è dovuta solo a classe_1) => risolvo
    if (h == in.N_HoursXDay() && out.ProfAssignedDayOff(prof_1) != in.ProfUnavailability(prof_1))
      cost--;
    // Vengono introdotte lezioni nel giorno libero di prof_1 E non c'era già violazione => introduco
    else if (h < in.N_HoursXDay() && out.ProfAssignedDayOff(prof_1) == in.ProfUnavailability(prof_1))
      cost++;
    // else => non cambio nulla
  }

  for (h = 0; h < in.N_HoursXDay(); h++)
    if (out.Prof_Schedule(prof_2, in.ProfUnavailability(prof_2), h) != -1 && out.Prof_Schedule(prof_2, in.ProfUnavailability(prof_2), h) != mv.class_2)
      break; // il prof_1 è impegnato con anche un'altra classe nel suo giorno libero
  if (h == in.N_HoursXDay()) // il prof_1 nella giornata libera ha solo la classe_1
  {
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (out.Prof_Schedule(prof_1, in.ProfUnavailability(prof_2), h) == mv.class_1)
        break; //  viene introdotta una lezione nel giorno libero di prof_2
    // Non vengono introdotte lezioni nel giorno libero di prof_2 E c'è una violazione (so già che è dovuta solo a classe_2) => risolvo
    if (h == in.N_HoursXDay() && out.ProfAssignedDayOff(prof_2) != in.ProfUnavailability(prof_2))
      cost--;
    // Vengono introdotte lezioni nel giorno libero di prof_2 E non c'era già violazione => introduco
    else if (h < in.N_HoursXDay() && out.ProfAssignedDayOff(prof_2) == in.ProfUnavailability(prof_2))
      cost++;
    // else => non cambio nulla
  }
  return cost;
}

int Sched_SwapProfDeltaProfMaxWeeklyHours::ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const
{
  unsigned p1, p2;
  int p1_extra_hours, p2_extra_hours;
  int cost = 0;

  p1 = out.Subject_Prof(mv.class_1, mv.subject);
  p2 = out.Subject_Prof(mv.class_2, mv.subject);

  p1_extra_hours = out.ProfWeeklyAssignedHours(p1) - in.ProfMaxWeeklyHours();
  p2_extra_hours = out.ProfWeeklyAssignedHours(p2) - in.ProfMaxWeeklyHours();

  // il costo è != 0 solo se una delle due materie non è completamente assegnata per la classe
  if (out.WeeklySubjectResidualHours(mv.class_1, mv.subject) > 0)
  {
    if (p2_extra_hours > 0)
    {
      if (p2_extra_hours > out.WeeklySubjectResidualHours(mv.class_1, mv.subject))
        cost -= out.WeeklySubjectResidualHours(mv.class_1, mv.subject);
      else
        cost -= p2_extra_hours;
    }

    if (p1_extra_hours > 0)
      cost += out.WeeklySubjectResidualHours(mv.class_1, mv.subject);
    else
      if (p1_extra_hours + out.WeeklySubjectResidualHours(mv.class_1, mv.subject) > 0)
        cost += (p1_extra_hours + out.WeeklySubjectResidualHours(mv.class_1, mv.subject));
  }

  if (out.WeeklySubjectResidualHours(mv.class_2, mv.subject) > 0)
  {
    if (p1_extra_hours > 0)
    {
      if (p1_extra_hours > out.WeeklySubjectResidualHours(mv.class_2, mv.subject))
        cost -= out.WeeklySubjectResidualHours(mv.class_2, mv.subject);
      else
        cost -= p1_extra_hours;
    }

    if (p2_extra_hours > 0)
      cost += out.WeeklySubjectResidualHours(mv.class_2, mv.subject);
    else
      if (p2_extra_hours + out.WeeklySubjectResidualHours(mv.class_2, mv.subject) > 0)
        cost += (p2_extra_hours + out.WeeklySubjectResidualHours(mv.class_2, mv.subject));
  }

  return cost;
}
