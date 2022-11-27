\mainpage

Unistdx is a C++ library that offers *object-oriented* interface to Linux
system calls. It is built on several core principles:
\arg use UNIX API where possible, but optimise for Linux,
\arg use standard C++ abstractions to map system calls (containers, iterators,
  input/output streams, traits classes, guards etc.),
\arg inherit standard libc structures for maximal transparency,
\arg throw an exception when system call fails,
\arg use asynchronous input/output by default.

\defgroup concepts C++ concepts
\brief Group classes and functions by concept.

\defgroup container Containers
\brief
Classes with
<a href="http://en.cppreference.com/w/cpp/concept/Container">container</a>
interface.
\ingroup concepts

\defgroup iter Iterators
\brief
Classes with
<a href="http://en.cppreference.com/w/cpp/concept/Iterator">iterator</a>
interface.
\ingroup concepts

\defgroup stream Streams
\brief Classes with member input/output operators.
\ingroup concepts

\defgroup streambuf Stream buffers
\brief Classes with \link std::streambuf \endlink interface.
\ingroup concepts

\defgroup mutex Mutexes
\brief Classes with \link std::mutex \endlink interface.
\ingroup concepts

\defgroup semaphore Semaphores
\brief Classes with \link std::condition_variable \endlink interface.
\ingroup concepts

\defgroup guard Guards
\brief
Sentry objects that enforce invariants with
<a href="http://www.stroustrup.com/bs_faq2.html#finally">RAII</a>.
\ingroup concepts

\defgroup traits Traits
\brief Traits classes.
\ingroup concepts

\defgroup exception Exceptions
\brief Exception classes.
\ingroup concepts


\defgroup system System abstractions
\brief System calls and system structures wrappers.

\defgroup ipc Processes
\brief Resource management and parallel execution.
\ingroup system

\defgroup fs File system
\brief File and directory manipulation.
\ingroup system

\defgroup io IO
\brief Input/output handling.
\ingroup system

\defgroup net Network
\brief Network abstractions.
\ingroup system

\defgroup macros Macros
\brief Macros for system error handling.
\ingroup system

\defgroup wrapper Wrappers
\brief Wrappers for system structures.
\ingroup system
