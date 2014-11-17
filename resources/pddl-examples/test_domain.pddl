(define (domain test_domain)
    (:requirements :strips :equality :typing :conditional-effects)
    (:types location actor service)
    (:predicates ( at ?x - actor ?l - location)
                 ( operative ?x - actor)
		 ( cannot_move ?x - actor)
                 ( embodies ?x ?y - actor)
                 ( mobile ?x - actor)
                 ( provides ?x - actor ?s - service)
    )

; General effects
; moving physically bounded objects affect position for all
(:action move
    :parameters (?obj - actor ?m ?l - location)
    :precondition (and (at ?obj ?m) (not (= ?m ?l)) (mobile ?obj) (operative ?obj))
    :effect (and (at ?obj ?l) (not (at ?obj ?m)))
)


; Reconfigure: connect two systems to create a third one
; requires them to be at the same position
; 
(:action reconfigure_join
    :parameters (?x ?y ?z - actor)
    :precondition (and 
            (not (= ?x ?y))
            (not (= ?x ?z))
            (not (= ?y ?z))
            (exists (?l - location) (and (at ?x ?l) (at ?y ?l)))
            (not (operative ?z))
            (operative ?x)
            (operative ?y)
            (embodies ?z ?x)
            (embodies ?z ?y)
            ); end and
    :effect (and 
                (not (operative ?y))
                (not (operative ?x))
                (operative ?z)
                ; Disable location for dormant objects
                (forall (?l) 
                    (when (at ?x ?l)                                        ; condition
                        (and (not (at ?x ?l)) (not (at ?y ?l)) (at ?z ?l))  ; effect
                    ); end when
                ); end forall
            ); end and
); end action

; Disconnecting an operative actor into two previously dormant actors
(:action reconfigure_split
    :parameters (?x ?y ?z - actor)
    :precondition (and 
            (not (= ?x ?y))
            (not (= ?x ?z))
            (not (= ?y ?z))
            (operative ?z) 
            (not (operative ?y))
            (not (operative ?x))
            (embodies ?z ?x)
            (embodies ?z ?y))
    :effect (and 
                (not (operative ?z))
                (operative ?x)
                (operative ?y)
                ; Disable location for dormant objects
                (forall (?l)
                    (when (at ?z ?l)                              ; condition
                    (and (at ?x ?l) (at ?y ?l) (not (at ?z ?l)))) ; effect
                ); end forall
            ); end and
); end action

; END DOMAIN
)

