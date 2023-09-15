#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <GarrysMod/Symbol.hpp>

#include <scanning/symbolfinder.hpp>
#include <detouring/hook.hpp>

#ifdef SYSTEM_LINUX
	#ifdef ARCHITECTURE_X86
		Symbol symbol = Symbol::FromName("_ZN11CBaseEntity28IsSimulatingOnAlternateTicksEv");
	#elif ARCHITECTURE_X86_64
		Symbol symbol = Symbol::FromName("_ZNK11CBaseEntity28IsSimulatingOnAlternateTicksEv"); // Invalid sigs, need to be updated
	#endif
	#elif SYSTEM_WINDOWS
	#ifdef ARCHITECTURE_X86
		Symbol symbol = Symbol::FromName("?IsSimulatingOnAlternateTicks@CBaseEntity@@QBE_NXZ"); // Invalid sigs, need to be updated
	#elif ARCHITECTURE_X86_64
		Symbol symbol = Symbol::FromName("?IsSimulatingOnAlternateTicks@CBaseEntity@@QEBA_NXZ"); // Invalid sigs, need to be updated
	#endif
#endif

bool enableHook = false;
typedef bool (*ShouldHideServer)();
Detouring::Hook AlternateTicks_detour;

bool IsSimulatingOnAlternateTicks_detour() {
	if (enableHook) {
		return true;
	}
	return AlternateTicks_detour.GetTrampoline<ShouldHideServer>()();
}

LUA_FUNCTION(enableAlternativeTicks) {
	LUA->CheckType(1, GarrysMod::Lua::Type::BOOL);
	enableHook = LUA->GetBool(1);
	return 0;
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

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "engine");
			LUA->PushCFunction(enableAlternativeTicks);
			LUA->SetField(-2, "EnableAlternativeTicks");
		LUA->Pop();
	LUA->Pop();

	return 1;
}

GMOD_MODULE_CLOSE() {
	AlternateTicks_detour.Destroy();

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "engine");
			LUA->PushNil();
			LUA->SetField(-2, "EnableAlternativeTicks");
		LUA->Pop();
	LUA->Pop();

	return 0;
}
