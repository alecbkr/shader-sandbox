cd ../build
cmake --build .
cd ../.mytools
prime-run gamescope -W 1920 -H 1080 -r 60 --force-grab-cursor -- ../bin/sandbox
