/**********************************************************************************
 *
 *  Message Payload Packer
 *
 *  Support for packing additional information (that is, information beyond the
 *  value of a given enum) into an INT_MESSAGE.
 *
 *  USAGE:
 *
 *  A Payload< ENUM, M > packs an enumerated constant and an unsigned value into
 *  a long integer, suitable for transmitting in an INT_MESSAGE: and unpacks the
 *  result on the other side.  ENUM is the type of the enumeration; this is used
 *  by Payload to produce values of the correct enumerated type.  M is the
 *  largest constant in the enumeration; Payload uses this to figure out how
 *  many bits to reserve for the enumerated constant.  Given:
 *
 *      Payload < E, M > payload (... see below ...);
 *
 *  the user can access the various values as:
 *
 *      payload._enum;    // the enumerated type
 *      payload._value;   // the additional value
 *      payload._payload; // the packed value to be sent or received though the
 *                        // message
 *
 *  Create Payloads by:
 *
 *   typedef Payload <E, M> MyPayloadType;
 *   MyPayloadType a ( enum, int ) // loads the enum and the value to be packed
 *   MyPayloadType b ( long )      // loads the payload and unpacks
 *
 *  If the user fiddles with the Payload's values themselves, use the pack() or
 *  unpack() functions to generate _payload from _enum and _value, or the other
 *  way around, respectively.
 *
 *  These are particularly used for AdjustmentMessages coming out of the trouble
 *  screen (gui/trouble.cpp), that is a good place to look for an example of use.
 *
 *
 **********************************************************************************/

#ifndef MESSAGE_PAYLOAD_PACKER
#define MESSAGE_PAYLOAD_PACKER

#include <utility> // Include for std::pair

// NOTE: there are *much much* better ways to do this in modern C++
// (e.g. constexpr), but we're dealing with a truly archaic compiler here, so
// it's best not to push things very much. --DGS

// First: compile-time compute log base 2 of an integer; E.G., position of highest bit.
template< unsigned N > struct static_log2
{
   enum { comp_next = N >> 1 };
   enum { old_val   = static_log2< comp_next >::VALUE, VALUE };
};

template <> struct static_log2< 1 >
{
   enum { VALUE =  1 };
};


// Second; the Payload template.  This handles the packing and unpacking of the
// enum and the additional value.
template< class ENUM, unsigned M >
struct Payload
{
   enum { SHIFT = static_log2<M>::VALUE };

   Payload()
      : _enum(0),
        _value(0),
        _payload(0) {}

   // Create from an enum and a value
   Payload(ENUM e, unsigned v)
      : _enum(e),
        _value(v)
   { pack(); }                  // pack to generate _payload

   // Create from a long; unpack to generate _enum and _value
   Payload(long p)
      : _payload(p) { unpack(); }

   // Assignment and Copy Ops -- pretty much what you'd think.  (We'd use the default ones if they
   // were available)
   Payload(const Payload& p)
      : _enum    (p._enum),
        _value   (p._value),
        _payload (p._payload)
   {}

   Payload& operator = (const Payload& p)
   {
      _enum    = p._enum    ;
      _value   = p._value   ;
      _payload = p._payload ;
      return *this;
   }

   // Implicit cast to long; this lets Payload<>s be used in Message.send() calls.
   operator long int() const { return _payload; }

   // We allow general access to the data -- it doesn't seem useful to hide these.
   mutable ENUM     _enum;
   mutable unsigned _value;
   mutable long     _payload;

   // Pack the current values of our enum and our value into an unsigned long, with the value bits
   // highest.
   void pack ()
   {
      _payload  = _value << SHIFT;    // Payload starts with value, shifted into higher bits
      _payload |= long(_enum);        // Load the enum constant into the low bites
   }

   // Unpack a payload into value and enum constant
   void unpack () const
   {
      // Recover value.  Be sure to operate on an unsigned long, so there's no question of what
      // the shift operator fills with
      _value = (unsigned)((unsigned long)(_payload) >> SHIFT);

      // Generate a mask that is 0 for the higher bits and 1 for the low bits where the enum
      // value sits
      unsigned long mask = (~0ul) << SHIFT;    // all 1's above, 0's below
      _enum = ENUM( (~mask) & _payload);
   }
};

template< class ENUM, unsigned M > inline
DataLog_Stream& operator << (DataLog_Stream& os, const Payload<ENUM, M>& payload)
{
   os << payload._enum << "(" << int(payload._enum)
      << ") & " << int(payload._value) << " <"
      << payload._payload
      << '>';


   return os;
}


#endif

/* FORMAT HASH 5910cedb859c7e0d6528765646d83cb5 */
