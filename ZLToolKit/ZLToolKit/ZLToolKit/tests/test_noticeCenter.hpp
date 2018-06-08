//
//  main.cpp
//  SignalSlot
//
//  Created by Haitao on 10/2/18.
//  Copyright © 2018年 mess. All rights reserved.
//

#include <iostream>
#include "NoticeCenter.h"


using namespace std::placeholders;


class String
{
public:
    String(const char* str)
    {
        printf("String ctor this %p\n", this);
    }
    
    String(const String& rhs)
    {
        printf("String copy ctor this %p, rhs %p\n", this, &rhs);
    }
    
    String(String&& rhs)
    {
        printf("String move ctor this %p, rhs %p\n", this, &rhs);
    }
};

class Foo
{
public:
    void zero();
    void zeroc() const;
    void one(int);
    void oner(int&);
    void onec(int) const;
    void oneString(const String& str);
    void Notify(const std::unordered_map<std::string,std::string> &dic);
    void Notify2(const std::unordered_map<std::string,std::string> &dic)
    {
        std::cout << "Notify2:" << dic.at("hello") << std::endl;
    }
    
    // void oneStringRR(String&& str);
    static void szero();
    static void sone(int);
    static void soneString(const String& str);
};

void Foo::Notify(const std::unordered_map<std::string,std::string> &dic)
{
    std::cout << "Notify:" << dic.at("hello") << std::endl;
}

void Foo::zero()
{
    printf("Foo::zero()\n");
}

void Foo::zeroc() const
{
    printf("Foo::zeroc()\n");
}

void Foo::szero()
{
    printf("Foo::szero()\n");
}

void Foo::one(int x)
{
    printf("Foo::one() x=%d\n", x);
}

void Foo::onec(int x) const
{
    printf("Foo::onec() x=%d\n", x);
}

void Foo::sone(int x)
{
    printf("Foo::sone() x=%d\n", x);
}

void Foo::oneString(const String& str)
{
    printf("Foo::oneString\n");
}

void Foo::soneString(const String& str)
{
    printf("Foo::soneString\n");
}

int test_notice()
{
    std::cout << "Hello, World!\n";
    Foo f;
    NoticeCenter::Instance().AddObserver("hello", std::bind(&Foo::Notify, &f, _1));
    NoticeCenter::Instance().AddObserver("hello", std::bind(&Foo::Notify2, &f, _1));
    std::unordered_map<std::string, std::string> info;
    info["hello"] = "world";
    NoticeCenter::Instance().PostNotificationName("hello", info);
    
    return 0;
}
