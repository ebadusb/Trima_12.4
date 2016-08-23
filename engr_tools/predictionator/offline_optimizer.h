/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      Offline_optimizer.hpp
 *             Focussed system prediction functions - wrapper for
 *             offline optimization.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the optimization algorithm
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 *   To use this class, the user class must first construct me, then
 *   initialize me using the cfg_main, config, and initialize methods.
 *
 *   A typical example to predict a product over a donor database:
 *
 *       Offline_optimizer pred;
 *
 *       //
 *       //
 *       //   Populate configuration using local config files
 *       //
 *       Sectionfile cfg_main, config;
 *       cfg_main.initialize("cfg_main.dat");
 *       config.initialize("config.dat");
 *       pred.cfg_main(cfg_main);
 *       pred.config(config);
 *
 *       //
 *       //
 *       //   Set the product I'd like to optimize
 *       //
 *       float yield = 4.0;
 *       float platelet_volume = 250.0;
 *       float plasma_volume = 100.0;
 *       CONFIG_DATA_LISTBOX_STRINGS rbc_enum = RBC_YES;
 *       float rbc_volume = 100.0;
 *       float maximum_time = 150.0;
 *       pred.initialize(yield, platelet_volume, plasma_volume,
 *                       rbc_enum, rbc_volume, maximum_time);
 *
 *       //
 *       //
 *       //   Predict this product using the donor database
 *       //
 *       int totalgood = 0;
 *       for (int donor=0; donor<total_donors; donor++) {
 *          if (pred.good(donordb[donor].weight,
 *                        donordb[donor].TBV,
 *                        donordb[donor].hematocrit,
 *                        donordb[donor].precount,
 *                        donordb[donor].blood_type)) totalgood++;
 *       }
 *
 */
#ifndef OFFLINE_OPTIMIZER_HPP
#define OFFLINE_OPTIMIZER_HPP
#include "Sectionfile.hpp"
#include "cfg_main_input.h"
#include "initial_conditions.h"
#include "cfg_ids.h"
#include "predict_msgs.h"
#include "safproc.h"
#include "procdet.h"
#include "predict.h"

class Offline_optimizer
{

   //
   //
   //   The following methods are available for general use.
   //
   public:

      // Constructor
      Offline_optimizer();

      // Destructor
      virtual ~Offline_optimizer();


      //
      //
      //  Initialize me with my desired targets.  I must be initialized before
      //  I can optimize.  My targets come from the user configuration.
      //

      virtual void initialize();
      virtual void setInitialConditions(const Initial_conditions &conditions);

      //
      //  Set my donor parameters.
      //
      //   Units: height, weight are in cfg_main units
      //          TBV is in ml
      //          hematocrit is expressed as a fraction, i.e. 0.43
      //          precount is in /ul*1000, i.e., 234.0
      //          blood type is as defined in gui/cfg_keys.h
      //
      virtual void donor(const float height,
                         const float weight,
                         const float TBV,
                         const float hematocrit,
                         const float adjustedcrit,
                         const float precount,
                         const BLOODTYPE blood_type);

      virtual void calculateBMI(float height, float weight);
 
      //
      //
      //  Tell me to optimize all products
      //  
      virtual void optimize();
      virtual const INTERNAL_PREDICT_RESULTS optimize(const int productindex);

      //
      //
      //   Return the detail for a given product/procedure given the config index
      //
      procedureDetail *getProduct(int procindex);

	  //
	  //
	  //  You can ask me for a string which represents an optimization result.
	  //
	  static void description(const INTERNAL_PREDICT_RESULTS result,
                             CString &string);

  

//
//
//-----------------------------------------------------------------------------
//                      Member data
//
   public: // for external use
      Predict m_predict;

};

#endif
