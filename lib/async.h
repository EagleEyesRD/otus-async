#pragma once

#include <cstddef>
#include <unordered_map>
#include <mutex>
#include "processor/utils.h"
#include "processor/threadpool.h"
#include "processor/logger.h"

namespace async {

    using handle_t = void *;

    handle_t connect(std::size_t bulk);

    void receive(handle_t handle, const char *data, std::size_t size);

    void disconnect(handle_t handle);

    void reserve_threads_for_tasks(std::size_t num_threads);

    void stop_all_tasks();

}


namespace async::implementation {
    class PackManagerAsync {
        std::vector<std::string> statpack;
        std::vector<std::string> dynampack;
    public:
        static PackManagerAsync& get_router() {
            static PackManagerAsync router;
            return router;
        }

        handle_t create_new_pma(std::size_t bulk);
        void remove_pma(handle_t handle);
        void run_pma(handle_t handle, const char* data, std::size_t size);
        void reserve_threads(std::size_t num_threads);
        void stop_thread_pool();
        void resume_thread_pool();
        
    private:
        PackManagerAsync();
        std::unordered_map<handle_t, std::shared_ptr<PackManager>> handles;
        std::mutex inner_data_mutex;
        ThreadPool thread_pool;
    };
}