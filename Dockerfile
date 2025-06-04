FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake   \
    git \
    curl    \
    autoconf    \
    automake    \
    libtool \
    pkg-config  \
    sudo    \
    libssl-dev      \
    gdb         \
    valgrind        \
    tmux            \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cd /app/lib/secp256k1 && \
    ./autogen.sh && \
    ./configure --enable-module-recovery && \
    make && \
    make install && \
    echo "/usr/local/lib" > /etc/ld.so.conf.d/secp256k1.conf && \
    ldconfig

# ENV LD_LIBRARY_PATH=/usr/local/lib:/app/:$LD_LIBRARY_PATH

RUN cmake -S . -B build && cmake --build build --target all

# FROM ubuntu:24.04 

# RUN apt-get update && apt-get install -y \
#     libstdc++6  \
#     ca-certificates \
#     bash    \
#     && rm -rf /var/lib/apt/lists/*

# WORKDIR /app

# COPY --from=builder /app/build/ /app/

ENTRYPOINT [ "/bin/bash" ]