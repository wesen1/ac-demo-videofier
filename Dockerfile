FROM debian:buster-slim as builder
RUN apt-get update && \
    apt-get install -y make gcc g++ clang && \
    apt-get install -y libsdl-image1.2-dev libsdl1.2-dev zlib1g-dev libogg-dev libvorbis-dev libopenal-dev libcurl4-openssl-dev
COPY ./source /source
RUN cd /source/src && \
    make client


FROM wesen1/assaultcube-client:1.2.0.2
COPY --from=builder /source/src/ac_client /ac-client/bin_unix/linux_64_client
RUN apt-get update && \
    apt-get install -y ffmpeg
