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
 *
 */
#ifndef OFFLINE_OPTIMIZER_HPP
#define OFFLINE_OPTIMIZER_HPP
#include "vxworks.h"
#include "cfg_ids.h"
#include "predict_msgs.h"
#include "safproc.h"
#include "procdet.h"
#include "predict.h"
#include "predict_init_structs.h"
#include "config_cds.h"
#include "alarms.h"

#include "toplevel.h"
#include "sharedstruct.h"

#include "predict_reader.h"
#include "log_format.h"

//Store data from procdet classes
struct ProductDetails
{
	struct IntermediateResults substate[ALLOCATED_SUBSTATES];
	struct TopLevelValues toplevel;

	float _PredVc;               // Predicted collect volume
	float _PredVp;               // Predicted plasma volume
	float _PredRBCVolume;        // total predicted rbc volume
	float _PredVac;
	float _PredVrep;
	float _PredTp;               // Predicted time in procedure (total)

	char status[30];
};


class Offline_optimizer
{
private:
	const float calculateBMI(float height, float weight);
	const float calculateTBV(float height, float weight, int female);
	const float calculateHeight(float tbv, float weight, int female);

public:
    Offline_optimizer();
    virtual ~Offline_optimizer();

     //  Initialize me with my desired targets.  I must be initialized before
     //  I can optimize.  My targets come from the user configuration.
     int initialize(const char* dlogPath, const char* cobeConfigPath);
	 ALARM_VALUES initialize(const char* userConfigPath, const char* cobeConfigPath, 
							  const char* dlogPath);

	 ALARM_VALUES initialize(const char* userConfigPath, const char* cobeConfigPath, 
							 init::InitialConditions ic);
	void update(init::InitialConditions& ic);

	void setInitialConditions(init::InitialConditions& ic);
      
    //  optimize all products
    void optimize();
    INTERNAL_PREDICT_RESULTS optimize(const int productindex);
	int getNumProcedures();

    //   Return the detail for a given product/procedure given the config index
    procedureDetail *getProduct(int procindex);

	//  You can ask me for a string which represents an optimization result.
	const char* description(const INTERNAL_PREDICT_RESULTS result); 

public: 
    Predict _predict;
	Config_CDS* _userconfig;
	init::InitialConditions _ic;
	DLOG_PredictReader _predictReader;
};

#endif
