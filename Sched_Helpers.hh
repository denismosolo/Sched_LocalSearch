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
 * Moves:
 ***************************************************************************/

class Sched_SwapHours_M
{
  friend bool operator==(const  Sched_SwapHours_M& m1, const  Sched_SwapHours_M& m2);
  friend bool operator!=(const  Sched_SwapHours_M& m1, const  Sched_SwapHours_M& m2);
  friend bool operator<(const  Sched_SwapHours_M& m1, const  Sched_SwapHours_M& m2);
  friend ostream& operator<<(ostream& os, const  Sched_SwapHours_M& c);
  friend istream& operator>>(istream& is,  Sched_SwapHours_M& c);
public:
  unsigned _class;
  unsigned day_1;
  unsigned hour_1;
  unsigned day_2;
  unsigned hour_2;

  Sched_SwapHours_M() : _class(0), day_1(0), hour_1(0), day_2(0), hour_2(0) {}
};

/***************************************************************************
 * Sched_Change Neighborhood Explorer:
 ***************************************************************************/

class Sched_ChangeDeltaProfUnavailability
  : public DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeDeltaProfUnavailability(const Sched_Input& in, Sched_ProfUnavailability_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>(in,cc,"Sched_ChangeDeltaProfUnavailability")
  {}
  int ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const override;
};

class Sched_ChangeDeltaMaxSubjectHoursXDay
  : public DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeDeltaMaxSubjectHoursXDay(const Sched_Input& in, Sched_MaxSubjectHoursXDay_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>(in,cc,"Sched_ChangeDeltaMaxSubjectHoursXDay")
  {}
  int ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const override;
};

class Sched_ChangeDeltaProfMaxWeeklyHours
  : public DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeDeltaProfMaxWeeklyHours(const Sched_Input& in, Sched_ProfMaxWeeklyHours_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>(in,cc,"Sched_ChangeDeltaProfMaxWeeklyHours")
  {}
  int ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const override;
};

class Sched_ChangeDeltaScheduleContiguity
  : public DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeDeltaScheduleContiguity(const Sched_Input& in, Sched_ScheduleContiguity_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>(in,cc,"Sched_ChangeDeltaScheduleContiguity")
  {}
  int ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const override;
};

class Sched_ChangeDeltaCompleteSolution
  : public DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeDeltaCompleteSolution(const Sched_Input& in, Sched_SolutionComplete_CC& cc)
    : DeltaCostComponent<Sched_Input,Sched_Output, Sched_SwapHours_M>(in,cc,"Sched_ChangeDeltaCompleteSolution")
  {}
  int ComputeDeltaCost(const Sched_Output& out, const  Sched_SwapHours_M& mv) const override;
};

class Sched_ChangeNeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output, Sched_SwapHours_M>
{
public:
  Sched_ChangeNeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output, Sched_SwapHours_M>(pin, psm, "Sched_ChangeNeighborhoodExplorer") {}
  void RandomMove(const Sched_Output&,  Sched_SwapHours_M&) const override;          
  bool FeasibleMove(const Sched_Output&, const  Sched_SwapHours_M&) const override;  
  void MakeMove(Sched_Output&, const  Sched_SwapHours_M&) const override;             
  void FirstMove(const Sched_Output&,  Sched_SwapHours_M&) const override;  
  bool NextMove(const Sched_Output&,  Sched_SwapHours_M&) const override;
protected:
  bool AnyNextMove(const Sched_Output&,  Sched_SwapHours_M&) const;
};
#endif