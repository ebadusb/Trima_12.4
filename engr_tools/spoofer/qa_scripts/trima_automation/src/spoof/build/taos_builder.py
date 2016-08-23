'''
Created on Jan 19, 2011

@author: jdavusb
'''

import subprocess
import os
from spoof.log import logger

class base_builder(object):
    '''
    Generic builder
    '''

    def __init__(self, buildroot, batchfile, logfile, tor_dir):
        '''
        Constructor
        '''
        self.buildroot = buildroot
        self.batchfile = batchfile
        self.full_batch_path = buildroot + batchfile
        self.logfile = logfile

        # Adds the tornado variables to the OS's environment variables. The build 
        # system needs these additions.
        os.environ["WIND_HOST_TYPE"] = "x86-win32"
        os.environ["WIND_BASE"] = tor_dir
        os.environ["PATH"] = tor_dir + "host/x86-win32/bin;" + os.environ["PATH"]

        try:
            f = open(self.full_batch_path)
        except Exception, e:
            BatchError = "Build batch file could not be opened. Error:" + e
            raise BatchError
        finally:
            if f != None: f.close()
            
        self.build_logger = logger.log(True, self.logfile)

    def build(self):

        # Working directory must be the build root.
        try:
            if os.path.exists(self.full_batch_path):
                os.chdir(self.buildroot)
        except:
            BadPath = "Invalid path or batch file name."
            self.build_logger.report(BadPath)
            raise BadPath
            
        try:
            if self.batchfile == "makefile.vx" : self.batchfile = "make -f " + self.batchfile 
            output= subprocess.Popen(self.batchfile, stdout=subprocess.PIPE, 
                                     stderr=subprocess.STDOUT).stdout
        except:
            BuildFailed = "Could not run batch file. Build Failed."
            self.build_logger.report(BuildFailed)
            raise BuildFailed

        for line in output:
            self.build_logger.report(line)
            if line.find("Tools build failed") > -1:
                self.build_logger.report("Build failed. See " + self.logfile + " for more information.")
                BuildFailed = "Tools Build Failed"
                raise BuildFailed

          

