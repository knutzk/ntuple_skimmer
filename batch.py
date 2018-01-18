from glob import glob
import subprocess
import sys
import os

input_path = os.environ['HOME'] + "/ttgamma/eos/v010_production/"
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


if __name__ == "__main__":
    files = []
    for pattern in search_patterns:
        find_count = 0
        for path in search_paths:
            if not os.path.isdir(path):
                print "Input file path does not exist"
                sys.exit(-1)
            matching_files = glob("%s/%s" % (path, pattern))
            files.extend(matching_files)
            find_count += len(matching_files)
        print "Found %s files matching pattern %s" % (find_count, pattern)


    # Go through all files, process a maximum of 10 files at a time. Otherwise,
    # wait for a process to finish first.
    import time
    processes = []
    file_counter = 0
    for f in files:
        ready_to_iterate = False
        while not ready_to_iterate:
            for p in processes:
                if not p.poll() == None:
                    processes.remove(p)
            if len(processes) > 10:
                # print "Waiting ..."
                time.sleep(1)
            else:
                ready_to_iterate = True

        command = "./skimmer %s %s" % (f, f.replace(input_path, output_path))
        file_counter += 1
        print "Processing file %s of %s: %s" % (file_counter, len(files), f)
        processes.append(subprocess.Popen([command], shell=True, stdin=None, stdout=None, stderr=None))


   # # Running this code instead can apparently crash lxplus machines. :-)
   #  for pattern in search_patterns:
   #      files = []
   #      for path in search_paths:
   #          if not os.path.isdir(path):
   #              print "Input file path does not exist"
   #              sys.exit(-1)
   #          files.extend(glob.glob("%s/%s" % (path, pattern)))
   #      print "Found %s files matching pattern %s" % (len(files), pattern)

   #      processes = []
   #      for f in files:
   #          command = "./skimmer %s %s" % (f, f.replace(input_path, output_path))
   #          processes.append(subprocess.Popen([command], shell=True, stdin=None, stdout=None, stderr=None))
   #      [p.communicate() for p in processes]
