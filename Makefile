.PHONY: build
build:
	cd build/ && make

.PHONY: run
run:
	clang -fsanitize=address -Xclang -load -Xclang build/skeleton/libSkeletonPass.* src/example.c

emit:
	clang -emit-llvm -S -o - src/example.c

emit_opt:
	clang -Xclang -load -Xclang build/skeleton/libSkeletonPass.* -emit-llvm -S -o - src/example.c