#include "async.h"
#include "processor/utils.h"

namespace async {

    handle_t connect(std::size_t bulk) {
        handle_t bulk_handle;
        bulk_handle = implementation::PackManagerAsync::get_router().create_new_pma(bulk);
        return bulk_handle;
    }

    void receive(handle_t handle, const char* data, std::size_t size) {
        implementation::PackManagerAsync::get_router().run_pma(handle, data, size);
    }

    void disconnect(handle_t handle) {
        implementation::PackManagerAsync::get_router().remove_pma(handle);
    }

    void reserve_threads_for_tasks(std::size_t num_threads) {
        implementation::PackManagerAsync::get_router().reserve_threads(num_threads);
    }

    void stop_all_tasks() {
        implementation::PackManagerAsync::get_router().stop_thread_pool();
    }

    void resume_tasks() {
        implementation::PackManagerAsync::get_router().resume_thread_pool();
    }
}

namespace async::implementation {
    PackManagerAsync::PackManagerAsync() {
        auto hardware_threads = std::thread::hardware_concurrency();
        if (hardware_threads > 0)
            thread_pool.configure_threads(hardware_threads);
        else
            thread_pool.configure_threads(1);
    }

    handle_t PackManagerAsync::create_new_pma(std::size_t bulk) {
        auto processor = new PackManager(bulk);
        std::shared_ptr<PackManager> processor_shared(processor);
        {
            std::lock_guard<std::mutex> guard{ inner_data_mutex };
            handles[processor] = processor_shared;
        }
        return processor;
    }

    void PackManagerAsync::run_pma(handle_t handle, const char* data, std::size_t size) {
        std::lock_guard<std::mutex> guard{ inner_data_mutex };
        if (handles.count(handle) > 0) {
            auto processor = handles[handle];
            std::string data_str(data, size);
            thread_pool.add_task([processor, data_str] { 
                processor->getData(data_str); 
            });
        }
    }

    void PackManagerAsync::remove_pma(handle_t handle) {
        std::lock_guard<std::mutex> guard{ inner_data_mutex };
        if (handles.count(handle) > 0) {
            handles.erase(handle);
        }
    }

    void PackManagerAsync::reserve_threads(std::size_t num_threads) {
        if (num_threads > 0) {
            thread_pool.configure_threads(num_threads);
        }
    }

    void PackManagerAsync::stop_thread_pool() {
        thread_pool.suspend_work();
    }

    void PackManagerAsync::resume_thread_pool() {
        thread_pool.resume_work();
    }
}