#include "Sched_Helpers.hh"

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
  
  // cost components: second parameter is the cost, third is the type (true -> hard, false -> soft)
  Sched_ProfUnavailability cc1(in, 1, false);
  Sched_MaxSubjectHoursXDay cc2(in, 1, false);
  Sched_ProfMaxWeeklyHours cc3(in, 1, false);
  Sched_ScheduleContiguity cc4(in, 1, false);
  Sched_CompleteSolution cc5(in, 1);
 
  Sched_ChangeDeltaProfUnavailability dcc1(in, cc1);
  Sched_ChangeDeltaMaxSubjectHoursXDay dcc2(in, cc2);
  Sched_ChangeDeltaProfMaxWeeklyHours dcc3(in, cc3);
  Sched_ChangeDeltaScheduleContiguity dcc4(in, cc4);
  Sched_ChangeDeltaCompleteSolution dcc5(in, cc5);

  // helpers
  Sched_SolutionManager Sched_sm(in);
  Sched_ChangeNeighborhoodExplorer Sched_nhe(in, Sched_sm);
  
  // All cost components must be added to the state manager
  Sched_sm.AddCostComponent(cc1);
  Sched_sm.AddCostComponent(cc2);
  Sched_sm.AddCostComponent(cc3);
  Sched_sm.AddCostComponent(cc4);
  Sched_sm.AddCostComponent(cc5);
  
  // All delta cost components must be added to the neighborhood explorer
  Sched_nhe.AddDeltaCostComponent(dcc1);
  Sched_nhe.AddDeltaCostComponent(dcc2);
  Sched_nhe.AddDeltaCostComponent(dcc3);
  Sched_nhe.AddDeltaCostComponent(dcc4);
  Sched_nhe.AddDeltaCostComponent(dcc5);
  
  // runners
  HillClimbing<Sched_Input, Sched_Output, Sched_Change> Sched_hc(in, Sched_sm, Sched_nhe, "HC");
  SteepestDescent<Sched_Input, Sched_Output, Sched_Change> Sched_sd(in, Sched_sm, Sched_nhe, "SD");
  SimulatedAnnealing<Sched_Input, Sched_Output, Sched_Change> Sched_sa(in, Sched_sm, Sched_nhe, "SA");

  // tester
  Tester<Sched_Input, Sched_Output> tester(in, Sched_sm);
  MoveTester<Sched_Input, Sched_Output, Sched_Change> swap_move_test(in, Sched_sm, Sched_nhe, "Sched_Change move", tester); 

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
		      //cout << "Time: " << result.running_time << endl;					
        }
   }
  return 0;
}