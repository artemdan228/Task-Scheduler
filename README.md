# Шедулер задач (TTaskScheduler) — C++ Pet Project

## Описание

Консольное приложение для управления зависимыми задачами с ленивым вычислением результатов.  
Программа позволяет создавать граф вычислений, где каждая задача может зависеть от результата другой задачи.  
Выполняются только необходимые задачи, а повторные вычисления одного и того же результата не происходят.

Пример практического применения: **нахождение корней квадратного уравнения** с использованием связанных вычислений.  

---

## Функциональность

- Добавление задач с использованием лямбд, функций или указателей на методы классов  
- Поддержка до 2 аргументов на задачу  
- Ленивое вычисление результатов через `FutureResult`  
- Получение результатов через `getResult`  
- Выполнение всех задач через `executeAll`  
- Автоматическое использование результатов предыдущих задач для зависимых вычислений  

---

## Требования

- **C++17** или новее  
- Стандартная библиотека (контейнеры, умные указатели, std::any, std::function)  
- Google Test для проверки работы шедулера  

---

## Сборка и запуск

### Сборка проекта

```bash
# Клонирование репозитория
git clone <repository-url>
cd <project-directory>

# Создание папки для сборки
mkdir build
cd build

# Конфигурация и сборка
cmake ..
make
````

### Запуск примера

```bash
./task_scheduler_example
```

---

## Использование

Пример использования для решения квадратного уравнения:

```cpp
struct AddNumber {
    float add(float a) const { return a + number; }
    float number;
};

float a = 1;
float b = -2;
float c = 0;
AddNumber add{.number = 3};

TTaskScheduler scheduler;

auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);
auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));
auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));
auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

scheduler.executeAll();

std::cout << "x1 = " << scheduler.getResult<float>(id5) << std::endl;
std::cout << "x2 = " << scheduler.getResult<float>(id6) << std::endl;
std::cout << "x3 = " << scheduler.getResult<float>(id7) << std::endl;
```

---

## Основные компоненты

* `TTaskScheduler` — главный класс шедулера
* `TaskWrapper<R>` — шаблонный класс для хранения задачи и её результата
* `FutureResult<T>` — объект для ленивого получения результата задачи
* `TaskId` — уникальный идентификатор задачи

---

## Тесты

Проект покрыт тестами с использованием Google Test:

* Проверка работы с различными типами возвращаемых значений (tuple, pair, int, float, string)
* Ленивое выполнение задач через `FutureResult`
* Проверка правильного вычисления зависимых задач
* Проверка повторного вызова `getResult`
* Проверка выброса исключений

---

## Особенности реализации

* Использование **type erasure** для хранения задач с разными типами возвращаемых значений
* Ленивое вычисление и кеширование результата каждой задачи
* Поддержка **Rvalue references** и **std::forward** для корректной передачи аргументов
* Простая интеграция с указателями на методы классов

---

## Идеи для развития

* Асинхронное выполнение задач в нескольких потоках
* Поддержка задач с большим количеством аргументов
* Визуализация графа зависимостей задач
* Оптимизация памяти для больших графов вычислений

---

Проект создан в учебных целях.

---
