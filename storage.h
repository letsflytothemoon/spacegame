#pragma once
#include <type_traits>
#include <vector>
#include <map>
#include <mutex>
#include <functional>

//----------------------------------------------------------------
template <class KeySelector, class ... Types>
struct StorageTuple { };

template <class KeySelector, class T, class ... Types>
struct StorageTuple<KeySelector, T, Types ...>
{
    std::map<typename KeySelector::KeyType, T*> storage;
    StorageTuple<KeySelector, Types ...> next;
};
//----------------------------------------------------------------
template <class T, class KeySelector, class NotMatched, class ... Types>
struct StorageGetter
{
    static std::map<typename KeySelector::KeyType, T*>& Get(StorageTuple<KeySelector, NotMatched, Types ...>& storageTuple)
    {
        return StorageGetter<T, KeySelector, Types ...>::Get(storageTuple.next);
    }
};

template <class T, class KeySelector, class ... Types>
struct StorageGetter<T, KeySelector, T, Types ...>
{
    static std::map<typename KeySelector::KeyType, T*>& Get(StorageTuple<KeySelector, T, Types ...>& storageTuple)
    {
        return storageTuple.storage;
    }
};
//----------------------------------------------------------------
template <bool Condition, class Action>
struct If
{
    template <class ... Args>
    static void Do(Args& ... args)
    { Action::Do(args ...); }
};

template <class Action>
struct If<false, Action>
{
    template <class ... Args>
    static void Do(Args& ...)
    { }
};
//----------------------------------------------------------------
template <template <class> class Condition, class Action, class KeySelector, class ... Types>
struct ForEachInStorageTuple { template <class ... Args> static void Do(Args ...) { } };

template <template <class> class Condition, class Action, class KeySelector, class T, class ... Types>
struct ForEachInStorageTuple<Condition, Action, KeySelector, T, Types ...>
{
    template <class ... Args>
    static void Do(StorageTuple<KeySelector, T, Types ...>& storageTuple, Args ... args)
    {
        If<Condition<T>::value, Action>::Do(storageTuple, args ...);
        ForEachInStorageTuple<Condition, Action, KeySelector, Types ...>::Do(storageTuple.next, args ...);
    }
};
//----------------------------------------------------------------
template <class T>
struct IsBaseOf
{
    template <class U>
    struct Check
    {
        static constexpr bool value = std::is_base_of<U, T>::value;
    };
};
//----------------------------------------------------------------
template <class T>
struct IsRelativeOf
{
    template <class U>
    struct Check
    {
        static constexpr bool value =
            std::is_base_of<U, T>::value ||
            std::is_base_of<T, U>::value;
    };
};
//----------------------------------------------------------------
struct ActionAddToMap
{
    template <class KeySelector, class ... Types, class T>
    static void Do(StorageTuple<KeySelector, Types ...>& storageTuple, T ptrObject)
    { storageTuple.storage[KeySelector::Get(*ptrObject)] = ptrObject; }
};

struct ActionRemoveFromMap
{
    template <class KeySelector, class ... Types>
    static void Do(StorageTuple<KeySelector, Types ...>& storageTuple, typename KeySelector::KeyType key)
    { storageTuple.storage.erase(key); }
};
//----------------------------------------------------------------
template <class KeySelector, class ... Types>
class Storage
{
    StorageTuple<KeySelector, Types ...> storageTuple;
protected:
    template <class T>
    std::map<typename KeySelector::KeyType, T*>& GetStorage()
    { return StorageGetter<T, KeySelector, Types ...>::Get(storageTuple); }
public:
    typedef typename KeySelector::KeyType KeyType;

    template<class T>
    bool Check(const typename KeySelector::KeyType& key)
    {
        std::map<typename KeySelector::KeyType, T*>& map = GetStorage<T>();
        return (map.find(key) != map.end());
    }

    template <class T>
    T& Get(const typename KeySelector::KeyType& key)
    { return *GetStorage<T>().find(key)->second; }

    template <class T>
    std::vector<typename KeySelector::KeyType> GetAllKeys()
    {
        std::vector<typename KeySelector::KeyType> result;
        std::map<typename KeySelector::KeyType, T*>& map = GetStorage<T>();
        for(auto i = map.begin(); i != map.end(); i++)
            result.push_back(i->first);
        return result;
    }

    template <class T, class ... Args>
    typename KeySelector::KeyType Add(Args ... args)
    {
        T* ptrObject = new T(args ...);

        ForEachInStorageTuple<IsBaseOf<T>::template Check, ActionAddToMap, KeySelector, Types ...>::Do(storageTuple, ptrObject);

        return KeySelector::Get(*ptrObject);
    }

    template <class T>
    void Remove(typename KeySelector::KeyType key)
    {
        T* ptrObject = GetStorage<T>().find(key)->second;
        ForEachInStorageTuple<IsRelativeOf<T>::template Check, ActionRemoveFromMap, KeySelector, Types ...>::Do(storageTuple, key);
        delete ptrObject;
    }
};