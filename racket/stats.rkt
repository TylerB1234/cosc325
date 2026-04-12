#lang racket

(provide mean
         median
         mode
         minimum
         maximum)

(define (minimum values)
  (unless (and (list? values) (pair? values))
    (error 'minimum "expects a non-empty list of numbers"))
  (apply min values))

(define (maximum values)
  (unless (and (list? values) (pair? values))
    (error 'maximum "expects a non-empty list of numbers"))
  (apply max values))

(define (mean values)
  (unless (and (list? values) (pair? values))
    (error 'mean "expects a non-empty list of numbers"))
  (exact->inexact
   (/ (apply + values)
      (length values))))

(define (median values)
  (unless (and (list? values) (pair? values))
    (error 'median "expects a non-empty list of numbers"))
  (define sorted-values (sort values <))
  (define count (length sorted-values))
  (define middle (quotient count 2))
  (if (odd? count)
      (list-ref sorted-values middle)
      (exact->inexact
       (/ (+ (list-ref sorted-values (sub1 middle))
             (list-ref sorted-values middle))
          2))))

(define (display-number value)
  (if (integer? value)
      value
      (real->decimal-string value 10)))

(define (mode values)
  (unless (and (list? values) (pair? values))
    (error 'mode "expects a non-empty list of numbers"))
  (define counts (make-hash))
  (for ([value values])
    (hash-update! counts value add1 0))
  (define highest-frequency (apply max (hash-values counts)))
  (apply min
         (for/list ([(value frequency) (in-hash counts)]
                    #:when (= frequency highest-frequency))
           value)))

(module+ main
  (define listA '(3 7 12 5 9 12 4 8 6 12 10 2 14 12 1 11 13 12 15 12 16))
  (define listB '(25 18 30 22 25 17 19 25 21 20 18 24 23 25 26 27 18 29 28 25))

  (for ([name '(listA listB)]
        [values (list listA listB)])
    (printf "~a\n" name)
    (printf "  mean: ~a\n" (display-number (mean values)))
    (printf "  median: ~a\n" (median values))
    (printf "  mode: ~a\n" (mode values))
    (printf "  min: ~a\n" (minimum values))
    (printf "  max: ~a\n" (maximum values))
    (newline)))
