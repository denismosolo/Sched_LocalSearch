// File Sched_Helpers.hh
#ifndef SCHED_HELPERS_HH
#define SCHED_HELPERS_HH

#include "Sched_Data.hh"
#include <easylocal.hh>

using namespace EasyLocal::Core;

/***************************************************************************
 * Solution Manager 
 ***************************************************************************/

class Sched_SolutionManager : public SolutionManager<Sched_Input,Sched_Output> 
{
public:
  Sched_SolutionManager(const Sched_Input&);
  void RandomState(Sched_Output& out) override;   
  void GreedyState(Sched_Output& out) override;   
  void DumpState(const Sched_Output& out, ostream& os) const override { out.Print(cout); }
  void PrettyPrintOutput(const Sched_Output& out, string filename) const { out.PrintTAB(filename); }
  bool CheckConsistency(const Sched_Output& out) const override;
}; 

/***************************************************************************
 * Cost Components
 ***************************************************************************/

class Sched_ProfUnavailability_CC : public CostComponent<Sched_Input,Sched_Output>
{
public:
  Sched_ProfUnavailability_CC(const Sched_Input& in, int w, bool hard) :    CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ProfUnavailability")
  {}
  int ComputeCost(const Sched_Output& out) const override;
  void PrintViolations(const Sched_Output& out, ostream& os = cout) const override;
};

class Sched_MaxSubjectHoursXDay_CC : public CostComponent<Sched_Input,Sched_Output>
{
public:
  Sched_MaxSubjectHoursXDay_CC(const Sched_Input & in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_MaxSubjectHoursXDay")
  {}
  int ComputeCost(const Sched_Output& out) const override;
  void PrintViolations(const Sched_Output& out, ostream& os = cout) const override;
};

class Sched_ProfMaxWeeklyHours_CC : public CostComponent<Sched_Input,Sched_Output>
{
public:
  Sched_ProfMaxWeeklyHours_CC(const Sched_Input& in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ProfMaxWeeklyHours")
  {}
  int ComputeCost(const Sched_Output& out) const override;
  void PrintViolations(const Sched_Output& out, ostream& os = cout) const override;
};

class Sched_ScheduleContiguity_CC : public CostComponent<Sched_Input,Sched_Output>
{
public:
  Sched_ScheduleContiguity_CC(const Sched_Input& in, int w, bool hard) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_ScheduleContiguity")
  {}
  int ComputeCost(const Sched_Output& out) const override;
  void PrintViolations(const Sched_Output& out, ostream& os = cout) const override;
};

class Sched_SolutionComplete_CC : public CostComponent<Sched_Input,Sched_Output>
{
public:
  Sched_SolutionComplete_CC(const Sched_Input& in, int w, bool hard = true) : CostComponent<Sched_Input,Sched_Output>(in,w,hard,"Sched_CompleteSolution")
  {}
  int ComputeCost(const Sched_Output& out) const override;
  void PrintViolations(const Sched_Output& out, ostream& os = cout) const override;
};

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer - Moves:
 ***************************************************************************/

class Sched_SwapHours
{
  friend bool operator==(const Sched_SwapHours& m1, const Sched_SwapHours& m2);
  friend bool operator!=(const Sched_SwapHours& m1, const Sched_SwapHours& m2);
  friend bool operator<(const Sched_SwapHours& m1, const Sched_SwapHours& m2);
  friend ostream& operator<<(ostream& os, const Sched_SwapHours& c);
  friend istream& operator>>(istream& is, Sched_SwapHours& c);

public:
  int _class;
  int new_day;
  int new_hour;
  int new_prof;
  bool new_free;
  int old_day;
  int old_hour;
  int old_prof;
  bool old_free;
  int index;

  Sched_SwapHours();
};

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer - Neighborhood Manager:
 ***************************************************************************/

class Sched_SwapHoursDeltaProfUnavailability
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>
{
public:
  Sched_SwapHoursDeltaProfUnavailability(const Sched_Input& in, Sched_ProfUnavailability_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>(in,cc,"Sched_SwapHoursDeltaProfUnavailability") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override;
};

class Sched_SwapHoursDeltaMaxSubjectHoursXDay
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>
{
public:
  Sched_SwapHoursDeltaMaxSubjectHoursXDay(const Sched_Input& in, Sched_MaxSubjectHoursXDay_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>(in,cc,"Sched_SwapHoursDeltaMaxSubjectHoursXDay") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override;
};

class Sched_SwapHoursDeltaProfMaxWeeklyHours
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>
{
public:
  Sched_SwapHoursDeltaProfMaxWeeklyHours(const Sched_Input& in, Sched_ProfMaxWeeklyHours_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>(in,cc,"Sched_SwapHoursDeltaProfMaxWeeklyHours") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override;
};

class Sched_SwapHoursDeltaScheduleContiguity
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>
{
public:
  Sched_SwapHoursDeltaScheduleContiguity(const Sched_Input& in, Sched_ScheduleContiguity_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>(in,cc,"Sched_SwapHoursDeltaScheduleContiguity") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override;
};

class Sched_SwapHoursDeltaCompleteSolution
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>
{
public:
  Sched_SwapHoursDeltaCompleteSolution(const Sched_Input& in, Sched_SolutionComplete_CC& cc)
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapHours>(in,cc,"Sched_SwapHoursDeltaCompleteSolution") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override;
};

class Sched_SwapHoursNeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_SwapHours> 
{
public:
  Sched_SwapHoursNeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_SwapHours>(pin, psm, "Sched_SwapHoursNeighborhoodExplorer") {} 
  void RandomMove(const Sched_Output&, Sched_SwapHours&) const override;          
  bool FeasibleMove(const Sched_Output&, const Sched_SwapHours&) const override;  
  void MakeMove(Sched_Output&, const Sched_SwapHours&) const override;             
  void FirstMove(const Sched_Output&, Sched_SwapHours&) const override;  
  bool NextMove(const Sched_Output&, Sched_SwapHours&) const override;   
protected:
  bool AnyNextMove(const Sched_Output&, Sched_SwapHours&) const;   
};


/***************************************************************************
 * Sched_AssignProf Neighborhood Explorer - Moves:
 ***************************************************************************/

class Sched_AssignProf
{
  friend bool operator==(const Sched_AssignProf& m1, const Sched_AssignProf& m2);
  friend bool operator!=(const Sched_AssignProf& m1, const Sched_AssignProf& m2);
  friend bool operator<(const Sched_AssignProf& m1, const Sched_AssignProf& m2);
  friend ostream& operator<<(ostream& os, const Sched_AssignProf& c);
  friend istream& operator>>(istream& is, Sched_AssignProf& c);

public:
  int _class;
  int day;
  int hour;
  int prof;
  int index;

  Sched_AssignProf();
};

/***************************************************************************
 * Sched_AssignProf Neighborhood Explorer - Neighborhood Manager:
 ***************************************************************************/

class Sched_AssignProfNeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_AssignProf> 
{
public:
  Sched_AssignProfNeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_AssignProf>(pin, psm, "Sched_AssignProfNeighborhoodExplorer") {} 
  void RandomMove(const Sched_Output&, Sched_AssignProf&) const override;          
  bool FeasibleMove(const Sched_Output&, const Sched_AssignProf&) const override;  
  void MakeMove(Sched_Output&, const Sched_AssignProf&) const override;             
  void FirstMove(const Sched_Output&, Sched_AssignProf&) const override;  
  bool NextMove(const Sched_Output&, Sched_AssignProf&) const override;   
protected:
  bool AnyNextMove(const Sched_Output&, Sched_AssignProf&) const;   
};
#endif