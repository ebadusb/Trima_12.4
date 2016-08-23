#pragma once

#include "TrimaPredict.h"

namespace TrimaPredict60 {

public ref class Predict {
public:
#pragma region Defined Functions


	static int LoadOptimizerFromLog(
			System::String ^dataLogPath,
			System::String ^cobeConfigPath)
	{
		System::IntPtr logFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dataLogPath);
		System::IntPtr datfileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(cobeConfigPath);

		int results = PredictInstance::GetInstance().LoadOptimizerFromLog(
				(char*)logFileP.ToPointer(),
				(char*)datfileP.ToPointer());

		System::Runtime::InteropServices::Marshal::FreeHGlobal(logFileP);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(datfileP);

		return results;
	}

	static int LoadOptimizerFromConfig(
			System::String ^userConfigPath,
			System::String ^cobeConfigPath)
	{
		System::IntPtr userConfigFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userConfigPath);
		System::IntPtr cobeConfigFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(cobeConfigPath);

		int results = PredictInstance::GetInstance().LoadOptimizerFromConfig(
				(char*)userConfigFileP.ToPointer(),
				(char*)cobeConfigFileP.ToPointer());

		System::Runtime::InteropServices::Marshal::FreeHGlobal(userConfigFileP);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(cobeConfigFileP);

		return results;
	}

	static int LoadOptimizerFromConfigs(
			System::String ^userConfigPath,
			System::String ^cobeConfigPath,
			System::String ^softwareConfigPath)
	{
		System::IntPtr userConfigFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userConfigPath);
		System::IntPtr cobeConfigFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(cobeConfigPath);
		System::IntPtr softConfigFileP = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(softwareConfigPath);

		int results = PredictInstance::GetInstance().LoadOptimizerFromConfig(
				(char*)userConfigFileP.ToPointer(), 
				(char*)cobeConfigFileP.ToPointer(),
				(char*)softConfigFileP.ToPointer());

		System::Runtime::InteropServices::Marshal::FreeHGlobal(userConfigFileP);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(cobeConfigFileP);

		return results;
	}

	static int UpdateConditions(void)
	{
		return PredictInstance::GetInstance().UpdateConditions();
	}

	static int UpdateConfigurations(void)
	{
		return PredictInstance::GetInstance().UpdateConfigurations();
	}

	static int OptimizeProduct(int product)
	{
		return PredictInstance::GetInstance().OptimizeProduct(product);
	}
	// The method allows you to speed up the  predict engine performance
	static int Optimize()
	{
		return PredictInstance::GetInstance().Optimize();
	}

    // T-POD 525- Duplicate procedures not getting filtered  similar to Trima.
    // Exposing this  method allows the caller to set the status of procs to Unoptimize
    // This should be done only once by the caller before calling optimizeProduct on individual procedure.
	static void SetUnoptimized()
	{
		return PredictInstance::GetInstance().SetUnoptimized();
	}
	
	static float calculateSafetyTBV(float height, float weight, int female)
	{
		float displaySafetyTBV = PredictInstance::GetInstance().calculateSafetyTBV(height,weight,female);
		return(displaySafetyTBV);
	}
#pragma endregion

#pragma region Generated Functions
#pragma endregion
};
}
