file_of_interest = src/hello.c

.PHONY: build
build:
	cd build/ && make

.PHONY: run
run:
	clang -fsanitize=address -fsanitize=undefined -g -Xclang -load -Xclang build/skeleton/libSkeletonPass.* $(file_of_interest)

.PHONY: emit
emit:
	clang -emit-llvm -g -S -o - $(file_of_interest)

.PHONY: emit_opt
emit_opt:
	clang -Xclang -load -g -Xclang build/skeleton/libSkeletonPass.* -emit-llvm -S -o - $(file_of_interest)

.PHONY: exe
exe:
	./a.out