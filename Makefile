EASYLOCAL = ./easylocal-3
FLAGS = -std=c++20 -Wall -Wfatal-errors -Wno-sign-compare -O3 -g -Wno-deprecated-declarations
COMPOPTS = -I$(EASYLOCAL)/include $(FLAGS)
LINKOPTS = -lboost_program_options -pthread

SOURCE_FILES = Sched_Data.cc Sched_Helpers.cc  Sched_Main.cc
OBJECT_FILES = Sched_Data.o Sched_Helpers.o Sched_Main.o
HEADER_FILES = Sched_Data.hh Sched_Helpers.hh  

csp: $(OBJECT_FILES)
	g++ $(OBJECT_FILES) $(LINKOPTS) -o csp

Sched_Data.o: Sched_Data.cc Sched_Data.hh
	g++ -c $(COMPOPTS) Sched_Data.cc

Sched_Helpers.o: Sched_Helpers.cc Sched_Helpers.hh Sched_Data.hh
	g++ -c $(COMPOPTS) Sched_Helpers.cc

Sched_Main.o: Sched_Main.cc Sched_Helpers.hh Sched_Data.hh
	g++ -c $(COMPOPTS) Sched_Main.cc

clean:
	rm -f $(OBJECT_FILES) csp

