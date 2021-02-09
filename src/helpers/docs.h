#pragma once

#include "cpp-sdk/SDK.h"
#include "Log.h"
#include "angelscript/include/angelscript.h"
#include "../resource.h"
#ifdef AS_GENERATE_DOCUMENTATION
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#endif

#define PAD_SPACE "    "

namespace Helpers
{
    class DocsGenerator
    {
        std::string module;
        std::vector<std::pair<std::string, std::string>> declarations;
        std::vector<std::pair<std::string, std::string>> variables;
        std::vector<std::pair<std::string, std::string>> funcDefs;
        std::vector<std::pair<std::string, std::string>> eventDeclarations;
        std::vector<std::pair<std::string, std::string>> objectTypes;
        std::vector<std::pair<std::string, std::string>> enumTypes;
        std::unordered_multimap<std::string, std::pair<std::string, int>> enumValues;
        std::unordered_multimap<std::string, std::string> objectDeclarations;
        std::unordered_multimap<std::string, std::string> objectConstructors;
        std::unordered_multimap<std::string, std::string> objectMethods;
    public:
        DocsGenerator(std::string module) : module(module) {};

        void PushDeclaration(std::string decl, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            declarations.push_back(std::pair(decl, desc));
            #endif
        }
        void PushVariable(std::string type, std::string prop)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            variables.push_back({type, prop});
            #endif
        }
        void PushFuncDef(std::string funcdef, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            funcDefs.push_back(std::pair(funcdef, desc));
            #endif
        }
        void PushEventDeclaration(std::string funcDef, std::string globalFunc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            eventDeclarations.push_back(std::pair(funcDef.insert(0, "funcdef "), globalFunc));
            #endif
        }
        void PushObjectType(std::string name, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            objectTypes.push_back(std::pair(name, desc));
            #endif
        }
        void PushObjectProperty(std::string object, std::string propertyDecl)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            objectDeclarations.insert({object, propertyDecl});
            #endif
        }
        void PushObjectConstructor(std::string object, std::string constructorDecl)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            objectConstructors.insert({object, constructorDecl});
            #endif
        }
        void PushObjectMethod(std::string object, std::string methodDecl)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            objectMethods.insert({object, methodDecl});
            #endif
        }
        void PushEnumValue(std::string enumName, std::string name, int value)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            enumValues.insert({enumName, {name, value}});
            #endif
        }
        void PushEnumType(std::string name, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            enumTypes.push_back({name, desc});
            #endif
        }

        void Generate()
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            std::stringstream stream;

            // Add generation date to top of file
            std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char* timeStr = std::ctime(&currentTime);
            stream << "// Generated on " << timeStr << "\n";

            // Add namespace
            stream << "namespace alt" << "\n";
            stream << "{" << "\n";

            // Add func defs
            stream << PAD_SPACE << "// ********** Funcdefs **********" << "\n";
            for(auto def : funcDefs)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << def.second << "\n";
                stream << PAD_SPACE << "funcdef " << def.first << ";" << "\n";
            }

            stream << "\n";

            // Add enums
            stream << PAD_SPACE << "// ********** Enums **********" << "\n";
            for(auto enumType : enumTypes)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << enumType.second << "\n";
                stream << PAD_SPACE << "enum " << enumType.first << "\n";
                stream << PAD_SPACE << "{" << "\n";
                for(auto value : enumValues)
                {
                    if(value.first != enumType.first) continue;
                    stream << PAD_SPACE << PAD_SPACE << value.second.first << " = " << std::to_string(value.second.second) << ",\n";
                }
                stream << PAD_SPACE << "};" << "\n";
            }

            stream << "\n";

            // Add variables
            stream << PAD_SPACE << "// ********** Global variables **********" << "\n";
            for(auto variable : variables)
            {
                stream << "\n";
                stream << PAD_SPACE << variable.first << " " << variable.second << ";" << "\n";
            }

            stream << "\n";

            // Add function declarations
            stream << PAD_SPACE << "// ********** Functions **********" << "\n";
            for(auto decl : declarations)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << decl.second << "\n";
                stream << PAD_SPACE << decl.first << ";" << "\n";
            }

            stream << "\n";

            // Add event declarations
            stream << PAD_SPACE << "// ********** Events **********" << "\n";
            for(auto decl : eventDeclarations)
            {
                stream << "\n";
                stream << PAD_SPACE << decl.first << ";" << "\n";
                stream << PAD_SPACE << decl.second << ";" << "\n";
            }

            stream << "\n";

            // Add object types
            stream << PAD_SPACE << "// ********** Objects **********\n";
            for(auto obj : objectTypes)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << obj.second << "\n";
                stream << PAD_SPACE << "class " << obj.first << "\n";
                stream << PAD_SPACE << "{\n";
                for(auto kv : objectDeclarations)
                {
                    if(kv.first != obj.first) continue;
                    stream << PAD_SPACE << PAD_SPACE << kv.second << ";\n";
                }
                stream << "\n";
                for(auto kv : objectConstructors)
                {
                    if(kv.first != obj.first) continue;
                    stream << PAD_SPACE << PAD_SPACE << obj.first << "(" << kv.second << ");\n"; 
                }
                for(auto kv : objectMethods)
                {
                    if(kv.first != obj.first) continue;
                    stream << PAD_SPACE << PAD_SPACE << kv.second << ";\n";
                }
                stream << PAD_SPACE << "};\n";
            }

            // Close namespace
            stream << "}" << "\n";

            // Write the docs to file
            std::ofstream file;
            file.open(module.append("Docs.as"));
            file << stream.str();
            file.close();
            #endif
        }
    };
}
