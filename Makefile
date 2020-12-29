libdcm2pnm.so: dcm2pnm.cpp wrapper.cpp
	g++ -o libdcm2pnm.so dcm2pnm.cpp wrapper.cpp  -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage -lijg12 -lijg16 -lijg8 -lpng \
	-std=c++17 -O3 -Wall -Wextra -fPIC -shared

#libdcm2pnm.so: dcm2pnm.cpp wrapper.cpp
#	g++ -o libdcm2pnm.so dcm2pnm.cpp wrapper.cpp -L/usr/lib -liconv -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage -lijg12 -lijg16 -lijg8 -lpng \
#-std=c++17 -O3 -Wall -Wextra -fPIC -shared
