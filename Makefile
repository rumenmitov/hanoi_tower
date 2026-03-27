PHANTOM_HOME 	:= /phantomuserland/src

WASI_SDK 			:= /opt/wasi-sdk
WAMR_DIR 			:= ${PHANTOM_HOME}/phantom/vm/wasm-micro-runtime

CC						:= ${WASI_SDK}/bin/clang


.PHONY: all build clean install uninstall
all: build install


build:
	${CC} --target=wasm32 -O0 -z stack-size=16192 \
        --sysroot=${WAMR_DIR}/wamr-sdk/app/libc-builtin-sysroot  \
        -Wl,--allow-undefined-file=${WAMR_DIR}/wamr-sdk/app/libc-builtin-sysroot/share/defined-symbols.txt \
        -Wl,--strip-all,--no-entry -nostdlib \
        -Wl,--export-all \
        -Wl,--allow-undefined \
				-I${WAMR_DIR}/samples/very-basic/wasm-apps/ \
        -o assets/hanoi_tower.wasm hanoi_tower.c


clean:
	rm assets/hanoi_tower.wasm


install: build
	mkdir ${PHANTOM_HOME}/plib/sys/src/hanoi_tower
	cp -r assets ${PHANTOM_HOME}/plib/sys/src/hanoi_tower
	cp hanoi_tower.ph ${PHANTOM_HOME}/plib/sys/src/ru.dz/ru.dz.demo.hanoi_tower.ph


uninstall: clean
	rm -rf ${PHANTOM_HOME}/plib/sys/src/hanoi_tower
	rm ${PHANTOM_HOME}/plib/sys/src/ru.dz/ru.dz.demo.hanoi_tower.ph

