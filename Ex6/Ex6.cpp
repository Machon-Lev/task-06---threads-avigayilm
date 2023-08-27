#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include <sstream>

using namespace std;


class mes
{
public:
    std::thread::id id;
    double val;
    bool isLast;
    mes() {}
    mes(thread::id id1, double val1, bool isLast1) :id(id1), val(val1), isLast(isLast1) {}
    friend std::ostream& operator<<(std::ostream& os, const mes& message) 
    {
        os << "id:" << message.id << ":" << message.val << endl;;
        return os;
    }
};


// Function to send messages
void sendMessages(list<mes>& out, mutex& mutex)
{
    double value= std::hash<std::thread::id>{}(std::this_thread::get_id());
    while (true)
    {
        
        // entering name to list 
        {
            lock_guard<std::mutex> lock(mutex);
            if (int(value) == 0)
            {
                out.push_back(mes(std::this_thread::get_id(), value, true));
                return;
            }
            out.push_back(mes(std::this_thread::get_id(), value, false));
            value /= 10;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}

// Consumer function
void consumeMessages(list<mes>& msg, mutex& mutex) {
    int end = 0;
    mes res;
    while (true)
    {
        if (!msg.empty())
        {
            {
                lock_guard<std::mutex> lock(mutex);
                res = msg.back();
                msg.pop_back();
            }

            if (res.isLast == true)
            {
                cout << res.id << " finished" << endl;
                end++;
                if (end == 2)
                    return;
            }
            else
                cout << res;
        }
    }
}

int main() 
{
    cout << "Main thread started" << std::endl;
    list<mes> msg;
    mutex mtx;
    // Create consumer thread
    thread consumerThread(consumeMessages, ref(msg),  ref(mtx));

    // Create producer threads
    thread producerThread1(sendMessages,ref(msg), ref(mtx));
    thread producerThread2(sendMessages,ref( msg), ref(mtx));

    // Join the threads
    consumerThread.join();
    producerThread1.join();
    producerThread2.join();

    cout << "Main thread finished" << std::endl;

    return 0;
}
