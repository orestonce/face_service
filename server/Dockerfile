FROM alpine AS builder
RUN apk --no-cache --update upgrade && apk --no-cache add ca-certificates 
RUN apk --no-cache add gcc g++ cmake wget git make
WORKDIR /data/model
RUN wget https://github.com/orestonce/face_service/releases/download/0.1/SeetaFace2.tar.bz2 && tar xfj SeetaFace2.tar.bz2 && rm -f SeetaFace2.tar.bz2

ADD 3rd /data/3rd
WORKDIR /data/3rd
RUN mkdir libfacedetection/build && cd libfacedetection/build && cmake .. && make
RUN sed -i 's/ -msse2 -mfma//g' SeetaFace2/SeetaNet/CMakeLists.txt && \
	mkdir SeetaFace2/build && cd SeetaFace2/build && cmake -DCMAKE_INSTALL_PREFIX=/data/face_service/build -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLE=OFF .. && make

WORKDIR /data
ADD src /data/face_service
RUN cd /data/face_service && make 

# docker_service
FROM alpine AS face_service
RUN apk --no-cache add libstdc++
COPY --from=builder /data/face_service/build/bin/face_service /usr/local/bin/face_service
COPY --from=builder /data/model /data/model
CMD ["/usr/local/bin/face_service"]
EXPOSE 8080
