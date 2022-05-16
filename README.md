# Chess-x86
 A bootable chess game running baremetal on x86_64 CPU without any OS. The chess engine part is a port of [micro-Max Chess Engine](https://home.hccnet.nl/h.g.muller/max-src2.html) and the low level drivers are taken from amazing [Tetris OS](https://www.youtube.com/watch?v=FaILnmUYS_U) by [jdh](https://github.com/jdah). The engine rating is around [1891 elo](https://ccrl.chessdom.com/ccrl/4040/cgi/engine_details.cgi?print=Details%20%28text%29&eng=Micro-Max%204.8).

## Compile 
    make

## Make Image
    make img

## Run in QEMU
    make run

## Make Clean
    make clean

# Demo Video on QEMU
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/I1wtI4ju2Aw/0.jpg)](https://www.youtube.com/watch?v=I1wtI4ju2Aw)


Watch on Youtube :  [https://www.youtube.com/watch?v=I1wtI4ju2Aw](https://www.youtube.com/watch?v=I1wtI4ju2Aw)
