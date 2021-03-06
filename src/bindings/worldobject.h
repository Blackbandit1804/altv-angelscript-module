#pragma once
#include "Log.h"
#include "../helpers/module.h"
#include "baseobject.h"
#include "vector3.h"

template<class T>
static Vector3<float> GetPosition(T* obj)
{
    alt::Vector3f pos = obj->GetPosition();
    return Vector3<float>(pos[0], pos[1], pos[2]);
}

template<class T>
static void SetPosition(Vector3<float> pos, T* obj)
{
    obj->SetPosition(alt::Point{pos.x, pos.y, pos.z});
}

template<class T>
static int GetDimension(T* obj)
{
    return obj->GetDimension();
}

template<class T>
static void SetDimension(int dimension, T* obj)
{
    obj->SetDimension(dimension);
}

using namespace Helpers;

namespace Helpers
{
    template<class T>
    static void RegisterAsWorldObject(asIScriptEngine* engine, DocsGenerator* docs, const char* type)
    {
        RegisterAsBaseObject<T>(engine, docs, type);

        REGISTER_PROPERTY_WRAPPER_GET(type, "Vector3f", "pos", GetPosition<T>);
        REGISTER_PROPERTY_WRAPPER_SET(type, "Vector3f", "pos", SetPosition<T>);

        REGISTER_PROPERTY_WRAPPER_GET(type, "int", "dimension", GetDimension<T>);
        REGISTER_PROPERTY_WRAPPER_SET(type, "int", "dimension", SetDimension<T>);
    }
}
