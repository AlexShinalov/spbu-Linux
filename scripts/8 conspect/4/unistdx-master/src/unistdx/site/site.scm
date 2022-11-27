;; Virtual Testbed — ship dynamics simulator for extreme conditions and rough sea.
;; Copyright © 2020 Ivan Gankevich
;;
;; This file is part of Virtual Testbed.
;;
;; Virtual Testbed is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; Virtual Testbed is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with Virtual Testbed.  If not, see <https://www.gnu.org/licenses/>.

(use-modules
 (sxml simple)
 (sxml transform)
 ((sxml xpath) #:select (sxpath))
 (ice-9 format)
 (ice-9 match)
 (ice-9 getopt-long)
 (ice-9 textual-ports)
 (ice-9 pretty-print)
 (srfi srfi-1)
 (oop goops)
 (haunt html)
 (syntax-highlight)
 (syntax-highlight scheme))

(define-class <declaration> ()
  (name #:init-keyword #:name #:accessor declaration-name)
  (kind #:init-keyword #:kind #:accessor declaration-kind)
  (refid #:init-keyword #:refid #:accessor declaration-refid)
  (parents #:init-keyword #:parents #:accessor declaration-parents #:init-value '()))

(define-method (equal? (a <declaration>) (b <declaration>) out)
  (let ((refid-a (declaration-refid a)) (refid-b (declaration-refid b)))
    (and (string? refid-a) (string? refid-b) (string= refid-a refid-b))))

(define-method (write (o <declaration>) out)
    (format out "<~a>" (declaration-refid o)))

(define* (make-declaration sxml #:optional (parents '()))
  (make <declaration>
    #:name (car ((sxpath '(name *text*)) sxml))
    #:kind (car ((sxpath '(@ kind *text*)) sxml))
    #:refid (car ((sxpath '(@ refid *text*)) sxml))
    #:parents parents))

(define-class <argument> ()
  (type #:init-keyword #:type #:accessor argument-type)
  (name #:init-keyword #:name #:accessor argument-name)
  (default-name #:init-keyword #:default-name #:accessor argument-default-name)
  (default-value #:init-keyword #:default-value #:accessor argument-default-value)
  (description #:init-keyword #:description #:accessor argument-description))

(define-method (equal? (a <argument>) (b <argument>) out)
  (let ((name-a (argument-name a)) (name-b (argument-name b))
        (type-a (argument-type a)) (type-b (argument-type b)))
    (cond
      ((and (string? name-a) (string? name-b)) (string= name-a name-b))
      ((and (string? type-a) (string? type-b)) (string= type-a type-b))
      (else #f))))

(define (make-linked-text sxml)
  (pre-post-order
   sxml
   `((defval      . ,(lambda (tag . kids) kids))
     (initializer . ,(lambda (tag . kids) kids))
     (*default*   . ,(lambda (tag . kids) `(,tag ,@kids)))
     (*text*      . ,(lambda (_ txt) txt)))))

(define (kind-human kind)
  (cond
   ((string= "see" kind) "See")
   ((string= "return" kind) "Return")
   ((string= "author" kind) "Author")
   ((string= "authors" kind) "Authors")
   ((string= "version" kind) "Version")
   ((string= "since" kind) "Since")
   ((string= "date" kind) "Date")
   ((string= "note" kind) "Note")
   ((string= "warning" kind) "Warning")
   ((string= "pre" kind) "Precondition")
   ((string= "post" kind) "Postcondition")
   ((string= "copyright" kind) "Copyright")
   ((string= "invariant" kind) "Invariant")
   ((string= "remark" kind) "Remark")
   ((string= "attention" kind) "Attention")
   (else kind)))

(define (absolute path)
  (if (string-prefix? "/" path) path (string-append (getcwd) "/" path)))

(define ignore
  (lambda (tag . kids)
    (cond
     ((string? kids) kids)
     ((null? kids) kids)
     ;; empty string
     ((and (list? kids) (= (length kids) 1)
           (string? (car kids)) (string-null? (string-trim-both (car kids))))
      '())
     ;; single tag
     ((and (list? kids) (= (length kids) 1)
           (list? (car kids)) (not (null? (car kids)))
           (not (eq? (caar kids) '@)))
      (car kids))
     (else `(span ,@kids)))))

(define (replace-with new-tag)
  (lambda (tag . kids) `(,new-tag ,@kids)))

(define (replace-with-character char)
  (lambda (tag . kids) `(,(string char))))

(define delete (lambda (tag . kids) '()))

(define (flatten x)
  (cond ((null? x) '())
        ((not (pair? x)) (list x))
        (else (append (flatten (car x))
                      (flatten (cdr x))))))

(define (range a b) (iota (+ (- b a) 1) a))

(define (escape-ampersand str)
  (string-map
    (lambda (char) (if (char=? char #\&) #\alarm char))
    str))

(define (unescape-ampersand str)
  (string-map
    (lambda (char) (if (char=? char #\alarm) #\& char))
    str))

(define* (make-description sxml #:key (delete-simplesect? #t)
                           (delete-short-description? #f)
                           (inside-variablelist? #f))
  (define (map-characters a b tags)
    (map
     (lambda (code tag) `(,tag . ,(replace-with-character (integer->char code))))
     (range a b) tags))
  (define (unbox lst)
    (define (unbox-inner ls)
      (match ls
        ((val) (unbox-inner val))
        (else ls)))
    (define unboxed-lst (unbox-inner lst))
    (cond
     ((null? unboxed-lst) '())
     ((eq? unboxed-lst 'span) '())
     (else (list unboxed-lst))))
  (define (recurse new-kids)
    (make-description
     new-kids
     #:delete-simplesect? delete-simplesect?
     #:delete-short-description? delete-short-description?
     #:inside-variablelist? inside-variablelist?))
  (unbox
   (pre-post-order
    sxml
    `((briefdescription     . ,ignore)
      (detaileddescription  . ,ignore)
      (inbodydescription    . ,ignore)
      (short-description    . ,(if delete-short-description? delete ignore))
      (para                 . ,ignore)
      (ulink                . ,(lambda (tag . kids)
                                 (let ((href ((sxpath '(url *text*)) (car kids))))
                                   `(a (@ (href ,@href)) ,@(cdr kids)))))
      (bold                 . ,(replace-with 'b))
      (strike               . ,(replace-with 'del))
      (underline            . ,(replace-with 'u))
      (emphasis             . ,(replace-with 'em))
      (computeroutput       . ,(lambda (tag . kids)
                                 `((code (@ (class "text-dark"))
                                         ,@(if (not (null? ((sxpath '(@)) (cons tag kids))))
                                               (cdr kids) kids)))))
      (subscript            . ,(replace-with 'sub))
      (superscript          . ,(replace-with 'sup))
      (center               . ,(lambda (tag . kids) `(div (@ (class "text-center")) ,@kids)))
      (htmlonly             . ,ignore)
      (xmlonly              . ,delete)
      (rtfonly              . ,delete)
      (latexonly            . ,delete)
      (dot                  . ,delete)
      (plantuml             . ,delete)
      (anchor               . ,(lambda (tag . kids)
                                 (let* ((sxml (cons tag kids))
                                        (id ((sxpath '(@ id *text*)) sxml))
                                        (name (if (null? id) "" (car id))))
                                   `(a (@ (name ,name)) ,@(cdr kids)))))
      (formula              . ,(lambda (tag . kids)
                                 (let* ((sxml (cons tag kids))
                                        (text ((sxpath '(*text*)) sxml))
                                        (str (if (null? text) "" (car text))))
                                   (cond
                                    ((and (string-prefix? "$" str)
                                          (string-suffix? "$" str))
                                     `(script (@ (type "math/tex"))
                                              ,(escape-ampersand
                                                 (substring str 1 (- (string-length str) 1)))))
                                    ((and (string-prefix? "\\[" str)
                                          (string-suffix? "\\]" str))
                                     `(script (@ (type "math/tex; mode=display"))
                                              ,(escape-ampersand
                                                 (substring str 2 (- (string-length str) 2)))))
                                    (else `(script (@ (type "math/tex; mode=display"))
                                                   ,(escape-ampersand str)))))))
      ;;(ref                . ,ignore)
      (emoji                . ,(lambda (tag . kids)
                                 `(,((sxpath '(unicode *text*)) (car kids)))))
      ,@(map-characters
         #xA0 #xFF
         '(nonbreakablespace
           iexcl cent pound curren yen brvbar sect umlaut copy ordf laquo
           not shy registered macr deg plusmn sup2 sup3 acute micro para
           middot cedil sup1 ordm raquo frac14 frac12 frac34 iquest Agrave
           Aacute Acirc Atilde Aumlaut Aring AElig Ccedil Egrave Eacute
           Ecirc Eumlaut Igrave Iacute Icirc Iumlaut ETH Ntilde Ograve
           Oacute Ocirc Otilde Oumlaut times Oslash Ugrave Uacute
           Ucirc Uumlaut Yacute THORN szlig agrave aacute acirc atilde
           aumlaut aring aelig ccedil egrave eacute ecirc eumlaut igrave
           iacute icirc iumlaut eth ntilde ograve oacute ocirc otilde
           oumlaut divide oslash ugrave uacute ucirc uumlaut yacute thorn yumlaut))
      (fnof                 . ,(replace-with-character #\x0192))
      ,@(map-characters
         #x391 #x3A1
         '(Alpha Beta Gamma Delta Epsilon Zeta Eta Theta Iota Kappa
                 Lambda Mu Nu Xi Omicron Pi Rho))
      ,@(map-characters #x3A3 #x3A9 '(Sigma Tau Upsilon Phi Chi Psi Omega))
      ,@(map-characters
         #x3B1 #x3C9
         '(alpha beta gamma delta epsilon zeta eta theta iota kappa lambda
                 mu nu xi omicron pi rho sigmaf sigma tau upsilon phi chi psi omega))
      (thetasym             . ,(replace-with-character #\x03D1))
      (upsih                . ,(replace-with-character #\x03D2))
      (piv                  . ,(replace-with-character #\x03D6))
      (bull . ,(replace-with-character #\x2022))
      (hellip . ,(replace-with-character #\x2026))
      (prime . ,(replace-with-character #\x2032))
      (Prime . ,(replace-with-character #\x2033))
      (oline . ,(replace-with-character #\x203E))
      (frasl . ,(replace-with-character #\x2044))
      (weierp . ,(replace-with-character #\x2118))
      (imaginary . ,(replace-with-character #\x2111))
      (real . ,(replace-with-character #\x211C))
      (trademark . ,(replace-with-character #\x2122))
      (alefsym . ,(replace-with-character #\x2135))
      (larr . ,(replace-with-character #\x2190))
      (uarr . ,(replace-with-character #\x2191))
      (rarr . ,(replace-with-character #\x2192))
      (darr . ,(replace-with-character #\x2193))
      (harr . ,(replace-with-character #\x2194))
      (crarr . ,(replace-with-character #\x21B5))
      (lArr . ,(replace-with-character #\x21D0))
      (uArr . ,(replace-with-character #\x21D1))
      (rArr . ,(replace-with-character #\x21D2))
      (dArr . ,(replace-with-character #\x21D3))
      (hArr . ,(replace-with-character #\x21D4))
      (forall . ,(replace-with-character #\x2200))
      (part . ,(replace-with-character #\x2202))
      (exist . ,(replace-with-character #\x2203))
      (empty . ,(replace-with-character #\x2205))
      (nabla . ,(replace-with-character #\x2207))
      (isin . ,(replace-with-character #\x2208))
      (notin . ,(replace-with-character #\x2209))
      (ni . ,(replace-with-character #\x220B))
      (prod . ,(replace-with-character #\x220F))
      (sum . ,(replace-with-character #\x2211))
      (minus . ,(replace-with-character #\x2212))
      (lowast . ,(replace-with-character #\x2217))
      (radic . ,(replace-with-character #\x221A))
      (prop . ,(replace-with-character #\x221D))
      (infin . ,(replace-with-character #\x221E))
      (ang . ,(replace-with-character #\x2220))
      (and . ,(replace-with-character #\x2227))
      (or . ,(replace-with-character #\x2228))
      (cap . ,(replace-with-character #\x2229))
      (cup . ,(replace-with-character #\x222A))
      (int . ,(replace-with-character #\x222B))
      (there4 . ,(replace-with-character #\x2234))
      (sim . ,(replace-with-character #\x223C))
      (cong . ,(replace-with-character #\x2245))
      (asymp . ,(replace-with-character #\x2248))
      (ne . ,(replace-with-character #\x2260))
      (equiv . ,(replace-with-character #\x2261))
      (le . ,(replace-with-character #\x2264))
      (ge . ,(replace-with-character #\x2265))
      (sub . ,(replace-with-character #\x2282))
      (sup . ,(replace-with-character #\x2283))
      (nsub . ,(replace-with-character #\x2284))
      (sube . ,(replace-with-character #\x2286))
      (supe . ,(replace-with-character #\x2287))
      (oplus . ,(replace-with-character #\x2295))
      (otimes . ,(replace-with-character #\x2297))
      (perp . ,(replace-with-character #\x22A5))
      (sdot . ,(replace-with-character #\x22C5))
      (lceil . ,(replace-with-character #\x2308))
      (rceil . ,(replace-with-character #\x2309))
      (lfloor . ,(replace-with-character #\x230A))
      (rfloor . ,(replace-with-character #\x230B))
      (lang . ,(replace-with-character #\x2329))
      (rang . ,(replace-with-character #\x232A))
      (loz . ,(replace-with-character #\x25CA))
      (spades . ,(replace-with-character #\x2660))
      (clubs . ,(replace-with-character #\x2663))
      (hearts . ,(replace-with-character #\x2665))
      (diams . ,(replace-with-character #\x2666))
      (OElig                . ,(replace-with-character #\x0152))
      (oelig                . ,(replace-with-character #\x0153))
      (Scaron               . ,(replace-with-character #\x0160))
      (scaron               . ,(replace-with-character #\x0161))
      (Yumlaut              . ,(replace-with-character #\x0178))
      (circ                 . ,(replace-with-character #\x02C6))
      (tilde                . ,(replace-with-character #\x02DC))
      (ensp                 . ,(replace-with-character #\x2002))
      (emsp                 . ,(replace-with-character #\x2003))
      (thinsp               . ,(replace-with-character #\x2009))
      (zwnj                 . ,(replace-with-character #\x200C))
      (zwj                  . ,(replace-with-character #\x200D))
      (lrm                  . ,(replace-with-character #\x200E))
      (rlm                  . ,(replace-with-character #\x200F))
      (ndash                . ,(replace-with-character #\x2013))
      (mdash                . ,(replace-with-character #\x2014))
      (lsquo                . ,(replace-with-character #\x2018))
      (rsquo                . ,(replace-with-character #\x2019))
      (sbquo                . ,(replace-with-character #\x201A))
      (ldquo                . ,(replace-with-character #\x201C))
      (rdquo                . ,(replace-with-character #\x201D))
      (bdquo                . ,(replace-with-character #\x201E))
      (dagger               . ,(replace-with-character #\x2020))
      (Dagger               . ,(replace-with-character #\x2021))
      (permil               . ,(replace-with-character #\x2030))
      (lsaquo               . ,(replace-with-character #\x2039))
      (rsaquo               . ,(replace-with-character #\x203A))
      (euro                 . ,(replace-with-character #\x20A0))
      (tm                   . ,(replace-with-character #\x2122))
      (linebreak            . ,(replace-with 'br))
      (hruler               . ,(replace-with 'hr))
      (preformatted         . ,(lambda (tag . kids) `(pre (@ (class "bg-light mt-1")) ,@kids)))
      (verbatim             . ,(lambda (tag . kids) `(pre (@ (class "bg-light mt-1")) ,@kids)))
      (programlisting *preorder*
                      . ,(lambda (tag . kids)
                           (let* ((sxml (cons tag kids))
                                  (filename ((sxpath '(@ filename *text*)) sxml)))
                             (cond
                              ((null? filename) `(pre ,@(cdr kids)))
                              ((string= (car filename) ".scm")
                               (let* ((new-sxml (make-description (cdr kids)))
                                      (str (apply string-append (flatten new-sxml))))
                                 `(pre (@ (class "bg-light"))
                                       ,@(highlights->sxml
                                          (highlight lex-scheme str)))))
                              (else `(pre ,@(make-description (cdr kids))))))))
      (codeline             . ,ignore)
      (highlight            . ,(lambda (tag . kids) (cdr kids)))
      (sp                   . ,(lambda (tag . kids) " "))
      (indexentry           . ,(replace-with 'delete))
      (itemizedlist         . ,(replace-with 'ul))
      (orderedlist          . ,(replace-with 'ol))
      (listitem             . ,(if inside-variablelist?
                                   (lambda (tag . kids)
                                     `((dd (@ (class "d-inline mb-0 ml-1")) ,@kids)
                                       (br)))
                                   (replace-with 'li)))
      (simplesect *preorder*
                  . ,(lambda (tag . kids)
                       (let* ((sxml (cons tag kids))
                              (kind ((sxpath '(@ kind *text*)) sxml))
                              (title ((sxpath '(title *text*)) sxml)))
                         (cond
                          ((and (not (null? kind)) (string= (car kind) "par"))
                           (if (not delete-simplesect?)
                               '()
                               (if (null? title)
                                   `((p ,@(recurse (cdr kids))))
                                   `((dl (@ (class "mb-0"))
                                         (dt (@ (class "d-inline-block"))
                                             ,(string-append (car title) " "))
                                         (dd (@ (class "d-inline-block mb-0 ml-1"))
                                             ,@(recurse (cddr kids))))))))
                          (else
                           (if delete-simplesect?
                               '()
                               `((dl (@ (class "mb-0"))
                                     (dt (@ (class "d-inline-block"))
                                         ,(kind-human (car kind)))
                                     (dd (@ (class "d-inline-block mb-0 ml-1"))
                                         ,@(recurse (cdr kids)))))))))))
      (title                . ,(replace-with 'h1))
      (variablelist *preorder*
                    . ,(lambda (tag . kids)
                         `(dl (@ (class "mb-0"))
                              ,@(make-description
                                 kids
                                 #:delete-simplesect? delete-simplesect?
                                 #:delete-short-description? delete-short-description?
                                 #:inside-variablelist? #t))))
      (varlistentry         . ,ignore)
      (term                 . ,(lambda (tag . kids)
                                 `(dt (@ (class "d-inline"))
                                      ,@kids)))
      (table                . ,(lambda (tag . kids)
                                 (if (and (not (null? kids))
                                          (not (null? (car kids)))
                                          (eq? (car (car kids)) '@))
                                     `(table (@ (class "table table-sm")) ,@(cdr kids))
                                     `(table (@ (class "table table-sm")) ,@kids))))
      (row                  . ,(replace-with 'tr))
      (entry                . ,(lambda (tag . kids)
                                 (let ((thead ((sxpath '(thead *text*)) (car kids))))
                                   (if (and (not (null? thead)) (string= (car thead) "yes"))
                                       `((th (@ (scope "col")) ,@(cdr kids)))
                                       `((td ,@(cdr kids)))))))
      (heading              . ,(lambda (tag . kids)
                                 `(,(string->symbol
                                     (string-append
                                       "h" (car ((sxpath '(level *text*)) (car kids)))))
                                    (@ (class "mt-3"))
                                   ,@(cdr kids))))
      (image                . ,(lambda (tag . kids)
                                 (let* ((sxml (cons tag kids))
                                        (type (car ((sxpath '(@ type *text*)) sxml)))
                                        (text (cdr kids))
                                        (src (car ((sxpath '(@ name *text*)) sxml))))
                                   (if (string= type "html")
                                       `(figure
                                          (@ (class "text-center"))
                                          (img (@ (src ,(string-append "/assets/" src))
                                                  (alt ,@text)
                                                  (class "rounded img-fluid")))
                                          (figcaption (@ (class "text-muted")) ,@text))
                                       '()))))
      (dotfile              . ,delete)
      (mscfile              . ,delete)
      (diafile              . ,delete)
      (toclist              . ,(replace-with 'ol))
      (tocitem              . ,(replace-with 'li))
      (language             . ,ignore)
      (parameterlist        . ,(lambda (tag . kids) `(dl (@ (class "mb-0")) ,@(cdr kids))))
      (parameteritem        . ,ignore)
      (parameternamelist    . ,(lambda (tag . kids) `(dt (@ (class "d-inline")) ,@kids)))
      (parameterdescription . ,(lambda (tag . kids)
                                 `((dd (@ (class "d-inline mb-0 ml-1")) ,@kids)
                                   (br))))
      (parametertype        . ,ignore)
      (parametername        . ,ignore)
      (xrefsect             . ,ignore)
      (copydoc              . ,ignore)
      (blockquote           . ,(lambda (tag . kids)
                                 `(blockquote (@ (class "blockquote")) ,@kids)))
      (parblock             . ,(lambda (tag . kids) `(,@kids)))
      (sect1                . ,ignore)
      (sect2                . ,ignore)
      (sect3                . ,ignore)
      (sect4                . ,ignore)
      (internal             . ,delete)
      (*default*            . ,(lambda (tag . kids) `(,tag ,@kids)))
      (*text*               . ,(lambda (_ txt) txt))))))

(define* (make-description-texinfo sxml #:key (formatter #f) (remove-para? #f))
  (define (unbox lst)
    (define (unbox-inner ls)
      (match ls
        ((val) (unbox-inner val))
        (else ls)))
    (define unboxed-lst (unbox-inner lst))
    (cond
     ((null? unboxed-lst) '())
     ((eq? unboxed-lst 'span) '())
     (else (list unboxed-lst))))
  (unbox
   (pre-post-order
    sxml
    `((texinfo              . ,ignore)
      (node                 . ,ignore)
      (*TOP*                . ,ignore)
      (section              . ,delete)
      (anchor               . ,(replace-with 'a))
      (para                 . ,(replace-with (if remove-para? 'span 'p)))
      (code                 . ,(lambda (tag . kids)
                                 `((code (@ (class "text-dark"))
                                         ,@(if (not (null? ((sxpath '(@)) (cons tag kids))))
                                               (cdr kids) kids)))))
      (math                 . ,(lambda (tag . kids)
                                 (let* ((sxml (cons tag kids))
                                        (text ((sxpath '(*text*)) sxml))
                                        (str (if (null? text) "" (car text))))
                                   `(script (@ (type "math/tex"))
                                            ,(escape-ampersand str)))))
      (verbatim             . ,(replace-with 'pre))
      (ref *preorder*       . ,(lambda (tag . kids)
                                 (let* ((sxml (cons tag kids))
                                        (node ((sxpath '(@ node *text*)) sxml))
                                        (href (string-append
                                               "#" (if (null? node) "" (car node))))
                                        (name (if (null? node) ""
                                                  (last (string-split (car node) #\ )))))
                                   `(a (@ (href ,href)) ,name))))
      (defun                . ,(lambda (tag . kids)
                                 (let ((name ((sxpath '(name *text*)) (car kids)))
                                       (arguments ((sxpath '(arguments *text*)) (car kids))))
                                   `((h5 ,(if (null? name) "" (car name))
                                         " "
                                         (span (@ (class "text-muted"))
                                               ,(if (null? arguments) "" (car arguments))))
                                     ,@(cdr kids)))))
      (table *preorder*     . ,(lambda (tag . kids)
                                 (let ((formatter ((sxpath '(formatter *text*)) (car kids))))
                                   `(table (@ (class "table table-sm"))
                                           ,@(make-description-texinfo
                                              (cdr kids)
                                              #:formatter
                                              (lambda (children)
                                                (cond
                                                 ((null? formatter) `((span ,children)))
                                                 ((string= (car formatter) "code")
                                                  `((code (@ (class "text-dark"))
                                                          ,children)))
                                                 (else `((span ,children)))))
                                              #:remove-para? #t)))))
      (entry                . ,(lambda (tag . kids)
                                 (let ((heading ((sxpath '(heading *text*)) (car kids))))
                                   `(tr
                                     (td (@ (class "text-nowrap"))
                                      ,@(if formatter (formatter heading) heading))
                                     (td ,@(make-description-texinfo (cdr kids)))))))
      (*default*            . ,(lambda (tag . kids) `(,tag ,@kids)))
      (*text*               . ,(lambda (_ txt) txt))))))

(define* (make-description-man sxml #:key (delete-paragraphs? #f))
  (define (unbox lst)
    (define (unbox-inner ls)
      (match ls
        ((val) (unbox-inner val))
        (else ls)))
    (define unboxed-lst (unbox-inner lst))
    (cond
     ((null? unboxed-lst) '())
     ((eq? unboxed-lst 'span) '())
     (else (list unboxed-lst))))
  (unbox
   (pre-post-order
    sxml
    `((*TOP*                . ,ignore)
      (body                 . ,ignore)
      (p                    . ,(if delete-paragraphs?
                                   ignore
                                   (lambda (tag . kids) `(,tag ,@kids))))
      (h1                   . ,delete)
      (h2                   . ,(lambda (tag . kids)
                                 (if (and (list? kids)
                                          (= (length kids) 2)
                                          (string? (last kids)))
                                     `(h4 ,(string-titlecase (last kids)))
                                     `(h4 ,@kids))))
      (hr                   . ,delete)
      (br                   . ,delete)
      (colgroup             . ,delete)
      (table                . ,(lambda (tag . kids)
                                 `(table (@ (class "table table-sm")) ,@(cdr kids))))
      (a                    . ,(lambda (tag . kids)
                                 (let ((href ((sxpath '(href *text*)) (car kids))))
                                   (if (and (not (null? href)) (string-prefix? "#" (car href)))
                                       '()
                                       `(,tag ,@kids)))))
      (td *preorder*        . ,(lambda (tag . kids)
                                 `(,tag
                                    ,@(make-description-man kids #:delete-paragraphs? #t))))
      (@                    . ,(lambda (tag . kids)
                                 (if (and (not (null? kids))
                                          (eq? (caar kids) 'href))
                                     `(,tag ,@kids)
                                     `(,tag))))
      (*default*            . ,(lambda (tag . kids) `(,tag ,@kids)))
      (*text*               . ,(lambda (_ txt) txt))))))

(define (remove-namespaces sxml)
  (pre-post-order
   sxml
   `((*default*            . ,(lambda (tag . kids)
                                (let ((str (symbol->string tag)))
                                  (if (string-contains str ":")
                                      `(,(string->symbol (second (string-split str #\:)))
                                         ,@kids)
                                      `(,tag ,@kids)))))
     (*text*               . ,(lambda (_ txt) txt)))))

(define (make-argument sxml)
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  (make <argument>
    #:type (sxml-value '(type) '())
    #:name (sxml-value '(declname *text*) '())
    #:default-name (sxml-value '(defname *text*) '())
    #:default-value (make-linked-text ((sxpath '(defval)) sxml))
    #:description (make-description ((sxpath '(briefdescription)) sxml))))

(define-class <reference> ()
  (refid #:init-keyword #:refid #:accessor reference-refid)
  (name #:init-keyword #:name #:accessor reference-name)
  (protection #:init-keyword #:protection #:accessor reference-protection)
  (virtual #:init-keyword #:virtual #:accessor reference-virtual)
  (local? #:init-keyword #:local? #:accessor reference-local?)
  (ambiguityscope #:init-keyword #:ambiguityscope #:accessor reference-ambiguityscope)
  (scope #:init-keyword #:scope #:accessor reference-scope))

(define-method (equal? (a <reference>) (b <reference>) out)
  (let ((refid-a (reference-refid a)) (refid-b (reference-refid b)))
    (and (string? refid-a) (string? refid-b) (string= refid-a refid-b))))

(define (make-reference sxml)
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  (define (sxml-bool path default-value)
    (let ((value (sxml-value path '())))
      (cond
       ((null? value) default-value)
       ((string= value "yes") #t)
       (#t default-value))))
  (make <reference>
    #:refid (sxml-value '(@ refid *text*) '())
    #:name (match (append ((sxpath '(*text*)) sxml) ((sxpath '(name *text*)) sxml))
                  (() '())
                  ((val) val))
    #:scope (sxml-value '(scope *text*) '())
    #:protection (sxml-value '(@ prot *text*) '())
    #:virtual (sxml-value '(@ virt *text*) '())
    #:ambiguityscope (sxml-value '(@ ambiguityscope *text*) '())
    #:local? (sxml-bool '(@ local *text*) '())))

(define-class <location> ()
  (file #:init-keyword #:file #:accessor location-file #:init-value '())
  (line #:init-keyword #:line #:accessor location-line)
  (column #:init-keyword #:column #:accessor location-column)
  (body-file #:init-keyword #:body-file #:accessor location-body-file)
  (body-start #:init-keyword #:body-start #:accessor location-body-start)
  (body-end #:init-keyword #:body-end #:accessor location-body-end))

(define-method (equal? (a <location>) (b <location>) out)
  (let ((file-a (location-file a)) (file-b (location-file b)))
    (and (string? file-a) (string? file-b) (string= file-a file-b))))

(define (make-location sxml)
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  (make <location>
    #:file (sxml-value '(@ file *text*) '())
    #:line (sxml-value '(@ line *text*) '())
    #:column (sxml-value '(@ column *text*) '())
    #:body-file (sxml-value '(@ column *text*) '())
    #:body-start (sxml-value '(@ bodystart *text*) '())
    #:body-end (sxml-value '(@ bodyend *text*) '())))

(define-class <enum-value> ()
  (id #:init-keyword #:id #:accessor enum-value-id)
  (protection #:init-keyword #:protection #:accessor enum-value-protection)
  (name #:init-keyword #:name #:accessor enum-value-name)
  (initialiser #:init-keyword #:initialiser #:accessor enum-value-initialiser)
  (short-description #:init-keyword #:short-description
                     #:accessor enum-value-short-description)
  (description #:init-keyword #:description #:accessor enum-value-description))

(define-method (equal? (a <enum-value>) (b <enum-value>) out)
  (let ((id-a (enum-value-id a)) (id-b (enum-value-id b)))
    (and (string? id-a) (string? id-b) (string= id-a id-b))))

(define (make-enum-value sxml)
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  (make <enum-value>
    #:id (sxml-value '(@ id *text*) '())
    #:protection (sxml-value '(@ prot *text*) '())
    #:name (sxml-value '(name *text*) '())
    #:initialiser (make-linked-text ((sxpath '(initializer)) sxml))
    #:short-description (make-description ((sxpath '(briefdescription)) sxml))
    #:description (make-description ((sxpath '(detaileddescription)) sxml))))

(define-class <definition> ()
  (id #:init-keyword #:id #:accessor definition-id)
  (name #:init-keyword #:name #:accessor definition-name)
  (kind #:init-keyword #:kind #:accessor definition-kind)
  (language #:init-keyword #:language #:accessor definition-language)
  (protection #:init-keyword #:protection #:accessor definition-protection
              #:init-value "public")
  (title #:init-keyword #:title #:accessor definition-title)
  (refqual #:init-keyword #:refqual #:accessor definition-refqual)
  (virtual #:init-keyword #:virtual #:accessor definition-virtual)
  (type #:init-keyword #:type #:accessor definition-type #:init-value '())
  (definition #:init-keyword #:definition #:accessor definition-definition #:init-value '())
  (argsstring #:init-keyword #:argsstring #:accessor definition-argsstring #:init-value '())
  (template-arguments #:init-keyword #:template-arguments #:accessor
                       definition-template-arguments #:init-value '())
  (arguments #:init-keyword #:arguments #:accessor definition-arguments #:init-value '())
  (return #:init-keyword #:return #:accessor definition-return #:init-value '())
  (enum-values #:init-keyword #:enum-values #:accessor definition-enum-values #:init-value '())
  (attributes #:init-keyword #:attributes #:accessor definition-attributes #:init-value '())
  (children #:init-keyword #:children #:accessor definition-children #:init-value '())
  (parents #:init-keyword #:parents #:accessor definition-parents #:init-value '())
  (simple-sections #:init-keyword #:simple-sections #:accessor definition-simple-sections
                   #:init-value '())
  (base-classes #:init-keyword #:base-classes #:accessor definition-base-classes
                #:init-value '())
  (derived-classes #:init-keyword #:derived-classes #:accessor definition-derived-classes
                   #:init-value '())
  (includes #:init-keyword #:includes #:accessor definition-includes)
  (included-by #:init-keyword #:included-by #:accessor definition-included-by)
  (members #:init-keyword #:members #:accessor definition-members #:init-value '())
  (member-of #:init-keyword #:member-of #:accessor definition-member-of #:init-value '())
  (short-description #:init-keyword #:short-description
                     #:accessor definition-short-description
                     #:init-value '())
  (description #:init-keyword #:description #:accessor definition-description
               #:init-value '())
  (inbody-description #:init-keyword #:inbody-description
                     #:accessor definition-inbody-description
                     #:init-value '())
  (location #:init-keyword #:location #:accessor definition-location #:init-value '())
  (files #:init-keyword #:files #:accessor definition-files #:init-value '())
  (output-path #:init-keyword #:output-path #:accessor definition-output-path
               #:init-value '()))

(define-method (equal? (a <definition>) (b <definition>) out)
  (let ((id-a (definition-id a)) (id-b (definition-id b)))
    (and (string? id-a) (string? id-b) (string= id-a id-b))))

(define-method (write (o <definition>) out) (format out "<~a>" (definition-id o)))

(define* (make-definition sxml #:optional (parents '()))
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  (define (sxml-bool path default-value)
    (let ((value (sxml-value path '())))
      (cond
       ((null? value) default-value)
       ((string= value "yes") #t)
       (#t default-value))))
  (define (sxml-attr path default-value name)
    (if (sxml-bool path default-value) (list name) '()))
  (define kind (sxml-value '(@ kind *text*) '()))
  (make <definition>
    #:id (sxml-value '(@ id *text*) '())
    #:kind kind
    #:language (sxml-value '(@ kind *text*) '())
    #:protection (sxml-value '(@ prot *text*) "")
    #:refqual (sxml-value '(@ refqual *text*) '())
    #:virtual (sxml-value '(@ virt *text*) '())
    #:type (sxml-value '(type) '())
    #:definition ((sxpath '(definition *text*)) sxml)
    #:argsstring ((sxpath '(argsstring *text*)) sxml)
    #:template-arguments (map make-argument ((sxpath '(templateparamlist param)) sxml))
    #:arguments (map make-argument ((sxpath '(param)) sxml))
    #:enum-values (map make-enum-value ((sxpath '(enumvalue)) sxml))
    #:simple-sections ((sxpath '(detaileddescription // simplesect)) sxml)
    #:attributes
    `(,@(sxml-attr '(@ final *text*) #f 'final)
      ,@(sxml-attr '(@ sealed *text*) #f 'sealed)
      ,@(sxml-attr '(@ abstract *text*) #f 'abstract)
      ,@(sxml-attr '(@ static *text*) #f 'static)
      ,@(sxml-attr '(@ strong *text*) #f 'strong)
      ,@(sxml-attr '(@ const *text*) #f 'const)
      ,@(sxml-attr '(@ explicit *text*) #f 'explicit)
      ,@(sxml-attr '(@ inline *text*) #f 'inline)
      ,@(sxml-attr '(@ volatile *text*) #f 'volatile)
      ,@(sxml-attr '(@ mutable *text*) #f 'mutable)
      ,@(sxml-attr '(@ readable *text*) #f 'readable)
      ,@(sxml-attr '(@ writable *text*) #f 'writable))
    #:children
    (append-map
     (lambda (path) (map make-reference ((sxpath path) sxml)))
     '((innerdir) (innerclass) (innernamespace) (innerpage) (innergroup)
                  ;;(listofallmembers member)
                  ))
    #:files (map make-reference ((sxpath '(innerfile)) sxml))
    #:base-classes (map make-reference ((sxpath '(basecompoundref)) sxml))
    #:derived-classes (map make-reference ((sxpath '(derivedcompoundref)) sxml))
    #:includes (map make-reference ((sxpath '(includes *text*)) sxml))
    #:included-by (map make-reference ((sxpath '(includedby *text*)) sxml))
    #:short-description
    (append
      (make-description ((sxpath '(briefdescription)) sxml))
      (make-description ((sxpath '(detaileddescription // short-description)) sxml)))
    #:description (make-description ((sxpath '(detaileddescription)) sxml)
                                    #:delete-short-description? #t)
    #:inbody-description (make-description ((sxpath '(inbodydescription)) sxml))
    #:location (make-location ((sxpath '(location)) sxml))
    #:name (string-join (append ((sxpath '(compoundname *text*)) sxml)
                                ((sxpath '(name *text*)) sxml)) "")
    #:title (sxml-value '(title *text*) "")
    #:parents parents))

(define (process-index sxml)
  (for-each
   (lambda (objects)
     (for-each
      (lambda (object) (hash-set! declarations (declaration-refid object) object))
      objects))
   (map
    (lambda (sxml)
      (define parent (make-declaration sxml))
      (cons parent
            (map (lambda (sxml) (make-declaration sxml (list parent)))
                 ((sxpath '(member)) sxml))))
    ((sxpath '(doxygenindex compound)) sxml))))

(define (process-doxyfile doxygen-file)
  (define (->symbol key)
    (string->symbol
     (string-map
      (lambda (ch) (if (char=? ch #\_) #\- ch))
      (string-downcase key))))
  (define text (call-with-input-file doxygen-file get-string-all))
  (filter
   (lambda (pair) (not (null? pair)))
   (map
    (lambda (line)
      (define i (string-index line #\=))
      (cond
       ((string-null? line) '())
       ((string-contains line "+=") #f)
       (i
        (cons (->symbol (string-trim-both (substring line 0 i)))
              (string-trim-both (substring line (+ 1 i)))))
       (else (throw 'bad-doxyfile (format #f "failed to parse: ~a" line)))))
    (string-split text #\newline))))

(define (process-index-page sxml)
  (car (map make-definition ((sxpath '(doxygen compounddef)) sxml))))

(define (process-definitions sxml filename)
  (for-each
   (lambda (objects)
     (for-each
      (lambda (object)
        (let ((existing-object (hash-ref definitions (definition-id object))))
          (hash-set! definitions (definition-id object) object)))
      objects))
   (map
    (lambda (sxml)
      (define parent (make-definition sxml))
      (cons parent
            (map (lambda (sxml)
                   (define child (make-definition sxml (list parent)))
                   (set! (definition-children parent)
                         (cons child (definition-children parent)))
                   child)
                 ((sxpath '(sectiondef memberdef)) sxml))))
    ((sxpath '(doxygen compounddef)) sxml))))

(define (process-texinfo sxml)
  (define (sxml-value path default-value)
    (match ((sxpath path) sxml)
      (() default-value)
      ((val) val)))
  ;;(define title (sxml-value '(texinfo @ title *text*) ""))
  ;;(define name (sxml-value '(texinfo node @ name *text*) ""))
  (define def
    (make <definition>
      #:id "texinfo"
      #:kind "page"
      #:language "Scheme"
      #:short-description '((span "List of all Scheme procedures."))
      #:description (make-description-texinfo sxml)
      #:name "scheme"
      #:title "Scheme reference"
      #:location (make <location>)))
  (hash-set! definitions (definition-id def) def))

(define (process-man)
  (define dir (opendir man-output-dir))
  (define parent (hash-ref definitions "group__commands"))
  (do ((entry (readdir dir) (readdir dir)))
      ((eof-object? entry))
    (if (and (not (string-prefix? "." entry)) (string-suffix? ".html" entry))
        (let* ((sxml (call-with-input-file
                         (string-append man-output-dir "/" entry)
                       (lambda (port)
                         (remove-namespaces
                          (xml->sxml port
                                     #:trim-whitespace? #t
                                     #:namespaces
                                     '((ns . "http://www.w3.org/1999/xhtml"))
                                     #:declare-namespaces? #t
                                     #:entities `((copy . ,(string #\xA9))
                                                  (minus . "-")
                                                  (rsquo . "'")))))))
               (short-desc ((sxpath '(html body p)) sxml))
               (name
                (string-downcase
                 (car ((sxpath '(html head title *text*)) sxml))))
               (def (make <definition>
                      #:id entry
                      #:kind "page"
                      #:language "man"
                      #:short-description
                      (make-description-man (second short-desc) #:delete-paragraphs? #t)
                      #:description (make-description-man ((sxpath '(html body)) sxml))
                      #:name name
                      #:title name
                      #:parent parent
                      #:location (make <location>))))
          (set! (definition-children parent) (cons def (definition-children parent)))
          (hash-set! definitions (definition-id def) def))))
  (closedir dir))

(define (process-images)
  (define dir (opendir assets-output-dir))
  (define output-path (string-append output-directory "/assets"))
  (mkdir-p output-path)
  (do ((entry (readdir dir) (readdir dir)))
      ((eof-object? entry))
    (let ((path (string-append assets-output-dir "/" entry)))
      (if (and (not (string-prefix? "." entry))
               (eq? (stat:type (stat path #t)) 'regular))
          (copy-file path (string-append output-path "/" entry)))))
  (closedir dir))

(define (doxyfile-tag-files)
  (let ((tag-files (assoc-ref doxyfile 'tagfiles)))
    (if tag-files
        (map
         (lambda (value)
           (let ((lst (string-split value #\=)))
             (cons (first lst) (second lst))))
         (list tag-files))
        '())))

(define (post-process-locations)
  (define prefix-length
    (cdr (reduce
           (lambda (a b)
             (let* ((str-a (car a)) (str-b (car b)) (len-a (cdr a)) (len-b (cdr b)))
               (cons str-a (min (string-prefix-length str-a str-b) len-a len-b))))
           0
           (map
             (lambda (str) (cons str (string-length str)))
             (filter
               (lambda (file)
                 (and (string? file)
                      (not (string-null? file))
                      (not (member file (map car (doxyfile-tag-files))))))
               (hash-map->list
                 (lambda (key value) (location-file (definition-location value)))
                 definitions))))))
  (hash-for-each
    (lambda (key value)
      (let ((old-file (location-file (definition-location value))))
        (set! (definition-output-path value)
          (cond
            ((member old-file (map car (doxyfile-tag-files)))
             "")
            ((string= (definition-kind value) "group")
             (string-append "modules/" (definition-name value) ".html"))
            ((string= (definition-kind value) "page")
             (if (string= (definition-id value) "indexpage")
               "index.html"
               (string-append "pages/" (definition-name value) ".html")))
            ((and (string? old-file) (not (string-null? old-file)))
             (if (string= (definition-kind value) "dir")
               (string-append "files/" (substring old-file prefix-length) "index.html")
               (string-append "files/" (substring old-file prefix-length) ".html")))
            (else "")))))
    definitions)
  (format #t "LCP: ~a\n" prefix-length))

(define (index-file-definitions)
  (hash-for-each
    (lambda (key value)
      (if (and (not (member (definition-kind value) '("dir")))
               (string? (definition-output-path value))
               (not (string-null? (definition-output-path value))))
          (let ((path (definition-output-path value)))
            (if (not (hash-ref definitions-by-file path))
                (hash-set! definitions-by-file path '()))
            (hash-set! definitions-by-file path
                       (cons value (hash-ref definitions-by-file path))))))
    definitions))

(define (index-namespace-definitions)
  (hash-for-each
   (lambda (key value)
     (define namespaces
       (filter
        (lambda (x) (string= (definition-kind x) "namespace"))
        (definition-parents value)))
     (for-each
      (lambda (ns)
        (if (not (hash-ref definitions-by-namespace ns))
            (hash-set! definitions-by-namespace ns '()))
        (hash-set! definitions-by-namespace ns
                   (cons value (hash-ref definitions-by-namespace ns))))
      namespaces))
   definitions))

(define (index-anchors)
  (hash-for-each
   (lambda (key value)
     (for-each
      (lambda (anchor) (hash-set! anchor-path anchor (definition-output-path value)))
      (append
       ((sxpath '(// a @ name *text*))
        (cons '*TOP* (definition-short-description value)))
       ((sxpath '(// a @ name *text*))
        (cons '*TOP* (definition-description value))))))
   definitions))

(define (plug-members)
  (hash-for-each
   (lambda (key value)
     (set! (definition-children value)
           (delete-duplicates
            (filter
             (lambda (def) (is-a? def <definition>))
             (map
              (lambda (ref)
                (if (is-a? ref <reference>)
                    (let ((object (hash-ref definitions (reference-refid ref))))
                      (if object object ref))
                    ref))
              (definition-children value))))))
   definitions))

(define (remove-anonymous-members)
  (hash-for-each
   (lambda (key value)
     (if (string-prefix? "@" (definition-name value))
         (begin
           (hash-remove! definitions key)
           (for-each
            (lambda (child)
              (hash-remove! definitions
                            (cond
                             ((is-a? child <reference>) (reference-refid child))
                             (else (definition-id child)))))
            (definition-children value)))))
   definitions))

(define (remove-src-dir)
  (hash-for-each
   (lambda (key value)
     (if (and (string= "dir" (definition-kind value))
              (string= "src" (definition-name value)))
         (hash-remove! definitions key)))
   definitions))

(define (namespace-components ns)
  (define str (definition-name ns))
  (filter (lambda (s) (not (string-null? s))) (string-split str #\:)))

(define (namespace-basename ns) (car (reverse (namespace-components ns))))

(define (namespace-href ns)
  (string-join (cons "namespaces" (namespace-components ns)) "/" 'prefix))

(define (definition-href def)
  (define kind (definition-kind def))
  (define path (definition-output-path def))
  (cond
   ((string= kind "namespace") (namespace-href def))
   ((string= kind "dir")
    (string-append "/" (if (null? path) "" (string-append (dirname path) "/"))))
   ((member kind '("group" "page"))
    (string-append "/" (if (null? path) "" path)))
   (else
    (string-append "/" (if (null? path) "" path) "#" (definition-id def)))))

(define (resolve-references sxml)
  (define tag-files (doxyfile-tag-files))
  (pre-post-order
   sxml
   `((ref       . ,(lambda (tag . kids)
                     (define refid
                       (match ((sxpath '(refid *text*)) (car kids))
                         (() '())
                         ((val) val)))
                     (define external
                       (match ((sxpath '(external *text*)) (car kids))
                         (() '())
                         ((val) val)))
                     (define href-1
                       (let ((def (hash-ref definitions refid)))
                         (if def (definition-href def) '())))
                     (define href-2
                       (let ((path (hash-ref anchor-path refid)))
                         (if path (string-append "/" path "#" refid) '())))
                     (define href-3
                       (let ((prefix (assoc-ref tag-files external)))
                         (if prefix (string-append prefix refid) '())))
                     (define href
                       (cond
                        ((not (null? href-1)) href-1)
                        ((not (null? href-2)) href-2)
                        ((not (null? href-3)) href-3)
                        (else "")))
                     (if (and (null? href-1)
                              (null? href-2)
                              (null? href-3)
                              (not (string-null? refid)))
                         (format #t "~a\n" (cons tag kids)))
                     `(a (@ (href ,href)) ,(cdr kids))))
     (a         . ,(lambda (tag . kids)
                     (define name
                       (match ((sxpath '(href *text*)) (car kids))
                         (() '())
                         ((val) val)))
                     (define href
                       (let ((path (hash-ref anchor-path name)))
                         (if path (string-append "/" path "#" name) '())))
                     (if (null? href) (cons tag kids)
                         `(a (@ (href ,href)) ,(cdr kids)))))
     (*default* . ,(lambda (tag . kids) `(,tag ,@kids)))
     (*text*    . ,(lambda (_ txt) txt)))))

(define (generate-page title body)
  (resolve-references
   `(html (@ (lang "en"))
          (head
           (meta (@ (charset "utf-8")))
           (meta (@ (name "viewport")
                    (content "width=device-width, initial-scale=1, shrink-to-fit=no")))
           (title ,title)
           (link (@ (rel "stylesheet")
                    (href "/bootstrap.min.css")
                    (integrity "sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm")))
           (link (@ (rel "stylesheet") (href "/katex.min.css")))
           (link (@ (rel "stylesheet") (href "/scheme.css"))))
          (body
           (div (@ (class "container"))
                (nav (@ (class "navbar navbar-expand-lg navbar-light bg-light"))
                     (a (@ (class "navbar-brand") (href "/"))
                        ,(definition-title project))
                     (ul (@ (class "navbar-nav"))
                         (li (@ (class "nav-item"))
                             (a (@ (class "nav-link") (href "/modules/"))
                                "Documentation"))
                         ;;(li (@ (class "nav-item"))
                         ;;    (a (@ (class "nav-link") (href "/pages/"))
                         ;;       "Pages"))
                         (li (@ (class "nav-item"))
                             (a (@ (class "nav-link") (href "/namespaces/"))
                                "Namespaces"))
                         (li (@ (class "nav-item"))
                             (a (@ (class "nav-link") (href "/files/"))
                                "Files"))
                         ))
                ,@body
                (script (@ (type "text/javascript") (defer "")
                           (src "/katex.min.js")))
                (script (@ (type "text/javascript") (defer "")
                           (src "/mathtex-script-type.min.js"))))))))

(define (display-page sxml port)
  (display
   (unescape-ampersand
    (sxml->html-string `((doctype html) ,sxml))) port))

(define (mkdir-p dir)
  (define d "")
  (for-each
    (lambda (component)
      (set! d (string-append d component "/"))
      (if (not (file-exists? d)) (mkdir d)))
    (string-split dir #\/)))

(define (write-page local-path title body)
  (define path (string-append output-directory "/" local-path))
  (mkdir-p (dirname path))
  (call-with-output-file path
    (lambda (port)
      (display-page (generate-page title body) port))))

(define (file-name str)
  (if (string-suffix? ".html" str)
      (substring str 0 (- (string-length str) 5))
      str))

(define (scan-path-components components)
  (fold
   (lambda (component prev)
     (cons
      (string-append (if (null? prev) "" (car prev)) "/" component)
      prev))
   '()
   components))

(define option-spec
  `((doxygen (required? #t) (value #t) (predicate ,file-exists?))
    (texinfo (required? #f) (value #t) (predicate ,file-exists?))
    (man (required? #f) (value #t) (predicate ,file-exists?))
    (assets (required? #f) (value #t) (predicate ,file-exists?))
    (doxyfile (required? #f) (value #t) (predicate ,file-exists?))
    (output (single-char #\o) (required? #t) (value #t))))

(define options (getopt-long (command-line) option-spec))

(define doxygen-file (option-ref options 'doxyfile #f))
(define doxygen-output-dir (option-ref options 'doxygen #f))
(define texinfo-output-dir (option-ref options 'texinfo #f))
(define man-output-dir (option-ref options 'man #f))
(define assets-output-dir (option-ref options 'assets #f))
(define output-directory (option-ref options 'output #f))

(format #t "Doxyfile: ~a\n" doxygen-file)
(format #t "Doxygen directory: ~a\n" doxygen-output-dir)
(format #t "TexInfo directory: ~a\n" texinfo-output-dir)
(format #t "Man directory: ~a\n" man-output-dir)
(format #t "Assets directory: ~a\n" assets-output-dir)
(format #t "Output directory: ~a\n" output-directory)

(chdir doxygen-output-dir)

(define declarations (make-hash-table))
(define definitions (make-hash-table))
(define definitions-by-file (make-hash-table))
(define definitions-by-namespace (make-hash-table))
(define anchor-path (make-hash-table))

(define doxyfile (process-doxyfile doxygen-file))
;;(format #t "Doxyfile: \n")
;;(pretty-print doxyfile)

(define project (process-index-page
 (call-with-input-file "indexpage.xml"
   (lambda (port)
     (xml->sxml port #:trim-whitespace? #f)))))

(format #t "Project: ~a\n" (definition-title project))

(process-index
 (call-with-input-file "index.xml"
   (lambda (port)
     (xml->sxml port #:trim-whitespace? #f))))

(format #t "Compounds: ~a\n" (hash-count (const #t) declarations))

(hash-for-each
 (lambda (refid decl)
   (define filename (string-append refid ".xml"))
   (if (and (null? (declaration-parents decl)) (file-exists? filename))
       (let ((sxml (call-with-input-file filename
                     (lambda (port)
                       (xml->sxml port #:trim-whitespace? #f)))))
         (process-definitions sxml filename))))
 declarations)

(format #t "Compounddefs: ~a\n" (hash-count (const #t) definitions))

(if texinfo-output-dir
    (process-texinfo
     (call-with-input-file (string-append texinfo-output-dir "/texinfo.xml")
       (lambda (port)
         (xml->sxml port #:trim-whitespace? #f)))))

(if man-output-dir (process-man))
(if assets-output-dir (process-images))
(remove-anonymous-members)
;;(remove-src-dir)
(post-process-locations)
(index-file-definitions)
(index-namespace-definitions)
(index-anchors)
(plug-members)


(format #t "Files: ~a\n" (hash-count (const #t) definitions-by-file))
(format #t "Namespaces: ~a\n" (hash-count (const #t) definitions-by-namespace))

(define (description->html def)
  (define kind (definition-kind def))
  (define elems
    (cond
     ((member kind '("group" "page")) (definition-description def))
     (else
      (append (definition-short-description def)
              (definition-description def)
              (definition-inbody-description def)))))
  (if (null? elems)
      '()
      `((p ,@elems))))

(define (remove-class-members all-definitions)
  (define members
    (append-map
     definition-children
     (filter (lambda (def) (member (definition-kind def) '("class" "struct" "union")))
             all-definitions)))
  (filter (lambda (def) (not (member def members))) all-definitions))

(define (remove-private-members all-definitions)
  (filter
   (lambda (def)
     (or (not (string? (definition-protection def)))
         (not (string= (definition-protection def) "private"))))
   all-definitions))

(define (group-by-kind all-definitions kinds)
  (define lst (map (lambda (kind) (cons kind '())) kinds))
  (for-each
   (lambda (def)
     (let* ((kind (definition-kind def))
            (existing (assoc-ref lst kind)))
       (if existing
           (assoc-set! lst kind (append existing (list def)))
           (assoc-set! lst kind (list def)))))
   all-definitions)
  (filter (lambda (pair) (not (null? (cdr pair)))) lst))

(define (null-nested? lst) (or (null? lst) (equal? lst '(()))))

(define (kind-plural kind def)
  (define class? (member (definition-kind def) '("class" "struct" "union")))
  (cond
    ((string= "typedef" kind) "Types")
    ((string= "variable" kind) (if class? "Fields" "Variables"))
    ((string= "function" kind) (if class? "Methods" "Functions"))
    ((string= "friend" kind) "Friends")
    ((string= "class" kind) "Classes")
    ((string= "struct" kind) "Structs")
    ((string= "union" kind) "Unions")
    ((string= "namespace" kind) "Namespaces")
    ((string= "enum" kind) "Enumerations")
    ((string= "group" kind) "Groups")
    ((string= "page" kind) "Pages")
    (else kind)))

(define (sort-by-kind defs order)
  (sort-list
   defs
   (lambda (a b)
     (let* ((kind-a (definition-kind a)) (kind-b (definition-kind b))
            (idx-a (list-index (lambda (x) (string= x kind-a)) order))
            (idx-b (list-index (lambda (x) (string= x kind-b)) order)))
       (and idx-a idx-b (< idx-a idx-b))))))

(define (make-type type)
  (pre-post-order
   type
   `((ref . ,(lambda (tag . kids)
               (let* ((id (car ((sxpath '(refid *text*)) (car kids))))
                      (name (cdr kids))
                      (object (hash-ref definitions id)))
                 (if object
                     `((a (@ (href ,(definition-href object)))
                          ,name))
                     `(,name)))))
     (type      . ,(lambda (tag . kids) kids))
     (*default* . ,(lambda (tag . kids) `(,tag ,@kids)))
     (*text*    . ,(lambda (_ txt) txt)))))

(define (arguments->sxml arguments)
  (map
   (lambda (arg)
     (append
      (list (make-type (argument-type arg))
            (if (not (null? (argument-name arg)))
                (list " " (argument-name arg))
                '())
            (if (not (null? (argument-default-value arg)))
                (list " = " (argument-default-value arg))
                '()))))
   arguments))

(define (template-arguments->sxml template-args)
  (if (null? template-args)
      '()
      `((code (@ (class "text-muted"))
              ,@(append
                 (list "template <")
                 (car template-args)
                 (append-map (lambda (arg) (cons ", " arg)) (cdr template-args))
                 (list "> ")))
        (br))))

(define* (make-title def #:key (ref? #f))
  (define (wrap-link lst)
    (if ref? `((a (@ (href ,(definition-href def))) ,lst)) lst))
  (cond
   ((is-a? def <reference>) (reference-name def))
   ;;((not (null? (definition-definition def)))
   ;; (list (definition-definition def) (definition-argsstring def)))
   (else
    (let ((args (arguments->sxml (definition-arguments def)))
          (template-args (arguments->sxml (definition-template-arguments def)))
          (kind (definition-kind def)))
      (append
       `((a (@ (name ,(definition-id def)))))
       (template-arguments->sxml template-args)
       (cond
        ((string= kind "typedef")
         `((code (@ (class "text-dark"))
                 ,@(list "using " (wrap-link (definition-name def))
                         " = " (make-type (definition-type def))))))
        ((string= kind "function")
         `((code (@ (class "text-dark"))
                 ,@(append
                    (list (wrap-link (definition-name def)))
                    (list "(")
                    (if (null? args)
                        args
                        (append
                         (car args)
                         (append-map (lambda (arg) (cons ", " arg)) (cdr args))))
                    (list ")")
                    (if (memq 'const (definition-attributes def)) (list " const") '())
                    (let ((type (make-type (definition-type def))))
                      (if (not (null? type)) (list " -> " type) '()))
                    `((span (@ (class "float-right"))
                            ,@(map
                               (lambda (attr)
                                 `((span (@ (class "badge badge-secondary ml-1"))
                                         ,(symbol->string attr))))
                               (filter
                                (lambda (attr) (memq attr (definition-attributes def)))
                                '(explicit mutable volatile)))
                            ,@(if (string= (definition-protection def) "protected")
                                  `((span (@ (class "badge badge-dark ml-1")) "protected"))
                                  '())
                            ,@(if (not (string= (definition-virtual def) "non-virtual"))
                                  `((span (@ (class "badge badge-info ml-1")) "virtual"))
                                  '())))))))
        ((string= kind "variable")
         `((code (@ (class "text-dark"))
            ,@(list (make-type (definition-type def)) " "
                    (wrap-link (list (definition-name def) (definition-argsstring def)))))))
        ((member kind '("class" "struct" "union"))
         `((code (@ (class "text-dark")) ,@(list kind " " (wrap-link (definition-name def))))))
        ((string= kind "enum")
         `((code (@ (class "text-dark"))
                 ,@(list kind " " (wrap-link (definition-name def))
                         (let ((type (make-type (definition-type def))))
                           (if (not (null? type)) (list ": " type) '()))))))
        ((string= kind "namespace")
         `((code (@ (class "text-dark"))
                 ,@(list "namespace " (wrap-link (namespace-basename def))))))
        ((string= kind "group")
         (list (wrap-link (definition-title def))))
        ((string= kind "page")
         (list (wrap-link (definition-title def))))
        ((string= kind "dir")
         (list (wrap-link (basename (dirname (definition-output-path def))))))
        (else (list (wrap-link (definition-name def))))))))))

(define (simple-section->html def)
  (make-description (definition-simple-sections def) #:delete-simplesect? #f))

(define (namespace-members->html members def)
  (map
   (lambda (pair)
     (let ((kind (car pair))
           (children (cdr pair)))
       `((h5 (@ (class "mb-1 mt-4"))
             ,(kind-plural kind def))
         (ul (@ (class "list-group list-group-flush"))
             ,@(map
                (lambda (child)
                  (let ((sections (simple-section->html child))
                        (description (definition-short-description child)))
                    `((li (@ (class "list-group-item py-1"))
                          (div (@ (class "row"))
                               (div (@ (class "col-sm-6"))
                                    ,@(make-title child #:ref? #t))
                               (div (@ (class "col-sm-6 d-flex align-items-end"))
                                    ,(if (null? description) '() `(,@description))))))))
                children)))))
   members))

(define all-directories
  (sort-list
   (hash-fold
    (lambda (key value result)
      (if (and (string= (definition-kind value) "dir")
               (not (string= (definition-name value) "site")))
          (cons value result) result))
    '()
    definitions)
   (lambda (a b) (string< (definition-output-path a) (definition-output-path b)))))

(define (directory->html dir)
  (define path-components (drop-right (string-split (definition-output-path dir) #\/) 1))
  (define breadcrumb-urls (drop-right (scan-path-components path-components) 1))
  (define prefix (dirname (definition-output-path dir)))
  (define subdirs
    (filter
     (lambda (dir)
       (define path (dirname (definition-output-path dir)))
       (string= (string-append prefix "/" (basename path)) path))
     all-directories))
  (define files
    (sort-list
     (definition-files dir)
     (lambda (a b) (string< (basename (reference-name a)) (basename (reference-name b))))))
  (write-page
   (definition-output-path dir)
   (definition-name dir)
   `(,@(if (null? breadcrumb-urls) '()
           `((nav (@ (aria-label "breadcrumb"))
                  (ol (@ (class "breadcrumb"))
                      (li (@ (class "breadcrumb-item") (aria-current "page"))
                          (a (@ (href "/files/")) "Files"))
                      ,@(map
                         (lambda (path)
                           `((li (@ (class "breadcrumb-item"))
                                 (a (@ (href ,path)) ,(basename path)))))
                         (reverse (cdr breadcrumb-urls)))
                      (li (@ (class "breadcrumb-item active") (aria-current "page"))
                          ,(basename (car breadcrumb-urls)))))))
     ,@(description->html dir)
     ,@(map
        (lambda (subdir)
          (let ((filename (basename (dirname (definition-output-path subdir)))))
            `((ul (@ (class "list-group"))
                  (li (@ (class "list-group-item py-1"))
                      (a (@ (href ,filename))
                         ,filename))))))
        subdirs)
     ,@(map
        (lambda (file)
          (let ((filename (basename (reference-name file))))
            `((ul (@ (class "list-group"))
                  (li (@ (class "list-group-item py-1"))
                      (a (@ (href ,(string-append filename ".html")))
                         ,filename))))))
        files))))

(define (directories->html dirs)
  `((nav (@ (aria-label "breadcrumb"))
         (ol (@ (class "breadcrumb"))
             (li (@ (class "breadcrumb-item active") (aria-current "page")) "Home")))
    (ul (@ (class "list-group"))
        ,@(map
           (lambda (dir)
             (let ((description (definition-short-description dir)))
               `((li (@ (class "list-group-item py-1"))
                     (div (@ (class "row"))
                          (div (@ (class "col-sm-6"))
                               ,@(make-title dir #:ref? #t))
                          (div (@ (class "col-sm-6 d-flex align-items-end"))
                               ,@(if (null? description) '() `(,@description))))))))
           (filter
            (lambda (dir)
              (let ((path (dirname (definition-output-path dir))))
                (string= (string-append "files/" (basename path)) path)))
            dirs)))))

(for-each directory->html all-directories)
(write-page "files/index.html" "Files" (directories->html all-directories))

(hash-for-each
 (lambda (path file-definitions)
   (define path-components (string-split path #\/))
   (define breadcrumb-urls (drop-right (scan-path-components path-components) 1))
   (define current-url (file-name (basename path)))
   (define (enum-value->html value)
     `((code (@ (class "text-dark"))
        ,(append
          (list (enum-value-name value))
          (if (enum-value-initialiser value)
              (list " " (enum-value-initialiser value))
              '())))))
   (define (members->html def)
     (let* ((kind (definition-kind def))
            (ref? (member kind '("group" "page"))))
       (cond
        ((string= kind "enum")
         `((ul (@ (class "list-group list-group-flush"))
               ,@(map
                  (lambda (value)
                    `((li (@ (class "list-group-item py-1"))
                          ,@(enum-value->html value))))
                  (definition-enum-values def)))))
        (else
         (if (and (not (null? (definition-children def)))
                  (not (member (definition-kind def) '("file"))))
             (map
              (lambda (pair)
                (let ((kind (car pair))
                      (children (cdr pair)))
                  `((h5 (@ (class "mb-1 mt-4"))
                        ,(kind-plural kind def))
                    (ul (@ (class "list-group list-group-flush"))
                        ,@(map
                           (lambda (child)
                             (let ((sections (simple-section->html child))
                                   (description (definition-short-description child)))
                               (cond
                                ((member kind '("group" "page"))
                                 `((li (@ (class "list-group-item py-1"))
                                       (div (@ (class "row"))
                                            (div (@ (class "col-sm-6"))
                                                 ,@(make-title child #:ref? #t))
                                            (div (@ (class "col-sm-6 d-flex align-items-end"))
                                                 ,@description)))))
                                (else
                                 `((li (@ (class "list-group-item py-1"))
                                       ,@(make-title child #:ref? ref?)
                                       ,@(description->html child)
                                       ,@(if (null? sections) '() `((small ,@sections)))))))))
                           children)))))
              (group-by-kind
               (remove-private-members (definition-children def))
               '(
                 "type"
                 "typedef"
                 "enum"
                 "class"
                 "struct"
                 "union"
                 "variable"
                 "function"
                 "friend"
                 "group"
                 "page"
                 )))
             '())))))
   (define* (classes->html classes title #:key (attributes? #t))
     (define elements
       (map
        (lambda (ref)
          (define def (hash-ref definitions (reference-refid ref)))
          (define attrs (list (reference-protection ref) " "
                              (let ((virtual (reference-virtual ref)))
                                (if (and (string? virtual) (string= virtual "virtual"))
                                    "virtual " '()))))
          `(li (@ (class "list-group-item pt-0 pb-0"))
               (code (@ (class "text-dark"))
                ,@(list (if attributes? attrs '())
                        (if def
                            `(a (@ (href ,(definition-href def))) ,(reference-name ref))
                            (reference-name ref))))))
        classes))
     (if (null? elements)
         '()
         `((h5 (@ (class "mb-1 mt-4")) ,title)
           (ul (@ (class "list-group list-group-flush")) ,@elements))))
   (define first-breadcrumb-name
     (cond
      ((string-prefix? "pages/" path) "Pages")
      ((string-prefix? "modules/" path) "Documentation")
      (else "Files")))
   (define first-breadcrumb-href
     (cond
      ((string-prefix? "pages/" path) "/pages/")
      ((string-prefix? "modules/" path) "/modules/")
      (else "/files/")))
   (define parent
     (delete-duplicates
      (append-map definition-parents file-definitions)))
   (define page-title
     (let ((result
            (filter (lambda (def) (string= path (definition-output-path def)))
                    (hash-map->list (lambda (key value) value) definitions))))
       (if (null? result) current-url
           (let ((title (definition-title (car result))))
             (if (or (null? title) (and (string? title) (string= title "")))
                 current-url title)))))
   (write-page
    path
    page-title
    `(,@(if (string= path "index.html")
            `((div (@ (class "mt-2"))))
            `((nav (@ (aria-label "breadcrumb"))
                   (ol (@ (class "breadcrumb"))
                       (li (@ (class "breadcrumb-item") (aria-current "page"))
                           (a (@ (href ,first-breadcrumb-href)) ,first-breadcrumb-name))
                       ,@(map
                          (lambda (path)
                            `((li (@ (class "breadcrumb-item"))
                                  (a (@ (href ,path)) ,(basename path)))))
                          (reverse (cdr breadcrumb-urls)))
                       (li (@ (class "breadcrumb-item active") (aria-current "page"))
                           ,page-title)))))
      ,@(map
         (lambda (def)
           (let* ((desc (description->html def))
                  (members (members->html def))
                  (sections (simple-section->html def))
                  (base-classes (classes->html (definition-base-classes def)
                                               "Base classes"))
                  (derived-classes (classes->html (definition-derived-classes def)
                                                  "Derived classes" #:attributes? #f))
                  (kind (definition-kind def)))
             (cond
              ((and (string= kind "file") (null? desc)) '())
              ((member kind '("group" "page"))
               `(,@desc
                 ,@members
                 ,@sections))
              (else
               `((div (@ (class "card mb-1"))
                      (div (@ (class "card-header"))
                           (h5 (@ (class "card-title mb-0"))
                               ,@(make-title def)))
                      ,@(if (and (null? desc) (null? members) (null? base-classes))
                            '()
                            `((div (@ (class "card-body"))
                                   ,@desc ,@base-classes ,@derived-classes ,@members)))
                      ,@(if (null? sections)
                            '()
                            `((div (@ (class "card-footer text-muted"))
                                   (small ,@sections))))))))))
         (filter
          (lambda (def) (not (member (definition-kind def) '("namespace"))))
          (remove-class-members
           (sort-by-kind
            file-definitions
            '("file"
              "define"
              "type"
              "typedef"
              "enum"
              "class"
              "struct"
              "union"
              "friend"
              "function"
              "variable"
              "namespace"
              "category"
              "dcop"
              "dir"
              "event"
              "example"
              "exception"
              "group"
              "interface"
              "module"
              "page"
              "property"
              "protocol"
              "prototype"
              "service"
              "signal"
              "singleton"
              "slot"))
           ))))))
 definitions-by-file)

(define all-namespaces
  (sort-list
   (hash-fold
    (lambda (key value result)
      (if (string= (definition-kind value) "namespace") (cons value result) result))
    '()
    definitions)
   (lambda (a b) (string< (namespace-basename a) (namespace-basename b)))))

(define (namespaces->html)
  (define top-level-namespaces
    (filter
     (lambda (ns) (not (string-contains (definition-name ns) "::")))
     all-namespaces))
  `((nav (@ (aria-label "breadcrumb"))
         (ol (@ (class "breadcrumb"))
             (li (@ (class "breadcrumb-item active") (aria-current "page")) "Namespaces")))
    (ul (@ (class "list-group"))
        ,@(map
           (lambda (ns)
             (let ((description (definition-short-description ns)))
               `((li (@ (class "list-group-item py-1"))
                     (div (@ (class "row"))
                          (div (@ (class "col-sm-6"))
                               ,@(make-title ns #:ref? #t))
                          (div (@ (class "col-sm-6 d-flex align-items-end"))
                               ,@(if (null? description) '() `(,@description))))))))
           top-level-namespaces))))

(write-page "namespaces/index.html" "Namespaces" (namespaces->html))

(define (namespace->html ns)
  (define breadcrumb-urls
    (scan-path-components
     (namespace-components ns)))
  (define children
    (sort
     (definition-children ns)
     (lambda (a b) (string< (namespace-basename a) (namespace-basename b)))))
  (define children-by-kind
    (group-by-kind
     children '("namespace" "type" "typedef" "enum" "class" "struct" "union" "variable"
                "function" "friend")))
  `((nav (@ (aria-label "breadcrumb"))
         (ol (@ (class "breadcrumb"))
             (li (@ (class "breadcrumb-item") (aria-current "page"))
                 (a (@ (href "/namespaces/")) "Namespaces"))
             ,@(map
                (lambda (path)
                  `((li (@ (class "breadcrumb-item"))
                        (a (@ (href ,(string-append "/namespaces" path))) ,(basename path)))))
                (reverse (cdr breadcrumb-urls)))
             (li (@ (class "breadcrumb-item active") (aria-current "page"))
                 ,(basename (car breadcrumb-urls)))))
    ,@(description->html ns)
    ,@(namespace-members->html children-by-kind ns)))

(for-each
 (lambda (ns)
   (write-page
    (string-append (namespace-href ns) "/index.html")
    (string-append "Namespace " (definition-name ns))
    (namespace->html ns)))
 all-namespaces)

(define all-groups
  (sort-list
   (hash-fold
    (lambda (key value result)
      (if (string= (definition-kind value) "group") (cons value result) result))
    '()
    definitions)
   (lambda (a b) (string< (definition-title a) (definition-title b)))))

(define (groups->html groups)
  `((nav (@ (aria-label "breadcrumb"))
         (ol (@ (class "breadcrumb"))
             (li (@ (class "breadcrumb-item active") (aria-current "page")) "Documentation")))
    (ul (@ (class "list-group"))
        ,@(map
           (lambda (group)
             (let ((description (definition-short-description group)))
               `((li (@ (class "list-group-item py-1"))
                     (div (@ (class "row"))
                          (div (@ (class "col-sm-6"))
                               ,@(make-title group #:ref? #t))
                          (div (@ (class "col-sm-6 d-flex align-items-end"))
                               ,@(if (null? description) '() `(,@description))))))))
           groups))))

(define more-groups
  (list
   (make <definition>
     #:id "namespaces"
     #:title "Namespaces"
     #:kind "group"
     #:short-description '((span "C++ namespaces."))
     #:output-path "namespaces/")
   (make <definition>
     #:id "files"
     #:title "Files"
     #:kind "group"
     #:short-description '((span "C++ files."))
     #:output-path "files/")))

(write-page "modules/index.html" "Documentation"
            (groups->html (append all-groups more-groups)))

(define all-pages
  (sort-list
   (hash-fold
    (lambda (key value result)
      (if (and (string= (definition-kind value) "page")
               (not (string= (definition-id value) "indexpage")))
          (cons value result) result))
    '()
    definitions)
   (lambda (a b) (string< (definition-title a) (definition-title b)))))

(define (pages->html pages)
  `((nav (@ (aria-label "breadcrumb"))
         (ol (@ (class "breadcrumb"))
             (li (@ (class "breadcrumb-item active") (aria-current "page")) "Pages")))
    (ul (@ (class "list-group"))
        ,@(map
           (lambda (page)
             (let ((description (definition-short-description page)))
               `((li (@ (class "list-group-item py-1"))
                     (div (@ (class "row"))
                          (div (@ (class "col-sm-6"))
                               ,@(make-title page #:ref? #t))
                          (div (@ (class "col-sm-6 d-flex align-items-end"))
                               ,@(if (null? description) '() `(,@description))))))))
           pages))))

(write-page "pages/index.html" "Pages" (pages->html all-pages))
