#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <GarrysMod/Symbol.hpp>

#include <scanning/symbolfinder.hpp>
#include <detouring/hook.hpp>

#ifdef SYSTEM_LINUX
	#ifdef ARCHITECTURE_X86
		Symbol symbol = Symbol::FromName("_ZN11CBaseEntity28IsSimulatingOnAlternateTicksEv");
	#elif ARCHITECTURE_X86_64
		Symbol symbol = Symbol::FromName("_ZNK11CBaseEntity28IsSimulatingOnAlternateTicksEv");
	#endif
	#elif SYSTEM_WINDOWS
	#ifdef ARCHITECTURE_X86
		Symbol symbol = Symbol::FromName("?IsSimulatingOnAlternateTicks@CBaseEntity@@QBE_NXZ");
	#elif ARCHITECTURE_X86_64
		Symbol symbol = Symbol::FromName("?IsSimulatingOnAlternateTicks@CBaseEntity@@QEBA_NXZ");
	#endif
#endif

Detouring::Hook AlternateTicks_detour;

bool IsSimulatingOnAlternateTicks_detour() {
	return true;
}

GMOD_MODULE_OPEN() {
	SourceSDK::ModuleLoader server_srv("server_srv");
	SymbolFinder symfinder;

	void* is_simulating_on_alternate_ticks = nullptr;
	is_simulating_on_alternate_ticks = symfinder.Resolve(server_srv.GetModule(), symbol.name.c_str(), symbol.length);

	if (is_simulating_on_alternate_ticks == nullptr) {
		LUA->ThrowError("Failed to find IsSimulatingOnAlternateTicks");
		return 0;
	}

	AlternateTicks_detour.Create(reinterpret_cast<void*>(is_simulating_on_alternate_ticks), reinterpret_cast<void*>(&IsSimulatingOnAlternateTicks_detour));

	return 1;
}

GMOD_MODULE_CLOSE() {
	AlternateTicks_detour.Destroy();
	return 0;
}
