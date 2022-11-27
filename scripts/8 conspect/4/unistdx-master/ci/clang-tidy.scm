(use-modules
  (ice-9 popen)
  (ice-9 pretty-print)
  (ice-9 ftw)
  (srfi srfi-1)
  (guix scripts build))

;; add include paths from environment variables
(define cpp-args
  (append-map
    (lambda (name)
      (define value (getenv name))
      (if value
        (fold
          (lambda (dir prev)
            (if (string=? (basename dir) "c++")
              prev
              (cons (string-append "-I" dir) prev)))
          '()
          (string-split value #\:))
        '()))
    '("CPLUS_INCLUDE_PATH" "C_INCLUDE_PATH")))

;; add Clang built-in headers to include path
(define clang-directory
  (string-trim-both
    (with-output-to-string
      (lambda ()
        (guix-build "-e" "(@ (gnu packages llvm) clang)")))))
(ftw clang-directory
     (lambda (filename statinfo flag)
       (if (member (basename filename) '("stddef.h" "limits.h"))
         (set! cpp-args (cons* "-isystem" (dirname filename) cpp-args)))
       #t))

;; execute clang-tidy
(set! cpp-args (delete-duplicates! cpp-args))
(define all-args (append (cdr (command-line)) cpp-args))
;;(pretty-print all-args)
(apply execlp (cons (car all-args) all-args))
