(define unistdx-manifest
  (let ((v (getenv "UNISTDX_MANIFEST")))
    (if v (string-split v #\,) '())))

(define* (if-enabled name lst #:optional (other '()))
  (if (member name unistdx-manifest) lst other))

(packages->manifest
  (append
    (list
      (@ (gnu packages build-tools) meson)
      (@ (stables packages ninja) ninja/lfs)
      (@ (gnu packages check) googletest)
      (@ (gnu packages pkg-config) pkg-config)
      (@ (gnu packages version-control) pre-commit)
      (@ (gnu packages python-xyz) python-chardet)
      (@ (gnu packages code) lcov)
      (@ (gnu packages code) kcov)
      (@ (gnu packages python-xyz) python-gcovr)
      (@ (gnu packages elf) elfutils)
      (@ (gnu packages compression) xz)
      (list (@ (gnu packages llvm) clang-10) "extra") ;; clang-tidy
      (@ (gnu packages valgrind) valgrind)
      (@ (gnu packages guile) guile-3.0))
    (if-enabled "clang"
      (list
        (@ (gnu packages llvm) libcxx)
        (@ (gnu packages llvm) clang-toolchain-10))
      (list
        (list (@ (gnu packages gcc) gcc-10) "lib")
        (@ (gnu packages commencement) gcc-toolchain-10)))
    (if-enabled "site"
      (list (@ (gnu packages curl) curl)
            (@ (gnu packages graphviz) graphviz)
            (@ (gnu packages documentation) doxygen)
            (@ (gnu packages perl) perl)
            (@ (gnu packages tex) texlive-bin)
            (@ (gnu packages guile-xyz) haunt)
            (@ (gnu packages guile-xyz) guile-syntax-highlight)
            (@ (gnu packages groff) groff)
            (@ (gnu packages xml) libxslt)))
    (if-enabled "ci"
      (list (@ (gnu packages rsync) rsync)
            (@ (gnu packages ssh) openssh)
            (@ (gnu packages base) coreutils)
            (@ (gnu packages base) findutils) ;; lcov needs find
            (@ (gnu packages bash) bash)
            (@ (gnu packages package-management) guix) ;; clang-tidy runs from guix repl
            ))))

;; vim:lispwords+=if-enabled
