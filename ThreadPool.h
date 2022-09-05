#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <string>
#include <atomic>
//int sendf(int a, std::string b)
//{
//    std::cout << a << std::endl
//        << b << std::endl;
//    return 0;
//}
class task
{
public:
    bool cnt;
    int a;
    virtual int run()
    {
     
        cnt= q(a);
        return 0;
    }
 
    int geta(int a)
    {
        this->a = a;
        return this->a;
    }
    char* getbuffer() {
        
        return this->buffer;
    }
    char* getb(char* b)
    {
        this->b = b;
        return this->b;
    }
    void getp(int (*p)(int, char* buf))
    {
        this->p = p;
    }
    void getaccpet(bool (*q)(int)) {
              this->q = q;
             
    }
    int getnum(int *max) {
         this->ma = max;
    }
    void getsd(fd_set  readfdset) {
        this->readfdset = readfdset;
    }
private:
 
    int num;
    char buffer[1024];
    
    int *ma;
    char* b;
    fd_set  readfdset;
    int (*p)(int, char* buf);
    bool (*q)(int);
    std::mutex m;
    std::mutex n;
};
class threadpool
{
public:
    threadpool(int count) : tpfg(true)
    {

        for (int i = 0; i < count; i++)
        {
            threads.emplace_back(&threadpool::work, this);
        }
    }
    ~threadpool()
    {
        threadstop();
    }
    void threadstart()
    {
        tpfg = true;
    }
    void threadstop()
    {
        tpfg = false;
        cv.notify_all();
        for (auto& th : threads) // joinable()函数是一个布尔类型的函数，他会返回一个布尔值来表示当前的线程是否是可执行线程
        {
            if (th.joinable())
            {
                th.join();
            }
        }
    }
    
    void  push_rewu_tasks(task* taskone)
    {
       
        std::unique_lock<std::mutex> ulock(this->mtx);
        this->tasks.push(taskone);
        ulock.unlock();
        this->cv.notify_one();
    }
    void work()
    {
        while (true)
        {
            if (!tpfg)
            {
                return;
            }
            task* taskone = nullptr;
            {
                std::unique_lock<std::mutex> lock(this->mtx);
                while (tasks.empty())
                {
                    cv.wait(lock, [&]
                        { return !tasks.empty() || !tpfg; });
                }
                if (tpfg)
                {
                    taskone = this->tasks.front();
                    this->tasks.pop();
                }
                if (taskone != nullptr)
                {
                   
                    taskone->run();
                    
                }
            }
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<task*> tasks;
    std::atomic_bool tpfg;
    std::condition_variable cv;
    std::mutex mtx;
};
