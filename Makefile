.PHONY: all clean

all: shell loader1_regular_nonopt loader1_regular_opt loader1_parallel loader2_regular_nonopt loader2_regular_opt loader2_parallel

shell:
 $(MAKE) -C shell

loader1_regular_nonopt:
 $(MAKE) -C loader1/regular nonoptimized

loader1_regular_opt:
 $(MAKE) -C loader1/regular optimized

loader1_parallel:
 $(MAKE) -C loader1/parallel

loader2_regular_nonopt:
 $(MAKE) -C loader2/regular nonoptimized

loader2_regular_opt:
 $(MAKE) -C loader2/regular optimized

loader2_parallel:
 $(MAKE) -C loader2/parallel

clean:
 $(MAKE) -C shell clean
 $(MAKE) -C loader1/regular clean
 $(MAKE) -C loader1/parallel clean
 $(MAKE) -C loader2/regular clean
 $(MAKE) -C loader2/parallel clean