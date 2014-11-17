(define (problem test_domain-1)
(:domain test_domain)
(:objects
        sherpa_0 - actor
	crex_0 - actor
	pl_0 - actor
        sherpa-crex_0 - actor
	location_s0 - location
	location_c0 - location
	location_p0 - location
        image_location_provider - service
)
(:init 
        (embodies sherpa-crex_0 sherpa_0)
        (embodies sherpa-crex_0 crex_0)
	(at sherpa_0 location_s0)
	(at crex_0 location_c0)
	(at pl_0 location_p0)
	(mobile sherpa_0)
	(mobile crex_0)
        (operative sherpa_0)
        (operative crex_0)
        ;(operative sherpa-crex_0)
        (provides sherpa_0 image_location_provider)
        (provides sherpa-crex_0 image_location_provider)
)
(:goal 
   ;(and (at sherpa-crex_0 location_p0))
   ;(and (at sherpa_0 location_p0))
   (exists (?a - actor) (and (provides ?a image_location_provider) (at ?a location_p0)))
)
)

