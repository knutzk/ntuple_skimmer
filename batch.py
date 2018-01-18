from glob import glob
import subprocess
import sys
import os
import time

input_path = os.environ['HOME'] + "/ttgamma/group-eos/v010_production/"
output_path = os.environ['HOME'] + "/eos/ttgamma_ntuples/v010_production/"

search_paths = []
search_paths.append(input_path + "QE2/ejets")
search_paths.append(input_path + "QE2/mujets")
search_paths.append(input_path + "CR1/ejets")
search_paths.append(input_path + "CR1/mujets")
search_paths.append(input_path + "CR1S/ejets")
search_paths.append(input_path + "CR1S/mujets")
search_paths.append(input_path + "CR1/ee")
search_paths.append(input_path + "CR1/mumu")
search_paths.append(input_path + "CR1/emu")
search_paths.append(input_path + "CR1S/ee")
search_paths.append(input_path + "CR1S/mumu")
search_paths.append(input_path + "CR1S/emu")

search_patterns = []
search_patterns.append("*ST_Wt_inclusive*p3138*")
search_patterns.append("*ST_others*p3138*")
search_patterns.append("*VV*p3317*")
search_patterns.append("*Wenu*p3317*")
search_patterns.append("*Wmunu*p3317*")
search_patterns.append("*Wtaunu*p3317*")
search_patterns.append("*Zee*p3317*")
search_patterns.append("*Zmumu*p3317*")
search_patterns.append("*Ztautau*p3317*")
search_patterns.append("*data15*3315*")
search_patterns.append("*data16*3315*")
search_patterns.append("*eegamma*")
search_patterns.append("*enugamma*")
search_patterns.append("*mumugamma*")
search_patterns.append("*munugamma*")
search_patterns.append("*taunugamma*")
search_patterns.append("*tautaugamma*")
search_patterns.append("410389.ttgamma_nonallhadronic.p3152*")
search_patterns.append("410501.ttbar_nonallhad_P8.p3138*")


def lookup_files(paths, patterns):
    dictionary = dict()
    for pattern in patterns:
        for path in paths:
            if not os.path.isdir(path):
                print "Input file path does not exist"
                sys.exit(-1)
            files = glob("%s/%s" % (path, pattern))
            dictionary["%s/%s" % (path, pattern)] = files
    return dictionary


class ProcessManager():
    def __init__(self, limit):
        self.processes = []
        self.proc_limit = limit

    def addProcess(self, command):
        with open(os.devnull, 'w') as fp:
            self.processes.append(subprocess.Popen([command], shell=True, stdout=fp))

    def removeFinishedProcs(self):
        for p in self.processes:
            if not p.poll() == None:
                self.processes.remove(p)

    def waitTillReady(self):
        while True:
            self.removeFinishedProcs()
            if len(self.processes) > self.proc_limit:
                time.sleep(1)
            else:
                break
        return

    def finalise(self):
        while True:
            self.removeFinishedProcs()
            if len(self.processes) > 0:
                time.sleep(1)
            else:
                break
        return


if __name__ == "__main__":
    # Store all files matching a certain combination of search pattern and
    # search path into a look-up dictionary.
    dictionary = lookup_files(search_paths, search_patterns)

    # Compile a list of all files to process.
    files = []
    for pattern in dictionary:
        if len(dictionary[pattern]) == 0: continue
        print "Found %s files matching %s" % (len(dictionary[pattern]), pattern)
        files.extend(dictionary[pattern])

    # Go through all files, process a maximum of 10 files at a time. Otherwise,
    # wait for a process to finish first.
    proc_manager = ProcessManager(10)
    file_counter = 0
    for f in files:
        proc_manager.waitTillReady()
        command = "./skimmer %s %s" % (f, f.replace(input_path, output_path))
        file_counter += 1
        print "Processing file %s of %s: %s" % (file_counter, len(files), f)
        proc_manager.addProcess(command)

    # Go through all search patterns again and add the processed files to one
    # large root file. Don't do more than 3 hadd commands simultaneously.
    proc_manager = ProcessManager(3)
    for pattern in dictionary:
        if len(dictionary[pattern]) == 0: continue
        corr_file_list = [f.replace(input_path, output_path) for f in dictionary[pattern]]

        proc_manager.waitTillReady()
        target_file = corr_file_list[0].replace("001.root", "root")
        print "Creating file %s" % target_file
        command = "hadd -f %s" % target_file
        for f in corr_file_list:
            command += " %s" % f
        proc_manager.addProcess(command)

    # Wait for all processes to finish before continuing.
    proc_manager.finalise()

    # Now remove the files that were added in the step before.
    for pattern in dictionary:
         for f in dictionary[pattern]:
             os.remove(f.replace(input_path, output_path))
