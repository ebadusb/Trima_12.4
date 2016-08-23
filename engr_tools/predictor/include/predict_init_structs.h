#ifndef __PREDICT_INIT_STRUCTS_H__
#define __PREDICT_INIT_STRUCTS_H__
#pragma pack(push, _init_structs_previous_pack, 1)
#pragma pack(1)
namespace init
{
	struct Donor
	{
		int id;
		float height;
		float weight;
		float tbv;
		float precount;
		float hct;
		float bmi;
		int female; /* 1-female 0-male*/
		int blood_type;
		
	};
	
	struct Volumes
	{
		float vin;
		float vac;
		float vplasma_bag;
		float vplasma_bag_ac;
		float vcollect_bag;
		float vcollect_bag_ac;
		float vrbc_bag;
		float vrbc_bag_ac;
		float dose_rbc_bag;
		float vrep;
		float vsaline_bolus;
		float platelet_yield;
		float time_of_vincv; /* Total Collect volume */
		float vincv;
		float vin_epca_start;

		float rbc_s1;
		float rbc_s2;
		float rbc_prime;

		float rbc_s1_double;
		float rbc_s2_double;
		float rbc_double;

	};
	
	struct Run
	{
		int substate;
		float qch_min;
		float qch_cur;
		int npurges;
		float adjusted_hct;
		int current_recovery;
		float vin_clearline;
		time_t wbc_purge_start_time;
		int wbc_purge_type;
	};

	struct Time
	{
		float proctime;
	};
	
	struct Cassette 
	{
		int type;
		bool rbc_detector_calibrated; 
	}; 

	struct Adjustments
	{
		float qin;
		float qrp;
		float ir;
		float ratio;
	};

	struct InitialConditions
	{
		Donor donor;
		Volumes volumes;
		Run run;
		Time time;
		Cassette cassette;
		Adjustments adjust;
	};
};

#pragma pack(pop, _init_structs_previous_pack)
#endif /* __PREDICT_INIT_STRUCTS_H__ */
