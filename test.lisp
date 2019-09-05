(def arg 20)

(println "Arg: " arg)

(defun fac (n)
    (if (= n 1)
        1
        (* n (fac (- n 1)))))

(println "Factorial of " arg ": " (fac arg))

(defun fib (n)
    (if (> n 1)
        (+ (fib (- n 1)) (fib (- n 2)))
        1))

(println "Fibonacci at position " arg ": " (fib arg))

(defun exp (a n)
    (if (> n 1)
        (* a (exp a (- n 1)))
        a
    )
)

(defun squared (a) (exp a 2))

(println arg " squared: " (squared arg))

(defun times (n body)
    (if (> n 0)
        (apply (body (- n 1)) (times (- n 1) body))
        (eval body)
    )
)

(times 5 (lambda (n) (println n)))