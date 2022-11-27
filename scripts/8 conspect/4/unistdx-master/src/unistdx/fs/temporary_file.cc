#include <random>

#include <unistdx/fs/temporary_file>

#if !defined(UNISTDX_HAVE_MKOSTEMP) && \
    !defined(UNISTDX_HAVE_MKSTEMP)
void sys::temporary_file::make(open_flag flags) {
    using f = open_flag;
    std::default_random_engine prng;
    prng.seed(std::random_device()());
    std::uniform_int_distribution<char> dist(0, 26+26+10);
    auto n = this->_name.size();
    do {
        for (auto i=n-6; i<n; ++i) {
            auto d = dist(prng);
            this->_name[i] = [d] () -> char {
                if (d < 26) { return d+'a'; }
                if (d < 26+26) { return d-26+'A'; }
                return d-26-26+'0';
            }();
        }
        this->_fd = ::open(this->_name.data(),
                           int(flags | f::exclude | f::close_on_exec | f::create),
                           0600);
    } while (this->_fd == -1 && errno == EEXIST);
    UNISTDX_CHECK(this->_fd);
}
#endif
