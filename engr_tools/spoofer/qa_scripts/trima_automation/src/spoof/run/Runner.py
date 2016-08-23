'''
Created on Feb 3, 2011

@author: jdavusb
'''
import telnetlib
# for debugging prints
import sys

from spoof.log import logger

class Commander(object):
    '''
    classdocs
    '''


    def __init__(self, address, do_log=True):
        '''
        Constructor
        '''
        self.Connection = telnetlib.Telnet()
        self.address = address
        self.Commands = []
        self.logger = logger.log(do_log)
        self.returnVal = ''

    def log(self,msg):
        self.logger.report(msg)

    def Close(self):
        self.Connection.close()
    
    def AddCommand(self, cmd, end, isSpoof=False):
        # cmd: command, end: delimiter, isSpoof: true for named spoofs, false otherwise.
        #store command sets as a tuple.
        self.Commands.append((cmd,end,isSpoof))
    
    def Run(self):
        self.Connection.open(self.address)
        print self.Connection.read_until("->")
        for cmd in self.Commands:
            # Is this a named spoof? Format the command using -id and -trace.
            if (cmd[2]): spoof_cmd = "spoof_main \"-id " + cmd[0] + " -trace\"\n"
            #if (cmd[2]): spoof_cmd = "spoof_main \"-id " + cmd[0] + "\"\n"
            # This isn't a named spoof
            else: spoof_cmd = cmd[0]
            # Send the command.
            self.Connection.write(spoof_cmd)
            incoming = ""
            # Look for delimiting string...
            while incoming.find(cmd[1]) == -1:
                # Get telnet output until we encounter cmd[1]
                try:
                    incoming = self.Connection.expect([cmd[1], "\n"])[2] # expect returns a tuple; index [2] contains the string.
                except EOFError:
                    raw_input("Telnet connection closed unexpectedly,\nexit all other connections or reboot machine.\nPress ENTER to close this window.")
                    break
                self.log(incoming)
                self.returnVal = incoming
        self.Connection.write("exit")
        self.Connection.close()
    
    def ResetCommands(self):
        self.Commands = []
		
    def ReturnValue(self):
        value = ''
        parts = self.returnVal.split('=')
        if (len(parts) > 1):
            value = parts[1]
            value = value.replace(' ','')
        return value
		