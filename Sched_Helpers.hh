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
  int day_1;
  int hour_1;
  int day_2;
  int hour_2;
  bool first_move;

  Sched_SwapHours();
};

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer - Neighborhood Manager:
 ***************************************************************************/

class Sched_SwapHours_NeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_SwapHours> 
{
public:
  Sched_SwapHours_NeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_SwapHours>(pin, psm, "Sched_SwapHours_NeighborhoodExplorer") {} 
  void RandomMove(const Sched_Output&, Sched_SwapHours&) const override;          
  bool FeasibleMove(const Sched_Output&, const Sched_SwapHours&) const override;  
  void MakeMove(Sched_Output&, const Sched_SwapHours&) const override;             
  void FirstMove(const Sched_Output&, Sched_SwapHours&) const override;  
  bool NextMove(const Sched_Output&, Sched_SwapHours&) const override;   
protected:
  bool AnyNextMove(const Sched_Output&, Sched_SwapHours&) const;   
};

/***************************************************************************
 * Sched_SwapHours Neighborhood Explorer - DeltaCosts:
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
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override { return 0; }   // SwapHours can't change this cost
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
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapHours& mv) const override { return 0; }   // SwapHours can't change this cost
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

class Sched_AssignProf_NeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_AssignProf> 
{
public:
  Sched_AssignProf_NeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input,Sched_Output>& psm)  
    : NeighborhoodExplorer<Sched_Input,Sched_Output,Sched_AssignProf>(pin, psm, "Sched_AssignProf_NeighborhoodExplorer") {} 
  void RandomMove(const Sched_Output&, Sched_AssignProf&) const override;          
  bool FeasibleMove(const Sched_Output&, const Sched_AssignProf&) const override;  
  void MakeMove(Sched_Output&, const Sched_AssignProf&) const override;             
  void FirstMove(const Sched_Output&, Sched_AssignProf&) const override;  
  bool NextMove(const Sched_Output&, Sched_AssignProf&) const override;   
protected:
  bool AnyNextMove(const Sched_Output&, Sched_AssignProf&) const;   
};

/***************************************************************************
 * Sched_AssignProf Neighborhood Explorer - DeltaCosts:
 ***************************************************************************/

class Sched_AssignProfDeltaProfUnavailability
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>
{
public:
  Sched_AssignProfDeltaProfUnavailability(const Sched_Input& in, Sched_ProfUnavailability_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>(in,cc,"Sched_AssignProfDeltaProfUnavailability") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const override;
};

class Sched_AssignProfDeltaMaxSubjectHoursXDay
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>
{
public:
  Sched_AssignProfDeltaMaxSubjectHoursXDay(const Sched_Input& in, Sched_MaxSubjectHoursXDay_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>(in,cc,"Sched_AssignProfDeltaMaxSubjectHoursXDay") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const override;
};

class Sched_AssignProfDeltaProfMaxWeeklyHours
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>
{
public:
  Sched_AssignProfDeltaProfMaxWeeklyHours(const Sched_Input& in, Sched_ProfMaxWeeklyHours_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>(in,cc,"Sched_AssignProfDeltaProfMaxWeeklyHours") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const override;
};

class Sched_AssignProfDeltaScheduleContiguity
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>
{
public:
  Sched_AssignProfDeltaScheduleContiguity(const Sched_Input& in, Sched_ScheduleContiguity_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>(in,cc,"Sched_AssignProfDeltaScheduleContiguity") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const override;
};

class Sched_AssignProfDeltaCompleteSolution
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>
{
public:
  Sched_AssignProfDeltaCompleteSolution(const Sched_Input& in, Sched_SolutionComplete_CC& cc)
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_AssignProf>(in,cc,"Sched_AssignProfDeltaCompleteSolution") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_AssignProf& mv) const override { return mv.prof == -1 || !out.IsClassHourFree(mv._class, mv.day, mv.hour) || !out.IsProfHourFree(mv.prof, mv.day, mv.hour) ? 0 : -1; }
};


/***************************************************************************
 * Sched_SwapProf Neighborhood Explorer - Moves:
 ***************************************************************************/

class Sched_SwapProf
{
  friend bool operator==(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2);
  friend bool operator!=(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2);
  friend bool operator<(const Sched_SwapProf& mv1, const Sched_SwapProf& mv2);
  friend ostream& operator<<(ostream& os, const Sched_SwapProf& c);
  friend istream& operator>>(istream& is, Sched_SwapProf& c);

public:
  int subject;
  int class_1;
  int class_2;
  bool moves;
  bool first_move;

  Sched_SwapProf();
};


/***************************************************************************
 * Sched_SwapProf Neighborhood Explorer - Neighborhood Manager:
 ***************************************************************************/

class Sched_SwapProf_NeighborhoodExplorer
  : public NeighborhoodExplorer<Sched_Input, Sched_Output, Sched_SwapProf>
{
public:
  Sched_SwapProf_NeighborhoodExplorer(const Sched_Input& pin, SolutionManager<Sched_Input, Sched_Output>& psm)
    : NeighborhoodExplorer<Sched_Input, Sched_Output, Sched_SwapProf>(pin, psm, "Sched_SwapProf_NeighborhoodExplorer") {}
  void RandomMove(const Sched_Output&, Sched_SwapProf&) const override;
  bool FeasibleMove(const Sched_Output&, const Sched_SwapProf&) const override;
  void MakeMove(Sched_Output&, const Sched_SwapProf&) const override;
  void FirstMove(const Sched_Output&, Sched_SwapProf&) const override;
  bool NextMove(const Sched_Output&, Sched_SwapProf&) const override;
protected:
  bool AnyNextMove(const Sched_Output&, Sched_SwapProf&) const;
};


/***************************************************************************
 * Sched_SwapProf Neighborhood Explorer - DeltaCosts:
 ***************************************************************************/

class Sched_SwapProfDeltaProfUnavailability
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>
{
public:
  Sched_SwapProfDeltaProfUnavailability(const Sched_Input& in, Sched_ProfUnavailability_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>(in,cc,"Sched_SwapProfDeltaProfUnavailability") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const override;
};

class Sched_SwapProfDeltaMaxSubjectHoursXDay
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>
{
public:
  Sched_SwapProfDeltaMaxSubjectHoursXDay(const Sched_Input& in, Sched_MaxSubjectHoursXDay_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>(in,cc,"Sched_SwapProfDeltaMaxSubjectHoursXDay") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const override { return 0; }
};

class Sched_SwapProfDeltaProfMaxWeeklyHours
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>
{
public:
  Sched_SwapProfDeltaProfMaxWeeklyHours(const Sched_Input& in, Sched_ProfMaxWeeklyHours_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>(in,cc,"Sched_SwapProfDeltaProfMaxWeeklyHours") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const override;
};

class Sched_SwapProfDeltaScheduleContiguity
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>
{
public:
  Sched_SwapProfDeltaScheduleContiguity(const Sched_Input& in, Sched_ScheduleContiguity_CC& cc) 
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>(in,cc,"Sched_SwapProfDeltaScheduleContiguity") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const override { return 0; }
};

class Sched_SwapProfDeltaCompleteSolution
  : public DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>
{
public:
  Sched_SwapProfDeltaCompleteSolution(const Sched_Input& in, Sched_SolutionComplete_CC& cc)
    : DeltaCostComponent<Sched_Input,Sched_Output,Sched_SwapProf>(in,cc,"Sched_SwapProfDeltaCompleteSolution") 
  {}
  int ComputeDeltaCost(const Sched_Output& out, const Sched_SwapProf& mv) const override { return 0; }
};
#endif