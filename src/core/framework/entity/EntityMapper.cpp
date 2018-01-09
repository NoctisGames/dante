//
//  EntityMapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityMapper.h"

#include "Box2D/Box2D.h"
#include "framework/entity/Entity.h"
#include "framework/entity/EntityController.h"

#include "framework/util/NGSTDUtil.h"
#include "framework/util/StringUtil.h"
#include <framework/file/portable/JsonFile.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>

EntityMapper* EntityMapper::getInstance()
{
    static EntityMapper instance = EntityMapper();
    return &instance;
}

void EntityMapper::sDeserializerFunc(const char* data)
{
    EntityMapper::getInstance()->initWithJson(data);
}

void EntityMapper::initWithJsonFile(const char* path, bool isBundled, bool useEncryption)
{
    const char* finalPath;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s("data/config/");
    s += std::string(path);
    finalPath = s.c_str();
#elif defined _WIN32
    std::string s("data\\config\\");
    s += std::string(path);
    finalPath = s.c_str();
#else
    finalPath = path;
#endif
    
    JsonFile jsonFile(finalPath, isBundled, useEncryption);
    jsonFile.setDeserializerFunc(sDeserializerFunc);
    jsonFile.load();
}

void EntityMapper::initWithJson(const char* json)
{
    _entityDescriptors.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(json);
    
    assert(d.HasMember("entities"));
    
    {
        Value& v = d["entities"];
        
        assert(v.IsObject());
        for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
        {
            const Value& iv = i->value;
            assert(iv.IsObject());
            
            std::string keyStr = i->name.GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            assert(!_entityDescriptors[key]);
            
            EntityDef* entry = new EntityDef();
            
            entry->type = key;
            entry->controller = iv["controller"].GetString();
            const Value& ivv = iv["mappings"];
            for (Value::ConstMemberIterator ii = ivv.MemberBegin(); ii != ivv.MemberEnd(); ++ii)
            {
                std::string name = ii->name.GetString();
                std::string value = ii->value.GetString();
                entry->mappings.insert(std::make_pair(name, value));
            }
            entry->width = static_cast<float>(iv["width"].GetInt());
            entry->height = static_cast<float>(iv["height"].GetInt());
            entry->restitution = iv["restitution"].GetFloat();
            entry->density = iv["density"].GetFloat();
            entry->friction = iv["friction"].GetFloat();
            entry->layer = iv["layer"].GetInt();
            entry->staticBody = iv["staticBody"].GetBool();
            entry->fixedRotation = iv["fixedRotation"].GetBool();
            entry->bullet = iv["bullet"].GetBool();
            entry->sensor = iv["sensor"].GetBool();
            entry->character = iv["character"].GetBool();
            entry->stateSensitive = iv["stateSensitive"].GetBool();
            
            _entityDescriptors[key] = entry;
        }
    }
}

Entity* EntityMapper::createEntity(uint32_t inFourCCName, b2World& world, bool isServer)
{
    auto q = _entityDescriptors.find(inFourCCName);
    
    assert(q != _entityDescriptors.end());
    
    EntityDef* entityDef = q->second;
    
    return new Entity(*entityDef, world, isServer);
}

void EntityMapper::registerFunction(std::string name, EntityControllerCreationFunc inCreationFunction)
{
    assert(inCreationFunction);
    
    _nameToEntityControllerCreationFunctionMap[name] = inCreationFunction;
}

EntityController* EntityMapper::createEntityController(std::string name, Entity* inEntity)
{
    EntityControllerCreationFunc creationFunc = _nameToEntityControllerCreationFunctionMap[name];
    
    assert(creationFunc);
    
    return creationFunc(inEntity);
}

std::map<uint32_t, EntityDef*>& EntityMapper::getEntityDescriptors()
{
    return _entityDescriptors;
}

EntityMapper::EntityMapper()
{
    // Empty
}

EntityMapper::~EntityMapper()
{
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptors);
}