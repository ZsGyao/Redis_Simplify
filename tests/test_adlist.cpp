/**
  ******************************************************************************
  * @file           : test_adlist.cpp
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-3-22
  ******************************************************************************
  */

#include "src/adlist.h"
#include <memory>
#include <string>
#include <iostream>
#include <sstream>

class Person {
public:
    Person(std::string&& name, int age, bool sex) : m_name(name), m_age(age), m_sex(sex) {}

    std::string toString() {
       return "name:" + m_name + " age:" + std::to_string(m_age) + " m_sex" + std::to_string(m_sex) + "\n";
    }

private:
    std::string       m_name;
    int               m_age;
    bool              m_sex;
};

int main(int argc, char* argv[]) {
    auto list = std::make_shared<redis_simplify::List>();
    list->listAddNodeTail(new Person("lili", 12, 1));
    list->listAddNodeTail(new Person("bibi", 14, 0));
    redis_simplify::listNode* node;
    std::cout << "------------------------" << std::endl;
    auto iter = list->listGetIterator(AL_START_HEAD);
    while ((node = redis_simplify::List::listNext(iter)) != nullptr) {
        std::cout << ((Person*)redis_simplify::List::listNodeValue(node))->toString();
    }
    std::cout << "------------------------" << std::endl;
    list->listAddNodeHead(new Person("sss", 24, 1));
    list->listAddNodeHead(new Person("hahaha", 24, 0));
    list->listRewind(iter);
    while ((node = redis_simplify::List::listNext(iter)) != nullptr) {
        std::cout << ((Person*)redis_simplify::List::listNodeValue(node))->toString();
    }
    std::cout << "------------------------" << std::endl;
    list->listAddNodeTail(new Person("niuniu", 14, 0));
    list->listRewindTail(iter);
    while ((node = redis_simplify::List::listNext(iter)) != nullptr) {
        std::cout << ((Person*)redis_simplify::List::listNodeValue(node))->toString();
    }

    redis_simplify::List::listReleaseIterator(iter);
    return 0;
}