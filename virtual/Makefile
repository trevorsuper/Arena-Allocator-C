cc = clang

src_dir = src
debug_objects_dir = dbg
uba_objects_dir = uba
point_objects_dir = point
leak_objects_dir = leak
thread_objects_dir = thread
opt_objects_dir = opt

src = $(wildcard $(src_dir)/*.c)
debug_objects       = $(patsubst $(src_dir)/%.c,$(debug_objects_dir)/%.o,$(src))
debug_dependencies  = $(patsubst $(src_dir)/%.c,$(debug_objects_dir)/%.d,$(src))
uba_objects         = $(patsubst $(src_dir)/%.c,$(uba_objects_dir)/%.o,$(src))
uba_dependencies    = $(patsubst $(src_dir)/%.c,$(uba_objects_dir)/%.d,$(src))
point_objects       = $(patsubst $(src_dir)/%.c,$(point_objects_dir)/%.o,$(src))
point_dependencies  = $(patsubst $(src_dir)/%.c,$(point_objects_dir)/%.d,$(src))
leak_objects        = $(patsubst $(src_dir)/%.c,$(leak_objects_dir)/%.o,$(src))
leak_dependencies   = $(patsubst $(src_dir)/%.c,$(leak_objects_dir)/%.d,$(src))
thread_objects      = $(patsubst $(src_dir)/%.c,$(thread_objects_dir)/%.o,$(src))
thread_dependencies = $(patsubst $(src_dir)/%.c,$(thread_objects_dir)/%.d,$(src))
opt_objects         = $(patsubst $(src_dir)/%.c,$(opt_objects_dir)/%.o,$(src))
opt_dependencies    = $(patsubst $(src_dir)/%.c,$(opt_objects_dir)/%.o,$(src))

debug_flags = -Wall -Wextra -Werror -Wpedantic -O0 -g -MMD -MP -std=c23
debug_uba_flags = -Wall -Wextra -Werror -Wpedantic -O0 -fsanitize=undefined,address -march=native -MMD -MP -std=c23
debug_point_flags = -Wall -Wextra -Werror -Wpedantic -O0 -fsanitize=address,pointer-compare -MMD -MP -std=c23
debug_leak_flags = -Wall -Wextra -Werror -Wpedantic -O0 -fsanitize=leak -march=native -MMD -MP -std=c23
debug_thread_flags = -Wall -Wextra -Werror -Wpedantic -O0 -fsanitize=thread -march=native -MMD -MP -std=c23
optimized_flags = -Wall -Wextra -Werror -Wpedantic -fPIE -O2 -march=native -MMD -MP -std=c23

target = arena_alloc
debug_target = $(target)_dbg
uba_target = $(target)_uba
point_target = $(target)_point
leak_target = $(target)_leak
thread_target = $(target)_thread
opt_target = $(target)

debug: $(debug_target)
	./$<

$(debug_target): $(debug_objects)
	$(cc) -o $@ $^

$(debug_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(debug_objects_dir)
	$(cc) $(debug_flags) -c $< -o $@
	-include $(debug_dependencies)

uba: $(uba_target)
	./$<

$(uba_target): $(uba_objects)
	$(cc) -o $@ $^ -fsanitize=undefined,address

$(uba_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(uba_objects_dir)
	$(cc) $(debug_uba_flags) -c $< -o $@
	-include $(uba_dependencies)

point: $(point_target)
	./$<

$(point_target): $(point_objects)
	$(cc) -o $@ $^ -fsanitize=address,pointer-compare

$(point_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(point_objects_dir)
	$(cc) $(debug_point_flags) -c $< -o $@
	-include $(point_dependencies)

leak: $(leak_target)
	./$<

$(leak_target): $(leak_objects)
	$(cc) -o $@ $^ -fsanitize=leak

$(leak_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(leak_objects_dir)
	$(cc) $(debug_leak_flags) -c $< -o $@
	-include $(leak_dependencies)

thread: $(thread_target)
	./$<

$(thread_target): $(thread_objects)
	$(cc) -o $@ $^ -fsanitize=thread

$(thread_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(thread_objects_dir)
	$(cc) $(debug_thread_flags) -c $< -o $@
	-include $(thread_dependencies)

opt: $(opt_target)
	./$<

$(opt_target): $(opt_objects)
	$(cc) -o $@ $^

$(opt_objects_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(opt_objects_dir)
	$(cc) $(optimized_flags) -c $< -o $@
	-include $(opt_dependencies)

clean:
	rm -rf $(debug_objects_dir) $(uba_objects_dir) $(point_objects_dir) $(leak_objects_dir) $(thread_objects_dir) $(opt_objects_dir) \
	$(debug_target) $(uba_target) $(point_target) $(leak_target) $(thread_target) $(opt_target)
