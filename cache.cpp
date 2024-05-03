#include "cache.h"
#include <iostream>
#include <sstream>

// TODO: 필요한 함수 구현
Cache::Cache() {
    size_ = 0;
    head_ = nullptr;
    tail_ = nullptr;
}

Cache::~Cache() {
    while (head_ != nullptr) {
        CacheNode* temp = head_;
        head_ = head_->next;
        freeCacheNode(temp);
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
        if (size_ >= CACHE_SIZE)
            deleteCacheNode(head_);

        tail_->next = newNode;
        newNode->prev = tail_;
        tail_ = newNode;
    }
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

void* Cache::getValue(std::string key, Type type) {
    CacheNode* temp = head_;
    void* value = nullptr;
    while (temp != nullptr) {
        if (temp->key == key && temp->type == type) {
            value = temp->value;
            if (temp == tail_) break;
            removeCacheNode(temp);
            addCacheNode(temp);
        }
        temp = temp->next;
    }
    return value;
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
