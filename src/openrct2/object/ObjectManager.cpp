/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectManager.h"

#include "../Context.h"
#include "../ParkImporter.h"
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../localisation/StringIds.h"
#include "../rct2/RCT2.h"
#include "FootpathItemObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectList.h"
#include "ObjectRepository.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "WallObject.h"

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

class ObjectManager final : public IObjectManager
{
private:
    IObjectRepository& _objectRepository;
    std::vector<std::vector<Object*>> _loadedObjects;

public:
    explicit ObjectManager(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    ~ObjectManager() override
    {
        UnloadAll();
    }

    Object* GetLoadedObject(int32_t objectType, size_t index) override
    {
        if (_loadedObjects.size() > objectType)
        {
            auto& objectList = _loadedObjects[objectType];
            if (objectList.size() > index)
            {
                return objectList[index];
            }
        }
        return nullptr;
    }

    Object* GetLoadedObject(const rct_object_entry* entry) override
    {
        Object* loadedObject = nullptr;
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
        if (ori != nullptr)
        {
            loadedObject = ori->LoadedObject;
        }
        return loadedObject;
    }

    uint8_t GetLoadedObjectEntryIndex(const Object* object) override
    {
        uint8_t result = std::numeric_limits<uint8_t>::max();
        auto objectType = object->GetObjectType();
        if (objectType < _loadedObjects.size())
        {
            auto& objectList = _loadedObjects[objectType];
            for (size_t index = 0; index < objectList.size(); index++)
            {
                if (objectList[index] == object)
                {
                    result = (uint8_t)index;
                    break;
                }
            }
        }
        return result;
    }

    Object* LoadObject(const rct_object_entry* entry) override
    {
        Object* loadedObject = nullptr;
        const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
        if (ori != nullptr)
        {
            loadedObject = ori->LoadedObject;
            if (loadedObject == nullptr)
            {
                uint8_t objectType = object_entry_get_type(&ori->ObjectEntry);
                int32_t slot = FindSpareSlot(objectType);
                if (slot != -1)
                {
                    loadedObject = GetOrLoadObject(ori);
                    if (loadedObject != nullptr)
                    {
                        auto& objectList = _loadedObjects[objectType];
                        objectList[slot] = loadedObject;
                        UpdateSceneryGroupIndexes();
                        ResetTypeToRideEntryIndexMap();
                    }
                }
            }
        }
        return loadedObject;
    }

    void LoadObjects(const rct_object_entry* entries, size_t count) override
    {
        // Find all the required objects
        auto requiredObjects = GetRequiredObjects(entries, count);

        // Load the required objects
        size_t numNewLoadedObjects = 0;
        auto loadedObjects = LoadObjects(requiredObjects, &numNewLoadedObjects);

        SetNewLoadedObjectList(loadedObjects);
        LoadDefaultObjects();
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
        log_verbose("%u / %u new objects loaded", numNewLoadedObjects, requiredObjects.size());
    }

    void UnloadObjects(const rct_object_entry* entries, size_t count) override
    {
        // TODO there are two performance issues here:
        //        - FindObject for every entry which is a dictionary lookup
        //        - GetLoadedObjectIndex for every entry which enumerates _loadedList

        size_t numObjectsUnloaded = 0;
        for (size_t i = 0; i < count; i++)
        {
            const rct_object_entry* entry = &entries[i];
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
            if (ori != nullptr)
            {
                Object* loadedObject = ori->LoadedObject;
                if (loadedObject != nullptr)
                {
                    UnloadObject(loadedObject);
                    numObjectsUnloaded++;
                }
            }
        }

        if (numObjectsUnloaded > 0)
        {
            UpdateSceneryGroupIndexes();
            ResetTypeToRideEntryIndexMap();
        }
    }

    void UnloadAll() override
    {
        for (auto& objectList : _loadedObjects)
        {
            for (auto object : objectList)
            {
                UnloadObject(object);
            }
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    void ResetObjects() override
    {
        for (auto& objectList : _loadedObjects)
        {
            for (auto loadedObject : objectList)
            {
                if (loadedObject != nullptr)
                {
                    loadedObject->Unload();
                    loadedObject->Load();
                }
            }
        }
        UpdateSceneryGroupIndexes();
        ResetTypeToRideEntryIndexMap();
    }

    std::vector<const ObjectRepositoryItem*> GetPackableObjects() override
    {
        std::vector<const ObjectRepositoryItem*> objects;
        size_t numObjects = _objectRepository.GetNumObjects();
        for (size_t i = 0; i < numObjects; i++)
        {
            const ObjectRepositoryItem* item = &_objectRepository.GetObjects()[i];
            if (item->LoadedObject != nullptr && IsObjectCustom(item) && item->LoadedObject->GetLegacyData() != nullptr)
            {
                objects.push_back(item);
            }
        }
        return objects;
    }

    void LoadDefaultObjects() override
    {
        // We currently will load new object types here that apply to all
        // loaded RCT1 and RCT2 save files.

        // Surfaces
        LoadObject("#RCT2SGR");
        LoadObject("#RCT2SSY");
        LoadObject("#RCT2SDI");
        LoadObject("#RCT2SRO");
        LoadObject("#RCT2SMA");
        LoadObject("#RCT2SCH");
        LoadObject("#RCT2SGC");
        LoadObject("#RCT2SIC");
        LoadObject("#RCT2SIR");
        LoadObject("#RCT2SIY");
        LoadObject("#RCT2SIP");
        LoadObject("#RCT2SIG");
        LoadObject("#RCT2SSR");
        LoadObject("#RCT2SSA");

        // Edges
        LoadObject("#RCT2ERO");
        LoadObject("#RCT2EWR");
        LoadObject("#RCT2EWB");
        LoadObject("#RCT2EIC");
        LoadObject("#RCT1EBR");
        LoadObject("#RCT1EIR");
        LoadObject("#RCT1EGY");
        LoadObject("#RCT1EYE");
        LoadObject("#RCT1ERE");
        LoadObject("#RCT1EPU");
        LoadObject("#RCT1EGR");
        LoadObject("#RCT1ESN");
        LoadObject("#RCT1ESG");
        LoadObject("#RCT1ESA");
        LoadObject("#RCT1ESB");

        // Stations
        LoadObject("#RCT2STN");
        LoadObject("#RCT2STW");
        LoadObject("#RCT2STV");
        LoadObject("#RCT2ST3");
        LoadObject("#RCT2ST4");
        LoadObject("#RCT2STJ");
        LoadObject("#RCT2STL");
        LoadObject("#RCT2STC");
        LoadObject("#RCT2STA");
        LoadObject("#RCT2STS");
        LoadObject("#RCT2STP");
        LoadObject("#RCT2STE");
        LoadObject("#ORCT2SN");
    }

    static rct_string_id GetObjectSourceGameString(const uint8_t sourceGame)
    {
        switch (sourceGame)
        {
            case OBJECT_SOURCE_RCT1:
                return STR_SCENARIO_CATEGORY_RCT1;
            case OBJECT_SOURCE_ADDED_ATTRACTIONS:
                return STR_SCENARIO_CATEGORY_RCT1_AA;
            case OBJECT_SOURCE_LOOPY_LANDSCAPES:
                return STR_SCENARIO_CATEGORY_RCT1_LL;
            case OBJECT_SOURCE_RCT2:
                return STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
            case OBJECT_SOURCE_WACKY_WORLDS:
                return STR_OBJECT_FILTER_WW;
            case OBJECT_SOURCE_TIME_TWISTER:
                return STR_OBJECT_FILTER_TT;
            case OBJECT_SOURCE_OPENRCT2_OFFICIAL:
                return STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
            default:
                return STR_OBJECT_FILTER_CUSTOM;
        }
    }

private:
    Object* LoadObject(const std::string& name)
    {
        rct_object_entry entry{};
        std::copy_n(name.c_str(), 8, entry.name);
        return LoadObject(&entry);
    }

    int32_t FindSpareSlot(uint8_t objectType)
    {
        // Ensure we have an object list for this type of object
        if (_loadedObjects.size() <= objectType)
        {
            _loadedObjects.resize((size_t)objectType + 1);
        }

        // Find spare slot in list for object type
        auto& objectList = _loadedObjects[objectType];
        for (size_t i = 0; i < objectList.size(); i++)
        {
            if (objectList[i] == nullptr)
            {
                return (int32_t)i;
            }
        }

        // No slot available, expand list
        auto result = objectList.size();
        objectList.resize(std::max<size_t>(8, objectList.size() * 2));
        return (int32_t)result;
    }

    void SetNewLoadedObjectList(const std::vector<Object*>& newLoadedObjects)
    {
        if (newLoadedObjects.empty())
        {
            UnloadAll();
        }
        else
        {
            UnloadObjectsExcept(newLoadedObjects);
        }

        for (size_t index = 0; index < newLoadedObjects.size(); index++)
        {
            auto objectTypeIndex = GetRCT2ObjectTypeIndex(index);
            auto objectType = objectTypeIndex.Type;
            auto entryIndex = objectTypeIndex.Index;
            if (_loadedObjects.size() <= objectType)
            {
                _loadedObjects.resize((size_t)objectType + 1);
            }
            auto& objectList = _loadedObjects[objectType];
            if (objectList.size() <= entryIndex)
            {
                objectList.resize((size_t)entryIndex + 1);
            }
            objectList[entryIndex] = newLoadedObjects[index];
        }
    }

    void UnloadObject(Object* object)
    {
        if (object != nullptr)
        {
            // TODO try to prevent doing a repository search
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(object->GetObjectEntry());
            if (ori != nullptr)
            {
                _objectRepository.UnregisterLoadedObject(ori, object);
            }

            // Because it's possible to have the same loaded object for multiple
            // slots, we have to make sure find and set all of them to nullptr
            for (auto& objectList : _loadedObjects)
            {
                for (auto& obj : objectList)
                {
                    if (obj == object)
                    {
                        obj = nullptr;
                    }
                }
            }

            object->Unload();
            delete object;
        }
    }

    void UnloadObjectsExcept(const std::vector<Object*>& newLoadedObjects)
    {
        // Build a hash set for quick checking
        auto exceptSet = std::unordered_set<Object*>();
        for (auto object : newLoadedObjects)
        {
            if (object != nullptr)
            {
                exceptSet.insert(object);
            }
        }

        // Unload objects that are not in the hash set
        size_t totalObjectsLoaded = 0;
        size_t numObjectsUnloaded = 0;
        for (auto& objectList : _loadedObjects)
        {
            for (auto object : objectList)
            {
                if (object != nullptr)
                {
                    totalObjectsLoaded++;
                    if (exceptSet.find(object) == exceptSet.end())
                    {
                        UnloadObject(object);
                        numObjectsUnloaded++;
                    }
                }
            }
        }

        log_verbose("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
    }

    void UpdateSceneryGroupIndexes()
    {
        for (auto& objectList : _loadedObjects)
        {
            for (auto loadedObject : objectList)
            {
                if (loadedObject != nullptr)
                {
                    rct_scenery_entry* sceneryEntry;
                    switch (loadedObject->GetObjectType())
                    {
                        case OBJECT_TYPE_SMALL_SCENERY:
                            sceneryEntry = (rct_scenery_entry*)loadedObject->GetLegacyData();
                            sceneryEntry->small_scenery.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                            break;
                        case OBJECT_TYPE_LARGE_SCENERY:
                            sceneryEntry = (rct_scenery_entry*)loadedObject->GetLegacyData();
                            sceneryEntry->large_scenery.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                            break;
                        case OBJECT_TYPE_WALLS:
                            sceneryEntry = (rct_scenery_entry*)loadedObject->GetLegacyData();
                            sceneryEntry->wall.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                            break;
                        case OBJECT_TYPE_BANNERS:
                            sceneryEntry = (rct_scenery_entry*)loadedObject->GetLegacyData();
                            sceneryEntry->banner.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                            break;
                        case OBJECT_TYPE_PATH_BITS:
                            sceneryEntry = (rct_scenery_entry*)loadedObject->GetLegacyData();
                            sceneryEntry->path_bit.scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                            break;
                        case OBJECT_TYPE_SCENERY_GROUP:
                            auto sgObject = dynamic_cast<SceneryGroupObject*>(loadedObject);
                            sgObject->UpdateEntryIndexes();
                            break;
                    }
                }
            }
        }

        // HACK Scenery window will lose its tabs after changing the scenery group indexing
        //      for now just close it, but it will be better to later tell it to invalidate the tabs
        window_close_by_class(WC_SCENERY);
    }

    uint8_t GetPrimarySceneryGroupEntryIndex(Object* loadedObject)
    {
        auto sceneryObject = dynamic_cast<SceneryObject*>(loadedObject);
        const rct_object_entry* primarySGEntry = sceneryObject->GetPrimarySceneryGroup();
        Object* sgObject = GetLoadedObject(primarySGEntry);

        uint8_t entryIndex = 255;
        if (sgObject != nullptr)
        {
            entryIndex = GetLoadedObjectEntryIndex(sgObject);
        }
        return entryIndex;
    }

    rct_object_entry* DuplicateObjectEntry(const rct_object_entry* original)
    {
        rct_object_entry* duplicate = Memory::Allocate<rct_object_entry>(sizeof(rct_object_entry));
        duplicate->checksum = original->checksum;
        strncpy(duplicate->name, original->name, 8);
        duplicate->flags = original->flags;
        return duplicate;
    }

    std::vector<rct_object_entry> GetInvalidObjects(const rct_object_entry* entries) override
    {
        std::vector<rct_object_entry> invalidEntries;
        invalidEntries.reserve(OBJECT_ENTRY_COUNT);
        for (int32_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
        {
            auto entry = entries[i];
            const ObjectRepositoryItem* ori = nullptr;
            if (object_entry_is_empty(&entry))
            {
                entry = {};
                continue;
            }

            ori = _objectRepository.FindObject(&entry);
            if (ori == nullptr)
            {
                if (object_entry_get_type(&entry) != OBJECT_TYPE_SCENARIO_TEXT)
                {
                    invalidEntries.push_back(entry);
                    ReportMissingObject(&entry);
                }
                else
                {
                    entry = {};
                    continue;
                }
            }
            else
            {
                Object* loadedObject = nullptr;
                loadedObject = ori->LoadedObject;
                if (loadedObject == nullptr)
                {
                    loadedObject = _objectRepository.LoadObject(ori);
                    if (loadedObject == nullptr)
                    {
                        invalidEntries.push_back(entry);
                        ReportObjectLoadProblem(&entry);
                    }
                    delete loadedObject;
                }
            }
        }
        return invalidEntries;
    }

    std::vector<const ObjectRepositoryItem*> GetRequiredObjects(const rct_object_entry* entries, size_t count)
    {
        std::vector<const ObjectRepositoryItem*> requiredObjects;
        std::vector<rct_object_entry> missingObjects;

        for (size_t i = 0; i < count; i++)
        {
            const rct_object_entry* entry = &entries[i];
            const ObjectRepositoryItem* ori = nullptr;
            if (!object_entry_is_empty(entry))
            {
                ori = _objectRepository.FindObject(entry);
                if (ori == nullptr && object_entry_get_type(entry) != OBJECT_TYPE_SCENARIO_TEXT)
                {
                    missingObjects.push_back(*entry);
                    ReportMissingObject(entry);
                }
            }
            requiredObjects.push_back(ori);
        }

        if (!missingObjects.empty())
        {
            throw ObjectLoadException(std::move(missingObjects));
        }

        return requiredObjects;
    }

    template<typename T, typename TFunc> static void ParallelFor(const std::vector<T>& items, TFunc func)
    {
        auto partitions = std::thread::hardware_concurrency();
        auto partitionSize = (items.size() + (partitions - 1)) / partitions;
        std::vector<std::thread> threads;
        for (size_t n = 0; n < partitions; n++)
        {
            auto begin = n * partitionSize;
            auto end = std::min(items.size(), begin + partitionSize);
            threads.emplace_back(
                [func](size_t pbegin, size_t pend) {
                    for (size_t i = pbegin; i < pend; i++)
                    {
                        func(i);
                    }
                },
                begin, end);
        }
        for (auto& t : threads)
        {
            t.join();
        }
    }

    std::vector<Object*> LoadObjects(std::vector<const ObjectRepositoryItem*>& requiredObjects, size_t* outNewObjectsLoaded)
    {
        std::vector<Object*> objects;
        std::vector<Object*> loadedObjects;
        std::vector<rct_object_entry> badObjects;
        objects.resize(OBJECT_ENTRY_COUNT);
        loadedObjects.reserve(OBJECT_ENTRY_COUNT);

        // Read objects
        std::mutex commonMutex;
        ParallelFor(requiredObjects, [this, &commonMutex, requiredObjects, &objects, &badObjects, &loadedObjects](size_t i) {
            auto ori = requiredObjects[i];
            Object* loadedObject = nullptr;
            if (ori != nullptr)
            {
                loadedObject = ori->LoadedObject;
                if (loadedObject == nullptr)
                {
                    loadedObject = _objectRepository.LoadObject(ori);
                    if (loadedObject == nullptr)
                    {
                        std::lock_guard<std::mutex> guard(commonMutex);
                        badObjects.push_back(ori->ObjectEntry);
                        ReportObjectLoadProblem(&ori->ObjectEntry);
                    }
                    else
                    {
                        std::lock_guard<std::mutex> guard(commonMutex);
                        loadedObjects.push_back(loadedObject);
                        // Connect the ori to the registered object
                        _objectRepository.RegisterLoadedObject(ori, loadedObject);
                    }
                }
            }
            objects[i] = loadedObject;
        });

        // Load objects
        for (auto obj : loadedObjects)
        {
            obj->Load();
        }

        if (!badObjects.empty())
        {
            // Unload all the new objects we loaded
            for (auto object : loadedObjects)
            {
                UnloadObject(object);
            }
            throw ObjectLoadException(std::move(badObjects));
        }

        if (outNewObjectsLoaded != nullptr)
        {
            *outNewObjectsLoaded = loadedObjects.size();
        }
        return objects;
    }

    Object* GetOrLoadObject(const ObjectRepositoryItem* ori)
    {
        Object* loadedObject = ori->LoadedObject;
        if (loadedObject == nullptr)
        {
            // Try to load object
            loadedObject = _objectRepository.LoadObject(ori);
            if (loadedObject != nullptr)
            {
                loadedObject->Load();

                // Connect the ori to the registered object
                _objectRepository.RegisterLoadedObject(ori, loadedObject);
            }
        }
        return loadedObject;
    }

    void ResetTypeToRideEntryIndexMap()
    {
        reset_type_to_ride_entry_index_map(*this);
    }

    static void ReportMissingObject(const rct_object_entry* entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object not found.", objName);
    }

    void ReportObjectLoadProblem(const rct_object_entry* entry)
    {
        utf8 objName[DAT_NAME_LENGTH + 1] = { 0 };
        std::copy_n(entry->name, DAT_NAME_LENGTH, objName);
        Console::Error::WriteLine("[%s] Object could not be loaded.", objName);
    }

    static int32_t GetIndexFromTypeEntry(int32_t objectType, size_t entryIndex)
    {
        int32_t result = 0;
        for (int32_t i = 0; i < objectType; i++)
        {
            result += object_entry_group_counts[i];
        }
        result += (int32_t)entryIndex;
        return result;
    }
};

std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository)
{
    return std::make_unique<ObjectManager>(objectRepository);
}

void* object_manager_get_loaded_object(const rct_object_entry* entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.GetLoadedObject(entry);
    return (void*)loadedObject;
}

uint8_t object_manager_get_loaded_object_entry_index(const void* loadedObject)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    const Object* object = static_cast<const Object*>(loadedObject);
    uint8_t entryIndex = objectManager.GetLoadedObjectEntryIndex(object);
    return entryIndex;
}

void* object_manager_load_object(const rct_object_entry* entry)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* loadedObject = objectManager.LoadObject(entry);
    return (void*)loadedObject;
}

void object_manager_unload_objects(const rct_object_entry* entries, size_t count)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadObjects(entries, count);
}

void object_manager_unload_all_objects()
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    objectManager.UnloadAll();
}

rct_string_id object_manager_get_source_game_string(const uint8_t sourceGame)
{
    return ObjectManager::GetObjectSourceGameString(sourceGame);
}
