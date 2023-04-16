#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <vector>

using namespace std;

// promise
namespace promiseExample
{

    void worker(int value, promise<int> &simple_promise)
    {

        std::thread::id id = this_thread::get_id();

        cout << "<wor START>"
             << " (thread: " << id << ")" << endl;

        for (int index = 0; index < 100; index++)
        {

            ++value;
            cout << index << " (thread: " << id << ")" << endl;
            this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // throw exception("Any exeption");
        //  на самом деле в таком случае достаточно
        //  обрботать исключение обернув get во вторичных потоках

        // если не установить значение программа уйдет в бесконечный цикл ожидания
        simple_promise.set_value(value);
        /*
        set_value нельзя вызывать несколько раз
        также, как и set_exeption
        simple_promise.set_exception(make_exception_ptr(exception("Any exeption")));
        */

        // значение уже получено, но далее работает како-нибудь финализирующий код
        this_thread::sleep_for(std::chrono::seconds(3));

        cout << endl
             << "<wor END>"
             << " (thread: " << id << ")" << endl;
    }

    void firstWorker(shared_future<int> &simple_future)
    {

        std::thread::id id = this_thread::get_id();

        cout << endl
             << "<fir START>"
             << " (thread: " << id << ")";

        int value = simple_future.get();

        cout << endl
             << "fir processed value: " << value;

        cout << endl
             << "<fir END>"
             << " (thread: " << id << ")";
    }

    void secondWorker(shared_future<int> &simple_future)
    {

        std::thread::id id = this_thread::get_id();

        cout << endl
             << "<sec START>"
             << " (thread: " << id << ")";

        int value = simple_future.get();

        cout << endl
             << "sec processed value: " << value;

        cout << endl
             << "<sec END>"
             << " (thread: " << id << ")";
    }

    void showExample()
    {

        int value = 0;

        // шаблонный параметр тип int под возвращаемое значение worker
        promise<int> simple_promise;

        // в случае использования promise асинхронный запуск организовывается по другому
        // методы класса promise позволяют инициализировать future
        shared_future<int> simple_futer = simple_promise.get_future().share();

        // в отличии от async поток не запускается в момент инициализации future
        // его необходимо передавать в ручную
        thread worThr(worker, value, ref(simple_promise));
        thread firThr(firstWorker, ref(simple_futer));
        thread secThr(secondWorker, ref(simple_futer));

        worThr.join();
        firThr.join();
        secThr.join();
    }
}

int main()
{
    promiseExample::showExample();

    return 0;
}