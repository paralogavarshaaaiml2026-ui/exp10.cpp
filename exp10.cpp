#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

queue<int> buffer;
mutex mtx;
condition_variable cv;

const int MAX_ITEMS = 10;
bool finished = false;

void producer()
{
    for (int i = 1; i <= MAX_ITEMS; i++)
    {
        {
            lock_guard<mutex> lock(mtx);
            buffer.push(i);
            cout << "Produced: " << i << endl;
        }

        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(mtx);
        finished = true;
    }

    cv.notify_all();
}

void consumer()
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);

        cv.wait(lock, [] {
            return !buffer.empty() || finished;
        });

        if (buffer.empty() && finished)
            break;

        int item = buffer.front();
        buffer.pop();

        cout << "Consumed: " << item << endl;
    }
}

int main()
{
    cout << "Producer-Consumer System Started\n\n";

    thread producerThread(producer);
    thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    cout << "\nTotal Items Produced: " << MAX_ITEMS << endl;
    cout << "Producer-Consumer System Completed" << endl;

    return 0;
}