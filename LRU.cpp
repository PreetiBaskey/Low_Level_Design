#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<list>
#include<string>
#include<memory>

using namespace std;

class CacheStorage {
    public:
        virtual ~CacheStorage() = default;
        virtual string get(const string &key) = 0;
        virtual void put(const string &key, const string &value) = 0;
        virtual void remove(const string &key) = 0;
        virtual bool isFull() = 0;
};

class CacheEvictionStrategy {
    public:
        virtual ~CacheEvictionStrategy() = default;
        virtual string evictKey() = 0;
        virtual void keyAccessed(const string &key) = 0; 
};

class InMemoryCacheStorage : public CacheStorage {
    private:
        unordered_map<string, string> map;
        int capacity;
    public:
        InMemoryCacheStorage(int cap) : capacity(cap) {}
        
        string get(const string &key) {
            return map.count(key) ? map[key] : "";
        }
        
        void put(const string &key, const string &value) {
            map[key] = value;
        }
        
        void remove(const string &key) {
            map.erase(key);
        }
        
        bool isFull() {
            return map.size() >= capacity;
        }
};

class LRUEvictionStrategy : public CacheEvictionStrategy {
    private:
        list<string> accessOrderList;
        unordered_set<string> existingKey;
        
        string evictKey() {
            if(accessOrderList.empty()) {
                return "";
            }
            string keyToEvict = accessOrderList.front();
            accessOrderList.pop_front();
            existingKey.erase(keyToEvict);
            
            return keyToEvict;
        }
        
        void keyAccessed(const string &key) {
            if(existingKey.count(key)) {
                accessOrderList.remove(key);
            }
            
            accessOrderList.push_back(key);
            existingKey.insert(key);
        }
}; 

class Cache {
    private:
        unique_ptr<CacheStorage> storage;
        unique_ptr<CacheEvictionStrategy> evictionStrategy;
        
    public:
        Cache(unique_ptr<CacheStorage> s, unique_ptr<CacheEvictionStrategy> e) : storage(move(s)), evictionStrategy(move(e)) {}
        
        string get(const string &key) {
            string value = storage->get(key);
            if(!value.empty()) {
                evictionStrategy->keyAccessed(key);
            }
            
            return value;
        }
        
        void put(const string &key, const string &value) {
            if(storage->isFull() && storage->get(key).empty()) {
                string keyToRemove = evictionStrategy->evictKey();
                storage->remove(keyToRemove);
            }
            storage->put(key, value);
            evictionStrategy->keyAccessed(key);
        }
};

int main() {
    
    int capacity = 3;
    auto storage = make_unique<InMemoryCacheStorage>(capacity);
    auto strategy = make_unique<LRUEvictionStrategy>();
    
    Cache cache(move(storage), move(strategy));
    
    cache.put("1", "Apple");
    cache.put("2", "Banana");
    cache.put("3", "Cherry");
    cache.put("4", "Date");
    
    string val1 = cache.get("1");
    cout<<"should be empty because of cache size 3 :- "<<val1<<endl;
    cout<<"value of key 2 is :- "<<cache.get("2")<<endl;
    
    return 0;
}
