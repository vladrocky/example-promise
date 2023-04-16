#  Получение значения из потока посредством promise

Представим, что в функции из примера `future` (после того как было вычисленно необходиое в другом потоке значение) происходит еще какое-то количество операции, тоесть некий финализирующий код. Естественно, что завершение его мы дожидаться не хотим. В таком случае, удобно использовать обещанные значения - тоесть промисы.
```cpp
void genValue(int value, promise<int>& simple_promise) 
{
	…
++value;
//если не установить значение 
// уйдем в бесконечное ожидание
simple_promise.set_value(value);
 /* 
set_value нельзя вызывать несколько раз
также, как и set_exeption 
simple_promise.set_exception(make_exception_ptr(exception("Any exeption")));
 */
 // значение уже получено, но далее работает 
 // какой-нибудь финализирующий код
 this_thread::sleep_for(std::chrono::seconds(3));
…
}
```
```cpp
// в основном потоке
…
// тип int под возвращаемое 
// значение worker
promise<int> simple_promise;
// инициализируем future
future<int> simple_futer = simple_promise.get_future();
// запуск genValue в отдельном потоке
thread workerThr(genValue, 10, ref(simple_promise));
// приостанавливаем основной поток 
// до получения value
int value = simple_future.get()
…
workerThr.join();
…
```
В данном случае, на сигнатуре функции необходимость в возвращаемом значении отразится только в аргументах - нам нужно передать промис (важно что не по значению а по ссылке). Теперь, тип возвращаемого значения нам нужно указывать в качестве шаблонного параметра `promise` при его объявлеии и экземпляра класса промис есть метод получения `future`. Далее уже знакомая работа с потоками.

## Пример

Есть три функции, которые будут выполняться во вторичных потоках. Нужно организовать их параллельное выполнение таким образом, чтобы результаты воркера были переданы в первый и второй воркеры.
```cpp
void worker(int val, promise<int> & smProm)
{
	…
	smProm.set_value(val);
…
// финализирующий код
}
void firWorker(shared_future<int> smFutu)
{
	…
	int x = smFutu.get();
	…
}
void secWorker(shared_future<int> smFutu)
{
	…
	int x = smFutu.get();
	…
}
```
Теперь в основном потоке мы ниего не получаем, а просто управляем запуском потоков.
```cpp
…
// создание promise
// инициализация future
promise<int> smProm;
shared_future<int> smFutu = smProm.get_future().shared();
…
// запуск потоков
thread worThr(worker, val, ref(smProm));
thread firThr(firWorker, smFutu);
thread secThr(secWorker, smFutu);
…
// ожидаем выполнение 
worThr.join();
firThr.join();
secThr.join();
…
```
Так как `future` не позволяет нам вызывать метод `get` более одного раза, можно воспользоваться его разделяемым аналогом. Отличие в том, что шаред реализует семантику копирования и позволяет нам вызывать методы одного `future` в разных потоках.