#include "RegionalizationKeyManager.h"

#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <cstdio>

#ifndef _WIN32
#include "../cds/crc16.h"
#else
#include "crc16.h"
#endif

#define DEBUG_PRINT_ENABLED 0
#if DEBUG_PRINT_ENABLED
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT while (0) printf
#endif

void printKeyDef (KeyDefinition const& def)
{
   printf("Key Definition:");
   if (def.keyType == BOOLEAN_KEY)
   {
      if (!def.groups.sites.empty())
      {
         printf("\nsites: ");
         for (std::vector<int>::const_iterator it = def.groups.sites.begin(); it != def.groups.sites.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.customers.empty())
      {
         printf("\nCustomers: ");
         for (std::vector<int>::const_iterator it = def.groups.customers.begin(); it != def.groups.customers.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.countries.empty())
      {
         printf("\nRegulatory Bodies: ");
         for (std::vector<int>::const_iterator it = def.groups.countries.begin(); it != def.groups.countries.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.serialNumbers.empty())
      {
         printf("\nSerial Numbers: ");
         for (std::vector<int>::const_iterator it = def.groups.serialNumbers.begin(); it != def.groups.serialNumbers.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.boolKey.opCodeTargetLists.empty())
      {
         for (std::vector<FeatureList>::const_iterator it = def.boolKey.opCodeTargetLists.begin(); it != def.boolKey.opCodeTargetLists.end(); it++)
         {
            printf("\nOp Code: %d; Targets:", (*it).opCode);
            for (std::vector<int>::const_iterator it2 = (*it).features.begin(); it2 != (*it).features.end(); it2++)
            {
               printf("%d; ", *it2);
            }
         }
      }
   }
   else
   {
      if (!def.groups.sites.empty())
      {
         printf("\nsites: ");
         for (std::vector<int>::const_iterator it = def.groups.sites.begin(); it != def.groups.sites.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.customers.empty())
      {
         printf("\nCustomers: ");
         for (std::vector<int>::const_iterator it = def.groups.customers.begin(); it != def.groups.customers.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.countries.empty())
      {
         printf("\nRegulatory Bodies: ");
         for (std::vector<int>::const_iterator it = def.groups.countries.begin(); it != def.groups.countries.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.groups.serialNumbers.empty())
      {
         printf("\nSerial Numbers: ");
         for (std::vector<int>::const_iterator it = def.groups.serialNumbers.begin(); it != def.groups.serialNumbers.end(); it++)
         {
            printf("%d; ", *it);
         }
      }

      if (!def.valueKey.valueTargetLists.empty())
      {
         for (std::vector<ValueFeatureItem>::const_iterator it = def.valueKey.valueTargetLists.begin(); it != def.valueKey.valueTargetLists.end(); it++)
         {
            printf("\nFeature ID: %d; Value: %d", (*it).feature, (*it).value);
         }
      }
   }
   printf ("\n");
}

#ifdef KEY_GEN_TEST_TOOL
int main ()
{
   RegionalizationKeyManager* rm = new RegionalizationKeyManager();

   printf("Generating...\n");
   KeyDefinition* def = new KeyDefinition();


   // def->customers.push_back(0);
   // def->groups.serialNumbers.push_back(3000);
   def->groups.sites.push_back(0);
   // def->sites.push_back(10);


   def->keyType = BOOLEAN_KEY;
   FeatureList fl;
   fl.opCode    = (FEATURE_OPCODE)1;
   fl.features.push_back(0);
   // fl.features.push_back(15);
   def->boolKey.opCodeTargetLists.push_back(fl);

   // def->keyType = VALUE_KEY;
   // ValueFeatureItem fl;
   // fl.feature   = 0;
   // fl.value     = 123456;
   // def->valueKey.valueTargetLists.push_back(fl);


   // fl.feature = 2;
   // fl.value   = 45;
   // def->valueKey.valueTargetLists.push_back(fl);

   // fl.features.clear();
   // fl.opCode = 1;
   // fl.features.push_back(1);
   // def->opCodeTargetLists.push_back(fl);

   printKeyDef(*def);

   std::string key = rm->createKey(*def);

   printf("%s\n", key.c_str());

   printf("\nDecoding...\n");

   key  = "0YE560X90XIJT";

   *def = rm->decodeKey(key);

   printKeyDef(*def);

   return 0;
}
#endif

RegionalizationKeyManager::RegionalizationKeyManager()
{}


RegionalizationKeyManager::~RegionalizationKeyManager()
{}

#if CPU==SIMNT
#define LOG2E 1.44269504088896340736 // log2(e)

double log2 (const double x)
{
   return log(x) * LOG2E;
}
#endif


const int MAX_KEY_LENGTH      = 256;
const int BOOLEAN_KEY_VERSION = 0;
const int VALUE_KEY_VERSION   = 1;
const int CHECK_SUM_BITS      = 16;

// Bit lengths of fields
const int KEY_FORMAT_VERSION_LENGTH = 4;
const int CHECK_SUM_SPACING_BITS    = 2;
const int KEY_NUM_MACHINE_GROUPS    = 4;
const int KEY_NUM_NIBBLES_CUSTOMER  = 3;
const int KEY_NUM_NIBBLES_REGION    = 2;
const int KEY_NUM_NIBBLES_REG_BODY  = 2;
const int KEY_NUM_NIBBLES_SERIAL    = 3;
const int KEY_NUM_OPCODES           = 2;  // length of field describing the number of opcodes in the key
const int KEY_NUM_BOOL_VALUES       = 3;  // length of field describing the number of values for a given opcode in the key
const int KEY_OPCODE                = 4;  // length of opcode value
const int KEY_VAL_NUM_VALUES_LENGTH = 3;  // length of number of values in key
const int KEY_VAL_IDS_LENGTH        = 5;  // length of field containing feature ids for value features
const int KEY_VAL_VALUES_LENGTH     = 32; // length of field containing values for value features
const int KEY_TYPE                  = 1;
const int KEY_VALUE_ID_LENGTH       = 5;  // length used for each feature id
const int KEY_TARGET_NIBBLES        = 4;  // length of number of nibbles used for each feature id
const int KEY_LENGTH                = 8;

// Values for fields
const int KEY_BITFIELD_TYPE = 0;
const int KEY_VARIABLE_TYPE = 1;

const int KEY_NUM_TARGETS   = 16;

// const int KEY_OPCODE_ENABLE = 0;
// const int KEY_OPCODE_DISABLE = 1;




// Set numBits of value at the currentBit of the currentByte in currentLoc
KeyLocation* RegionalizationKeyManager::setValue (KeyLocation* currentLoc, unsigned int value, unsigned int numBits)
{
   // Set a mask (e.g. 0x00000111) to zero any bits in value outside of the least significant numBits

   // Need to be sure to not left shift by more than 31 bits or behavior is undefined (and in
   // the case of our processor results in a no-op).  Just use original "value" if numBits is 32 or more.
   if (numBits < 32)
   {
      value &= ~((~0) << numBits);
   }

   // Handle using two cases.  First case: the value fits in the current byte.  Second: the value does not fit in the current byte.
   if (currentLoc->getCurrentBit() >= (numBits - 1))
   {
      // Shift the relevant bits of value to the left so they line up with the desired bits in currentByte
      unsigned char tmpValue = value << (currentLoc->getCurrentBit() - (numBits - 1));

      // set the target bits to 0
      unsigned char mask = ~((~0) << numBits);
      mask                      = ~(mask << (currentLoc->getCurrentBit() - (numBits - 1)));
      *(currentLoc->getByte()) &= mask;

      // Set the desired bits
      *(currentLoc->getByte()) |= tmpValue;

      // Increment the currentBit and move to the next byte if required
      currentLoc->setCurrentBit(currentLoc->getCurrentBit() - numBits);
      if (currentLoc->getCurrentBit() < 0)
      {
         currentLoc->MoveToNextByte();
         currentLoc->setCurrentBit(8 + currentLoc->getCurrentBit());
      }
   }
   else  // value didn't fit in the current byte
   {
      // how much does fit in the current byte
      int diff = (currentLoc->getCurrentBit() + 1);

      // right shift value into a tmp buffer for the amount that will fit in the current byte
      int tmpValue = value >> (numBits - diff);

      // Write into the rest of the current byte
      currentLoc = setValue(currentLoc, tmpValue, diff);

      // Write the remainder into the next byte (previous call should increment currentLoc)
      currentLoc = setValue(currentLoc, value, numBits - diff);
   }

   return currentLoc;
}

// Start bit is num of bits from MSB
unsigned int RegionalizationKeyManager::getBits (unsigned char* key, int* startBit, int numBits)
{
   if (numBits > 32)
   {
      printf("getBits error.  Cannot read data larger than 32 bits (%d).\n", numBits);
      return 0;
   }

   unsigned int fullValue = 0;
   // Read one byte at a time, up to a max of 4 bytes
   for (int i = 0; i < numBits; i += 8)
   {
      int           bitsInThisByte = numBits - i > 8 ? 8 : numBits - i;
      fullValue <<= bitsInThisByte;
      unsigned char byteValue      = 0;

      // Find the starting byte
      int byte = *startBit / 8;

      // find the starting offset
      int bit = *startBit % 8;

      // If entirely within this byte
      if (bit + (bitsInThisByte - 1) < 8)
      {
         byteValue   = key[byte];
         byteValue <<=  bit;
         byteValue >>=  (8 - bitsInThisByte);
      }
      else   // not entirely in one byte
      {
         byteValue   = key[byte];
         byteValue <<= bit;
         // value = value >> (8-bit);
         byteValue   = byteValue >> (8 - bitsInThisByte);

         unsigned int tmpValue = key[byte + 1];
         tmpValue   = tmpValue >> (8 - (bitsInThisByte - (8 - bit)));

         byteValue |= tmpValue;
      }

      *startBit += bitsInThisByte;
      fullValue |= byteValue;
   }

   return fullValue;
}

const int KEY_CHARACTER_BITS = 5;


std::string RegionalizationKeyManager::convertToString (unsigned char* key1, int numBits)
{
   std::string  str;
   unsigned int value;
   int          currentBit   = 0;
   bool         xyz012Toggle = false;
   int          xyzOffset    = 0;

   while (currentBit < numBits)
   {
      value = getBits(key1, &currentBit, KEY_CHARACTER_BITS);

      // Use 0-9 for first 10 values
      if (value < 10)
      {
         xyzOffset = 0;

         if (value == 0 || value == 1 || value == 2)
         {
            if (xyz012Toggle)
            {
               xyzOffset = 'X' - '0';
            }
            else
            {
               xyzOffset = 0;
            }
            xyz012Toggle = !xyz012Toggle;
         }

         str += (char)(value + '0' + xyzOffset);
      }
      // Then A-N
      else if (value < 24)
      {
         str += (char)((value - 10) + 'A');
      }
      // Then P-W (skip O to avoid confusion entering key)
      else if (value < 32)
      {
         str += (char)((value - 10) + 'A' + 1);
      }
      else
      {
         printf ("String conversion error: Invalid character value %d\n", value);
         return "";
      }
   }

   // Remove trailing 0's
   const size_t endStr = str.find_last_not_of("0");
   const size_t range  = endStr + 1;

   return str.substr(0, range);
   // return str;
}

std::string RegionalizationKeyManager::convertToHex (unsigned char* key, int numBits)
{
   char        tmp[10];
   std::string ret;
   for (int i = 0; i < ceil(numBits / 8.0); i++)
   {
      sprintf(tmp, "%02X", key[i]);
      ret += tmp[0];
      ret += tmp[1];
   }
   return ret;
}

int RegionalizationKeyManager::nibblesForBits (int numBits)
{
   return (int)ceil((double)numBits / 4);
}

int RegionalizationKeyManager::getNibbles (int value)
{
   // can't compute log(0), so treat that as special case
   if (!value)
   {
      return 1;
   }

   int bits    = (int)floor(log2((float)value)) + 1;
   int nibbles = nibblesForBits(bits);

   return nibbles;
}


KeyLocation* RegionalizationKeyManager::writeMachineGroup (KeyLocation* currentLoc, std::vector<int> const& list, MachineGroupType type)
{
   for (std::vector<int>::const_iterator it = list.begin(); it != list.end(); ++it)
   {
      const int value           = *it;

      int       nibbles         = getNibbles(value);

      int       customerNibbles = 0;
      int       regionNibbles   = 0;
      int       regBodyNibbles  = 0;
      int       serialNibbles   = 0;
      switch (type)
      {
         case CUSTOMER_TYPE : customerNibbles = nibbles; break;
         case REGION_TYPE :   regionNibbles   = nibbles; break;
         case REG_BODY_TYPE : regBodyNibbles  = nibbles; break;
         case SERIAL_TYPE :   serialNibbles   = nibbles; break;
      }

      // Add negation bit
      currentLoc = setValue(currentLoc, customerNibbles, 1);

      currentLoc = setValue(currentLoc, customerNibbles, KEY_NUM_NIBBLES_CUSTOMER);
      currentLoc = setValue(currentLoc, regionNibbles, KEY_NUM_NIBBLES_REGION);
      currentLoc = setValue(currentLoc, regBodyNibbles, KEY_NUM_NIBBLES_REG_BODY);
      currentLoc = setValue(currentLoc, serialNibbles, KEY_NUM_NIBBLES_SERIAL);

      currentLoc = setValue(currentLoc, value, nibbles * 4);
   }
   return currentLoc;
}

// length in bits
int RegionalizationKeyManager::computeChecksumSpaceBits (int length, int random)
{
   int spaceBits = (int)ceil(length / (float)CHECK_SUM_BITS);
   spaceBits += random;

   for (int i = 0; i < 2; i++)
   {
      if (spaceBits > 1)
      {
         --spaceBits;
      }
   }
   return spaceBits;
}

// length in bits
// return bits
int RegionalizationKeyManager::insertChecksum (unsigned char* key1, int length, unsigned char* withChecksum)
{
   int            byteLength = (int)ceil(length / 8.0);
   Crc16          crcCalc;
   unsigned short crcValue   = crcCalc.doCRC16(key1, byteLength);
   unsigned char  crc[2];
   crc[0] = crcValue >> 8;
   crc[1] = (unsigned char)crcValue;

   srand (crcValue);
   int           r         = rand();
   unsigned char random    = r % 4;
   int           spaceBits = computeChecksumSpaceBits(byteLength * 8 - KEY_FORMAT_VERSION_LENGTH, random);

   DEBUG_PRINT("checksum = %d\n", crcValue);
   DEBUG_PRINT("spaceBits = %d\n", spaceBits);

   int          currentChecksumBit = 0;
   KeyLocation  startKey(withChecksum);
   KeyLocation* loc                = &startKey; // new KeyLocation(withChecksum);


   // Set the version number
   int currentBit = 0;
   loc = setValue(loc, getBits(key1, &currentBit, KEY_FORMAT_VERSION_LENGTH), KEY_FORMAT_VERSION_LENGTH);

   // Set the key length w/o checksum bits (in bytes)
   loc = setValue(loc, byteLength, KEY_LENGTH);

   // Insert the checksum bit spacing
   loc = setValue(loc, random, CHECK_SUM_SPACING_BITS);

   while (currentBit < byteLength * 8)
   {
      if (((byteLength * 8) - currentBit) < spaceBits)
      {
         spaceBits = (byteLength * 8) - currentBit;
      }

      loc = setValue(loc, getBits(key1, &currentBit, spaceBits), spaceBits);
      if (currentChecksumBit < CHECK_SUM_BITS)
      {
         loc = setValue(loc, getBits(crc, &currentChecksumBit, 1), 1);
      }
   }

   while (currentChecksumBit < CHECK_SUM_BITS)
   {
      loc = setValue(loc, getBits(crc, &currentChecksumBit, 1), 1);
   }

   length = loc->getNumBits();

   // delete loc;

   return length;
}

std::string RegionalizationKeyManager::createKey (KeyDefinition def)
{
   unsigned char key1[MAX_KEY_LENGTH] = {0};
   // unsigned char  key2[MAX_KEY_LENGTH] = {0};

   int            startBit    = 7;
   unsigned char* currentByte = &key1[0];

   KeyLocation    startKey(&key1[0]);
   KeyLocation*   currentLoc = &startKey; // new KeyLocation(&key1[0]);

   // Key format version section ////////////////////////////////////////////////////
   if (def.keyType == BOOLEAN_KEY)
   {
      currentLoc = setValue(currentLoc, BOOLEAN_KEY_VERSION, KEY_FORMAT_VERSION_LENGTH);
   }
   else
   {
      currentLoc = setValue(currentLoc, VALUE_KEY_VERSION, KEY_FORMAT_VERSION_LENGTH);
   }

   // Machine Group section ////////////////////////////////////////////////////////

   // Number of machine groups
   int numGroups = 0;
   numGroups += def.groups.customers.size();
   numGroups += def.groups.sites.size();
   numGroups += def.groups.countries.size();
   numGroups += def.groups.serialNumbers.size();

   if (numGroups > 15)
   {
      printf("Error: Too many machine groups (%d)\n", numGroups);
      return "";
   }

   currentLoc = setValue(currentLoc, numGroups, KEY_NUM_MACHINE_GROUPS);

   currentLoc = writeMachineGroup(currentLoc, def.groups.customers, CUSTOMER_TYPE);
   currentLoc = writeMachineGroup(currentLoc, def.groups.sites, REGION_TYPE);
   currentLoc = writeMachineGroup(currentLoc, def.groups.countries, REG_BODY_TYPE);
   currentLoc = writeMachineGroup(currentLoc, def.groups.serialNumbers, SERIAL_TYPE);

   std::string key;
   if (def.keyType == BOOLEAN_KEY)
   {
      currentLoc = createBooleanKey(def.boolKey, currentLoc);
   }
   else
   {
      currentLoc = createValueKey(def.valueKey, currentLoc);
   }

   if (currentLoc == NULL)
      return "";

   DEBUG_PRINT("Hex Key (no checksum): %s\n", convertToHex(key1, currentLoc->getNumBits()).c_str());

   unsigned char withChecksum[MAX_KEY_LENGTH] = {0};
   // int lengthInBitsNoChecksum = currentLoc->numBytes*8 + (7-currentLoc->currentBit);
   int           newLength                    = insertChecksum(key1, currentLoc->getNumBits(), withChecksum);

   /*if (strlen(key1) < strlen(key2)) {
       return std::string(key1);
   }*/

   // delete currentLoc;

   return convertToString((unsigned char*)withChecksum, newLength);
   // return convertToString((unsigned char*) key1, currentLoc->numBytes *8 + 8);
   // return convertToHex((unsigned char*) key1, currentLoc->numBytes *8 + 8);

   return key;

}


KeyLocation* RegionalizationKeyManager::createBooleanKey (BooleanKeyDefinition& def,
                                                          KeyLocation* currentLoc)
{
   // Opcode section //////////////////////////////////////////////////////////////
   currentLoc = setValue(currentLoc, def.opCodeTargetLists.size(), KEY_NUM_OPCODES);

   // Begin computing two different styles of key.
   // Get key2 and currentLoc2 up to date with key1 and currentLoc
   // memcpy(key2, key1, sizeof(key1));
   // KeyLocation  startKey2(&key2[0] + currentLoc->getNumBytes());
   // KeyLocation* currentLoc2 = &startKey2;  // new KeyLocation(&key2[0] + currentLoc->numBytes);
   // currentLoc2->setCurrentBit(currentLoc->getCurrentBit());

   // Write each opcode with targets
   for (std::vector<FeatureList>::iterator it = def.opCodeTargetLists.begin(); it != def.opCodeTargetLists.end(); ++it)
   {
      FeatureList fl = *it;

      if (fl.opCode == FEATURE_ON || fl.opCode == FEATURE_OFF)
      {
         // key 1
         std::vector<int>::iterator max = std::max_element (fl.features.begin(), fl.features.end());
         int numTargetBits              = *max + 1;
         int nibbles                    = nibblesForBits(numTargetBits);

         currentLoc = setValue(currentLoc, fl.opCode, KEY_OPCODE);
         currentLoc = setValue(currentLoc, KEY_BITFIELD_TYPE, KEY_TYPE);
         currentLoc = setValue(currentLoc, nibbles, KEY_TARGET_NIBBLES);

         int targets = 0;
         for (std::vector<int>::iterator it = fl.features.begin(); it != fl.features.end(); ++it)
         {
            targets |= 1 << *it;
         }

         currentLoc = setValue(currentLoc, targets, nibbles * 4);


         // key2
         // currentLoc2 = setValue(currentLoc2, KEY_VARIABLE_TYPE, KEY_TYPE);

      }
      else
      {
         printf("Unsupported Opcode (%d)\n", fl.opCode);
         return NULL;
      }

   }

   return currentLoc;
}

static bool order_value_item (const ValueFeatureItem& a, const ValueFeatureItem& b) { return a.feature < b.feature; }

KeyLocation* RegionalizationKeyManager::createValueKey (ValueKeyDefinition& def,
                                                        KeyLocation* currentLoc)
{
   currentLoc = setValue(currentLoc, def.valueTargetLists.size(), KEY_VAL_NUM_VALUES_LENGTH);

   // compute length needed for feature ids
   std::vector<ValueFeatureItem>::iterator max = std::max_element (def.valueTargetLists.begin(),
                                                                   def.valueTargetLists.end(),
                                                                   order_value_item);
   int numTargetBits = (*max).feature + 1;
   int nibbles       = nibblesForBits(numTargetBits);

   // write length used for each id
   // currentLoc = setValue(currentLoc, nibbles, KEY_VALUE_ID_LENGTH);

   // Write each value and feature id
   for (std::vector<ValueFeatureItem>::iterator it = def.valueTargetLists.begin(); it != def.valueTargetLists.end(); ++it)
   {
      ValueFeatureItem fl = *it;

      currentLoc = setValue(currentLoc, fl.feature, KEY_VALUE_ID_LENGTH);
      currentLoc = setValue(currentLoc, fl.value, KEY_VAL_VALUES_LENGTH);
   }


   return currentLoc;
}

int RegionalizationKeyManager::convertKeyToBits (std::string key, unsigned char* bitKey)
{
   KeyLocation    startKey(&bitKey[0]);
   KeyLocation*   currentLoc = &startKey; // new KeyLocation(&bitKey[0]);
   unsigned short value      = 0;
   unsigned char  currentLetter;

   for (std::string::iterator it = key.begin(); it < key.end(); it++)
   {
      currentLetter = *it;
      if (currentLetter >= '0' && currentLetter <= '9')
      {
         value = currentLetter - '0';
      }
      else if (currentLetter >= 'A' && currentLetter <= 'N')
      {
         // offset by 10 to account for 0-9 being numbers, so A = 10
         value = currentLetter - 'A' + 10;
      }
      else if (currentLetter == 'O')
      {
         // 'O' is treated as '0'
         value = 0;
      }
      else if (currentLetter >= 'P' && currentLetter <= 'W')
      {
         // offset by 10 to account for 0-9 being numbers, so A = 10
         // Account for missing 'O' in decoding by subtracting 1 (e.g. 'P' = 25; not 26 because 'O' is skipped)
         value = currentLetter - 'A' - 1 + 10;
      }
      else if (currentLetter >= 'X' && currentLetter <= 'Z')
      {
         value = currentLetter - 'X';
      }
      else
      {
         printf("Invalid key letter (%c %d)\n", currentLetter, currentLetter);
         return 0;
      }

      currentLoc = setValue(currentLoc, value, KEY_CHARACTER_BITS);
   }

   // delete currentLoc;

   return key.size() * KEY_CHARACTER_BITS;
}


// length in bits
unsigned short RegionalizationKeyManager::removeChecksum (unsigned char* bitKeyWithChecksum,
                                                          unsigned char* bitKeyWithoutChecksum)
{
   int            currentBit = 0;

   KeyLocation    startKey(bitKeyWithoutChecksum);
   KeyLocation*   loc      = &startKey; // new KeyLocation(bitKeyWithoutChecksum);
   unsigned short checksum = 0;

   // Set the version
   loc = setValue(loc, getBits(bitKeyWithChecksum, &currentBit, KEY_FORMAT_VERSION_LENGTH), KEY_FORMAT_VERSION_LENGTH);

   unsigned short keyLengthNoChecksum = getBits(bitKeyWithChecksum, &currentBit, KEY_LENGTH);

   int            random              = getBits(bitKeyWithChecksum, &currentBit, CHECK_SUM_SPACING_BITS);

   int            spaceBits           = computeChecksumSpaceBits(keyLengthNoChecksum * 8 - KEY_FORMAT_VERSION_LENGTH, random);

   DEBUG_PRINT("spaceBits = %d\n", spaceBits);

   unsigned short bit                = 0;
   int            currentChecksumBit = 0;

   while (loc->getNumBits() < (keyLengthNoChecksum * 8))
   // for (int i = CHECK_SUM_SPACING_BITS; i < length; i+=spaceBits+1, currentChecksumBit++)
   {
      if (keyLengthNoChecksum * 8 - (loc->getNumBits()) < spaceBits)
      {
         spaceBits = (keyLengthNoChecksum * 8 - loc->getNumBits());
      }
      loc = setValue(loc, getBits(bitKeyWithChecksum, &currentBit, spaceBits), spaceBits);

      if (currentChecksumBit < 16)
      {
         bit        = getBits(bitKeyWithChecksum, &currentBit, 1);
         checksum <<= 1;
         checksum  |= bit;

         currentChecksumBit++;
      }
   }

   // pad with zeros to fill out the checksum
   while (currentChecksumBit < 16)
   {
      bit        = getBits(bitKeyWithChecksum, &currentBit, 1);
      checksum <<= 1;
      checksum  |= bit;

      currentChecksumBit++;
      // checksum <<= 1;
   }

   DEBUG_PRINT("checksum = %d\n", checksum);

   DEBUG_PRINT("Hex Key (no checksum): %s\n", convertToHex(bitKeyWithoutChecksum, keyLengthNoChecksum * 8).c_str());
   // delete loc;
   return checksum;

}

void RegionalizationKeyManager::decodeMachineGroups (unsigned char* bitKeyWithoutChecksum, int* currentBit, unsigned short numGroups, KeyGroups* def)
{
   int          nibblesCustomer = 0;
   int          nibblesRegion   = 0;
   int          nibblesRegBody  = 0;
   int          nibblesSerial   = 0;
   unsigned int machineGroupId  = 0;

   for (int i = 0; i < numGroups; i++)
   {
      // read negation bit
      getBits(bitKeyWithoutChecksum, currentBit, 1);

      nibblesCustomer = getBits(bitKeyWithoutChecksum, currentBit, KEY_NUM_NIBBLES_CUSTOMER);
      nibblesRegion   = getBits(bitKeyWithoutChecksum, currentBit, KEY_NUM_NIBBLES_REGION);
      nibblesRegBody  = getBits(bitKeyWithoutChecksum, currentBit, KEY_NUM_NIBBLES_REG_BODY);
      nibblesSerial   = getBits(bitKeyWithoutChecksum, currentBit, KEY_NUM_NIBBLES_SERIAL);

      if (nibblesCustomer)
      {
         machineGroupId = getBits(bitKeyWithoutChecksum, currentBit, nibblesCustomer * 4);
         def->customers.push_back(machineGroupId);
      }

      if (nibblesRegion)
      {
         machineGroupId = getBits(bitKeyWithoutChecksum, currentBit, nibblesRegion * 4);
         def->sites.push_back(machineGroupId);
      }

      if (nibblesRegBody)
      {
         machineGroupId = getBits(bitKeyWithoutChecksum, currentBit, nibblesRegBody * 4);
         def->countries.push_back(machineGroupId);
      }

      if (nibblesSerial)
      {
         machineGroupId = getBits(bitKeyWithoutChecksum, currentBit, nibblesSerial * 4);
         def->serialNumbers.push_back(machineGroupId);
      }
   }

   return;
}

void RegionalizationKeyManager::decodeOpCodes (unsigned char* bitKeyWithoutChecksum, int* currentBit, unsigned short numOpCodes, KeyDefinition* def)
{
   FEATURE_OPCODE opCode        = FEATURE_OFF;
   int            type          = 0;
   unsigned int   targetNibbles = 0;
   unsigned int   bitField      = 0;
   FeatureList    fl;

   for (int i = 0; i < numOpCodes; i++)
   {
      fl.features.clear();

      opCode    = (FEATURE_OPCODE)getBits(bitKeyWithoutChecksum, currentBit, KEY_OPCODE);
      fl.opCode = opCode;

      type      = getBits(bitKeyWithoutChecksum, currentBit, KEY_TYPE);
      if (type == KEY_BITFIELD_TYPE)
      {
         targetNibbles = getBits(bitKeyWithoutChecksum, currentBit, KEY_TARGET_NIBBLES);
         bitField      =  getBits(bitKeyWithoutChecksum, currentBit, targetNibbles * 4);
         for (unsigned int i = 0; i < targetNibbles * 4; i++)
         {
            if (bitField & 0x1)
            {
               fl.features.push_back(i);
            }
            bitField >>= 1;
         }
      }
      else if (type == KEY_VARIABLE_TYPE)
      {}
      else
      {
         printf("Unknown bitfield type (%d)\n", type);
         return;
      }

      def->boolKey.opCodeTargetLists.push_back(fl);
   }
}

void RegionalizationKeyManager::decodeValues (unsigned char* bitKeyWithoutChecksum,
                                              int* currentBit,
                                              KeyDefinition* def)
{
   unsigned int     targetNibbles = 0;
   unsigned int     bitField      = 0;
   ValueFeatureItem fl;

   unsigned int     numValues = getBits(bitKeyWithoutChecksum, currentBit, KEY_VAL_NUM_VALUES_LENGTH);

   for (unsigned int i = 0; i < numValues; i++)
   {

      fl.feature = getBits(bitKeyWithoutChecksum, currentBit, KEY_VALUE_ID_LENGTH);

      fl.value   = getBits(bitKeyWithoutChecksum, currentBit, KEY_VAL_VALUES_LENGTH);

      def->valueKey.valueTargetLists.push_back(fl);
   }
}





KeyDefinition RegionalizationKeyManager::decodeKey (std::string key)
{
   KeyDefinition def;

   def.error = true;

   unsigned char bitKeyWithChecksum[MAX_KEY_LENGTH]    = {0};
   unsigned char bitKeyWithoutChecksum[MAX_KEY_LENGTH] = {0};

   int           length = convertKeyToBits(key, bitKeyWithChecksum);
   DEBUG_PRINT("%s\n", convertToString(bitKeyWithChecksum, length).c_str());

   int            currentBit = 0;
   unsigned short version    = getBits(bitKeyWithChecksum, &currentBit, KEY_FORMAT_VERSION_LENGTH);
   if (version != BOOLEAN_KEY && version != VALUE_KEY)
   {
      printf("Invalid key version id (%d)\n", version);
      return def;
   }

   unsigned short keyLengthNoChecksum = getBits(bitKeyWithChecksum, &currentBit, KEY_LENGTH);

   unsigned short checkSum            = removeChecksum(bitKeyWithChecksum, bitKeyWithoutChecksum);

   // length -= CHECK_SUM_BITS + CHECK_SUM_SPACING_BITS;
   Crc16          crcCalc;
   unsigned short crcValue = crcCalc.doCRC16(bitKeyWithoutChecksum, keyLengthNoChecksum);

   if (crcValue != checkSum)
   {
      printf("Invalid checksum bits\n");
      return def;
   }

   currentBit = KEY_FORMAT_VERSION_LENGTH;
//	unsigned short version = getBits(bitKeyWithoutChecksum, &currentBit, KEY_FORMAT_VERSION_LENGTH);

   unsigned short numGroups = getBits(bitKeyWithoutChecksum, &currentBit, KEY_NUM_MACHINE_GROUPS);

   decodeMachineGroups(bitKeyWithoutChecksum, &currentBit, numGroups, &def.groups);

   def.keyType = (KeyType)version;

   if (version == BOOLEAN_KEY)
   {
      unsigned short numOpcodes = getBits(bitKeyWithoutChecksum, &currentBit, KEY_NUM_OPCODES);

      decodeOpCodes(bitKeyWithoutChecksum, &currentBit, numOpcodes, &def);
   }
   else
   {
      decodeValues(bitKeyWithoutChecksum, &currentBit, &def);
   }

   def.error = false;

   return def;
}

/* FORMAT HASH 20c3c7b07e6383085cddaaeea9c98b75 */
