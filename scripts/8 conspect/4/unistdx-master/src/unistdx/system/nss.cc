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

#include <unistd.h>

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <vector>

#include <unistdx/base/check>
#include <unistdx/config>
#include <unistdx/it/intersperse_iterator>
#include <unistdx/system/nss>

#if defined(UNISTDX_ENTITY_MUTEX)
#include <mutex>
#endif

namespace {

    inline long
    get_max_size(int name) {
        long n = ::sysconf(name);
        return n == -1L ? 1024L : n;
    }

    template <class Id>
    struct By {

        By(const char* name):
        _name(name),
        _byname(true)
        {}

        By(Id uid):
        _id(uid),
        _byname(false)
        {}

        inline const char*
        name() const noexcept {
            return this->_name;
        }

        inline Id
        uid() const noexcept {
            return this->_id;
        }

        inline Id
        gid() const noexcept {
            return this->_id;
        }

        inline bool
        by_name() const noexcept {
            return this->_byname;
        }

        union {
            const char* _name;
            Id _id;
        };
        bool _byname;

    };

    #if defined(UNISTDX_ENTITY_MUTEX)
    std::mutex ent_mutex;
    #endif

    template <class Entity>
    struct Entity_traits {};

    template <>
    struct Entity_traits<sys::user> {

        typedef sys::uid_type id_type;
        typedef sys::passwd_type result_type;
        typedef sys::user entity_type;

        inline static constexpr int
        conf_name() noexcept {
            return _SC_GETPW_R_SIZE_MAX;
        }

        #if defined(UNISTDX_HAVE_GETPWUID_R) || \
        defined(UNISTDX_HAVE_GETPWNAM_R)
        inline static int
        get(
            const By<id_type>& field,
            entity_type& u,
            std::string& buf,
            result_type** res
        ) {
            int ret;
            if (field.by_name()) {
                #if defined(UNISTDX_HAVE_GETPWNAM_R)
                ret = ::getpwnam_r(field.name(), &u, &buf[0], buf.size(), res);
                #else
                ret = -1;
                #endif
            } else {
                #if defined(UNISTDX_HAVE_GETPWUID_R)
                ret = ::getpwuid_r(field.uid(), &u, &buf[0], buf.size(), res);
                #else
                ret = -1;
                #endif
            }
            return ret;
        }
        #else
        inline static result_type*
        get(const By<id_type>& field) {
            result_type* res;
            if (field.by_name()) {
                res = ::getpwnam(field.name());
            } else {
                res = ::getpwuid(field.uid());
            }
            return res;
        }
        #endif

        #if !defined(UNISTDX_HAVE_GETPWUID_R) || \
        !defined(UNISTDX_HAVE_GETPWNAM_R)
        inline static void
        copy(result_type* pw, entity_type& u, std::string& buf) {
            buf.clear();
            const size_t pos_name = buf.size();
            buf.append(pw->pw_name);
            buf.push_back('\0');
            const size_t pos_passwd = buf.size();
            buf.append(pw->pw_passwd);
            buf.push_back('\0');
            #if defined(UNISTDX_HAVE_GECOS)
            const size_t pos_gecos = buf.size();
            buf.append(pw->pw_gecos);
            buf.push_back('\0');
            #endif
            const size_t pos_dir = buf.size();
            buf.append(pw->pw_dir);
            buf.push_back('\0');
            const size_t pos_shell = buf.size();
            buf.append(pw->pw_shell);
            buf.push_back('\0');
            u.pw_name = &buf[0] + pos_name;
            u.pw_passwd = &buf[0] + pos_passwd;
            u.pw_uid = pw->pw_uid;
            u.pw_gid = pw->pw_gid;
            #if defined(UNISTDX_HAVE_GECOS)
            u.pw_gecos = &buf[0] + pos_gecos;
            #endif
            u.pw_dir = &buf[0] + pos_dir;
            u.pw_shell = &buf[0] + pos_shell;
        }

        #endif

    };

    template <>
    struct Entity_traits<sys::group> {

        typedef sys::gid_type id_type;
        typedef sys::group_type result_type;
        typedef sys::group entity_type;

        inline static constexpr int
        conf_name() noexcept {
            return _SC_GETGR_R_SIZE_MAX;
        }

        #if defined(UNISTDX_HAVE_GETGRGID_R) || \
        defined(UNISTDX_HAVE_GETGRNAM_R)
        inline static int
        get(
            const By<id_type>& field,
            entity_type& u,
            std::string& buf,
            result_type** res
        ) {
            int ret;
            if (field.by_name()) {
                #if defined(UNISTDX_HAVE_GETGRNAM_R)
                ret = ::getgrnam_r(field.name(), &u, &buf[0], buf.size(), res);
                #else
                ret = -1;
                #endif
            } else {
                #if defined(UNISTDX_HAVE_GETGRGID_R)
                ret = ::getgrgid_r(field.gid(), &u, &buf[0], buf.size(), res);
                #else
                ret = -1;
                #endif
            }
            return ret;
        }
        #else
        inline static result_type*
        get(const By<id_type>& field) {
            result_type* res;
            if (field.by_name()) {
                res = ::getgrnam(field.name());
            } else {
                res = ::getgrgid(field.gid());
            }
            return res;
        }
        #endif

        #if !defined(UNISTDX_HAVE_GETGRGID_R) || \
        !defined(UNISTDX_HAVE_GETGRNAM_R)
        inline static void
        copy(result_type* gr, entity_type& u, std::string& buf) {
            typedef sys::group::iterator iterator;
            buf.clear();
            const size_t pos_name = buf.size();
            buf.append(gr->gr_name);
            buf.push_back('\0');
            const size_t pos_passwd = buf.size();
            buf.append(gr->gr_passwd);
            buf.push_back('\0');
            std::vector<size_t> pos_mem;
            std::for_each(
                iterator(gr->gr_mem),
                iterator(),
                [&] (char* mem) {
                    pos_mem.emplace_back(buf.size());
                    buf.append(mem);
                    buf.push_back('\0');
                }
            );
            // pad buffer to align group members addresses
            const size_t padding = buf.size() % sizeof(gr->gr_mem);
            buf.resize(buf.size() + padding);
            buf.reserve(buf.size() + (pos_mem.size() + 1)*sizeof(gr->gr_mem));
            u.gr_name = &buf[0] + pos_name;
            u.gr_passwd = &buf[0] + pos_passwd;
            u.gr_gid = gr->gr_gid;
            u.gr_mem = reinterpret_cast<char**>(&buf[0] + buf.size());
            union Bytes {
                char* ptr;
                char chars[sizeof(ptr)];
            };
            for (size_t pos : pos_mem) {
                Bytes bytes;
                bytes.ptr = &buf[0] + pos;
                buf.append(bytes.chars, sizeof(bytes.chars));
            }
            Bytes last;
            last.ptr = nullptr;
            buf.append(last.chars, sizeof(last.chars));
        }

        #endif // if !defined(UNISTDX_HAVE_GETGRGID_R) ||
        // !defined(UNISTDX_HAVE_GETGRNAM_R)

    };

    #if defined(UNISTDX_ENTITY_MUTEX)
    template <class Id, class Entity>
    bool
    find_entity_nr(By<Id> field, Entity& u, std::string& buf) {
        using namespace sys;
        typedef Entity_traits<Entity> traits_type;
        typedef typename traits_type::result_type result_type;
        bool found;
        std::lock_guard<std::mutex> lock(ent_mutex);
        errno = 0;
        result_type* res = traits_type::get(field);
        if (!res) {
            if (errno) {
                throw ::sys::bad_call();
            } else {
                found = false;
            }
        } else {
            found = true;
        }
        if (found) {
            traits_type::copy(res, u, buf);
        }
        return found;
    }
    #endif

    #if defined(UNISTDX_HAVE_GETPWUID_R) || \
    defined(UNISTDX_HAVE_GETPWNAM_R) || \
    defined(UNISTDX_HAVE_GETGRGID_R) || \
    defined(UNISTDX_HAVE_GETGRNAM_R)
    template <class Id, class Entity>
    bool
    find_entity_r(By<Id> field, Entity& u, std::string& buf) {
        using namespace sys;
        typedef Entity_traits<Entity> traits_type;
        typedef typename traits_type::result_type result_type;
        enum status_type {undefined, found, not_found};
        status_type status = undefined;
        result_type* res;
        buf.resize(get_max_size(traits_type::conf_name()));
        int ret;
        do {
            // N.B. this function returns errno value
            // but does not set errno itself
            ret = traits_type::get(field, u, buf, &res);
            if (!res) {
                if (ret == 0 || ret == ENOENT) {
                    status = not_found;
                } else if (ret == ERANGE) {
                    buf.resize(buf.size()*2);
                } else {
                    // LCOV_EXCL_START
                    throw sys::bad_call(std::errc(ret));
                    // LCOV_EXCL_STOP
                }
            } else {
                status = found;
            }
        } while (status == undefined);
        return status == found;
    }

    #endif

}

std::ostream&
sys::operator<<(std::ostream& out, const user& rhs) {
    return out
           << rhs.name() << ':'
           << rhs.password() << ':'
           << rhs.id() << ':'
           << rhs.group_id() << ':'
           << (rhs.real_name() ? rhs.real_name() : "") << ':'
           << rhs.home() << ':'
           << rhs.shell();
}

bool
sys::find_user(uid_type uid, user& u) {
    #if defined(UNISTDX_HAVE_GETPWUID_R)
    return find_entity_r<uid_type>(uid, u, u._buf);
    #else
    return find_entity_nr<uid_type>(uid, u, u._buf);
    #endif
}

bool
sys::find_user(const char* name, user& u) {
    #if defined(UNISTDX_HAVE_GETPWNAM_R)
    return find_entity_r<uid_type>(name, u, u._buf);
    #else
    return find_entity_nr<uid_type>(name, u, u._buf);
    #endif
}

std::ostream&
sys::operator<<(std::ostream& out, const group& rhs) {
    out << rhs.name() << ':'
        << rhs.password() << ':'
        << rhs.id() << ':';
    std::copy(
        rhs.begin(),
        rhs.end(),
        intersperse_iterator<const char*,char>(out, ',')
    );
    return out;
}


bool
sys::find_group(gid_type gid, group& u) {
    #if defined(UNISTDX_HAVE_GETGRGID_R)
    return find_entity_r<gid_type>(gid, u, u._buf);
    #else
    return find_entity_nr<gid_type>(gid, u, u._buf);
    #endif
}

bool
sys::find_group(const char* name, group& u) {
    #if defined(UNISTDX_HAVE_GETGRNAM_R)
    return find_entity_r<gid_type>(name, u, u._buf);
    #else
    return find_entity_nr<gid_type>(name, u, u._buf);
    #endif
}
