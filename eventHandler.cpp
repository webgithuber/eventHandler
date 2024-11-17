#include <stack>
#include <typeinfo>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <cstdlib>
#include <time.h>
#include <memory>
#include <functional>
#include <mutex>
#include <semaphore>
#include "single_include/nlohmann/json.hpp"
#include "PriceUpdate.hpp"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

using json = nlohmann::json;
using namespace std;

int B = 0, A = 0, C = 0;
thread t_pool;
void fun(int &cnt)
{
    this_thread::sleep_for(chrono::milliseconds(100));
    std::cout << "count: " << cnt << "\n";
}

////////////////////////////////////////////////////// TreadPool starts///////////////////////////////////////////////////
class ThreadPool
{
private:
    vector<thread> pool;
    queue<function<void()>> tasks;
    condition_variable cv;
    mutex mtx;
    void worker();

public:
    int stop = 0;
    ThreadPool(int n);
    template <typename T>
    void addtask(T func);
    template <class F, class... Args>
    void addtask(F &&f, Args &&...args);
    ~ThreadPool();
};

ThreadPool::ThreadPool(int n)
{
    for (int i = 0; i < n; i++)
    {
        // pool.emplace_back(this->worker());  it is not callable object
        pool.emplace_back([this]
                          { this->worker(); });
        // Create a new thread and add it to the workers vector
        // workers.emplace_back(&ThreadPool::workerThread, this);
    }
}
template <typename T>
void ThreadPool::addtask(T func)
{
    mtx.lock();
    tasks.push(func);
    stop = 1;
    mtx.unlock();
    cv.notify_all();
}
template <class F, class... Args>
void ThreadPool::addtask(F &&f, Args &&...args)
{
    mtx.lock();
    tasks.emplace([&f, &args...] { // cout << "Inside lambda, before invoke\n";
        std::invoke(f, args...);

        // cout << "Inside lambda, after invoke\n";
    });
    stop = 1;
    mtx.unlock();
    // cout<<"successfully added\n";
    cv.notify_all();
}

void ThreadPool::worker()
{
    while (1)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]
                { return stop; });
        if (tasks.size() != 0)
        {
            function<void()> task = std::move(tasks.front());
            tasks.pop();
            if (tasks.size() == 0)
            {
                stop = 0;
            }
            // cout<<"executing";
            try
            {
                if (C == 0)
                {
                    t_pool = thread(fun, ref(C));
                    t_pool.detach();
                }
                C++;
                task();
            }
            catch (const std::exception &e)
            {
                cout << "Exception during task execution: " << e.what() << endl;
            }
        }
        lock.unlock();
    }
}

ThreadPool::~ThreadPool()
{
}
////////////////////////////////////////////////////// ThreadPool end///////////////////////////////////////////////////














////////////////////////////////////////////////////// Strategy Component starts///////////////////////////////////////////////////

class I_Strategy_Implementation
{
public:
    virtual void analyse() = 0;
};

class Spatial_Arbitrage : public I_Strategy_Implementation
{
public:
    void analyse() { this_thread::sleep_for(chrono::milliseconds(100)); }
};
class Statistical_Arbitrage : public I_Strategy_Implementation
{
public:
    void analyse() { this_thread::sleep_for(chrono::milliseconds(200)); }
};
class Delta_Neutral : public I_Strategy_Implementation
{
public:
    void analyse() { this_thread::sleep_for(chrono::milliseconds(300)); }
};

class I_Strategy
{
public:
    I_Strategy_Implementation *strategy;
    I_Strategy(I_Strategy_Implementation *implementation) : strategy(implementation) {}
    virtual void place() = 0;
    virtual void notify(PriceUpdate &obj) = 0;
    void analyse() { strategy->analyse(); }
};
class Strategy : public I_Strategy
{

public:
    Strategy(I_Strategy_Implementation *implementation) : I_Strategy(implementation) {}
    void place() override {}

    void notify(PriceUpdate &obj) override
    {
        thread t1 = thread(analyse, this);
        t1.detach(); // dont wait for thread to finish
    }
};

////////////////////////////////////////////////////// Strategy Component end///////////////////////////////////////////////////















ThreadPool pool(12);
////////////////////////////////////////////////////// EvenHandler Component starts///////////////////////////////////////////////////
class EventHandler
{
    void notify_all(PriceUpdate &obj)
    { // simple iterating and notifying observer
        for (auto &observer : observers)
        {
            observer->notify(obj);
        }
    }
    void notify_all_pool(PriceUpdate &obj)
    { // pool handling notifying
        for (auto &observer : observers)
        { // observer.notify(obj);
            pool.addtask([observer, &obj]()
                                { observer->notify(obj); });
        }
    }

    void worker()
    {
        thread t1;
        while (1)
        {
            if (B == 0)
            {
                t1 = thread(fun, ref(B));
                t1.detach();
            }
            {
                unique_lock<mutex> lock(mtx_event_queue);
                cv.wait(lock, [this]
                        { return !event_queue.empty(); });
                PriceUpdate event = event_queue.front();
                event_queue.pop();
                notify_all_pool(event);
            }
            B++;
            this_thread::sleep_for(chrono::microseconds(10)); // limiting price component
        }
    }

    queue<PriceUpdate> event_queue;
    vector<I_Strategy *> observers;
    mutex mtx_event_queue;
    condition_variable cv;
    thread t_worker;

public:
    EventHandler()
    {
        t_worker = thread(worker, this);
    }
    void add_event(PriceUpdate &obj)
    {

        {
            unique_lock<mutex> lock(mtx_event_queue);
            event_queue.push(obj);
            cv.notify_one();
        }
    }
    void subscribe(I_Strategy *z) { observers.push_back(z); }
    void unsubscribe() {}
};
////////////////////////////////////////////////////// eventHandler Component end///////////////////////////////////////////////////










int main()
{
    int n = 12;
    I_Strategy *s1 = new Strategy(new Spatial_Arbitrage);
    I_Strategy *s2 = new Strategy(new Statistical_Arbitrage);
    I_Strategy *s3 = new Strategy(new Delta_Neutral);
    EventHandler handler;
    handler.subscribe(s1);
    handler.subscribe(s2);
    handler.subscribe(s3);

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Create a socket for connecting to the exchange
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET)
    {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Setup the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the exchange
    if (connect(ConnectSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Connect failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    int i = 0;
    thread t1;
    stack<char> stk;
    string json_string_obj;

    int buffer_size = 2048;
    char buffer[buffer_size];
    while (1)
    {

        int bytes_received = recv(ConnectSocket, buffer, buffer_size - 1, 0);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0'; // Null-terminate the buffer to treat it as a string
                                           // cout << "Received data: " << buffer << endl;
            try
            {

                // cout<<bytes_received<<"\n";
                buffer[bytes_received] = '\0';

                for (int i = 0; i < bytes_received; i++)
                {
                    char ch = buffer[i];
                    if (ch == '{')
                    {
                        if (stk.size() != 0 && stk.top() == '}')
                        {
                            stk.pop();
                        }
                        else
                        {
                            stk.push(ch);
                        }
                    }
                    else if (ch == '}')
                    {
                        if (stk.size() != 0 && stk.top() == '{')
                        {
                            stk.pop();
                        }
                        else
                        {
                            stk.push(ch);
                        }
                    }

                    json_string_obj += ch;

                    if (stk.size() == 0)
                    {
                        if (A == 0)
                        {
                            //t1 = thread(fun, ref(A));
                            //t1.detach();
                        }
                        A++;
                        json received_json = json::parse(json_string_obj);

                        // json to PriceUpdate,added to event queue
                        PriceUpdate update = PriceUpdate::from_json(received_json);
                        handler.add_event(update);
                        // cout<<json_string_obj<<"\n";
                        json_string_obj.clear();
                    }
                }
            }
            catch (json::parse_error &e)
            {
                cerr << "JSON parsing error: " << e.what() << endl;
            }
        }
        else if (bytes_received == 0)
        {
            cout << "Connection closed by server." << endl;
        }
        else
        {
            cerr << "Error receiving data: " << WSAGetLastError() << endl;
        }
    }

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

//f(t)=approx. max no of threads in a system at a given time t
//i=minimum analysis time amonge all strategies
//O= no of strategies (observer)
//f(t)=O*(No of event out for execution from Event queue in i time period);