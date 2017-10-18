#!/bin/bash
# File: sbatch-node.template.hadd.sh
# Version: 0.2

# unset JAVA_HOME, because hadoop commands might not work
# this is especially true if one has sourced necessary files for the GRID proxy
echo 'Unsetting JAVA_HOME=$JAVA_HOME'
unset JAVA_HOME

# This value is provided by sbatchManager.py that creates sbatch scripts based this template
echo 'Running version (sbatch-node.template.hadd.sh)'


RUNNING_COMMAND="{{ RUNNING_COMMAND }}"
SCRATCH_DIR="{{ scratch_dir }}/$SLURM_JOBID"

# Runs executable, wrapped into failure wrapper + wrapped into node scratchdir

main() {
    run_failure_wrapped_executable >> "{{ wrapper_log_file }}" 2>&1
    EXIT_CODE=$?
    return $EXIT_CODE
}


# Executed on node, checks that CVMFS is mounted to this cluster node, or redirects task

run_failure_wrapped_executable() {
    TRY_COUNT=$[TRY_COUNT+1]
    echo "Started (Try $TRY_COUNT)"
    EXIT_CODE=1

    if [[ -f /cvmfs/cms.cern.ch/cmsset_default.sh ]]; then
        echo "Network drive mounted, started running on $HOSTNAME"
        run_wrapped_executable
        EXIT_CODE=$?
    else
        echo "Unable to use node $HOSTNAME, will mark node offline: sudo scontrol update nodename=$HOSTNAME state=drain reason=Testing"
        sudo scontrol update nodename=$HOSTNAME state=drain reason=Testing

        if [[ $TRY_COUNT -lt 3 ]]; then
            echo "Will resubmit job to other node: TRY_COUNT=$TRY_COUNT $RUNNING_COMMAND"
            TRY_COUNT=$TRY_COUNT $RUNNING_COMMAND
            EXIT_CODE=$?
        else
            echo "Maximum tries reached, will not try to resubmit any more. GL & HF"
        fi
    fi

    echo "Delete Scratch directory: rm -r $SCRATCH_DIR"
    rm -r $SCRATCH_DIR

    return $EXIT_CODE
}


# Creates scratch dir on cluster node and runs executable

run_wrapped_executable() {
    EXECUTABLE_LOG_FILE="{{ executable_log_file }}"
    EXECUTABLE_LOG_DIR="`dirname $EXECUTABLE_LOG_FILE`"
    EXECUTABLE_LOG_FILE_NAME="`basename $EXECUTABLE_LOG_FILE`"
    TEMPORARY_EXECUTABLE_LOG_DIR="$SCRATCH_DIR/$EXECUTABLE_LOG_DIR/"
    TEMPORARY_EXECUTABLE_LOG_FILE="$TEMPORARY_EXECUTABLE_LOG_DIR/$EXECUTABLE_LOG_FILE_NAME"
    RANDOM_SLEEP={{ random_sleep }}

    echo "Time is: `date`"
    echo "Hostname: `hostname`"
    echo "Current directory: `pwd`"

    echo "Sleeping for $RANDOM_SLEEP seconds"
    sleep $RANDOM_SLEEP

    # Check that input histograms are ok on /home
    export SCRIPTS_DIR="$CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/scripts"
    python $SCRIPTS_DIR/check_that_histograms_are_valid.py {{ inputFiles }}
    check_that_histograms_are_valid_exit_status=$?

    if [[ $check_that_histograms_are_valid_exit_status -ne 0 ]]; then
	echo "ERROR: Some of the input histograms are not valid. Will stop execution."
	return 1
    fi

    echo "Create scratch directory: mkdir -p $SCRATCH_DIR"
    mkdir -p $SCRATCH_DIR

    echo "Create temporary log directory: mkdir -p $TEMPORARY_EXECUTABLE_LOG_DIR"
    mkdir -p $TEMPORARY_EXECUTABLE_LOG_DIR

    echo "Create final log directory: mkdir -p $EXECUTABLE_LOG_DIR"
    mkdir -p $EXECUTABLE_LOG_DIR

    cd {{ working_dir }}
    cd $SCRATCH_DIR

    echo "Time is: `date`"

    CMSSW_SEARCH_PATH="$SCRATCH_DIR:{{ cmssw_base_dir }}/src"

    echo "Execute command: {{ exec_name }} {{ command_line_parameter }} &> $TEMPORARY_EXECUTABLE_LOG_FILE"    
    # CV: use newer hadd version that supports increasing cachesize, to reduce random disk access
    export PATH=/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_9_4_0_pre2/external/slc6_amd64_gcc630/bin/:$PATH
    export LD_LIBRARY_PATH=/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_9_4_0_pre2/biglib/slc6_amd64_gcc630:/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_9_4_0_pre2/lib/slc6_amd64_gcc630:/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_9_4_0_pre2/external/slc6_amd64_gcc630/lib:/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/llvm/4.0.1/lib64:/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/gcc/6.3.0/lib64:/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/gcc/6.3.0/lib:$LD_LIBRARY_PATH
    {{ exec_name }} {{ command_line_parameter }} &> $TEMPORARY_EXECUTABLE_LOG_FILE
    HADD_EXIT_CODE=$?
    echo "Command {{ exec_name }} exited with code $HADD_EXIT_CODE"

    echo "Time is: `date`"

    if [[ $HADD_EXIT_CODE -ne 0 ]]; then
	echo 'ERROR: hadd exited w/ non-zero return code. Will stop execution.'
	return 1
    fi

    OUTPUT_FILES="{{ outputFiles }}"
    echo "Copying output files: {{ outputFiles }}"
    for OUTPUT_FILE in $OUTPUT_FILES
    do
      # Create metadata file for output histogram on scratch
      python $SCRIPTS_DIR/create_histogram_metadata.py $OUTPUT_FILE

      # Check that input histograms are equal to output histogram
      python $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/scripts/check_that_histograms_are_equal.py $OUTPUT_FILE {{ inputFiles }}
      CHECK_THAT_HISTOGRAMS_ARE_EQUAL_EXIT_CODE=$?

      if [[ $CHECK_THAT_HISTOGRAMS_ARE_EQUAL_EXIT_CODE -ne 0 ]]; then
	  echo 'ERROR: Input histograms do not equal output histogram. Will stop execution.'
	  return 1
      fi

      echo "cp $OUTPUT_FILE {{ outputDir }}"
      cp $OUTPUT_FILE {{ outputDir }}

      OUTPUT_FILE_SIZE=$(stat -c '%s' $OUTPUT_FILE)
      if [ -n "$OUTPUT_FILE_SIZE" ] && [ $OUTPUT_FILE_SIZE -ge 1000 ]; then
        echo "cp $OUTPUT_FILE {{ outputDir }}"
        cp $OUTPUT_FILE {{ outputDir }}
      else
        echo "$OUTPUT_FILE is broken, will exit with code 1."
        rm $OUTPUT_FILE
        EXIT_CODE=1
      fi
    done

    echo "Time is: `date`"

    echo "Contents of temporary log dir: ls -laR $TEMPORARY_EXECUTABLE_LOG_DIR"
    ls -laR $TEMPORARY_EXECUTABLE_LOG_DIR

    echo "Copy from temporary output dir to output dir: cp -a $TEMPORARY_EXECUTABLE_LOG_DIR/* $EXECUTABLE_LOG_DIR/"
    cp -a $TEMPORARY_EXECUTABLE_LOG_DIR/* $EXECUTABLE_LOG_DIR/

    echo "End time is: `date`"

    echo "Waiting for 10 seconds before returning"
    sleep 10

    return $HADD_EXIT_CODE
}


# Calls main method

main

EXIT_CODE=$?
echo "Final exit code is: $EXIT_CODE"
exit $EXIT_CODE

# End of file
