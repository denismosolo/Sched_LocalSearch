// File Data.hh
#ifndef DATA_HH
#define DATA_HH

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>

using namespace std;

enum days
{
  lun,
  mar,
  mer,
  gio,
  ven,
  sab,
  dom
};

class Sched_Input 
{
  // Output operator
  friend ostream& operator<<(ostream& os, const Sched_Input& in);
  
public:
  // Constructor
  Sched_Input(string file_name);

  // Schedule data selectors
  unsigned N_Days() const { return n_days; }
  unsigned N_HoursXDay() const { return n_hours_x_day; }
  string Day_Name(days d) const;

  // Professors' data selectors
  unsigned N_Profs() const { return n_profs; }
  string Prof_Name(unsigned p) const { return prof_name[p]; }
  unsigned ProfUnavailability(unsigned p) const { return prof_unavailability[p]; }
  unsigned ProfSubject(unsigned p) const { return prof_subject[p]; }

  unsigned N_ProfsXSubject(unsigned s) const { return profs_x_subject[s].size(); }
  unsigned SubjectProf(unsigned s, unsigned i) const { return profs_x_subject[s][i]; } // 'i' because it's an iterator not the prof's name
  vector<vector<unsigned>> GetSubjectProfs() const { return profs_x_subject; }  // get all profs divided by subject
  vector<unsigned> GetSubjectProfsVector(unsigned s) const { return profs_x_subject[s]; }

  // Classes' data selectors
  unsigned N_Classes() const { return n_classes; }
  string Class_Name(unsigned c) const { return class_name[c]; }

  // Subjects' data selector
  unsigned N_Subjects() const { return n_subjects; }
  string Subject_Name(unsigned s) const { return subject_name[s]; }
  unsigned N_HoursXSubject(unsigned s) const { return n_hours_x_subject[s]; }
  
  // Cost threshold selectors
  unsigned ProfMaxWeeklyHours() const { return max_prof_weekly_hours; }
  unsigned SubjectMaxHoursXDay() const { return subject_max_hours_x_day; }

  // Print methods
  void Print(ostream& os) const;

  // Cost selectors
  unsigned UnavailabilityViolationCost() const { return unavailability_violation_cost; }
  unsigned MaxSubjectHoursXDayViolationCost() const { return max_subject_hours_x_day_violation_cost; }
  unsigned MaxProfWeeklyHoursViolationCost() const { return max_prof_weekly_hours_violation_cost; }
  unsigned ScheduleContiguityViolationCost() const { return schedule_contiguity_violation_cost; }

  private:

  // Schedule parameters
  unsigned n_days;
  unsigned n_hours_x_day;
  unsigned n_subjects;
  vector<string> subject_name;
  vector<unsigned> n_hours_x_subject;

  // Professors data
  unsigned n_profs;
  vector<string> prof_name;
  vector<unsigned> prof_subject;
  vector<unsigned> prof_unavailability;

  // Classes data
  unsigned n_classes;
  vector<string> class_name;

  // Subject data structures
  vector<vector<unsigned>> profs_x_subject;   //profs for each subject
  
  // Cost thresholds
  unsigned subject_max_hours_x_day;
  unsigned max_prof_weekly_hours;

  // Cost components
  unsigned unavailability_violation_cost;
  unsigned max_subject_hours_x_day_violation_cost;
  unsigned max_prof_weekly_hours_violation_cost;
  unsigned schedule_contiguity_violation_cost;
};


class Sched_Output 
{
  // Output and input operator (useful to save and restore solution states)
  friend ostream& operator<<(ostream& os, const Sched_Output& out);
  friend istream& operator>>(istream& is, Sched_Output& out);

  // Equality operator
  friend bool operator==(const Sched_Output& out1, const Sched_Output& out2);

public:
  // Constructor
  Sched_Output(const Sched_Input& i);

  // Assignment operator
  Sched_Output& operator=(const Sched_Output& out);
  void Reset();

  // Classes selectors
  int Class_Schedule(unsigned c, unsigned d, unsigned h) const { return schedule_class[c][d][h]; }  // Get the class' schedule
                                                                                                    // NOTE: Return the prof assigned to an hour
  int Subject_Prof(unsigned c, unsigned s) const { return class_profs[c][s]; } // Get the subject's prof of a specific class
  unsigned DailySubjectAssignedHours(unsigned c, unsigned d, unsigned s) const { return daily_subject_assigned_hours[c][d][s]; }
  unsigned WeeklySubjectAssignedHours(unsigned c, unsigned s) const { return weekly_subject_assigned_hours[c][s]; }
  unsigned WeeklySubjectResidualHours(unsigned c, unsigned s) const { return in.N_HoursXSubject(s) - weekly_subject_assigned_hours[c][s]; }
  [[deprecated("used in older Greedy versions now removed")]] pair<int, int> GetFirstFreeHour(unsigned c, unsigned p);  //return first hour free both for class and prof - constrains not considered

  // Profs selectors
  int Prof_Schedule(unsigned p, unsigned d, unsigned h) const { return schedule_prof[p][d][h]; }  // Get the prof's schedule
  unsigned ProfWeeklyAssignedHours(unsigned p) const { return prof_weekly_hours[p]; }
  int ProfAssignedDayOff(unsigned p) const { return prof_day_off[p]; } // Get prof day off

  // Costs and violations computing methods
  [[deprecated("moved to relative cost component class")]] unsigned ProfUnavailabilityViolations() const;
  [[deprecated("moved to relative cost component class")]] unsigned MaxSubjectHoursXDayViolations() const;
  [[deprecated("moved to relative cost component class")]] unsigned ProfMaxWeeklyHoursViolations() const;
  [[deprecated("moved to relative cost component class")]] unsigned ScheduleContiguityViolations() const;

  [[deprecated("moved to relative cost component class")]] unsigned ProfUnavailabilityViolationsCost() const { return ProfUnavailabilityViolations() * in.UnavailabilityViolationCost(); }
  [[deprecated("moved to relative cost component class")]] unsigned MaxSubjectHoursXDayViolationsCost() const { return MaxSubjectHoursXDayViolations() * in.MaxSubjectHoursXDayViolationCost(); }
  [[deprecated("moved to relative cost component class")]] unsigned ProfMaxWeeklyHoursViolationsCost() const { return ProfMaxWeeklyHoursViolations() * in.MaxProfWeeklyHoursViolationCost(); }
  [[deprecated("moved to relative cost component class")]] unsigned ScheduleContiguityViolationsCost() const { return ScheduleContiguityViolations() * in.ScheduleContiguityViolationCost(); }

  //unsigned SolutionTotalCost() const { return ProfUnavailabilityViolationsCost() + MaxSubjectHoursXDayViolationsCost() + ProfMaxWeeklyHoursViolationsCost() + ScheduleContiguityViolationsCost(); }

  // Print methods
  void Print(ostream& os) const;  // Print output class in a user-readable manner
  void PrintTAB(string output_filename) const; // same as Print but with TABs instead of spaces and dump in .txt for easy import in Excel

  // Schedule management methods
  void AssignHour(unsigned c, unsigned d, unsigned h, unsigned p);
  void FreeHour(unsigned c, unsigned d, unsigned h);
  void SwapHours(unsigned c1, unsigned d1, unsigned h1, unsigned c2, unsigned d2, unsigned h2);

  //boolean check functions
  bool IsClassHourFree(unsigned c, unsigned d, unsigned h) const {return Class_Schedule(c, d, h) == -1; }
  bool IsProfHourFree(unsigned p, unsigned d, unsigned h) const {return Prof_Schedule(p, d, h) == -1; }
  [[deprecated("used in older Greedy versions now removed")]] bool SuitableProf(unsigned c, unsigned p);
  

private:

  void ComputeProfDayOff(unsigned p);

  const Sched_Input& in;

  // Classes data structures
  vector<vector<vector<int>>> schedule_class;    // output: class schedule for each class
  vector<vector<int>> class_profs; // professors teaching to a class
  vector<vector<vector<unsigned>>> daily_subject_assigned_hours;  //hours of each subject scheduled for each day for each class
  vector<vector<unsigned>> weekly_subject_assigned_hours;   // quantity of hours of each subject already scheduled for each class

  // Professors data structures
  vector<vector<vector<int>>> schedule_prof;    // output: professor schedule for each professor
  vector<unsigned> prof_weekly_hours;   // hours weekly assigned to each professor
  vector<int> prof_day_off;   // day off of each professor

};
#endif
