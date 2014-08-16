CFLAGS += -I./v8/include

execjs: main.o timer.o
	clang++ \
	-o execjs $^ $(LDFLAGS) $(V8LIB) \
	-Wl,--start-group ./v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,snapshot},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -luv \
	-g -O0

main.o: src/main.cc
	clang++ $(CFLAGS) -c $<

timer.o: src/timer.cc
	clang++ $(CFLAGS) -c $<
