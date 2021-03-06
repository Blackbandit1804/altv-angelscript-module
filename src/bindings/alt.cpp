#include "Log.h"
#include "../helpers/module.h"
#include "../runtime.h"
#include "angelscript/addon/scriptdictionary/scriptdictionary.h"
#include "angelscript/addon/scriptany/scriptany.h"
#include "../helpers/convert.h"

using namespace Helpers;

static void Log(const std::string& msg)
{
    Log::Colored << msg << Log::Endl;
}

static void LogWarning(const std::string& msg)
{
    Log::Warning << msg << Log::Endl;
}

static void LogError(const std::string& msg)
{
    Log::Error << msg << Log::Endl;
}

static uint32_t SetTimeout(asIScriptFunction* callback, uint32_t timeout)
{
    GET_RESOURCE();
    return resource->CreateTimer(timeout, callback, true);
}

static uint32_t SetInterval(asIScriptFunction* callback, uint32_t interval)
{
    GET_RESOURCE();
    return resource->CreateTimer(interval, callback, false);
}

static uint32_t NextTick(asIScriptFunction* callback)
{
    GET_RESOURCE();
    return resource->CreateTimer(0, callback, true);
}

static uint32_t EveryTick(asIScriptFunction* callback)
{
    GET_RESOURCE();
    return resource->CreateTimer(0, callback, false);
}

static void ClearTimer(uint32_t id)
{
    GET_RESOURCE();
    resource->RemoveTimer(id);
}

static uint32_t Hash(std::string& value)
{
    return alt::ICore::Instance().Hash(value);
}

static CScriptArray* GetAllPlayers()
{
    GET_RESOURCE();
    static asITypeInfo* playerArrayTypeInfo = nullptr;
    if(playerArrayTypeInfo == nullptr) {
        playerArrayTypeInfo = resource->GetRuntime()->GetEngine()->GetTypeInfoByDecl("array<alt::Player@>");
        playerArrayTypeInfo->AddRef();
    }
    auto players = alt::ICore::Instance().GetPlayers();
    auto arr = CScriptArray::Create(playerArrayTypeInfo, players.GetSize());
    for(int i = 0; i < players.GetSize(); i++)
    {
        void* player = players[i].Get();
        arr->SetValue(i, &player);
    }
    return arr;
}

static CScriptArray* GetAllEntities()
{
    GET_RESOURCE();
    static asITypeInfo* entityArrayTypeInfo = nullptr;
    if(entityArrayTypeInfo == nullptr) {
        entityArrayTypeInfo = resource->GetRuntime()->GetEngine()->GetTypeInfoByDecl("array<alt::Entity@>");
        entityArrayTypeInfo->AddRef();
    }
    auto entities = alt::ICore::Instance().GetEntities();
    auto arr = CScriptArray::Create(entityArrayTypeInfo, entities.GetSize());
    for(int i = 0; i < entities.GetSize(); i++)
    {
        void* entity = entities[i].Get();
        arr->SetValue(i, &entity);
    }
    return arr;
}

static std::string GetResourceName()
{
    GET_RESOURCE();
    return resource->GetResource()->GetName().ToString();
}

static std::string GetResourceMain()
{
    GET_RESOURCE();
    return resource->GetResource()->GetMain().ToString();
}

static std::string GetResourcePath(const std::string& name)
{
    auto resource = alt::ICore::Instance().GetResource(name);
    if(resource == nullptr)
    {
        THROW_ERROR("Resource not found");
        return nullptr;
    }
    return resource->GetPath().ToString();
}

static bool HasResource(const std::string& name)
{
    auto resource = alt::ICore::Instance().GetResource(name);
    return resource != nullptr && resource->IsStarted();
}

// todo: add get resource exports
/*
static CScriptArray* GetResourceExports(const std::string& name)
{
    GET_RESOURCE();
    auto res = alt::ICore::Instance().GetResource(name);
    if(res == nullptr)
    {
        THROW_ERROR("Resource not found");
        return nullptr;
    }
    auto exports = res->GetExports();
    auto dict = CScriptDictionary::Create(resource->GetRuntime()->GetEngine());
    for (auto it = exports->Begin(); it; it = exports->Next())
    {
        
    }
}
*/

static void StartResource(const std::string& name)
{
    alt::ICore::Instance().StartResource(name);
}

static void StopResource(const std::string& name)
{
    alt::ICore::Instance().StopResource(name);
}

static void RestartResource(const std::string& name)
{
    alt::ICore::Instance().RestartResource(name);
}

static uint32_t GetNetTime()
{
    return alt::ICore::Instance().GetNetTime();
}

static void SetPassword(const std::string& password)
{
    alt::ICore::Instance().SetPassword(password);
}

static std::string GetRootDir()
{
    return alt::ICore::Instance().GetRootDirectory().ToString();
}

static int GetDefaultDimension()
{
    return alt::DEFAULT_DIMENSION;
}

static int GetGlobalDimension()
{
    return alt::GLOBAL_DIMENSION;
}

static std::string GetVersion()
{
    return alt::ICore::Instance().GetVersion().ToString();
}

static std::string GetBranch()
{
    return alt::ICore::Instance().GetBranch().ToString();
}

static uint32_t GetSDKVersion()
{
    return alt::ICore::SDK_VERSION;
}

static std::string ReadFile(const std::string& path)
{
    GET_RESOURCE();
    auto file = resource->ReadFile(path);
    if(file.IsEmpty())
    {
        THROW_ERROR("File not found");
        return nullptr;
    }
    return file.ToString();
}

static bool FileExists(const std::string& path)
{
    GET_RESOURCE();
    auto file = resource->ReadFile(path);
    return !file.IsEmpty();
}

static void On(const std::string& name, asIScriptFunction* handler)
{
    GET_RESOURCE();
    resource->RegisterCustomEventHandler(name, handler, true);
}

static void OnClient(const std::string& name, asIScriptFunction* handler)
{
    GET_RESOURCE();
    resource->RegisterCustomEventHandler(name, handler, false);
}

static void Emit(asIScriptGeneric* gen)
{
    GET_RESOURCE();
    void* ref = gen->GetArgAddress(0);
	int typeId = 0;
	std::string event = *static_cast<std::string*>(ref);
    alt::MValueArgs args;

    for(int i = 1; i < gen->GetArgCount(); i++)
    {
        ref = gen->GetArgAddress(i);
        typeId = gen->GetArgTypeId(i);
        if(typeId == resource->GetRuntime()->GetStringTypeId() && *static_cast<std::string*>(ref) == VARIADIC_ARG_INVALID) continue;
        auto mvalue = Helpers::ValueToMValue(typeId, ref);
        args.Push(mvalue);
    }
    alt::ICore::Instance().TriggerLocalEvent(event, args);
}

static ModuleExtension altExtension("alt", [](asIScriptEngine* engine, DocsGenerator* docs)
{
    // Generic
    REGISTER_GLOBAL_FUNC("uint Hash(const string &in value)", Hash, "Hashes the given string using the joaat algorithm");
    REGISTER_GLOBAL_FUNC("array<Player@>@ GetAllPlayers()", GetAllPlayers, "Gets all players on the server");
    REGISTER_GLOBAL_FUNC("array<Entity@>@ GetAllEntities()", GetAllEntities, "Gets all entities on the server");
    REGISTER_GLOBAL_PROPERTY("int", "defaultDimension", GetDefaultDimension);
    REGISTER_GLOBAL_PROPERTY("int", "globalDimension", GetGlobalDimension);
    REGISTER_GLOBAL_PROPERTY("string", "version", GetVersion);
    REGISTER_GLOBAL_PROPERTY("string", "branch", GetBranch);
    REGISTER_GLOBAL_PROPERTY("uint", "sdkVersion", GetSDKVersion);

    // Filesystem
    REGISTER_GLOBAL_FUNC("string ReadFile(const string&in path)", ReadFile, "Reads the specified file contents");
    REGISTER_GLOBAL_FUNC("bool FileExists(const string&in path)", FileExists, "Checks if the given file exists");
    REGISTER_GLOBAL_PROPERTY("string", "rootDir", GetRootDir);

    // Resource
    REGISTER_GLOBAL_FUNC("string GetResourcePath(const string&in name)", GetResourcePath, "Gets the path to the specified resource");
    REGISTER_GLOBAL_FUNC("bool HasResource(const string&in name)", HasResource, "Returns whether the specified resource exists and is started");
    REGISTER_GLOBAL_FUNC("void StartResource(const string&in name)", StartResource, "Starts the specified resource");
    REGISTER_GLOBAL_FUNC("void StopResource(const string&in name)", StopResource, "Stops the specified resource");
    REGISTER_GLOBAL_FUNC("void RestartResource(const string&in name)", RestartResource, "Restarts the specified resource");
    REGISTER_GLOBAL_PROPERTY("string", "resourceMain", GetResourceMain);
    REGISTER_GLOBAL_PROPERTY("string", "resourceName", GetResourceName);

    // Server
    REGISTER_GLOBAL_FUNC("uint GetNetTime()", GetNetTime, "Gets the total time the server has been running for");
    REGISTER_GLOBAL_FUNC("void SetPassword(const string&in password)", SetPassword, "Sets the current server password");

    // Logging
    REGISTER_GLOBAL_FUNC("void Log(const string&in msg)", Log, "Logs the specified message to the console");
    REGISTER_GLOBAL_FUNC("void LogWarning(const string&in msg)", LogWarning, "Logs the specified message as a warning to the console");
    REGISTER_GLOBAL_FUNC("void LogError(const string&in msg)", LogError, "Logs the specified message as an error to the console");

    // Timers
    REGISTER_FUNCDEF("void TimerCallback()", "Callback used for timers");
    REGISTER_GLOBAL_FUNC("uint SetTimeout(TimerCallback@ callback, uint timeout)", SetTimeout, "Sets a timeout");
    REGISTER_GLOBAL_FUNC("uint SetInterval(TimerCallback@ callback, uint interval)", SetInterval, "Sets a interval");
    REGISTER_GLOBAL_FUNC("uint NextTick(TimerCallback@ callback)", NextTick, "Sets a next tick handler");
    REGISTER_GLOBAL_FUNC("uint EveryTick(TimerCallback@ callback)", EveryTick, "Sets a every tick handler");
    REGISTER_GLOBAL_FUNC("void ClearTimeout(uint timerId)", ClearTimer, "Clears specified timer");
    REGISTER_GLOBAL_FUNC("void ClearInterval(uint timerId)", ClearTimer, "Clears specified timer");
    REGISTER_GLOBAL_FUNC("void ClearNextTick(uint timerId)", ClearTimer, "Clears specified timer");
    REGISTER_GLOBAL_FUNC("void ClearEveryTick(uint timerId)", ClearTimer, "Clears specified timer");
    REGISTER_GLOBAL_FUNC("void ClearTimer(uint timerId)", ClearTimer, "Clears specified timer");

    // Events
    REGISTER_FUNCDEF("void LocalEventCallback(array<any> args)", "Event callback used for custom events");
    REGISTER_FUNCDEF("void RemoteEventCallback(Player@ player, array<any>@ args)", "Event callback used for custom events");
    REGISTER_GLOBAL_FUNC("void On(const string&in event, LocalEventCallback@ callback)", On, "Registers an event handler for a local custom event");
    REGISTER_GLOBAL_FUNC("void OnClient(const string&in event, RemoteEventCallback@ callback)", OnClient, "Registers an event handler for a remote custom event");
    REGISTER_VARIADIC_FUNC("void", "Emit", "string&in event", 32, Emit, "Emits a local event (Max 32 args)");
});
