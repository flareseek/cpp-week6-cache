#include "cache.h"
#include <sstream>

// TODO: 필요한 함수 구현
Cache::Cache() {
    size_ = 0;
    head_ = nullptr;
    tail_ = nullptr;

    for (int i = 0; i < CACHE_SIZE; i++) {
        HashList* hashList = new HashList;
        hashList->tail = nullptr;
        hashMap_[i] = hashList;
    }
}

Cache::~Cache() {
    // free cache
    while (head_ != nullptr) {
        CacheNode* temp = head_;
        head_ = head_->next;
        freeCacheNode(temp);
    }

    // free hashmap
    for (int i = 0; i < CACHE_SIZE; i++) {
        while(hashMap_[i]->tail != nullptr) {
            HashNode* temp = hashMap_[i]->tail;
            hashMap_[i]->tail = hashMap_[i]->tail->prev;
            delete temp;
        }
        delete hashMap_[i];
    }
}

void Cache::freeCacheNode(CacheNode* node) {
    switch (node->type) {
        case INT:
            delete (int*)node->value;
            break;
        case DOUBLE:
            delete (double*)node->value;
            break;
        default:
            break;
    }
    delete node;
}

void Cache::removeCacheNode(CacheNode* node) {
    if (node->prev != nullptr)
        node->prev->next = node->next;
    if (node->next != nullptr)
        node->next->prev = node->prev;

    if (node == head_)
        head_ = node->next;
    if (node == tail_)
        tail_ = node->prev;

    size_ --;
}

void Cache::deleteCacheNode(CacheNode* node) {
    deleteHashMap(node->key);
    removeCacheNode(node);
    freeCacheNode(node);
}

void Cache::addCacheNode(CacheNode* newNode) {
    newNode->next = nullptr;
    if (size_ == 0) {
        newNode->prev = nullptr;
        head_ = newNode;
        tail_ = newNode;
    } else {
        // head 삭제
        if (size_ >= CACHE_SIZE) {
            deleteHashMap(head_->key);
            deleteCacheNode(head_);
        }
        tail_->next = newNode;
        newNode->prev = tail_;
        tail_ = newNode;
    }
    addHashMap(newNode->key, newNode);

    size_ ++;
}

void Cache::addCacheNode(std::string key, void* value, Type type) {
    CacheNode* newNode = new CacheNode;
    newNode->key = key;
    newNode->type = type;
    newNode->value = value;
    newNode->prev = nullptr;
    newNode->next = nullptr;

    addCacheNode(newNode);
}

void Cache::add(std::string key, int value) {
    addCacheNode(key, new int(value), INT);
}

void Cache::add(std::string key, double value) {
    addCacheNode(key, new double(value), DOUBLE);
}


void Cache::refreshCacheNode(CacheNode* node) {
    removeCacheNode(node);
    addCacheNode(node);
}

void* Cache::getValue(std::string key, Type type) {
    CacheNode* cacheNode = getHashMap(key);
    if (cacheNode == nullptr) return nullptr;

    refreshCacheNode(cacheNode);
    return cacheNode->value;
}

bool Cache::get(std::string key, int &value) {
    void* voidValue = getValue(key, INT);
    if (voidValue == nullptr) return false;
    value = *(int*)voidValue;
    return true;
}

bool Cache::get(std::string key, double &value) {
    void* voidValue = getValue(key, DOUBLE);
    if (voidValue == nullptr) return false;
    value = *(double*)voidValue;
    return true;
}

std::string Cache::toString() {
    std::string result = "";
    CacheNode* temp = tail_;
    while(temp != nullptr) {
        std::string value = "";

        switch (temp->type) {
            case INT:
                value = std::to_string(*(int*)temp->value);
                break;
            case DOUBLE: {
                std::ostringstream ss;
                ss << *(double*)temp->value;
                value = ss.str();
                break;
            }
            default:
                break;
        }
        result += "["  + temp->key  + ": " + value + "] -> ";
        temp = temp->prev;
    }
    if (result.size() >= 4) {
        result.erase(result.size() - 4, 4);
    }

    result += "\n";
    return result;
}

int Cache::hash(std::string key) {
    int sum = 0;
    for (int i = 0; i < key.length(); i++) sum += key[i];
    return sum % CACHE_SIZE;
}

void Cache::addHashMap(std::string key, CacheNode* value) {
    HashNode* hashNode = new HashNode;
    hashNode->key = key;
    hashNode->value = value;
    hashNode->prev = nullptr;
    hashNode->next = nullptr;

    int hashIndex = hash(key);

    HashNode* temp = hashMap_[hashIndex]->tail;
    while(temp != nullptr) {
        if (temp->key == key) {
            temp->value = value;
            return;
        }
        temp = temp->prev;
    }

    if (hashMap_[hashIndex]->tail == nullptr) {
        hashMap_[hashIndex]->tail = hashNode;
    } else {
        hashMap_[hashIndex]->tail->next = hashNode;
        hashNode->prev = hashMap_[hashIndex]->tail;
        hashMap_[hashIndex]->tail = hashNode;
    }
}

void Cache::deleteHashMap(std::string key) {
    int hashIndex = hash(key);

    HashNode* temp = hashMap_[hashIndex]->tail;
    while(temp != nullptr) {
        if (temp->value->key == key) {
            if (temp->prev != nullptr)
                temp->prev->next = temp->next;
            if (temp->next != nullptr)
                temp->next->prev = temp->prev;

            if (temp == hashMap_[hashIndex]->tail)
                hashMap_[hashIndex]->tail = temp->prev;

            delete temp;
        }
        temp = temp->prev;
    }
}
