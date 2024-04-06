// File Data.cc
#include "Sched_Data.hh"
#include <fstream>
#include <iomanip>

Sched_Input::Sched_Input(string input_filename)
  : subject_max_hours_x_day(0),
    max_prof_weekly_hours(0),
    unavailability_violation_cost(0),
    max_subject_hours_x_day_violation_cost(0),
    max_prof_weekly_hours_violation_cost(0),
    schedule_contiguity_violation_cost(0)
{  
  unsigned input_unsigned;
  unsigned i;
  string input_buffer;

  // Open input file
  ifstream is(input_filename);
  if(!is)
  {
    cerr << "Cannot open input file " << input_filename << endl;
    exit(1);
  }
  

  is >> input_buffer;
  while (!is.eof() && input_buffer[0] == '$')
  {
    if (input_buffer == "$orario")
    {
      is >> input_buffer;

      while (!is.eof() && input_buffer[0] != '$')
      {
        if (input_buffer == "Days")
          is >> n_days;

        if (input_buffer == "HoursXDay")
          is >> n_hours_x_day;

        if (input_buffer == "SubjectMaxHoursXDay")
          is >> subject_max_hours_x_day;

        if (input_buffer == "ProfMaxWeeklyHours")
          is >> max_prof_weekly_hours;

        is >> input_buffer;
      }

      // If SubjectMaxHoursXDay not specified, that limit is disabled
      if (subject_max_hours_x_day == 0)
      {
        subject_max_hours_x_day = n_hours_x_day;
      }
      
      // If ProfMaxWeeklyHours not specified, that limit is disabled
      if (max_prof_weekly_hours == 0)
      {
        max_prof_weekly_hours = n_hours_x_day * n_days;
      }
    }
    else if (input_buffer == "$materie")
    {
      is >> input_buffer;

      while (!is.eof() && input_buffer[0] != '$')
      {
        subject_name.push_back(input_buffer);

        is >> input_unsigned;
        n_hours_x_subject.push_back(input_unsigned);

        is >> input_buffer;
      }

      n_subjects = subject_name.size();

    }
    else if (input_buffer == "$professori")
    {
      is >> input_buffer;

      while (!is.eof() && input_buffer[0] != '$')
      {
        prof_name.push_back(input_buffer);

        is >> input_buffer;
        
        i = 0;
        while (i < n_subjects && subject_name[i] != input_buffer)
          i++;

        if(i == n_subjects)
        { 
          cerr << "Inexistent subject" << endl;
          exit(1);
        }

        prof_subject.push_back(i);

        is >> input_buffer;

        if (input_buffer == "lun")
          prof_unavailability.push_back(lun);
        else if (input_buffer == "mar")
          prof_unavailability.push_back(mar);
        else if (input_buffer == "mer")
          prof_unavailability.push_back(mer);
        else if (input_buffer == "gio")
          prof_unavailability.push_back(gio);
        else if (input_buffer == "ven")
          prof_unavailability.push_back(ven);
        else if (input_buffer == "sab")
          prof_unavailability.push_back(sab);
        else if (input_buffer == "dom")
          prof_unavailability.push_back(dom);
        else
        {
          cerr << "Inexistent day of week" << endl;
          exit(1);
        }

        is >> input_buffer;
      }

      n_profs = prof_name.size();

      profs_x_subject.resize(n_subjects, vector<unsigned>());

      for (i = 0; i < n_profs; i++)
        profs_x_subject[prof_subject[i]].push_back(i);

    }
    else if (input_buffer == "$classi")
    {
      is >> input_buffer;

      while (!is.eof() && input_buffer[0] != '$')
      {
        class_name.push_back(input_buffer);

        is >> input_buffer;
      }

      n_classes = class_name.size();
    }
    else if (input_buffer == "$costi")
    {
      is >> input_buffer;

      while (!is.eof() && input_buffer[0] != '$')
      {
        if (input_buffer == "UnavailabilityViolation")
          is >> unavailability_violation_cost;

        if (input_buffer == "MaxSubjectHoursXDayViolation")
          is >> max_subject_hours_x_day_violation_cost;

        if (input_buffer == "MaxProfWeeklyHoursViolation")
          is >> max_prof_weekly_hours_violation_cost;

        if (input_buffer == "ScheduleContiguityViolation")
          is >> schedule_contiguity_violation_cost;

        is >> input_buffer;
      }

    }
    else
    {
      cerr << "Bad input file " << endl;
      exit(1);
    }
  }
}

ostream& operator<<(ostream& os, const Sched_Input& in)
{
  unsigned i;

  os << "$orario" << endl;
  os << "Days " << in.n_days << endl;
  os << "HoursXDay " << in.n_hours_x_day << endl;
  os << "SubjectMaxHoursXDay " << in.subject_max_hours_x_day << endl;
  os << "ProfMaxWeeklyHours " << in.max_prof_weekly_hours << endl;
  os << endl;

  os << "$materie" << endl;
  for (i = 0; i < in.n_subjects; i++)
    os << in.subject_name[i]<< " " << in.n_hours_x_subject[i] << endl;
  os << endl;

  os << "$professori" << endl;
  for (i = 0; i < in.n_profs; i++)
  {
    os << in.prof_name[i] << " " << in.subject_name[in.prof_subject[i]] << " ";

    switch (in.prof_unavailability[i])
    {
    case lun:
      os << "lun";
      break;

    case mar:
      os << "mar";
      break;

    case mer:
      os << "mer";
      break;

    case gio:
      os << "gio";
      break;

    case ven:
      os << "ven";
      break;

    case sab:
      os << "sab";
      break;

    case dom:
      os << "dom";
      break;
    }

    os << endl;
  }
  
  os << endl;

  os << "$classi" << endl;
  for (i = 0; i < in.n_classes; i++)
    os << in.class_name[i] << endl;
  os << endl;


  os << "$costi" << endl;
  os << "UnavailabilityViolation " << in.unavailability_violation_cost << endl;
  os << "MaxSubjectHoursXDayViolation " << in.max_subject_hours_x_day_violation_cost << endl;
  os << "MaxProfWeeklyHoursViolation " << in.max_prof_weekly_hours_violation_cost << endl;
  os << "ScheduleContiguityViolation " << in.schedule_contiguity_violation_cost << endl;

  os << endl;

  return os;
}

string Sched_Input::Day_Name(days d) const
{
  switch(d)
  {
    case lun:
      return "lun";

    case mar:
      return "mar";

    case mer:
      return "mer";

    case gio:
      return "gio";

    case ven:
      return "ven";

    case sab:
      return "sab";

    case dom:
      return "dom";
    
    default:
      return "Invalid day";
  }
}

void Sched_Input::Print(ostream& os) const
{
  unsigned s;
  
  os << "======================" << endl;
  os << "Timetable parameters" << endl;
  os << "======================" << endl;
  os << "Days: " << n_days << endl;
  os << "HoursXDay: " << n_hours_x_day << endl;
  os << "----------------------" << endl;
  
  os << endl << endl;

  os << "======================" << endl;
  os << "Subjects" << endl;
  os << "======================" << endl;
  os << "Subjects count: " << n_subjects << endl;
  os << "----------------------" << endl;
  os << "Subject name / Weekly hours" << endl;
  os << "----------------------" << endl;
  for (s = 0; s < n_subjects; s++)
  {
    os << subject_name[s] << " / " << n_hours_x_subject[s] << endl;
  }

  os << "----------------------" << endl;

  os << endl << endl;

  os << "======================" << endl;
  os << "Professors" << endl;
  os << "======================" << endl;
  os << "Professors count: " << n_profs << endl;
  os << "----------------------" << endl;
  os << "Name / Subject / Unavailability" << endl;
  os << "----------------------" << endl;
  for (s = 0; s < prof_name.size(); s++)
  {
    os << prof_name[s] << " / ";

    os << subject_name[prof_subject[s]] << " / ";

    switch (prof_unavailability[s])
    {
      case lun:
        os << "lun";
        break;
      
      case mar:
        os << "mar";
        break;

      case mer:
        os << "mer";
        break;

      case gio:
        os << "gio";
        break;

      case ven:
        os << "ven";
        break;

      case sab:
        os << "sab";
        break;

      case dom:
        os << "dom";
        break;
    }

    os << endl;
  }

  os << "----------------------" << endl;

  os << endl << endl;

  os << "======================" << endl;
  os << "Constraints and Violations cost" << endl;
  os << "======================" << endl;
  os << "Maximum daily subject hours: " << subject_max_hours_x_day << endl;
  os << "Prof Maximum weekly hours: " << max_prof_weekly_hours << endl;
  os << "Unavailability violation cost: " << unavailability_violation_cost << endl;
  os << "Max subject hours per day violation cost: " << max_subject_hours_x_day_violation_cost << endl;
  os << "Prof Maximum weekly hours violation cost: " << max_prof_weekly_hours_violation_cost << endl;
  os << "Schedule contiguity violation cost: " << schedule_contiguity_violation_cost << endl;
  os << "----------------------" << endl;
}

bool operator==(const Sched_Output& out1, const Sched_Output& out2)
{
  if (
    //&out1.in == &out2.in &&
    out1.schedule_class == out2.schedule_class &&
    out1.class_profs == out2.class_profs &&
    out1.daily_subject_assigned_hours == out2.daily_subject_assigned_hours &&
    out1.weekly_subject_assigned_hours == out2.weekly_subject_assigned_hours &&

    out1.schedule_prof == out2.schedule_prof &&
    out1.prof_weekly_hours == out2.prof_weekly_hours &&
    out1.prof_day_off == out2.prof_day_off)
  {
    return true;
  }

  return false;
}

Sched_Output::Sched_Output(const Sched_Input& my_in)
  : in(my_in),
  schedule_class(in.N_Classes(), vector(in.N_Days(), vector<int>(in.N_HoursXDay(), -1))),
  class_profs(in.N_Classes(), vector<int>(in.N_Subjects(), -1)),
  daily_subject_assigned_hours(in.N_Classes(), vector(in.N_Days(), vector<unsigned>(in.N_Subjects(), 0))),
  weekly_subject_assigned_hours(in.N_Classes(), vector<unsigned>(in.N_Subjects(), 0)),

  schedule_prof(in.N_Profs(), vector(in.N_Days(), vector<int>(in.N_HoursXDay(), -1))),
  prof_weekly_hours(in.N_Profs(), 0),
  prof_day_off(in.N_Profs())
{
  for (unsigned i = 0; i < in.N_Profs(); i++)
    prof_day_off[i] = (int)in.ProfUnavailability(i);
}

Sched_Output& Sched_Output::operator=(const Sched_Output& out)
{
  if (&in != &out.in)
    throw runtime_error("Impossible Sched_Output assignation. The field 'const Sched_Input& in' is different.");

  schedule_class = out.schedule_class;
  daily_subject_assigned_hours = out.daily_subject_assigned_hours;
  weekly_subject_assigned_hours = out.weekly_subject_assigned_hours;
  class_profs = out.class_profs;
  
  schedule_prof = out.schedule_prof;
  prof_weekly_hours = out.prof_weekly_hours;
  prof_day_off = out.prof_day_off;

  return *this;
}

void Sched_Output::Reset()
{
  unsigned c, p, s, d, h;

  for (c = 0; c < in.N_Classes(); c++)
  {
    for (d = 0; d < in.N_Days(); d++)
    {
      for (h = 0; h < in.N_HoursXDay(); h++)
        schedule_class[c][d][h] = -1;

      for (s = 0; s < in.N_Subjects(); s++)
      {
        daily_subject_assigned_hours[c][d][s] = 0;
      }
    }

    for (s = 0; s < in.N_Subjects(); s++)
    {
      class_profs[c][s] = -1;
      weekly_subject_assigned_hours[c][s] = 0;
    }
  }
    

  for (p = 0; p < in.N_Profs(); p++)
  {
    for (d = 0; d < in.N_Days(); d++)
      for (h = 0; h < in.N_HoursXDay(); h++)
        schedule_prof[p][d][h] = -1;

    prof_weekly_hours[p] = 0;
    
    prof_day_off[p] = (int)in.ProfUnavailability(p);
  }
}

pair<int, int> Sched_Output::GetFirstFreeHour(unsigned c, unsigned p)
{
  unsigned d, h;

  for (d = 0; d < in.N_Days(); d++)
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (IsClassHourFree(c, d, h) && IsProfHourFree(p, d, h))
        return make_pair(d, h);
  return make_pair(6, 0); //lo metto di domenica
}

void Sched_Output::Print(ostream& os) const
{
  unsigned s, c, p, d, h;

  unsigned print_subject_normalization_length = 0;
  unsigned print_classes_normalization_length = 0;

  // Get the best tabulation size for printing subjects inside timetable
  for (s = 0; s < in.N_Subjects(); s++)
  {
    if (in.Subject_Name(s).size() > print_subject_normalization_length)
      print_subject_normalization_length = in.Subject_Name(s).size();
  }
  print_subject_normalization_length++;

  if (print_subject_normalization_length < 4)
    print_subject_normalization_length = 4;

  // Get the best tabulation size for printing classes inside timetable
  for (c = 0; c < in.N_Classes(); c++)
  {
    if (in.Class_Name(c).size() > print_classes_normalization_length)
      print_classes_normalization_length = in.Class_Name(c).size();
  }
  print_classes_normalization_length++;

  if (print_classes_normalization_length < 4)
    print_classes_normalization_length = 4;

  // Print classes timetable
  os << "======================" << endl;
  os << "Classes timetable" << endl;
  os << "======================" << endl;

  os << left;
  for (c = 0; c < in.N_Classes(); c++)
  {
    os << in.Class_Name(c) << endl;
    os << "----------------------" << endl;
    os << setw(print_subject_normalization_length) << "Lun"
      << setw(print_subject_normalization_length) << "Mar"
      << setw(print_subject_normalization_length) << "Mer"
      << setw(print_subject_normalization_length) << "Gio"
      << setw(print_subject_normalization_length) << "Ven"
      << setw(print_subject_normalization_length) << "Sab"
      << setw(print_subject_normalization_length) << "Dom"
      << endl;

    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      for (d = lun; d < dom + 1; d++)
      {
        if (d < in.N_Days() && Class_Schedule(c, d, h) != -1)
          os << setw(print_subject_normalization_length) << in.Subject_Name(in.ProfSubject((unsigned)Class_Schedule(c, d, h)));
        else
          os << setw(print_subject_normalization_length) << "n/a";
      }

      os << endl;
    }

    os << "----------------------" << endl;
  }
  
  // Print professors timetable
  os << "======================" << endl;
  os << "Professors timetable" << endl;
  os << "======================" << endl;

  os << left;
  for (p = 0; p < in.N_Profs(); p++)
  {
    os << in.Prof_Name(p) << "   (" << in.Subject_Name(in.ProfSubject(p)) << ")" << endl;

    os << "Day off: ";

    switch (ProfAssignedDayOff(p))
    {
    case lun:
      os << "lun";
      break;

    case mar:
      os << "mar";
      break;

    case mer:
      os << "mer";
      break;

    case gio:
      os << "gio";
      break;

    case ven:
      os << "ven";
      break;

    case sab:
      os << "sab";
      break;

    case dom:
      os << "dom";
      break;

    case -1:
      os << "n/a";
      break;
    }

    os << endl;

    os << "----------------------" << endl;
    os << setw(print_classes_normalization_length) << "Lun"
      << setw(print_classes_normalization_length) << "Mar"
      << setw(print_classes_normalization_length) << "Mer"
      << setw(print_classes_normalization_length) << "Gio"
      << setw(print_classes_normalization_length) << "Ven"
      << setw(print_classes_normalization_length) << "Sab"
      << setw(print_classes_normalization_length) << "Dom"
      << endl;

    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      for (d = lun; d < dom + 1; d++)
      {
        if (d < in.N_Days() && Prof_Schedule(p, d, h) != -1)
          os << setw(print_classes_normalization_length) << in.Class_Name(Prof_Schedule(p, d, h));
        else
          os << setw(print_classes_normalization_length) << "n/a";
      }

      os << endl;
    }
    
    //os << "----------------------" << endl;
    os << "======================" << endl;
  }

  // Print violations and costs
  /*os << "======================" << endl;
  os << "Violations and costs" << endl;
  os << "======================" << endl;
  os << "Unavailability violations: " << ProfUnavailabilityViolations() << endl;
  os << "Maximum daily subject's hours violations: " << MaxSubjectHoursXDayViolations() << endl;
  os << "Maximum professor weekly hours violations: " << ProfMaxWeeklyHoursViolations() << endl;
  os << "Schedule contiguity violations: " << ScheduleContiguityViolations() << endl;
  os << "----------------------" << endl;
  os << "Unavailability violations cost: " << ProfUnavailabilityViolationsCost() << endl;
  os << "Maximum daily subject's hours violations cost: " << MaxSubjectHoursXDayViolationsCost() << endl;
  os << "Maximum professor weekly hours violations cost: " << ProfMaxWeeklyHoursViolationsCost() << endl;
  os << "Schedule contiguity violations cost: " << ScheduleContiguityViolationsCost() << endl;
  os << "----------------------" << endl;
  os << "Total cost: " << SolutionTotalCost() << endl;
  os << "----------------------" << endl;*/
}


void Sched_Output::PrintTAB(string output_filename) const
{
  unsigned c, p, d, h;

  ofstream os(output_filename);
  if(!os)
  {
    cerr << "Cannot open input file " << output_filename << endl;
    exit(1);
  }

  // Print classes timetable
  os << "======================" << endl;
  os << "Classes timetable" << endl;
  os << "======================" << endl;

  os << left;
  for (c = 0; c < in.N_Classes(); c++)
  {
    os << in.Class_Name(c) << endl;
    os << "----------------------" << endl;

    os << "Lun" << "\t" 
      << "Mar" << "\t"
      << "Mer" << "\t"
      << "Gio" << "\t"
      << "Ven" << "\t"
      << "Sab" << "\t"
      << "Dom" << endl;

    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      for (d = lun; d < dom + 1; d++)
      {
        if (d < in.N_Days() && Class_Schedule(c, d, h) != -1)
          os << in.Subject_Name(in.ProfSubject((unsigned)Class_Schedule(c, d, h))) << "\t";
        else
          os << "n/a" << "\t";
      }

      os << endl;
    }

    os << "----------------------" << endl;
  }
  
  // Print professors timetable
  os << "======================" << endl;
  os << "Professors timetable" << endl;
  os << "======================" << endl;

  os << left;
  for (p = 0; p < in.N_Profs(); p++)
  {
    os << in.Prof_Name(p) << "   (" << in.Subject_Name(in.ProfSubject(p)) << ")" << endl;

    os << "Day off: ";

    switch (ProfAssignedDayOff(p))
    {
    case lun:
      os << "lun";
      break;

    case mar:
      os << "mar";
      break;

    case mer:
      os << "mer";
      break;

    case gio:
      os << "gio";
      break;

    case ven:
      os << "ven";
      break;

    case sab:
      os << "sab";
      break;

    case dom:
      os << "dom";
      break;

    case -1:
      os << "n/a";
      break;
    }

    os << endl;

    os << "----------------------" << endl;
    os << "Lun" << "\t"
      << "Mar" << "\t"
      << "Mer" << "\t"
      << "Gio" << "\t"
      << "Ven" << "\t"
      << "Sab" << "\t"
      << "Dom" << endl;

    for (h = 0; h < in.N_HoursXDay(); h++)
    {
      for (d = lun; d < dom + 1; d++)
      {
        if (d < in.N_Days() && Prof_Schedule(p, d, h) != -1)
          os  << in.Class_Name(Prof_Schedule(p, d, h)) << "\t";
        else
          os << "n/a" << "\t";
      }

      os << endl;
    }
    
    os << "======================" << endl;
  }

  // Print violations and costs
  /*os << "======================" << endl;
  os << "Violations and costs" << endl;
  os << "======================" << endl;
  os << "Unavailability violations: " << ProfUnavailabilityViolations() << endl;
  os << "Maximum daily subject's hours violations: " << MaxSubjectHoursXDayViolations() << endl;
  os << "Maximum professor weekly hours violations: " << ProfMaxWeeklyHoursViolations() << endl;
  os << "Schedule contiguity violations: " << ScheduleContiguityViolations() << endl;
  os << "----------------------" << endl;
  os << "Unavailability violations cost: " << ProfUnavailabilityViolationsCost() << endl;
  os << "Maximum daily subject's hours violations cost: " << MaxSubjectHoursXDayViolationsCost() << endl;
  os << "Maximum professor weekly hours violations cost: " << ProfMaxWeeklyHoursViolationsCost() << endl;
  os << "Schedule contiguity violations cost: " << ScheduleContiguityViolationsCost() << endl;
  os << "----------------------" << endl;
  os << "Total cost: " << SolutionTotalCost() << endl;
  os << "----------------------" << endl;*/

}


bool Sched_Output::AssignHour(unsigned c, unsigned d, unsigned h, unsigned p)
{
  // If the hour isn't already free in class or prof schedule, return false
  if (Class_Schedule(c, d, h) != -1 || Prof_Schedule(p, d, h) != -1)
    return false;
  
  // Assign hour to class and prof schedule
  schedule_class[c][d][h] = p;
  schedule_prof[p][d][h] = c;
  
  // If there weren't hours of a specific subject assigned yet
  // the class "gains" a prof
  if (weekly_subject_assigned_hours[c][in.ProfSubject(p)] == 0)
    class_profs[c][in.ProfSubject(p)] = p;

  // Update Daily and weekly assigned hours
  weekly_subject_assigned_hours[c][in.ProfSubject(p)]++;
  daily_subject_assigned_hours[c][d][in.ProfSubject(p)]++;
  
  // Update prof weekly assigned hours
  prof_weekly_hours[p]++;

  ComputeProfDayOff(p);

  return true;
}


bool Sched_Output::FreeHour(unsigned c, unsigned d, unsigned h)
{
  unsigned p;

  // If the hour is already free return false
  if (Class_Schedule(c, d, h) == -1)
    return false;

  p = (unsigned)Class_Schedule(c, d, h);

  // Frees hour from class and prof schedule
  schedule_class[c][d][h] = -1;
  schedule_prof[p][d][h] = -1;

  // Update Daily and weekly assigned hours
  weekly_subject_assigned_hours[c][in.ProfSubject(p)]--;
  daily_subject_assigned_hours[c][d][in.ProfSubject(p)]--;

  // Update prof weekly assigned hours
  prof_weekly_hours[p]--;

  // If there are no more assigned hours of a specific subject
  // the class "loses" a prof
  if (weekly_subject_assigned_hours[c][in.ProfSubject(p)] == 0)
  {
    class_profs[c][in.ProfSubject(p)] = -1;
  }

  ComputeProfDayOff(p);

  return true;
}

bool Sched_Output::SwapHours(unsigned c1, unsigned d1, unsigned h1, unsigned c2, unsigned d2, unsigned h2)
{
  unsigned p1, p2;

  // If the hours are both free or assigned to the same professor, return false
  if (Class_Schedule(c1, d1, h1) == Class_Schedule(c2, d2, h2))
    return false;
  
  else if (Class_Schedule(c1, d1, h1) != -1 && Class_Schedule(c2, d2, h2) != -1)
  {
    p1 = Class_Schedule(c1, d1, h1);
    p2 = Class_Schedule(c2, d2, h2);

    // Is this check necessary?
    if (Prof_Schedule(p1, d2, h2) == -1 && Prof_Schedule(p2, d1, h1) == -1)
    {
      FreeHour(c1, d1, h1);
      FreeHour(c2, d2, h2);

      AssignHour(c1, d1, h1, p2);
      AssignHour(c2, d2, h2, p1);
    }
    else
      // Is this case possible ?
      return false;
  }
  else // Case (Class_Schedule(c1, d1, h1) != -1 && Class_Schedule(c2, d2, h2) == -1)
       //   or (Class_Schedule(c1, d1, h1) == -1 && Class_Schedule(c2, d2, h2) != -1)
  {
    if(Class_Schedule(c1, d1, h1) == -1 && Class_Schedule(c2, d2, h2) != -1)
    {
      swap<unsigned>(c1, c2);
      swap<unsigned>(d1, d2);
      swap<unsigned>(h1, h2);
    }

    p1 = Class_Schedule(c1, d1, h1);

    if (Prof_Schedule(p1, d2, h2) == -1)
    {
      FreeHour(c1, d1, h1);
      AssignHour(c2, d2, h2, p1);

      ComputeProfDayOff(p1);
    }
    else
      return false;
  }

  return true;
}

bool Sched_Output::SuitableProf(unsigned c, unsigned p)
{
  unsigned d, h;
  unsigned compatible_hours;

  compatible_hours = 0;
  
  for (d = 0; d < in.N_Days(); d++)
    for (h = 0; h < in.N_HoursXDay(); h++)
      if (IsProfHourFree(p, d, h) && IsClassHourFree(c ,d, h))
        compatible_hours++;
  
  return compatible_hours >= in.N_HoursXSubject(in.ProfSubject(p));
}

void Sched_Output::ComputeProfDayOff(unsigned p)
{
  unsigned d, h;
  bool school_day = false;

  // If the professor has multiple free days and one of these
  // is his unavailability day, we consider that as professor day off.
  for (h = 0; h < in.N_HoursXDay(); h++)
  {
    if (Prof_Schedule(p, in.ProfUnavailability(p), h) != -1)
    {
      school_day = true;
      break;
    }
  }
  
  if (!school_day)
  {
    prof_day_off[p] = (int)in.ProfUnavailability(p);
    return;
  }

  // ...If not, We consider another day as professor day off
  for (d = lun; d < in.N_Days(); d++)
  {
    if (d != in.ProfUnavailability(p))
    {
      school_day = false;

      for (h = 0; h < in.N_HoursXDay(); h++)
      {
        if (Prof_Schedule(p, d, h) != -1)
        {
          school_day = true;
          break;
        }
      }

      if (!school_day)
      {
        prof_day_off[p] = (int)d;
        return;
      }
    }
  }

  // Day off doesn't exist
  prof_day_off[p] = -1;
  return;
}

unsigned  Sched_Output::ProfUnavailabilityViolations() const
{
  unsigned p;
  unsigned solution_unavailability_violations = 0;
  

  for (p = 0; p < in.N_Profs(); p++)
    if (ProfAssignedDayOff(p) == -1 || (unsigned)ProfAssignedDayOff(p) != in.ProfUnavailability(p))
      solution_unavailability_violations++;

  return solution_unavailability_violations;
}

unsigned  Sched_Output::MaxSubjectHoursXDayViolations() const
{
  unsigned c, s, d;
  unsigned solution_max_subject_hours_x_day_violations = 0;

  for (c = 0; c < in.N_Classes(); c++)
    for (d = 0; d < in.N_Days(); d++)
      for (s = 0; s < in.N_Subjects(); s++)
      {
        if (daily_subject_assigned_hours[c][d][s] > in.SubjectMaxHoursXDay())
          solution_max_subject_hours_x_day_violations += daily_subject_assigned_hours[c][d][s] - in.SubjectMaxHoursXDay();
      }

  return solution_max_subject_hours_x_day_violations;
}

unsigned Sched_Output::ProfMaxWeeklyHoursViolations() const
{
  unsigned p;
  unsigned solution_max_prof_weekly_hours_violations = 0;

  for (p = 0; p < in.N_Profs(); p++)
  {
    if (prof_weekly_hours[p] > in.ProfMaxWeeklyHours())
      solution_max_prof_weekly_hours_violations += prof_weekly_hours[p] - in.ProfMaxWeeklyHours();
  }

  return solution_max_prof_weekly_hours_violations;
}

unsigned Sched_Output::ScheduleContiguityViolations() const
{
  unsigned c, s, d, h;
  int subject_last_position;
  unsigned contiguity_violations = 0;

  if (in.N_HoursXDay() < 3)
    return 0;

  for(c = 0; c < in.N_Classes(); c++)
    for (s = 0; s < in.N_Subjects(); s++)
      for (d = 0; d < in.N_Days(); d++)
      {
        subject_last_position = -1;

        for (h = 0; h < in.N_HoursXDay(); h++)
          if (Class_Schedule(c, d, h) != -1 && (unsigned)Class_Schedule(c, d, h) == s)
          {
            if (subject_last_position != -1 && h - subject_last_position > 1)
              contiguity_violations++;

            subject_last_position = h;
          }
      }

  return contiguity_violations;
}

ostream& operator<<(ostream& os, const Sched_Output& out)
{
  unsigned c, d, h;

  os << "$classes_schedule" << endl;
  for (c = 0; c < out.in.N_Classes(); c++)
  {
    for (d = 0; d < out.in.N_Days(); d++)
      for (h = 0; h < out.in.N_HoursXDay(); h++)
        os << out.Class_Schedule(c, d, h) << " ";

    os << endl;
  }
  
  os << endl;

  return os;
}

istream& operator>>(istream& is, Sched_Output& out)
{
  unsigned c, d, h;
  long p;
  string input_buffer;

  out.Reset();
  
  is >> input_buffer;
  while (!is.eof() && input_buffer[0] == '$')
  {
    if (input_buffer == "$classes_schedule")
    {
      for (c = 0; c < out.in.N_Classes(); c++)
        for (d = 0; d < out.in.N_Days(); d++)
          for (h = 0; h < out.in.N_HoursXDay(); h++)
          {
            is >> p;

            if (p != -1)
            {

              out.AssignHour(c, d, h, p);
            }
          }
    }
    else
    {
      cerr << "Bad input file " << endl;
      exit(1);
    }

    is >> input_buffer;
  }

  return is;
}
