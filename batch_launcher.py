#! /usr/bin/env python3
import os
from datetime import date
import random
import math

# list of names of instances
instances = ["2", "2_2", "2_3", "3", "4", "5", "6", "7"]

# directory of the instances
instance_directory = "./Instances"

# directory of the initstates
init_state_directory = "./InitStates"

# directory of the solutions
solution_directory = "./Solutions"

# ALGORITHM PARAMETERS
max_evaluations = 100000000
max_idle_iterations = 10000000

for instance in instances:
  in_file_path = instance_directory + "/instance" + instance + ".txt"
  init_state_path = init_state_directory + "/Random" + instance
  if os.path.isfile(in_file_path) & os.path.isfile(init_state_path): # Check if the files exist
    if not os.path.isdir(solution_directory):
      os.mkdir(solution_directory) 

    print("sol-from" + init_state_path)
    
    # the solution file
    sol_file = solution_directory + "/sol-from" + "Random" + instance + ".sol"
    
    # command line
    command = "./csp" + \
    " --main::instance " +  in_file_path  + \
    " --main::init_state " + init_state_path + \
    " --main::output_file " + sol_file + \
    " --main::method HC" + \
    " --HC::max_evaluations " + str(max_evaluations) + \
    " --HC::max_idle_iterations " + str(max_idle_iterations) + \
    " --Sched_solver::timeout 600" + \
    " --Sched_solver::random_state-disable" + \
    " --Sched_solver::init_trials 0"

    print(command + "\n")
    os.system(command)
