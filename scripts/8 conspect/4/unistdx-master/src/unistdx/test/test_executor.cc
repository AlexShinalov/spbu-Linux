/*
UNISTDX — C++ library for Linux system calls.
© 2020, 2021 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <iostream>
#include <unordered_map>

#include <unistdx/io/event_file_descriptor>
#include <unistdx/io/pipe>
#include <unistdx/io/terminal>
#include <unistdx/net/network_interface>
#include <unistdx/system/error>
#include <unistdx/system/resource>
#include <unistdx/test/arguments>
#include <unistdx/test/test_executor>

#if defined(UNISTDX_HAVE_BACKTRACE)
#include <execinfo.h>
#endif

thread_local sys::test::Test* sys::test::current_test{};

namespace {

    sys::test::Backtrace_thread* backtrace_thread_ptr{};
    std::atomic_flag entered{ATOMIC_FLAG_INIT};

    void backtrace_on_signal_static(int sig, sys::signal_information* info, void*) noexcept {
        // prevent recursive calls
        if (entered.test_and_set()) { std::_Exit(sig); }
        if (info->process_id() != sys::this_process::id()) {
            sys::print(std::cerr, to_string(sys::signal(sig)), sys::stack_trace());
            std::_Exit(sig);
        } else {
            if (!backtrace_thread_ptr) {
                sys::backtrace_on_signal(sig);
                std::_Exit(sig);
            }
            backtrace_thread_ptr->capture_backtrace(sig);
        }
        entered.clear();
    }

}

void sys::test::Symbol::demangled_name(std::string&& rhs) {
    this->_demangled_name = std::move(rhs);
    auto pos1 = this->_demangled_name.find('(');
    auto pos2 = this->_demangled_name.find('[');
    auto pos = std::string::npos;
    if (pos1 != std::string::npos && pos2 != std::string::npos) {
        pos = std::min(pos1,pos2);
        this->_short_name = this->_demangled_name.substr(0,std::min(pos1,pos2));
    } else if (pos1 != std::string::npos) {
        pos = pos1;
    } else if (pos2 != std::string::npos) {
        pos = pos2;
    }
    this->_short_name = this->_demangled_name.substr(0,pos);
}

void sys::test::Backtrace_thread::run(Test* t) {
    thread = std::thread([t,this] () {
        using sys::test::current_test;
        current_test = t;
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this,&lock] () -> bool {
            if (!backtrace.empty()) {
                sys::string buf(4096);
                std::stringstream tmp;
                sys::print(tmp, to_string(signal), backtrace);
                current_test->status(sys::test::Test::Status::Exception);
                current_test->exception_text(tmp.str());
                current_test->child_write_status(std::clog);
                lock.unlock();
                std::_Exit(int(signal));
            }
            return stopped;
        });
    });
}

void sys::test::Backtrace_thread::capture_backtrace(int sig) {
    mutex.lock();
    backtrace = sys::stack_trace();
    signal = sys::signal(sig);
    stopped = true;
    mutex.unlock();
    cv.notify_one();
    join();
    std::_Exit(sig);
}

int sys::test::Test::run() {
    if (this->_arguments.address()) {
        using argument_array = ::sys::test::arguments<void*>;
        argument_array* args{};
        switch (this->_arguments.type()) {
            case elf::symbol_types::code:
                args = this->_arguments.call<argument_array*>();
                break;
            case elf::symbol_types::data:
                args = reinterpret_cast<argument_array*>(this->_arguments.address());
                break;
            default:
                throw std::invalid_argument("unknown args type");
                break;
        }
        if (!args) {
            throw std::invalid_argument("unable to find arguments");
        }
        int num_finished = 0;
        auto& raw_data = args->raw_data();
        int num_args = raw_data.size();
        for (const auto& a : raw_data) {
            bool success = this->_function.apply<bool>(a);
            ++num_finished;
            std::stringstream message;
            message << '[' << num_finished << '/' << num_args << "] ";
            using namespace sys::terminal;
            message << bold();
            if (success) {
                message << color(colors::fg_green) << "ok";
            } else {
                message << color(colors::fg_red) << "fail";
            }
            message << reset();
            message << ' ' << this->_function.short_name() << '('
                << this->_arguments.short_name() << '[' << num_finished-1 << "])";
            message << '\n';
            std::clog << message.str() << std::flush;
        }
    } else {
        this->_function.call<bool>();
        using namespace sys::terminal;
        std::stringstream message;
        message << bold();
        if (this->_status == Status::Success) {
            message << color(colors::fg_green) << "ok";
        } else {
            message << color(colors::fg_red) << "fail";
        }
        message << reset();
        message << ' ' << this->_function.short_name() << '\n';
        std::clog << message.str() << std::flush;
    }
    /*
    // serialise the result
    sys::byte_buffer buf;
    auto old_position = buf.position();
    write(buf);
    auto new_position = buf.position();
    buf.write(uint64_t(new_position-old_position));
    std::clog.write(buf.data(), buf.position());
    */
    child_write_status(std::clog);
    return this->_status == Status::Success ? 0 : 1;
}

void sys::test::Test::record_error(error err) {
    this->_status = Status::Exception;
    ++this->_num_errors;
    std::clog << err.what() << std::flush;
}

void sys::test::Test::record_process_status(sys::process_status ps) {
    exit_code(ps.exit_code());
    switch (ps.exit_code()) {
        case 0: status(Test::Status::Success); break;
        case 1: status(Test::Status::Exception); break;
        case 77: status(Test::Status::Skipped); break;
        default: signal(ps.term_signal());
                 status(Test::Status::Killed);
                 break;
    }
}

void sys::test::Test::write(sys::byte_buffer& out) const {
    out.write(this->_status);
    out.write(this->_exception_type);
    out.write(this->_exception_text);
}

void sys::test::Test::read(sys::byte_buffer& in) {
    in.read(this->_status);
    in.read(this->_exception_type);
    in.read(this->_exception_text);
}

void sys::test::Test::child_write_status(std::ostream& out) const {
    out << "Function: " << this->_function.short_name() << '\n';
    out << "Arguments: " << this->_arguments.short_name() << '\n';
    out << "Status: " << this->_status << '\n';
    switch (this->_status) {
        case Status::Success:
            break;
        case Status::Exception:
            out << "Exception type: " << this->_exception_type << '\n';
            out << "Exception text: " << this->_exception_text << '\n';
            break;
        case Status::Unknown_exception:
            out << "Exception type: unknown\n";
            out << "Exception text: unknown\n";
            break;
        default:
            break;
    }
    if (this->_arguments.type() == elf::symbol_types::code) {
        out << "Seeds:";
        for (auto seed : this->_seeds) { out << ' ' << seed; }
        out << '\n';
    }
    out.flush();
}

void sys::test::Test::parent_write_status(std::ostream& out) const {
    if (this->_signal != sys::signal{}) {
        out << "Termination signal: " << this->_signal << '\n';
    }
    out << "Exit code: " << this->_exit_code << '\n';
}

void sys::test::Test_output::write_status(std::ostream& out) const {
    out.write(output.data(), output.position());
}

const char* sys::test::to_string(Test::Status rhs) noexcept {
    switch (rhs) {
        case Test::Status::Success: return "success";
        case Test::Status::Exception: return "exception";
        case Test::Status::Unknown_exception: return "unknown-exception";
        case Test::Status::Killed: return "killed";
        case Test::Status::Skipped: return "skipped";
        default: return nullptr;
    }
}

std::ostream& sys::test::operator<<(std::ostream& out, Test::Status rhs) {
    auto* s = to_string(rhs);
    return out << (s ? s : "unknown");
}

int sys::test::Test_executor::run() {
    size_t num_tests = this->_tests.size();
    size_t num_finished = 0;
    std::unordered_map<sys::pid_type,Test_output> tests_by_process;
    std::unordered_map<sys::fd_type,sys::pid_type> process_by_fd;
    tests_by_process.reserve(this->_tests.size());
    int exit_code = 0;
    const auto max_threads = 1;//sys::thread_concurrency();
    /*
       {
       auto n = this->_tests.size();
       for (size_t i=0; i<n; ++i) {
       Test test = std::move(this->_tests.front());
       this->_tests.pop_front();
       current_test = &test;
       std::clog << "test.symbol().short_name()=" << test.symbol().short_name() << std::endl;
       try {
       test.run();
       } catch (const std::exception& err) {
       std::clog << "err=" << err.what() << std::endl;
       }
       }
       }
       return 0;
       */
    for (; (!this->_child_processes.empty() || !this->_tests.empty()); ) {
        if (this->_child_processes.size() < max_threads && !this->_tests.empty()) {
            auto n = std::min(max_threads - this->_child_processes.size(),
                              this->_tests.size());
            for (size_t i=0; i<n; ++i) {
                Test test = std::move(this->_tests.front());
                this->_tests.pop_front();
                sys::pipe stderr;
                stderr.out().unsetf(sys::open_flag::non_blocking);
                sys::event_file_descriptor notifier(
                    0, sys::event_file_descriptor::flag::close_on_exec);
                this->_child_processes.emplace_back([this,&test,&stderr,&notifier]() noexcept {
                    if (bool(this->_process_flags & sys::process::flags::unshare_users)) {
                        notifier.read();
                    }
                    notifier.close();
                    //using f = sys::unshare_flag;
                    //sys::this_process::unshare(f::network | f::users);
                    this->_poller.close();
                    if (bool(this->_process_flags & sys::process::flags::unshare_network)) {
                        sys::network_interface lo("lo");
                        lo.setf(sys::network_interface::flag::up);
                    }
                    current_test = &test;
                    std::clog.iword(0) = sys::is_a_terminal(STDERR_FILENO);
                    stderr.in().close();
                    if (redirect()) {
                        sys::fildes out(STDOUT_FILENO);
                        out = stderr.out();
                        out.release();
                    }
                    if (redirect()) {
                        sys::fildes err(STDERR_FILENO);
                        err = stderr.out();
                        err.release();
                    }
                    Backtrace_thread backtrace_thread;
                    if (catch_errors()) {
                        using sys::this_process::bind_signal;
                        using sys::this_process::ignore_signal;
                        using s = sys::signal;
                        bind_signal(s::segmentation_fault, backtrace_on_signal_static);
                        bind_signal(s::bad_memory_access, backtrace_on_signal_static);
                        bind_signal(s::keyboard_interrupt, backtrace_on_signal_static);
                        bind_signal(s::abort, backtrace_on_signal_static);
                        ignore_signal(s::broken_pipe);
                        std::set_terminate([] () {
                            if (auto ptr = std::current_exception()) {
                                sys::string buf(4096);
                                try {
                                    std::rethrow_exception(ptr);
                                } catch (const error& err) {
                                    current_test->status(Test::Status::Exception);
                                    current_test->exception_type(sys::demangle(typeid(err).name(), buf));
                                    current_test->exception_text(err.what());
                                } catch (const std::exception& err) {
                                    current_test->status(Test::Status::Exception);
                                    current_test->exception_type(sys::demangle(typeid(err).name(), buf));
                                    current_test->exception_text(error(err.what()).what());
                                } catch (...) {
                                    current_test->status(Test::Status::Unknown_exception);
                                    current_test->exception_type("unknown");
                                    current_test->exception_text(error("Uncaught exception").what());
                                }
                            } else {
                                current_test->status(Test::Status::Unknown_exception);
                                current_test->exception_type("unknown");
                                current_test->exception_text(error("Uncaught exception").what());
                            }
                            current_test->child_write_status(std::clog);
                            std::_Exit(1);
                        });
                        backtrace_thread_ptr = &backtrace_thread;
                        backtrace_thread.run(current_test);
                    }
                    auto ret = current_test->run();
                    if (catch_errors()) {
                        backtrace_thread.stop();
                        backtrace_thread.join();
                    }
                    return ret;
                }, this->_process_flags, 4096*512);
                auto& process = this->_child_processes.back();
                if (bool(this->_process_flags & sys::process::flags::unshare_users)) {
                    process.init_user_namespace();
                    notifier.write(1);
                }
                stderr.out().close();
                this->_poller.emplace(stderr.in().fd(), sys::event::in);
                process_by_fd[stderr.in().fd()] = process.id();
                tests_by_process[process.id()] =
                    Test_output{Clock::now(), std::move(test),std::move(stderr.in())};
            }
        }
        using namespace std::chrono;
        bool finished = false;
        while (!finished) {
            this->_poller.wait_for(this->_mutex, milliseconds(99), [&] () {
                finished = false;
                for (const auto& event : this->_poller) {
                    if (event.fd() == this->_poller.pipe_in()) { continue; }
                    auto result = process_by_fd.find(event.fd());
                    if (result == process_by_fd.end()) { continue; }
                    auto process_id = result->second;
                    auto result2 = tests_by_process.find(process_id);
                    if (result2 == tests_by_process.end()) { continue; }
                    auto& output = result2->second;
                    if (event.in()) {
                        auto fd = event.fd();
                        output.output.fill(fd);
                    }
                    if (!event) {
                        exit_code |= print_child_process_status(
                            process_id, output, num_finished, num_tests);
                        process_by_fd.erase(result);
                        // TODO uncommenting this somehow affects pipe descriptor
                        //tests_by_process.erase(result2);
                        finished = true;
                    }
                }
                for (auto& child : this->_child_processes) {
                    const auto& output = tests_by_process[child.id()];
                    if (this->_timeout < Clock::now()-output.start_time) {
                        child.terminate();
                    }
                }
                return finished;
            });
        }
    }
    return exit_code;
}

int sys::test::Test_executor::print_child_process_status(sys::pid_type process_id,
                                                         Test_output& output,
                                                         size_t& num_finished,
                                                         size_t num_tests) {
    int exit_code = 0;
    auto result = std::find_if(
        this->_child_processes.begin(), this->_child_processes.end(),
        [process_id] (const sys::process& p) { return process_id == p.id(); });
    auto& process = *result;
    auto status = process.wait();
    auto& t = output.test;
    t.record_process_status(status);
    if (status.exit_code() != 77) { exit_code |= status.exit_code(); }
    {
        ++num_finished;
        using namespace sys::terminal;
        std::stringstream message;
        message.iword(0) = sys::is_a_terminal(STDERR_FILENO);
        message << '[' << num_finished << '/' << num_tests << "] ";
        message << bold();
        switch (t.status()) {
            case Test::Status::Success:
                message << color(colors::fg_green);
                break;
            case Test::Status::Skipped:
                message << color(colors::fg_yellow);
                break;
            default:
                message << color(colors::fg_red);
                break;
        }
        message << t.status();
        message << reset();
        message << ' ' << t.symbol().short_name();
        if (!this->_tests.empty()) {
            message << " (next is " << this->_tests.front().symbol().short_name()
                << ')';
        }
        message << '\n';
        std::clog << message.str() << std::flush;
    }
    if ((t.status() != Test::Status::Success &&
        t.status() != Test::Status::Skipped) || this->_verbose) {
        std::stringstream message;
        message << "======== Output ======== \n";
        output.write_status(message);
        t.parent_write_status(message);
        message << "========  End   ======== \n";
        std::clog << message.str() << std::flush;
    }
    this->_child_processes.erase(result);
    return exit_code;
}

bool sys::test::Test_executor::test_arguments(const std::string& test_name, Symbol&& arguments) {
    auto result = std::find_if(this->_tests.begin(), this->_tests.end(),
                 [&] (const Test& t) { return t.symbol().short_name() == test_name; });
    if (result == this->_tests.end()) { return false; }
    result->arguments(std::move(arguments));
    return true;
}
