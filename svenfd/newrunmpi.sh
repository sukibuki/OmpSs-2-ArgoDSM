#! /bin/bash -l
#SBATCH -p cluster
#SBATCH -t 30:00
#SBATCH -N 4
# Set up environment
#export NANOS6_COMMUNICATION=argo
export NANOS6_COMMUNICATION=mpi-2sided
#export NANOS6_SCHEDULER=cluster-locality
export NANOS6_LOCAL_MEMORY=4GB
export NANOS6_STACK_SIZE=16MB
export NANOS6_DISTRIBUTED_MEMORY=4MB
#export ARGO_DISTRIBUTED_MEMORY=1GB
export NANOS6=debug
export NANOS6_VERBOSE='AddTask,TaskExecution,TaskWait,TaskStatus'

# Run program
mpirun  --map-by ppr:1:node         \
        --mca osc pt2pt             \
        --mca mpi_leave_pinned 1    \
        -n 4 ./fdclustersven
