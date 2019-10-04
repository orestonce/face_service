FACECNN := -lfacedetection
LIBS := -lSeetaFaceDetector  -lSeetaFaceLandmarker  -lSeetaFaceRecognizer  -lSeetaNet  -lfacedetection
COMPILE := g++ -c -std=c++11 -Ibuild/include

face_service: psf.cpp pcnn.cpp autofree.cpp cvimage.cpp main.cpp
	make libs
	g++ -fmax-errors=4 -std=c++11 -Ibuild/include $^ -o build/bin/face_service -L./build/lib64/ -L./build/lib/ -lpthread $(FACECNN) $(LIBS)
libs:
	mkdir -p build/include build/lib build/bin/
	cp /data/3rd/ajson/ajson.hpp build/include
	cp /data/3rd/cpp-httplib/httplib.h build/include
	cp /data/3rd/stb/stb_image.h build/include
	cp /data/3rd/libfacedetection/build/libfacedetection.a /data/face_service/build/lib && cp /data/3rd/libfacedetection/src/facedetectcnn.h /data/face_service/build/include
	cd /data/3rd/SeetaFace2/build && make install 
