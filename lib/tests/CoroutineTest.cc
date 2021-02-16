#include <drogon/utils/coroutine.h>
#include <exception>
#include <type_traits>
#include <iostream>

using namespace drogon;

int main()
{
    // Basic checks making sure coroutine works as expected
    static_assert(is_awaitable_v<Task<>>);
    static_assert(is_awaitable_v<Task<int>>);
    static_assert(std::is_same_v<await_result_t<Task<int>>, int>);
    static_assert(std::is_same_v<await_result_t<Task<>>, void>);
    static_assert(is_awaitable_v<Task<>>);
    static_assert(is_awaitable_v<Task<int>>);

    // No, you cannot await AsyncTask. By design
    static_assert(is_awaitable_v<AsyncTask> == false);

    // Make sure sync_wait works
    if (sync_wait([]() -> Task<int> { co_return 1; }()) != 1)
    {
        std::cerr << "Expected coroutine return 1. Didn't get that\n";
        exit(1);
    }

    // co_future converts coroutine into futures
    auto fut = co_future([]() -> Task<std::string> { co_return "zxc"; }());
    if (fut.get() != "zxc")
    {
        std::cerr << "Expected future return \'zxc\'. Didn't get that\n";
        exit(1);
    }

    // Testing that exceptions can propergate through coroutines
    auto throw_in_task = []() -> Task<> {
        auto f = []() -> Task<> { throw std::runtime_error("test error"); };

        try
        {
            f();
            std::cerr << "Exception should have been thrown\n";
            exit(1);
        }
        catch (const std::exception& e)
        {
            if (std::string(e.what()) != "test error")
            {
                std::cerr << "Not the right exception\n";
                exit(1);
            }
        }
        catch (...)
        {
            std::cerr << "Shouldn't reach here\n";
            exit(1);
        }
        co_return;
    };
    sync_wait(throw_in_task());

    std::cout << "Done testing coroutines. No error." << std::endl;
}
