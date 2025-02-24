FROM ubuntu:latest
RUN apt-get update && apt-get install -y cmake g++ libboost-all-dev

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. && make

CMD ["./build/backend-socket-client"]
