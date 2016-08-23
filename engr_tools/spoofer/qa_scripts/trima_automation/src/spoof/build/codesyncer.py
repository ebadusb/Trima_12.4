'''
Created on Jan 25, 2011

@author: jdavusb
'''

import subprocess

class p4(object):
    '''
    classdocs
    '''
    P4SERVER = "usbsrdcms01:1666"
    P4DEPOT = "//depot/main/embedded/Taos/Taos/..."
    
    class p4info(object):
        def __init__(self, workspace, user, pw):
            self.ws = workspace
            self.user = user
            self.pw = pw

    def __init__(self, ws, user, pw):
        '''
        Perforce sync command line follows this format:
        C:\>p4 -c <workspace name> -p usbsrdcms01:1666 -u <username> -P <password> sync //depot/main/embedded/Taos/Taos/...
        User must run "p4 client <workspace name>" first.
        Return p4 output list of strings.
        '''
        self.p4workspace = ws
        self.p4user = user
        self.p4pass = pw
        self.p4depot = p4.P4DEPOT
        self.p4server = p4.P4SERVER
        self.syncCommand = "p4 -c " + self.p4workspace + " -p " + self.p4server + " -u " + self.p4user
        self.syncCommand = self.syncCommand + " -P " + self.p4pass + " sync " + self.p4depot

    def sync(self):
        output = subprocess.Popen(self.syncCommand, stdout=subprocess.PIPE).stdout
        return output