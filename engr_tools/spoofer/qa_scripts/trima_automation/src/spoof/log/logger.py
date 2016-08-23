'''
Created on Jan 26, 2011

@author: jdavusb
'''

class log(object):
    '''
    classdocs
    '''

    def __init__(self, do_print, name=None):
        '''
        Constructor
        '''
        self.do_print = do_print
        self.logfile_name = name
        # Set up logger
        try:
            if (self.logfile_name != None):
                self.log = open(self.logfile_name, 'w')
                self.log.close()
            else: self.log = None
        except:
            print "Could not open logfile at " + self.logfile_name
            self.logfile_name = None
            self.log = None

    def report(self, msg):
        if self.do_print: print msg
        if self.log != None :
            try:
                if self.log.closed : 
                    self.log = open(self.logfile_name, 'w')
                self.log.write(str(msg))
                self.log.flush()
            except Exception, e:
                print e