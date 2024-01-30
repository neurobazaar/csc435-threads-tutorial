#include <iostream>
#include <vector>
#include <random>
#include <deque>
#include <mutex>
#include <thread>

int task()
{
    int length = 10000000;
    std::vector<int> vector(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> uniform_dist(0, length - 1);

    for (auto i = 0; i < length; i++) {
        vector[i] = i;
    }

    for (auto i = 0; i < length; i++) {
        int j = uniform_dist(gen);
        std::swap(vector[i], vector[j]);
    }

    return vector[length - 1];
}

struct queue_t
{
    int head;
    int tail;
    std::deque<int> deque;
    std::mutex mtx;
};

void run_worker(int thread_id, queue_t& queue)
{
    int head;
    int taskid;
    int rc;

    while (true) {
        {
            std::lock_guard<std::mutex> lock(queue.mtx);
            head = queue.head;
            queue.head++;
        }

        while (head >= queue.tail) {
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queue.mtx);
            taskid = queue.deque.front();
            queue.deque.pop_front();
        }

        if (taskid == -1) {
            break;
        }

        rc = task();

        std::cout << "Worker " << thread_id << " completed task " << head << " (" << rc << ")" << std::endl;;
    }
}

int main(int argc, char** argv)
{
    int num_workers;
    int num_tasks;

    if (argc != 3) {
        std::cerr << "USE: ./run-tasks <number of worker threads> <number of tasks>" << std::endl;
        return 1;
    }

    num_workers = std::atoi(argv[1]);
    num_tasks = std::atoi(argv[2]);

    queue_t queue;
    queue.head = 0;
    queue.tail = 0;
    std::vector<std::thread> threads;

    for (auto i = 0; i < num_workers; i++) {
        threads.push_back(std::thread(run_worker, i + 1, std::ref(queue)));
    }

    for (auto i = 0; i < num_tasks; i++) {
        std::lock_guard<std::mutex> lock(queue.mtx);
        queue.deque.push_back(i + 1);
        queue.tail++;
    }

    for (auto i = 0; i < num_workers; i++) {
        std::lock_guard<std::mutex> lock(queue.mtx);
        queue.deque.push_back(-1);
        queue.tail++;
    }

    for (auto i = 0; i < num_workers; i++) {
        threads[i].join();
    }

    return 0;
}