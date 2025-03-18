//
// Created by FCWY on 25-3-6.
//

#ifndef SDL_ASYNCIO_HPP
#define SDL_ASYNCIO_HPP
#include <SDL3/SDL_asyncio.h>
#include <filesystem>

namespace SDL::AsyncIO{
    enum class TaskType: UnderlyingType {
        READ = SDL_ASYNCIO_TASK_READ,
        WRITE = SDL_ASYNCIO_TASK_WRITE,
        CLOSE = SDL_ASYNCIO_TASK_CLOSE
    };
    constexpr SDL_AsyncIOTaskType legacy(const TaskType type) noexcept {
        return static_cast<SDL_AsyncIOTaskType>(type);
    }

    enum class Result: UnderlyingType {
        COMPLETE = SDL_ASYNCIO_COMPLETE,
        FAILURE = SDL_ASYNCIO_FAILURE,
        CANCELED = SDL_ASYNCIO_CANCELED
    };
    constexpr SDL_AsyncIOResult legacy(const Result type) noexcept {
        return static_cast<SDL_AsyncIOResult>(type);
    }

    struct Outcome {
        SDL_AsyncIO*
            asyncio;
        TaskType
            type;
        Result
            result;
        void*
            v_buffer;
        Uint64
            offset;
        Uint64
            bytes_requested;
        Uint64
            bytes_transferred;
        void*
            v_userdata;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        Outcome(const Outcome& underly) noexcept:
            asyncio{underly.asyncio},
            type{underly.type},
            result{underly.result},
            v_buffer{underly.buffer},
            offset{underly.offset},
            bytes_requested{underly.bytes_requested},
            bytes_transferred{underly.bytes_transferred},
            v_userdata{underly.userdata}{}

        // ReSharper disable once CppNonExplicitConversionOperator
        operator SDL_AsyncIOOutcome () noexcept {
            return {
                asyncio,
                static_cast<SDL_AsyncIOTaskType>(static_cast<UnderlyingType>(type)),
                static_cast<SDL_AsyncIOResult>(static_cast<UnderlyingType>(result)),
                v_buffer,
                offset,
                bytes_requested,
                bytes_transferred,
                v_userdata
            };
        }

        template<typename T>
        T* buffer() const noexcept {
            return static_cast<T*>(buffer);
        }

        template<typename T>
        T* userdata() const noexcept {
            return static_cast<T*>(userdata);
        }

        Outcome& operator = (const SDL_AsyncIOOutcome& sdl_async_io_outcome) {
            asyncio = sdl_async_io_outcome.asyncio;
            type = static_cast<TaskType>(sdl_async_io_outcome.type);
            result = static_cast<Result>(sdl_async_io_outcome.result);
            v_buffer = sdl_async_io_outcome.buffer;
            offset = sdl_async_io_outcome.offset;
            bytes_requested = sdl_async_io_outcome.bytes_requested;
            bytes_transferred = sdl_async_io_outcome.bytes_transferred;
            v_userdata = sdl_async_io_outcome.userdata;
            return *this;
        }
    };

    struct TaskQueue {
        using handle_t = SDL_AsyncIOQueue*;

        std::atomic_uint64_t*
            ref_count{nullptr};
        handle_t
            handle{nullptr};

        TaskQueue() noexcept:
            ref_count{new std::atomic_uint64_t{1}},
            handle{SDL_CreateAsyncIOQueue()}{
            // ReSharper disable once CppDFAConstantConditions
            if (handle == nullptr)
                // ReSharper disable once CppDFAUnreachableCode
                throw Error{};
        }

        TaskQueue(const TaskQueue& other) noexcept:
            ref_count{other.ref_count},
            handle{other.handle}{
            ++ *ref_count;
        }
        TaskQueue(TaskQueue&& expired) noexcept{
            std::swap(ref_count, expired.ref_count);
            std::swap(handle, expired.handle);
            ++ *ref_count;
        }

        // ReSharper disable CppMemberFunctionMayBeConst
        bool get(Outcome& result) noexcept {
            SDL_AsyncIOOutcome o;
            const auto ret = SDL_GetAsyncIOResult(handle, &o);
            result = o;
            return ret;
        }

        bool wait(Outcome& result, const std::chrono::milliseconds timeout) noexcept {
            SDL_AsyncIOOutcome o;
            const auto ret = SDL_WaitAsyncIOResult(handle, &o, timeout.count());
            result = o;
            return ret;
        }

        bool wait(Outcome& result) noexcept {
            SDL_AsyncIOOutcome o;
            const auto ret = SDL_WaitAsyncIOResult(handle, &o, -1);
            result = o;
            return ret;
        }

        void signal() noexcept {
            return SDL_SignalAsyncIOQueue(handle);
        }

        bool LoadFileAsync(
            const std::filesystem::path& file,
            void* userdata) noexcept {
            return SDL_LoadFileAsync(
                reinterpret_cast<const char*>(file.generic_u8string().c_str()),
                handle,
                userdata);
        }
        // ReSharper restore CppMemberFunctionMayBeConst

        ~TaskQueue() {
            if (ref_count == nullptr)
                return;

            -- *ref_count;
            if (*ref_count == 0) {
                delete ref_count;
                if (handle != nullptr)
                    SDL_DestroyAsyncIOQueue(handle);
            }
        }
    };

    struct Task {
        using handle_t = SDL_AsyncIO*;

        handle_t
            handle{nullptr};
        TaskQueue
            bind_queue;
        bool
            auto_buffer;
        std::byte*
            buffer;

        Task(const TaskQueue& bind_queue, const std::size_t buf_size, const handle_t handle=nullptr) noexcept:
            handle{handle},
            bind_queue{bind_queue},
            auto_buffer{true},
            buffer{new std::byte[buf_size]{}}{}

        Task(const TaskQueue& bind_queue, std::unique_ptr<std::byte[]> buffer, const handle_t handle=nullptr) noexcept:
            handle{handle},
            bind_queue{bind_queue},
            auto_buffer{true},
            buffer{buffer.release()}{}

        template<typename T, std::size_t N>
        Task(const TaskQueue& bind_queue, T (&buffer)[N], const handle_t handle=nullptr) noexcept:
            handle{handle},
            bind_queue{bind_queue},
            auto_buffer{false},
            buffer{buffer}{}


        Task(const Task& other) = delete;
        Task& operator=(const Task& other) = delete;

        Task(Task&& other) noexcept:
            bind_queue{other.bind_queue}{
            std::swap(handle, other.handle);
            std::swap(auto_buffer, other.auto_buffer);
            std::swap(buffer, other.buffer);
        }
        Task& operator = (Task&& other) noexcept {
            std::swap(handle, other.handle);
            std::swap(auto_buffer, other.auto_buffer);
            std::swap(buffer, other.buffer);
            return *this;
        }

        Sint64 size() const {
            const auto ret = SDL_GetAsyncIOSize(handle);
            if (ret < 0)
                throw Error{};
            return ret;
        }

        void clear() noexcept {
            if (auto_buffer)
                delete[] buffer;
            auto_buffer = false;
        }
        // ReSharper disable once CppParameterMayBeConstPtrOrRef
        void rebind(TaskQueue& queue) noexcept {
            bind_queue = queue;
        }

        template<std::size_t N>
        void rebind(std::byte (&buffer)[N]) noexcept {
            clear();
            this -> buffer = buffer;
        }

        void rebind(std::unique_ptr<std::byte[]> buffer) noexcept {
            clear();
            this -> buffer = buffer.release();
            auto_buffer = true;
        }

        [[nodiscard]]
        std::unique_ptr<std::byte[]>
            pick() noexcept {
            if (!auto_buffer)
                return nullptr;
            auto ptr = std::unique_ptr<std::byte[]>{
                buffer
            };
            auto_buffer = false;
            return ptr;
        }



        template<typename U>
        bool read(Uint64 rd_offset, Uint64 rd_size, U* userdata=nullptr) noexcept {
            return SDL_ReadAsyncIO(handle, static_cast<void*>(buffer), rd_offset, rd_size, static_cast<void*>(userdata));
        }

        template<typename U>
        bool write(Uint64 rd_offset, Uint64 rd_size, U* userdata=nullptr) noexcept {
            return SDL_WriteAsyncIO(handle, static_cast<void*>(buffer), rd_offset, rd_size, static_cast<void*>(userdata));
        }


        ~Task() noexcept {
            if (auto_buffer)
                delete[] buffer;
            if (handle)
                SDL_CloseAsyncIO(handle, true, bind_queue.handle, nullptr);
        }

        enum class OpenMode {
            READ,
            WRITE,
            READPLUS,
            WRITEPLUS,
        };
        static handle_t fromFile(
            const std::filesystem::path& file,
            const OpenMode mode
            ) noexcept {
            using enum OpenMode;

            handle_t ret{nullptr};

            switch (mode) {
            case READ:
                ret = SDL_AsyncIOFromFile(
                    reinterpret_cast<const char*>(file.generic_u8string().c_str()),
                    "r"
                    );
                break;
            case WRITE:
                ret = SDL_AsyncIOFromFile(
                    reinterpret_cast<const char*>(file.generic_u8string().c_str()),
                    "w"
                    );
                break;
            case READPLUS:
                ret = SDL_AsyncIOFromFile(
                    reinterpret_cast<const char*>(file.generic_u8string().c_str()),
                    "r+"
                    );
                break;
            case WRITEPLUS:
                ret = SDL_AsyncIOFromFile(
                    reinterpret_cast<const char*>(file.generic_u8string().c_str()),
                    "w+"
                    );
                break;
            }

            if (ret == nullptr)
                throw Error{};

            return ret;
        }
    };
}

#endif //SDL_ASYNCIO_HPP
