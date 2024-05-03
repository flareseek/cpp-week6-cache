#include "cached_runner.h"
#include "cache.h"
#include "task_runner.h"

// TODO: 필요한 함수 구현

CachedRunner::CachedRunner(Cache &cache) {
    cache_ = &cache;
    hits_ = 0;
    misses_ = 0;
}

double CachedRunner::multiply(std::string filename) {
    double value = 0.0f;
    std::string key = "multiply(" + filename + ")";

    if (cache_->get(key, value)) {
        std::cout << "cache hit multiply"<< std::endl;
        hits_ ++;
    } else {
        misses_ ++;
        value = TaskRunner::multiply(filename);
        cache_->add(key, value);
    }
    return value;
}

int CachedRunner::palindrome(std::string filename) {
    int value = 0;
    std::string key = "palindrome(" + filename + ")";

    if(cache_->get(key, value)) {
        std::cout << "cache hit palindrome"<< std::endl;
        hits_ ++;
    } else {
        misses_ ++;
        value = TaskRunner::palindrome(filename);
        cache_->add(key, value);
    }
    return value;
}

int CachedRunner::hits() {
    return hits_;
}

int CachedRunner::misses() {
    return misses_;
}
