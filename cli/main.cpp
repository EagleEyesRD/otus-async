#include <iostream>
#include "lib/async.h"

int main(int, char *[]) {
    async::reserve_threads_for_tasks(2);
    std::size_t bulk = 5;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);

    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);
    async::disconnect(h);
    async::disconnect(h2);

    Logger::get_logger().suspend_work();
    async::stop_all_tasks();
    Logger::get_logger().resume_work();


    return 0;
}
