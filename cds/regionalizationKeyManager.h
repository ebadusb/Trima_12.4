#ifndef REGIONALIZATION_KEY_MANAGER_H
#define REGIONALIZATION_KEY_MANAGER_H

#include <string>
#include <vector>

#ifdef _WIN32
#ifndef uint32_t
typedef int uint32_t;
#endif
#endif

enum FEATURE_OPCODE
{
   FEATURE_ON,
   FEATURE_OFF
};

struct FeatureList
{
   FEATURE_OPCODE   opCode;
   std::vector<int> features;
};

struct ValueFeatureItem
{
   uint32_t value;
   int      feature;
};

enum KeyType
{
   BOOLEAN_KEY,
   VALUE_KEY
};

struct KeyGroups
{
   std::vector<int> sites;
   std::vector<int> customers;
   std::vector<int> countries;
   std::vector<int> serialNumbers;
};

struct BooleanKeyDefinition
{
   BooleanKeyDefinition(){}

   std::vector<FeatureList> opCodeTargetLists;
};

struct ValueKeyDefinition
{
   ValueKeyDefinition(){}

   std::vector<ValueFeatureItem> valueTargetLists;
};

struct KeyDefinition
{
   KeyDefinition ()
      : keyType(BOOLEAN_KEY), error(true){}
   KeyType              keyType;
   bool                 error;
   KeyGroups            groups;
   BooleanKeyDefinition boolKey;
   ValueKeyDefinition   valueKey;


};

class KeyLocation
{
public:
   int  getCurrentBit () const {return currentBit; }
   void setCurrentBit (int newBit) {currentBit = newBit; }
   int  getNumBytes () const {return numBytes; }

   KeyLocation(unsigned char* initialByte) {currentByte = initialByte; currentBit = 7; numBytes = 0; }
   void           MoveToNextByte () {currentByte += 1; numBytes++; }
   unsigned char* getByte () const {return currentByte; }
   int            getNumBits () {return numBytes * 8 + 7 - currentBit; }

private:
   int            currentBit;
   int            numBytes;
   unsigned char* currentByte;
};


class RegionalizationKeyManager
{
public:
   RegionalizationKeyManager();
   ~RegionalizationKeyManager();

   std::string   createKey (KeyDefinition def);
   KeyDefinition decodeKey (std::string key);


private:
   KeyLocation* createValueKey (ValueKeyDefinition& def,
                                KeyLocation* currentLoc);

   KeyLocation* createBooleanKey (BooleanKeyDefinition& def,
                                  KeyLocation* currentLoc);
   enum MachineGroupType {CUSTOMER_TYPE, REGION_TYPE, REG_BODY_TYPE, SERIAL_TYPE};

   KeyLocation* setValue (KeyLocation* currentLoc, unsigned int value, unsigned int numBits);

   unsigned int getBits (unsigned char* key, int* startBit, int numBits);
   std::string  convertToString (unsigned char* key1, int numBits);
   int          nibblesForBits (int numBits) ;
   int          getNibbles (int value);
   int          getMaxNibbles (std::vector<int>* list);
   KeyLocation* writeMachineGroup (KeyLocation* currentLoc, std::vector<int> const& list, MachineGroupType type);
   int          insertChecksum (unsigned char* key1, int length, unsigned char* withChecksum);
   std::string  convertToHex (unsigned char* key, int numBits);

   int            convertKeyToBits (std::string key, unsigned char* bitKey);
   unsigned short removeChecksum (unsigned char* bitKeyWithChecksum, unsigned char* bitKeyWithoutChecksum);
   int            computeChecksumSpaceBits (int length, int random);

   void decodeMachineGroups (unsigned char* bitKeyWithoutChecksum, int* currentBit, unsigned short numGroups, KeyGroups* def);
   void decodeOpCodes (unsigned char* bitKeyWithoutChecksum, int* currentBit, unsigned short numOpcodes, KeyDefinition* def);
   void decodeValues (unsigned char* bitKeyWithoutChecksum,
                      int* currentBit,
                      KeyDefinition* def);

};

#endif

/* FORMAT HASH 9c181c4a86cb412a7cd5ae6e00b798c8 */
