// This class is used by the client to verify spoof elements (e.g., that the object name entered is valid)
// Recommendation: only use these methods when the -debug argument is used
// At this point, there's no real need for this to be a class, but I'm setting it up as such as I may be adding to it
//  for additional verifications.

#ifndef VERIFY_HPP
#define VERIFY_HPP

class verify {
public:
    static bool isSupportedObject(const char*);
};

#endif
