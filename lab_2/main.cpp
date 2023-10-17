#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <execution>
#include <thread>
#include <algorithm>

using namespace std;
using namespace std::chrono;
using namespace std::execution;

// Структура для збереження результатів: кількість потоків і тривалість виконання.
struct Result {
    int threads;
    nanoseconds::rep duration;
};

// Функція для дослідження швидкодії none_of за відсутності політики.
void execute_none_of(const vector<int>& data, bool(*predicate)(int)) {
    auto start = high_resolution_clock::now();
    none_of(data.begin(), data.end(), predicate);
    auto end = high_resolution_clock::now();
    nanoseconds duration = duration_cast<nanoseconds>(end - start);
    cout << "Execution time: " << duration.count() << " nanoseconds, data size: " << data.size() << endl;
}

// Предикати для none_of.
bool is_greater_than_ten(int val) {
    return val > 10;
}
bool is_zero(int val) {
    return val == 0;
}

// Функція для дослідження швидкодії none_of з різними політиками виконання.
void timeit(const vector<int>& data, const string& policy_name, auto policy) {
    auto start = high_resolution_clock::now();
    none_of(policy, data.begin(), data.end(), is_zero);
    auto end = high_resolution_clock::now();
    nanoseconds duration = duration_cast<nanoseconds>(end - start);

    cout << "Execution time with " << policy_name << ": " << duration.count() << " nanoseconds" << endl;
}


// Функція для обробки частини даних в паралельних потоках.
void process_chunk(int start_idx, int end_idx, const vector<int>& data, vector<bool>& results, int thread_idx) {
    results[thread_idx] = none_of(data.begin() + start_idx, data.begin() + end_idx, [](int val) {
        return val == 0;
    });
}

// Функція для дослідження швидкодії власного паралельного алгоритму за умови використання різної кількості потоків,
nanoseconds::rep custom_parallel_none_of(const vector<int>& data, int K) {
    int chunk_size = data.size() / K;
    vector<thread> threads;
    vector<bool> results(K, true);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < K; ++i) {
        int start_idx = i * chunk_size;
        int end_idx = (i == K - 1) ? data.size() : start_idx + chunk_size;

        threads.push_back(thread(process_chunk, start_idx, end_idx, cref(data), ref(results), i));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = high_resolution_clock::now();
    nanoseconds duration = duration_cast<nanoseconds>(end - start);

    return duration.count();
}

int main() {
    vector<int> data(1000000);
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(0, 1000000);

    for (int& value : data) {
        value = dist(rng);
    }

    int max_threads = thread::hardware_concurrency();
    vector<Result> results;

    cout << "Without policy:" << endl;
    //execute_none_of досліджується на різних обсягах даних та за різних предикатів.
    for (int size = 1000; size <= 1000000; size *= 10) {
        vector<int> data_to_test(size);
        mt19937 rng(random_device{}());
        uniform_int_distribution<int> dist(0, 1000000);

        for (int& value : data_to_test) {
            value = dist(rng);
        }

        cout << "Testing with predicate: is_greater_than_ten" << endl;
        execute_none_of(data_to_test, is_greater_than_ten);

        cout << "Testing with predicate: is_zero" << endl;
        execute_none_of(data_to_test, is_zero);
        cout << endl;
    }


    timeit(data, "seq policy", seq);
    timeit(data, "par policy", par);
    timeit(data, "par_unseq policy", par_unseq);

    for (int k = 1; k <= max_threads; ++k) {
        results.push_back({k, custom_parallel_none_of(data, k)});
    }

    cout << endl;
    cout << "K\tExecution Time (ns)" << endl;
    for (const auto& result : results) {
        cout << result.threads << "\t" << result.duration << endl;
    }

    auto best_result = min_element(results.begin(), results.end(), [](const Result& a, const Result& b) {
        return a.duration < b.duration;
    });

    cout << "\nBest K: " << best_result->threads << " with execution time: " << best_result->duration << " nanoseconds." << endl;
    return 0;
}
