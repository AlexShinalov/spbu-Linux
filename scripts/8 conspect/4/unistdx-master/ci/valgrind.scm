(use-modules (ice-9 format))
(define meson-build-root (getenv "MESON_BUILD_ROOT"))
(define meson-source-root (getenv "MESON_SOURCE_ROOT"))
(define meson-subdir (getenv "MESON_SUBDIR"))
(chdir meson-build-root)
(system* "meson" "test" "--verbose"
         (string-join
           (list
             "--wrap=valgrind"
             "--quiet"
             "--error-exitcode=1"
             "--trace-children=yes"
             "--leak-check=full"
             "--gen-suppressions=all"
             (format #f "--suppressions=~a/~a/valgrind.supp" meson-source-root meson-subdir)
             " ")))
