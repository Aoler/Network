//
//  Notice.h
//  SignalSlot
//
//  Created by Haitao on 1/6/18.
//  Copyright © 2018年 mess. All rights reserved.
//

#ifndef Notice_h
#define Notice_h
#include <unordered_map>
#include <list>
#include <mutex>
#include <memory>
#include <string>

template<typename Signature>
class SignalTrivial;

template <typename RET, typename... ARGS>
class SignalTrivial<RET(ARGS...)>
{
public:
    typedef std::function<void (ARGS...)> Functor;
    
    void connect(Functor &func)
    {
        functors_.push_back(func);
    }
    
    void call(ARGS&... args)
    {
        typename std::vector<Functor>::iterator it = functors_.begin();
        for (; it != functors_.end(); ++it)
        {
            (*it)(args...);
        }
    }
private:
    std::vector<Functor> functors_;
};


class NoticeCenter {
public:
    typedef std::function<void (const std::unordered_map<std::string,std::string>)> Functor;
    typedef SignalTrivial<void(const std::unordered_map<std::string,std::string>)> Signal;
    
    virtual ~NoticeCenter(){}
    static NoticeCenter &Instance()
    {
        static NoticeCenter *instance(new NoticeCenter);
        return *instance;
    }
    
    static void Destroy()
    {
        delete &NoticeCenter::Instance();
    }
    
    void AddObserver(const std::string &key,  Functor fun)
    {
        std:std::lock_guard<std::recursive_mutex> lck(mtxObserver);
        if (mapObserver.find(key) != mapObserver.end())
        {
            mapObserver[key]->connect(fun);
        } else
        {
            std::shared_ptr<Signal> signal(new Signal);
            signal->connect(fun);
            mapObserver[key] = signal;
        }
        
    }
    
    void PostNotificationName(const std::string &key,  std::unordered_map<std::string,std::string> &info)
    {
        std::shared_ptr<Signal> signal = mapObserver[key];
        signal->call(info);
    }
    
private:
    std::recursive_mutex mtxObserver;
    std::unordered_map<std::string,std::shared_ptr<Signal>> mapObserver;
    

};

#endif /* Notice_h */
