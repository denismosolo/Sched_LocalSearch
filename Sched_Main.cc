#include "Sched_Headers.hh"

using namespace EasyLocal::Debug;

int main(int argc, const char* argv[])
{
  ParameterBox main_parameters("main", "Main Program options");
  Parameter<string> instance("instance", "Input instance", main_parameters); 
  Parameter<int> seed("seed", "Random seed", main_parameters);
  Parameter<string> method("method", "Solution method (empty for tester)", main_parameters);   
  Parameter<string> init_state("init_state", "Initial state (to be read from file)", main_parameters);
  Parameter<string> output_file("output_file", "Write the output to a file (filename required)", main_parameters);
 
  // 3rd parameter: false = do not check unregistered parameters
  // 4th parameter: true = silent
  CommandLineParameters::Parse(argc, argv, false, true);  

  if (!instance.IsSet())
  {
    cout << "Error: --main::instance filename option must always be set" << endl;
    return 1;
  }

  Sched_Input in(instance);

  if (seed.IsSet())
    Random::SetSeed(seed);
  
  // Cost Components: second parameter is the cost, third is the type (true -> hard, false -> soft)
  Sched_ProfUnavailability_CC cc_PU(in, in.UnavailabilityViolationCost(), false);
  Sched_MaxSubjectHoursXDay_CC cc_MSHD(in, in.MaxSubjectHoursXDayViolationCost(), false);
  Sched_ProfMaxWeeklyHours_CC cc_PWMH(in, in.MaxProfWeeklyHoursViolationCost(), false);
  Sched_ScheduleContiguity_CC cc_SC(in, in.ScheduleContiguityViolationCost(), false);
  Sched_SolutionComplete_CC cc_CS(in, 1, true);
 
  //  Delta Cost Components
  Sched_SwapHoursDeltaProfUnavailability SwapH_dcc_PU(in, cc_PU);
  Sched_SwapHoursDeltaMaxSubjectHoursXDay SwapH_dcc_MSHD(in, cc_MSHD);
  Sched_SwapHoursDeltaProfMaxWeeklyHours SwapH_dcc_PMWH(in, cc_PWMH);
  Sched_SwapHoursDeltaScheduleContiguity SwapH_dcc_SC(in, cc_SC);
  Sched_SwapHoursDeltaCompleteSolution SwapH_dcc_CS(in, cc_CS);

  Sched_AssignProfDeltaProfUnavailability  AssignP_dcc_PU(in, cc_PU);
  Sched_AssignProfDeltaMaxSubjectHoursXDay AssignP_dcc_MSHD(in, cc_MSHD);
  Sched_AssignProfDeltaProfMaxWeeklyHours AssignP_dcc_PMWH(in, cc_PWMH);
  Sched_AssignProfDeltaScheduleContiguity AssignP_dcc_SC(in, cc_SC);
  Sched_AssignProfDeltaCompleteSolution AssignP_dcc_CS(in, cc_CS);

  Sched_SwapProfDeltaProfUnavailability  SwapP_dcc_PU(in, cc_PU);
  Sched_SwapProfDeltaMaxSubjectHoursXDay SwapP_dcc_MSHD(in, cc_MSHD);
  Sched_SwapProfDeltaProfMaxWeeklyHours SwapP_dcc_PMWH(in, cc_PWMH);
  Sched_SwapProfDeltaScheduleContiguity SwapP_dcc_SC(in, cc_SC);
  Sched_SwapProfDeltaCompleteSolution SwapP_dcc_CS(in, cc_CS);

  // helpers
  Sched_SolutionManager Sched_sm(in);
  Sched_SwapHours_NeighborhoodExplorer Sched_SwapH_nhe(in, Sched_sm);
  Sched_AssignProf_NeighborhoodExplorer Sched_AssignP_nhe(in, Sched_sm);
  Sched_SwapProf_NeighborhoodExplorer Sched_SwapP_nhe(in, Sched_sm);
  
  // All cost components must be added to the state manager
  Sched_sm.AddCostComponent(cc_PU);
  Sched_sm.AddCostComponent(cc_MSHD);
  Sched_sm.AddCostComponent(cc_PWMH);
  Sched_sm.AddCostComponent(cc_SC);
  Sched_sm.AddCostComponent(cc_CS);

  // Add all delta cost components for SwapHour to the neighborhood explorer
  Sched_SwapH_nhe.AddDeltaCostComponent(SwapH_dcc_PU);
  Sched_SwapH_nhe.AddDeltaCostComponent(SwapH_dcc_MSHD);
  Sched_SwapH_nhe.AddDeltaCostComponent(SwapH_dcc_PMWH);
  Sched_SwapH_nhe.AddDeltaCostComponent(SwapH_dcc_SC);
  Sched_SwapH_nhe.AddDeltaCostComponent(SwapH_dcc_CS);

  // Add all delta cost components for AssignProf to the neighborhood explorer
  Sched_AssignP_nhe.AddDeltaCostComponent(AssignP_dcc_PU);
  Sched_AssignP_nhe.AddDeltaCostComponent(AssignP_dcc_MSHD);
  Sched_AssignP_nhe.AddDeltaCostComponent(AssignP_dcc_PMWH);
  Sched_AssignP_nhe.AddDeltaCostComponent(AssignP_dcc_SC);
  Sched_AssignP_nhe.AddDeltaCostComponent(AssignP_dcc_CS);

  // Add all delta cost components for SwapProf to the neighborhood explorer
  Sched_SwapP_nhe.AddDeltaCostComponent(SwapP_dcc_PU);
  Sched_SwapP_nhe.AddDeltaCostComponent(SwapP_dcc_MSHD);
  Sched_SwapP_nhe.AddDeltaCostComponent(SwapP_dcc_PMWH);
  Sched_SwapP_nhe.AddDeltaCostComponent(SwapP_dcc_SC);
  Sched_SwapP_nhe.AddDeltaCostComponent(SwapP_dcc_CS);

  // Union of neighborhoods creation
  SetUnionNeighborhoodExplorer <Sched_Input, Sched_Output, DefaultCostStructure<int>, Sched_SwapHours_NeighborhoodExplorer, Sched_AssignProf_NeighborhoodExplorer, Sched_SwapProf_NeighborhoodExplorer> Union_nhe(in, Sched_sm, "Union NHE", Sched_SwapH_nhe, Sched_AssignP_nhe, Sched_SwapP_nhe);
  
  // Runners
  // Union
  HillClimbing<Sched_Input, Sched_Output, tuple <ActiveMove<Sched_SwapHours>, ActiveMove<Sched_AssignProf>, ActiveMove<Sched_SwapProf>>> Sched_hc(in, Sched_sm, Union_nhe, "HC");
  SteepestDescent<Sched_Input, Sched_Output, tuple <ActiveMove<Sched_SwapHours>, ActiveMove<Sched_AssignProf>, ActiveMove<Sched_SwapProf>>> Sched_sd(in, Sched_sm, Union_nhe, "SD");
  SimulatedAnnealing<Sched_Input, Sched_Output, tuple <ActiveMove<Sched_SwapHours>, ActiveMove<Sched_AssignProf>, ActiveMove<Sched_SwapProf>>> Sched_sa(in, Sched_sm, Union_nhe, "SA");

  // SwapProf
  //HillClimbing<Sched_Input, Sched_Output, Sched_SwapProf> Sched_hc(in, Sched_sm, Sched_SwapP_nhe, "HC");
  //SteepestDescent<Sched_Input, Sched_Output, Sched_SwapProf> Sched_sd(in, Sched_sm, Sched_SwapP_nhe, "SD");
  //SimulatedAnnealing<Sched_Input, Sched_Output, Sched_SwapProf> Sched_sa(in, Sched_sm, Sched_SwapP_nhe, "SA");

  // SwapHours
  //HillClimbing<Sched_Input, Sched_Output, Sched_SwapHours> Sched_hc(in, Sched_sm, Sched_SwapH_nhe, "HC");
  //SteepestDescent<Sched_Input, Sched_Output, Sched_SwapHours> Sched_sd(in, Sched_sm, Sched_SwapH_nhe, "SD");
  //SimulatedAnnealing<Sched_Input, Sched_Output, Sched_SwapHours> Sched_sa(in, Sched_sm, Sched_SwapH_nhe, "SA");

  // AssignProf
  //HillClimbing<Sched_Input, Sched_Output, Sched_AssignProf> Sched_hc(in, Sched_sm, Sched_AssignP_nhe, "HC");
  //SteepestDescent<Sched_Input, Sched_Output, Sched_AssignProf> Sched_sd(in, Sched_sm, Sched_AssignP_nhe, "SD");
  //SimulatedAnnealing<Sched_Input, Sched_Output, Sched_AssignProf> Sched_sa(in, Sched_sm, Sched_AssignP_nhe, "SA");

  // Tester
  Tester<Sched_Input, Sched_Output> tester(in, Sched_sm);
  MoveTester<Sched_Input, Sched_Output, Sched_SwapHours> swapH_move_test(in, Sched_sm, Sched_SwapH_nhe, "Sched_SwapHours move", tester);
  MoveTester<Sched_Input, Sched_Output, Sched_AssignProf> assignP_move_test(in, Sched_sm, Sched_AssignP_nhe, "Sched_AssignProf move", tester); 
  MoveTester<Sched_Input, Sched_Output, Sched_SwapProf> swapP_move_test(in, Sched_sm, Sched_SwapP_nhe, "Sched_SwapProf move", tester);

  SimpleLocalSearch<Sched_Input, Sched_Output> Sched_solver(in, Sched_sm, "Sched solver");
  if (!CommandLineParameters::Parse(argc, argv, true, false))
    return 1;

  if (!method.IsSet())
  { // if no search method is set -> enter the tester
    if (init_state.IsSet())
    tester.RunMainMenu(init_state);
    else
    tester.RunMainMenu();
  }
  else
  {
    if (method == "SA")
    {
      Sched_solver.SetRunner(Sched_sa);
    }
    else if (method == "HC")
    {
      Sched_solver.SetRunner(Sched_hc);
    }
    else if (method == "SD")
    {
      Sched_solver.SetRunner(Sched_sd);
    }
    else
    {
      cerr << "Unknown method " << static_cast<string>(method) << endl;
      exit(1);
    }

    SolverResult<Sched_Input, Sched_Output> result = Sched_solver.Solve();
    Sched_Output out = result.output;
    if (output_file.IsSet())
    { // write the output on the file passed in the command line
      ofstream os(static_cast<string>(output_file));
      os << out << endl;
      os << "Cost: " << result.cost.total << endl;
      os << "Time: " << result.running_time << "s " << endl;
      os.close();
    }
    else
    { // write the solution in the standard output
      cout << out << endl;
      cout << "Cost: " << result.cost.total << endl;
      cout << "Time: " << result.running_time << "s" << endl;				
    }
  }

  return 0;
}