/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <unistdx/base/websocketbuf>

#include <algorithm>
#include <functional>
#include <ostream>
#include <random>
#include <sstream>

#include <unistdx/base/base64>
#include <unistdx/base/contracts>
#include <unistdx/base/log_message>
#include <unistdx/base/sha1>
#include <unistdx/base/types>
#include <unistdx/base/websocket>
#include <unistdx/config>
#include <unistdx/net/bytes>

#if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
#include <unistdx/base/log_message>
#endif

namespace {

    typedef std::independent_bits_engine<std::random_device,8,unsigned char>
        key_engine_type;

    typedef std::independent_bits_engine<
        std::random_device,
        8*sizeof(sys::websocket_frame::mask_type),
        sys::websocket_frame::mask_type
    > mask_engine_type;

    key_engine_type key_rng;
    mask_engine_type mask_rng;

    const char websocket_request[] =
        "GET / HTTP/1.1\r\n"
        "User-Agent: unistdx/" UNISTDX_VERSION "\r\n"
        "Connection: Upgrade\r\n"
        "Upgrade: websocket\r\n"
        "Sec-WebSocket-Protocol: binary\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "Sec-WebSocket-Key: ";

    const char websocket_response[] =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Protocol: binary\r\n"
        "Sec-WebSocket-Accept: ";

    const char websocket_bad_response[] =
        "HTTP/1.1 400 Bad Request\r\n\r\n";

    const char http_header_separator[] = "\r\n";

    const std::string websocket_guid =
        "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    template <class T>
    inline const T*
    ccast(T* c) {
        return static_cast<const T*>(c);
    }

    template <class T>
    inline std::string
    lowercase_string(const T* first, const T* last) {
        std::string s;
        s.reserve(last-first);
        while (first != last) {
            s.push_back(std::tolower(*first));
            ++first;
        }
        return s;
    }

    void
    websocket_accept_header(const std::string& encoded_key, char* result) {
        using namespace sys;
        bytes<u32[5]> hash;
        sha1 sha;
        sha.put(encoded_key.data(), encoded_key.size());
        sha.put(websocket_guid.data(), websocket_guid.size());
        sha.compute();
        sha.digest(hash.value());
        u32* v = hash.value();
        for (int i=0; i<5; ++i) {
            v[i] = to_network_format(v[i]);
        }
        base64_encode(hash.begin(), hash.end(), result);
    }

}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::put_header() {
    // LCOV_EXCL_START
    UNISTDX_ASSERTION(this->_valid);
    UNISTDX_ASSERTION(
        (this->_role == role_type::server &&
         this->_sstate == server_state::end) ||
        (this->_role == role_type::client &&
         this->_cstate == client_state::end)
    );
    // LCOV_EXCL_STOP
    // put dummy frame with the max length
    constexpr const size_t n = websocket_frame::max_size();
    char buf[n] = {0};
    this->sputn(buf, n);
}

template<class Ch, class Tr>
std::streamsize
sys::basic_websocketbuf<Ch,Tr>::overwrite_header(std::streamsize n) {
    // LCOV_EXCL_START
    UNISTDX_ASSERTION(this->_valid);
    UNISTDX_ASSERTION(
        (this->_role == role_type::server &&
         this->_sstate == server_state::end) ||
        (this->_role == role_type::client &&
         this->_cstate == client_state::end)
    );
    // LCOV_EXCL_STOP
    // put real frame with the correct length
    constexpr const size_t max_hs = websocket_frame::max_size();
    websocket_frame frame;
    frame.opcode(opcode_type::binary_frame);
    frame.fin(1);
    frame.payload_size(n - max_hs);
    frame.mask(mask_rng());
    const size_t hs = frame.size();
    traits_type::copy(this->opacket_begin(), frame.begin(), hs);
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message("ws", "put _", frame);
    #endif
    // mask the payload
    frame.mask_payload(this->opacket_begin() + max_hs, this->opacket_end());
    return hs;
}

template<class Ch, class Tr>
bool
sys::basic_websocketbuf<Ch,Tr>::xgetheader(
    std::streamsize& header_size,
    std::streamsize& payload_size
) {
    // LCOV_EXCL_START
    UNISTDX_ASSERTION(this->_valid);
    UNISTDX_ASSERTION(
        (this->_role == role_type::server &&
         this->_sstate == server_state::end) ||
        (this->_role == role_type::client &&
         this->_cstate == client_state::end)
    );
    // LCOV_EXCL_STOP
    bool success = false;
    const size_t n = this->egptr() - this->gptr();
    constexpr const size_t min_hs = websocket_frame::min_size();
    if (n >= min_hs) {
        websocket_frame& frame = this->_iframe;
        frame.clear();
        // read minimal header to determine its
        // full size
        traits_type::copy(frame.begin(), this->gptr(), min_hs);
        const size_t hs = frame.size();
        #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
        log_message("ws", "get min frame _", frame);
        #endif
        if (n >= hs) {
            // read full header
            traits_type::copy(
                frame.begin() + min_hs,
                this->gptr() + min_hs,
                hs - min_hs
            );
            header_size = hs;
            payload_size = frame.payload_size();
            success = true;
            #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
            log_message("ws", "get _", frame);
            #endif
        }
    }
    return success;
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::on_payload() {
    UNISTDX_ASSERTION(this->_valid);
    UNISTDX_ASSERTION(
        (this->_role == role_type::server &&
         this->_sstate == server_state::end) ||
        (this->_role == role_type::client &&
         this->_cstate == client_state::end)
    );
    // ignore all frame types except binary
    if (this->_iframe.opcode() != opcode_type::binary_frame) {
        this->skip_packet();
    }
    this->_iframe.mask_payload(this->ipayload_begin(), this->ipayload_end());
}

template<class Ch, class Tr>
bool
sys::basic_websocketbuf<Ch,Tr>::server_handshake() {
    server_state old_state;
    do {
        old_state = this->_sstate;
        switch (this->_sstate) {
        case server_state::initial:
            this->setstate(server_state::parsing_http_method);
            break;
        case server_state::parsing_http_method:
            this->parse_http_method();
            break;
        case server_state::parsing_http_headers:
            this->parse_http_headers();
            break;
        case server_state::validating_headers:
            this->validate_http_headers();
            break;
        case server_state::writing_handshake:
            this->write_handshake();
            break;
        case server_state::end:
            this->_headers.clear();
            break;
        }
    } while (old_state != this->_sstate);
    return this->_valid && this->_sstate == server_state::end;
}

template<class Ch, class Tr>
bool
sys::basic_websocketbuf<Ch,Tr>::client_handshake() {
    client_state old_state;
    do {
        old_state = this->_cstate;
        switch (this->_cstate) {
        case client_state::initial:
            this->initiate_handshake();
            break;
        case client_state::writing_handshake:
            this->write_handshake();
            break;
        case client_state::parsing_http_status:
            this->parse_http_status();
            break;
        case client_state::parsing_http_headers:
            this->parse_http_headers();
            break;
        case client_state::validating_headers:
            this->validate_http_headers();
            break;
        case client_state::end:
            this->_headers.clear();
            break;
        }
    } while (old_state != this->_cstate);
    return this->_valid && this->_cstate == client_state::end;
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::initiate_handshake() {
    this->sputn(websocket_request, sizeof(websocket_request)-1);
    this->put_websocket_key();
    this->sputn("\r\n\r\n", 4);
    this->setstate(client_state::writing_handshake);
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message(
        "ws",
        "client request >\n_",
        std::string(this->pbase(), this->pptr())
    );
    #endif
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::write_handshake() {
    if (this->pptr() == this->pbase()) {
        if (this->_role == role_type::server) {
            this->setstate(server_state::end);
        } else {
            this->setstate(client_state::parsing_http_status);
        }
    }
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::put_websocket_key() {
    char key[16], encoded[24];
    std::generate_n(key, 16, std::ref(key_rng));
    sys::base64_encode(key, 16, encoded);
    this->sputn(encoded, 24);
    // save websocket key for validation
    this->_headers["sec-websocket-key"].assign(encoded, 24);
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::put_websocket_accept_header() {
    char encoded[28];
    websocket_accept_header(this->_headers["sec-websocket-key"], encoded);
    this->sputn(encoded, 28);
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::parse_http_method() {
    char_type* result = this->find_the_end_of_the_line();
    if (result != this->egptr()) {
        int cmp;
        const std::streamsize n = result - this->gptr();
        try {
            // skip bad lines
            if (n < 4) {
                throw 0;
            }
            // find http method
            cmp = traits_type::compare(this->gptr(), "GET", 3);
            if (cmp != 0) {
                throw 1;
            }
            this->setstate(server_state::parsing_http_headers);
        } catch (...) {
            this->_valid = false;
        }
        this->gbump(n+sizeof(http_header_separator)-1);
    }
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message("ws", "_ _", __func__, this->_valid);
    #endif
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::parse_http_status() {
    char_type* last = this->find_the_end_of_the_line();
    if (last != this->egptr()) {
        #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
        log_message("ws", "_ _", __func__, std::string(this->gptr(), last));
        #endif
        int cmp;
        const std::streamsize n = last - this->gptr();
        try {
            // skip bad lines
            if (n < 4) {
                throw 0;
            }
            // find protocol name
            cmp = traits_type::compare(this->gptr(), "HTTP", 4);
            if (cmp != 0) {
                throw 1;
            }
            // find HTTP status
            const char_type* result2 = traits_type::find(this->gptr(), n, ' ');
            if (!result2) {
                throw 2;
            }
            if (last - result2 < 3) {
                throw 3;
            }
            cmp = traits_type::compare(result2+1, "101", 3);
            if (cmp != 0) {
                throw 4;
            }
            this->setstate(client_state::parsing_http_headers);
        } catch (...) {
            this->_valid = false;
        }
        this->gbump(n+sizeof(http_header_separator)-1);
    }
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message("ws", "_ _", __func__, this->_valid);
    #endif
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::parse_http_headers() {
    bool finished = false;
    while (!finished) {
        char_type* result = this->find_the_end_of_the_line();
        const size_t n = result - this->gptr();
        if (n > sizeof(http_header_separator)-1) {
            try {
                const char_type* sep = traits_type::find(this->gptr(), n, ':');
                if (!sep) {
                    throw 0;
                }
                #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
                log_message(
                    "ws",
                    "header _",
                    std::string(this->gptr(), result)
                );
                #endif
                this->_headers.emplace(
                    lowercase_string(ccast(this->gptr()), sep),
                    std::string(sep+2, ccast(result))
                );
            } catch (...) {
                this->_valid = false;
            }
        } else {
            if (this->_role == role_type::server) {
                this->setstate(server_state::validating_headers);
            } else {
                this->setstate(client_state::validating_headers);
            }
            finished = true;
        }
        this->gbump(n+sizeof(http_header_separator)-1);
    }
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message("ws", "_ _", __func__, this->_valid);
    #endif
}

template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::validate_http_headers() {
    try {
        if (!this->_valid) {
            throw 0;
        }
        this->ensure_header("sec-websocket-protocol", "binary");
        this->ensure_header("upgrade", "websocket");
        this->ensure_header_contains("connection", "Upgrade");
        if (this->_role == role_type::server) {
            this->ensure_header("sec-websocket-key");
            this->ensure_header("sec-websocket-version");
            // write response
            this->sputn(websocket_response, sizeof(websocket_response)-1);
            this->put_websocket_accept_header();
            this->sputn("\r\n\r\n", 4);
            #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
            log_message(
                "ws",
                "server response >\n_",
                std::string(this->pbase(), this->pptr())
            );
            #endif
        this->setstate(server_state::writing_handshake);
        } else {
            char encoded[28];
            websocket_accept_header(
                this->_headers["sec-websocket-key"],
                encoded
            );
            this->ensure_header(
                "sec-websocket-accept",
                std::string(encoded, 28)
            );
            this->setstate(client_state::end);
        }
    } catch (...) {
        this->_valid = false;
        if (this->_role == role_type::server) {
            this->sputn(
                websocket_bad_response,
                sizeof(websocket_bad_response)-1
            );
        }
    }
    #if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
    log_message("ws", "_ _", __func__, this->_valid);
    #endif
}

#if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::setstate(server_state s) {
    server_state old = this->_sstate;
    this->_sstate = s;
    log_message("ws", "server state change: _ -> _", int(old), int(s));
}

#endif

#if !defined(NDEBUG) && defined(UNISTDX_DEBUG_WEBSOCKETBUF)
template<class Ch, class Tr>
void
sys::basic_websocketbuf<Ch,Tr>::setstate(client_state s) {
    client_state old = this->_cstate;
    this->_cstate = s;
    log_message("ws", "client state change: _ -> _", int(old), int(s));
}

#endif

template<class Ch, class Tr>
typename sys::basic_websocketbuf<Ch,Tr>::char_type*
sys::basic_websocketbuf<Ch,Tr>::find_the_end_of_the_line() noexcept {
    return std::search(
        this->gptr(),
        this->egptr(),
        http_header_separator,
        http_header_separator + sizeof(http_header_separator)-1
    );
}

template class sys::basic_websocketbuf<char,std::char_traits<char> >;
