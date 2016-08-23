#ifndef __PIRALGO_H__
#define __PIRALGO_H__

#include "sharedstruct.h"
#include "cobeconfig.h"

class procedureDetailPltPlsRBC;



/////////////////////////////////////////////////////////////////////////////////////////////

class IPirAlgo
{
public:
   virtual float       averagePIRQc (const IntermediateResults&, float& PIRVcAtMax, const float Qch);
   virtual float       getLockin () const {return _PIRLockin; }
   virtual bool        calcPirRequired () = 0;
   virtual void        setPirConv (int value) {_PIRConv = value; }
   virtual int         getPirConv () const { return _PIRConv; }
   virtual float       getCollectMaxFlow () { return _CollectMaximumFlow; }
   virtual float       calcLockinBracket (const float qch) = 0;
   virtual int         getConvCriteria (const float qch)   = 0;
   virtual int         getPirRequired () const {return _PIRRequired; }
   virtual void        setPirRequired (int _PirRequired) { _PIRRequired = _PirRequired; }
   virtual const float CalculatePIRQchdot (float Qc) const;
   virtual const float CalculatePIRRampTime (float Qcho, float endQc) const;
   virtual const float CalculatePIRQch (float timeInSubstate, float Qcho) const;
   virtual const float CalculatePIRRampVolume (float Qcho, float endQc) const;

   IPirAlgo()
      : _cc(CobeConfig::data()),
        _PIRLockin(0.0f),
        _PIRConv(0),
        _PIRRequired(0),
        _CollectMaximumFlow(0),
        _YtargLoss(0.0f),
        _YtargLossLogged(false)
   { }

   IPirAlgo(const CobeConfig::CobeConfigStruct& cc)
      : _cc(cc),
        _PIRLockin(0.0f),
        _PIRConv(0),
        _PIRRequired(0),
        _CollectMaximumFlow(0),
        _YtargLoss(0.0f),
        _YtargLossLogged(false)
   { }

   virtual ~IPirAlgo() {}


protected:
   const CobeConfig::CobeConfigStruct& _cc;
   float _PIRLockin;
   int   _PIRConv;
   int   _PIRRequired;
   float _CollectMaximumFlow;
   float _YtargLoss;
   bool  _YtargLossLogged;

};

/////////////////////////////////////////////////////////////////////////////////////////////

class PirAlgo
   : public IPirAlgo
{

public:
   PirAlgo(procedureDetailPltPlsRBC* pd);
   virtual ~PirAlgo(){}
   virtual bool  calcPirRequired ();
   virtual float calcLockinBracket (const float qch);
   virtual int   getConvCriteria (const float qch);


protected:
   procedureDetailPltPlsRBC* _pd;

private:

   float _FirstMaxPIRChamberFlow;  // 1st PIR chamber limit
   float _SecondMaxPIRChamberFlow;
   float _FirstPIRFlowLimit;


};

/////////////////////////////////////////////////////////////////////////////////////////////

class PirAlgoLR
   : public IPirAlgo
{


public:
   PirAlgoLR(procedureDetailPltPlsRBC* pd);
   virtual ~PirAlgoLR() { }
   virtual bool  calcPirRequired ();
   virtual float calcLockinBracket (const float qch);
   virtual int   getConvCriteria (const float qch);

protected:
   procedureDetailPltPlsRBC* _pd;

};

#endif /* __PIRALGO_H__ */

/* FORMAT HASH d81018f48a1a9bf8a2dc5f8eac4c8dc3 */
