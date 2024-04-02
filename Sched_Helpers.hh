// File Sched_Helpers.hh
#ifndef SCHED_HELPERS_HH
#define SCHED_HELPERS_HH

#include "Sched_Data.hh"
#include <easylocal.hh>

using namespace EasyLocal::Core;

class Sched_Change
{
  friend bool operator==(const Sched_Change& m1, const Sched_Change& m2);
  friend bool operator!=(const Sched_Change& m1, const Sched_Change& m2); //da implementare
  friend bool operator<(const Sched_Change& m1, const Sched_Change& m2);  //da implementare
  friend ostream& operator<<(ostream& os, const Sched_Change& c);
  friend istream& operator>>(istream& is, Sched_Change& c);
 public:
  int store, old_w, new_w;    //che variabili devo mettere qua? vecchia ora, nuova ora e professore? e se cambio professore?
  Sched_Change();
};

/***************************************************************************
 * State Manager 
 ***************************************************************************/

class Sched_SolutionManager : public SolutionManager<Sched_Input,Sched_Output> 
{
public:
  Sched_SolutionManager(const Sched_Input &);
  void RandomState(Sched_Output& out) override;   
  void GreedyState(Sched_Output& out) override;   
  void DumpState(const Sched_Output& out, ostream& os) const override { out.Print(cout); } //dove viene usata?
  void PrettyPrintOutput(const Sched_Output& st, string filename) const { st.PrintTAB(filename); }
  bool CheckConsistency(const Sched_Output& st) const override;
}; 

class Sched_ProfUnavailability : public CostComponent<Sched_Input,Sched_Output> 
{
public:
  Sched_ProfUnavailability(const Sched_Input & in, int w, bool hard) :    CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ProfUnavailability") 
  {}
  int ComputeCost(const Sched_Output& st) const override;
  void PrintViolations(const Sched_Output& st, ostream& os = cout) const override;
};

class Sched_MaxSubjectHoursXDay: public CostComponent<Sched_Input,Sched_Output> 
{
public:
  Sched_MaxSubjectHoursXDay(const Sched_Input & in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_MaxSubjectHoursXDay") 
  {}
  int ComputeCost(const Sched_Output& st) const override;
  void PrintViolations(const Sched_Output& st, ostream& os = cout) const override;
};

class Sched_ProfMaxWeeklyHours: public CostComponent<Sched_Input,Sched_Output> 
{
public:
  Sched_ProfMaxWeeklyHours(const Sched_Input & in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ProfMaxWeeklyHours") 
  {}
  int ComputeCost(const Sched_Output& st) const override;
  void PrintViolations(const Sched_Output& st, ostream& os = cout) const override;
};

class Sched_ScheduleContiguity: public CostComponent<Sched_Input,Sched_Output> 
{
public:
  Sched_ScheduleContiguity(const Sched_Input & in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ScheduleContiguity") 
  {}
  int ComputeCost(const Sched_Output& st) const override;
  void PrintViolations(const Sched_Output& st, ostream& os = cout) const override;
};

/***************************************************************************
 * Sched_Change Neighborhood Explorer:
 ***************************************************************************/

class Sched_ChangeDeltaProfUnavailability
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>
{
public:
  Sched_ChangeDeltaProfUnavailability(const Sched_Input & in, Sched_ProfUnavailability& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>(in,cc,"Sched_ChangeDeltaProfUnavailability") 
  {}
  int ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const override;
};

class Sched_ChangeDeltaMaxSubjectHoursXDay
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>
{
public:
  Sched_ChangeDeltaMaxSubjectHoursXDay(const Sched_Input & in, Sched_MaxSubjectHoursXDay& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>(in,cc,"Sched_ChangeDeltaMaxSubjectHoursXDay") 
  {}
  int ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const override;
};

class Sched_ChangeDeltaProfMaxWeeklyHours
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>
{
public:
  Sched_ChangeDeltaProfMaxWeeklyHours(const Sched_Input & in, Sched_ProfMaxWeeklyHours& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>(in,cc,"Sched_ChangeDeltaProfMaxWeeklyHours") 
  {}
  int ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const override;
};

class Sched_ChangeDeltaScheduleContiguity
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>
{
public:
  Sched_ChangeDeltaScheduleContiguity(const Sched_Input & in, Sched_ScheduleContiguity& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_Change>(in,cc,"Sched_ChangeDeltaScheduleContiguity") 
  {}
  int ComputeDeltaCost(const Sched_Output& st, const Sched_Change& mv) const override;
};

class Sched_ChangeNeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_Change> 
{
public:
  Sched_ChangeNeighborhoodExplorer(const Sched_Input & pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_Change>(pin, psm, "Sched_ChangeNeighborhoodExplorer") {} 
  void RandomMove(const Sched_Output&, Sched_Change&) const override;          
  bool FeasibleMove(const Sched_Output&, const Sched_Change&) const override;  
  void MakeMove(Sched_Output&, const Sched_Change&) const override;             
  void FirstMove(const Sched_Output&, Sched_Change&) const override;  
  bool NextMove(const Sched_Output&, Sched_Change&) const override;   
protected:
  bool AnyNextMove(const Sched_Output&, Sched_Change&) const;   
};
#endif