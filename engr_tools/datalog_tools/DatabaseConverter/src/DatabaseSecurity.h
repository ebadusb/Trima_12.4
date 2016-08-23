/*****************************************************************************\
**
** DatabaseSecurity.h
**
** Provides security to the converter library.
**
\*****************************************************************************/

#ifndef DATABASESECURITY_H
#define DATABASESECURITY_H

namespace CONVERT {

/* Classes *******************************************************************/

public interface class IDatabaseSecurity {
	bool IsActivated();
};

[System::Runtime::InteropServices::ComDefaultInterface(IDatabaseSecurity::typeid)]
public ref class DatabaseSecurity : public IDatabaseSecurity {
public:
	/**
	 *
	 * IsProductActivated
	 *
	 * Checks if the product is activated.
	 *
	 * @return TRUE if the product is activated. 
	 *
	 */
	static bool IsProductActivated(void)
	{
		DatabaseSecurity ^security = gcnew DatabaseSecurity();

		return security->IsActivated();
	}

	DatabaseSecurity()
	{
		System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(this, &DatabaseSecurity::ResolveHandler);
	}

	virtual bool IsActivated(void)
	{
		int level = 0; 
		int timeLeft = 0;
		System::String ^appName= ""; 

		ClientSideRegistration::ClientReg ^clientReg = gcnew ClientSideRegistration::ClientReg();
		BCTRegistration::ActivationKey::ActivationStatus actStat = clientReg->AreWeRegistered(
				level, 
				timeLeft, 
				appName, 
				"C:\\Program Files\\Gambro BCT\\DLDGui\\DLDGuiRegKey.txt", 
				239147, 
				System::Environment::MachineName);
						
		return actStat == BCTRegistration::ActivationKey::ActivationStatus::Successful;
	}

private:
	System::Reflection::Assembly ^ResolveHandler(System::Object ^sender, System::ResolveEventArgs ^args)
	{
		System::Reflection::Assembly ^assemblies = System::Reflection::Assembly::GetExecutingAssembly();
		System::String ^path = "C:\\Program Files\\Gambro BCT\\DLDGui\\";
		System::String ^name = args->Name->Substring(0, args->Name->IndexOf(",")) + ".dll";

		return System::Reflection::Assembly::LoadFrom(path + name);
	}
};

} // namespace CONVERT

#endif // DATABASESECURITY_H