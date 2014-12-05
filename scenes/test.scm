;(use-modules (scenes scene))

(define (scene-add-segments scene segments previous)
  (cond ((and (not (null? segments)) (null? previous))
         (scene-add-segments scene (cdr segments) (car segments)))
        ((null? segments) #t)
        (else (let ((current (car segments)))
                (scene-add-segment scene
                                   (car previous) (cdr previous)
                                   (car current)  (cdr current))
                (scene-add-segments scene (cdr segments) current)))))

(scene-add-segments
 *scene*
 (list
  (cons -200 0)
  (cons 0 20)
  (cons 400 30))
  '())
