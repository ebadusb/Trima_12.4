// FILENAME: spoofedelement.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedelement.hpp 1.1 2002/10/08 18:31:30 td07711 Exp td07711 $
// PURPOSE: manages spoofing of specific data element within a SpoofedObject
// CHANGELOG:
//   $Log: spoofedelement.hpp $
//   Revision 1.1  2002/10/08 18:31:30  td07711
//   Initial revision


#ifndef SpoofedElement_HPP // prevents multiple inclusion
#define SpoofedElement_HPP


#include <string> // STL string


// forward declarations
class SpoofableData;
class SpoofedObject;


// CLASSNAME: SpoofedElement
// RESPONSIBILITIES:
//   1. manages spoofing a specific data element within a SpoofedObject
class SpoofedElement
{
    public:

        SpoofedElement( SpoofedObject* pObject, const char* elementName );
        virtual ~SpoofedElement( );

        void setSpoofValue( const char* spoofValue );
        void setSpoofValue( float multiplier, float offset );
        void spoof( ); // spoofs element within the parent object's spoof buffer
        void dump( ) const; // dumps info on this object

        const SpoofableData& dataInfo() const { return *_pDataInfo; };
    
    protected:


    private:

        const SpoofedObject* _pParent;
        const SpoofableData* _pDataInfo; // info about this field
        void* _pSpoofConstant; // converted spoof constant, 0 if using mult/offset
        void* _pSpoofedElement; // location of element to spoof
        void* _pOriginalElement; // location of original unspoofed element
        float _spoofOffset;
        float _spoofMult;

        SpoofedElement(const SpoofedElement&); // catch unauthorized use 
        SpoofedElement& operator=(const SpoofedElement& rhs); // catch unauthorized use
};

#endif // SpoofedElement_HPP
