#include "gtest/gtest.h"
#include "../lib/scheduler.h"
#include <string>
#include <tuple>
#include <stdexcept>
#include <utility>

TEST(SchedulerTest, TaskReturnsTuple) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return std::make_tuple(1, 2.0, std::string("test")); });

    scheduler.executeAll();
    auto result = scheduler.getResult<std::tuple<int, double, std::string>>(id);

    EXPECT_EQ(std::get<0>(result), 1);
    EXPECT_EQ(std::get<1>(result), 2.0);
    EXPECT_EQ(std::get<2>(result), "test");
}

TEST(SchedulerTest, TaskReturnsPair) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return std::make_pair(5, std::string("pair")); });

    scheduler.executeAll();
    auto result = scheduler.getResult<std::pair<int, std::string>>(id);

    EXPECT_EQ(result.first, 5);
    EXPECT_EQ(result.second, "pair");
}

TEST(SchedulerTest, FutureResultImplicitCast) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return 42; });

    scheduler.executeAll();

    TTaskScheduler::FutureResult<int> future = scheduler.getFutureResult<int>(id);
    int result = future;

    EXPECT_EQ(result, 42);
}

TEST(SchedulerTest, FutureExecutesLazily) {
    TTaskScheduler scheduler;

    bool ran = false;
    auto id = scheduler.add([&ran]() { ran = true; return 77; });

    auto result = scheduler.getFutureResult<int>(id);
    EXPECT_FALSE(scheduler.isExecuted(id));

    int val = result;
    EXPECT_TRUE(scheduler.isExecuted(id));
    EXPECT_EQ(val, 77);
    EXPECT_TRUE(ran);
}

TEST(SchedulerTest, MultipleTasksIndependent) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return 10; });
    auto id2 = scheduler.add([]() { return 20; });

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 10);
    EXPECT_EQ(scheduler.getResult<int>(id2), 20);
}

TEST(SchedulerTest, GetResultIsConsistent) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([]() { return 100; });
    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id), 100);
    EXPECT_EQ(scheduler.getResult<int>(id), 100);
}

TEST(SchedulerTest, TaskThrowsException) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([]() -> int {
        throw std::runtime_error("oops");
    });

    EXPECT_THROW(scheduler.getResult<int>(id), std::runtime_error);
}

TEST(SchedulerTest, LongComputation) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([]() {
        int sum = 0;
        for (int i = 1; i <= 1000; ++i) sum += i;
        return sum;
    });

    scheduler.executeAll();
    EXPECT_EQ(scheduler.getResult<int>(id), 500500);
}

TEST(SchedulerTest, ResultWithoutExecuteDefaultBehavior) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return 99; });

    EXPECT_EQ(scheduler.getResult<int>(id), 99);
}

TEST(SchedulerTest, LambdaWithCapture) {
    TTaskScheduler scheduler;

    int a = 3, b = 4;
    auto id = scheduler.add([=]() { return a * b; });

    scheduler.executeAll();
    EXPECT_EQ(scheduler.getResult<int>(id), 12);
}
