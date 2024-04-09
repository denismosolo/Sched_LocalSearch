EASYLOCAL = ./easylocal-3
FLAGS = -std=c++20 -Wall -Wfatal-errors -Wno-sign-compare -O3 -g -Wno-deprecated-declarations
COMPOPTS = -I$(EASYLOCAL)/include $(FLAGS)
LINKOPTS = -lboost_program_options -pthread

SOURCE_FILES = Sched_Data.cc Sched_SolutionManager.cc Sched_SwapHours_NHE.cc Sched_CostComponents.cc  Sched_Main.cc
OBJECT_FILES = Sched_Data.o Sched_SolutionManager.o Sched_SwapHours_NHE.o Sched_CostComponents.o Sched_Main.o
HEADER_FILES = Sched_Data.hh Sched_Helpers.hh  

csp: $(OBJECT_FILES)
	g++ $(OBJECT_FILES) $(LINKOPTS) -o csp

Sched_Data.o: Sched_Data.cc Sched_Data.hh
	g++ -c $(COMPOPTS) Sched_Data.cc

Sched_SolutionManager.o: Sched_SolutionManager.cc $(HEADER_FILES)
	g++ -c $(COMPOPTS) Sched_SolutionManager.cc

Sched_SwapHours_NHE.o: Sched_SwapHours_NHE.cc $(HEADER_FILES)
	g++ -c $(COMPOPTS) Sched_SwapHours_NHE.cc

Sched_CostComponents.o: Sched_CostComponents.cc $(HEADER_FILES)
	g++ -c $(COMPOPTS) Sched_CostComponents.cc

Sched_Main.o: Sched_Main.cc $(HEADER_FILES)
	g++ -c $(COMPOPTS) Sched_Main.cc

clean:
	rm -f $(OBJECT_FILES) csp

