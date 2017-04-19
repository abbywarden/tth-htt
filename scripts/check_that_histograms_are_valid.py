from tthAnalysis.HiggsToTauTau.jobTools import run_cmd
import time, os, sys, ROOT

def main():
    input_histograms = sys.argv[1:len(sys.argv)]

    print('check_that_histograms_are_valid.py %s' % " ".join(input_histograms))

    for input_histogram in input_histograms:
        check_that_histogram_is_valid(input_histogram)

    print('All input files are ok.')
    sys.exit(0)


def check_that_histogram_is_valid(input_histogram):

    check_that_histogram_exists(input_histogram)
    check_that_histogram_is_ready_for_usage(input_histogram)
    check_that_histogram_is_big_enough(input_histogram)
    check_that_histogram_is_not_zombie(input_histogram)

    print('File is ok: %s' % input_histogram )


def check_that_histogram_exists(input_histogram):
    print('check_that_histogram_exists: %s' % input_histogram)

    if not os.path.isfile(input_histogram):
        print('ERROR: root input file is missing: %s' % input_histogram)
        sys.exit(1)

def check_that_histogram_is_ready_for_usage(input_histogram):
    print('check_that_histogram_is_ready_for_usage: %s' % input_histogram)

    polling_delay    = 1 # in seconds
    polling_cmd      = "fuser %s" % input_histogram

    is_file_ready = False
    while not is_file_ready:

        stdout, stderr = run_cmd(polling_cmd, return_stderr = True)
        
        print('Did run command: %s' % polling_cmd)
        print('Output was: %s' % stdout)
        print('Error was: %s' % stdout)

        if not stdout and not stderr:
            # No one uses this file, it's free to use for everyone
            break
        if not stdout and stderr:
            # The file still doesn't exist?
            print(stderr.rstrip('\n'))
            sys.exit(1)
        else:
            # Both stdout and stderr contain text (PID and filename, respectively); wait ...
            time.sleep(polling_delay)

def check_that_histogram_is_big_enough(input_histogram):
    print('check_that_histogram_is_big_enough: %s' % input_histogram)

    filesize = os.path.getsize(input_histogram)

    if filesize < 5:
        print('ERROR: root input file is too small (%s bytes): %s' % (filesize, input_histogram))
        sys.exit(1)
    else:
        print('OK: root input file size is OK (%s bytes): %s' % (filesize, input_histogram))


def check_that_histogram_is_not_zombie(input_histogram):
    print('check_that_histogram_is_not_zombie: %s' % input_histogram)

    root_tfile = ROOT.TFile(input_histogram, "read")

    if root_tfile.IsZombie(): # THIS IS NOT WORKING :(
        print('ERROR: root input file is IsZombie: %s' % input_histogram)
        sys.exit(1)

main()
