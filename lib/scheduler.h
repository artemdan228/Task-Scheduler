#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <any>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <cmath>
#include <iostream>

class TaskId {
public:
    explicit TaskId(int id = -1) : id_(id) {}
    bool operator==(const TaskId& other) const { return id_ == other.id_; }
    bool operator!=(const TaskId& other) const { return !(*this == other); }
    int getId() const { return id_; }

private:
    int id_;
};

namespace std {
    template<>
    struct hash<TaskId> {
        std::size_t operator()(const TaskId& id) const noexcept {
            return std::hash<int>()(id.getId());
        }
    };
}

class ITask {
public:
    virtual ~ITask() = default;
    virtual void execute() = 0;
    virtual std::any getResult() = 0;
    virtual bool isExecuted() const = 0;
};

template<typename R>
class TaskWrapper : public ITask {
public:
    using ResultType = R;

    template<typename Func, typename... Args>
    TaskWrapper(Func&& func, Args&&... args)
        : func_([f = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
            return std::invoke(f, std::forward<Args>(args)...);
        }), executed_(false) {}

    void execute() override {
        if (!executed_) {
            result_ = func_();
            executed_ = true;
        }
    }

    std::any getResult() override {
        if (!executed_) execute();
        return result_;
    }

    bool isExecuted() const override {
        return executed_;
    }

private:
    std::function<R()> func_;
    R result_{};
    bool executed_;
};

class TTaskScheduler {
public:
    TTaskScheduler() : nextId_(0) {}

    template<typename Func, typename... Args>
    TaskId add(Func&& func, Args&&... args) {
        using R = decltype(std::invoke(std::forward<Func>(func), std::forward<Args>(args)...));

        auto task = std::make_shared<TaskWrapper<R>>(std::forward<Func>(func), std::forward<Args>(args)...);
        TaskId id{nextId_++};
        tasks[id] = std::move(task);
        return id;
    }

    template<typename T>
    class FutureResult {
    public:
        FutureResult(TaskId id, TTaskScheduler* scheduler)
            : id_(id), scheduler_(scheduler) {}

        operator T() const {
            return scheduler_->getResult<T>(id_);
        }

    private:
        TaskId id_;
        TTaskScheduler* scheduler_;
    };

    template<typename T>
    FutureResult<T> getFutureResult(TaskId id) {
        return FutureResult<T>(id, this);
    }

    template<typename T>
    T getResult(TaskId id) {
        auto task = getTask(id);
        return std::any_cast<T>(task->getResult());
    }

    bool isExecuted(TaskId id) const {
        auto it = tasks.find(id);
        if (it == tasks.end()) throw std::runtime_error("Invalid task id");
        return it->second->isExecuted();
    }

    void executeAll() {
        for (auto& [id, task] : tasks) {
            if (!task->isExecuted()) {
                task->execute();
            }
        }
    }

private:
    ITask* getTask(TaskId id) {
        auto it = tasks.find(id);
        if (it == tasks.end()) throw std::runtime_error("Task not found");
        return it->second.get();
    }

    const ITask* getTask(TaskId id) const {
        auto it = tasks.find(id);
        if (it == tasks.end()) throw std::runtime_error("Task not found");
        return it->second.get();
    }

    std::unordered_map<TaskId, std::shared_ptr<ITask>> tasks;
    int nextId_;
};
