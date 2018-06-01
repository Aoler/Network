//
//  test_SignalSlot.h
//  ZLToolKit
//
//  Created by Haitao on 1/6/18.
//  Copyright © 2018年 mess. All rights reserved.
//

#include <string>
#include "Util/SignalSlot.h"
using namespace std;

class Foo
{
public:
    void zero();
    void zeroc() const;
    void one(int);
    void oner(int&);
    void onec(int) const;
    void oneString(const string& str);
    // void oneStringRR(String&& str);
    static void szero();
    static void sone(int);
    static void soneString(const string& str);
};

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

void Foo::oneString(const string& str)
{
    printf("Foo::oneString\n");
}

void Foo::soneString(const string& str)
{
    printf("Foo::soneString\n");
}


void test_SignalSlotOne()
{
    Signal<void(int)> signal;
    
    printf("========\n");
    signal.call(50);
    
    Slot s4;
    {
        Slot s1 = signal.connect(&Foo::sone);
        
        printf("========\n");
        signal.call(51);
        
        Foo f;
        Slot s2 = signal.connect(std::bind(&Foo::one, &f, _1));
        
        printf("========\n");
        signal.call(52);
        
        const Foo cf;
        Slot s3 = signal.connect(std::bind(&Foo::onec, &cf, _1));
        
        printf("========\n");
        signal.call(53);
        
        s4 = s3;
    }
    
    printf("========\n");
    signal.call(54);
}
