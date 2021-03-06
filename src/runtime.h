#pragma once

#include "cpp-sdk/SDK.h"
#include "Log.h"
#include "angelscript/include/angelscript.h"
#include "helpers/docs.h"

class AngelScriptResource;
class AngelScriptRuntime : public alt::IScriptRuntime
{
    asIScriptEngine* engine;

    // Types
    asITypeInfo* arrayStringTypeInfo = nullptr;
    asITypeInfo* arrayIntTypeInfo = nullptr;
    asITypeInfo* arrayUintTypeInfo = nullptr;
    asITypeInfo* arrayAnyTypeInfo = nullptr;

public:
    AngelScriptRuntime();
    alt::IResource::Impl* CreateImpl(alt::IResource* resource) override;
    void DestroyImpl(alt::IResource::Impl* impl) override;

    asIScriptEngine* GetEngine()
    {
        return engine;
    }

    CScriptArray* CreateStringArray(uint32_t len);
    CScriptArray* CreateIntArray(uint32_t len);
    CScriptArray* CreateUIntArray(uint32_t);
    CScriptArray* CreateAnyArray(uint32_t);
    void RegisterTypeInfos();
    // Register the script interfaces (the scripting api)
    void RegisterScriptInterfaces(asIScriptEngine* engine, Helpers::DocsGenerator* docs);

    int GetStringTypeId()
    {
        static int stringType = engine->GetTypeIdByDecl("string");
        return stringType;
    }
    int GetBaseObjectTypeId()
    {
        static int baseObjectType = engine->GetTypeIdByDecl("BaseObject");
        return baseObjectType;
    }
    int GetWorldObjectTypeId()
    {
        static int worldObjectType = engine->GetTypeIdByDecl("WorldObject");
        return worldObjectType;
    }
    int GetEntityTypeId()
    {
        static int entityType = engine->GetTypeIdByDecl("Entity");
        return entityType;
    }
    int GetPlayerTypeId()
    {
        static int playerType = engine->GetTypeIdByDecl("Player");
        return playerType;
    }
    int GetVehicleTypeId()
    {
        static int vehicleType = engine->GetTypeIdByDecl("Vehicle");
        return vehicleType;
    }

    // Gets the current runtime instance or creates one if not exists
    static AngelScriptRuntime& Instance()
	{
		static AngelScriptRuntime _instance;
		return _instance;
	}
};