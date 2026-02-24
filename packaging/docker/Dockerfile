# Lwanga Compiler Docker Image
# Cosmc Technologies
# https://github.com/cosmah/Project-Lwanga

FROM ubuntu:24.04

LABEL maintainer="Cosmc Technologies <cosmahke4@gmail.com>"
LABEL description="Lwanga - Security-focused systems programming language"
LABEL version="1.0.2"

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    llvm-18 \
    llvm-18-dev \
    clang-18 \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /lwanga

# Copy source code
COPY . /lwanga/

# Build compiler
RUN cd compiler && \
    mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make test

# Install binaries
RUN cd compiler/build && \
    install -m 755 lwangac /usr/local/bin/lwangac && \
    install -m 755 formatter/lwangafmt /usr/local/bin/lwangafmt && \
    install -m 755 highlighter/lwangacat /usr/local/bin/lwangacat

# Create workspace directory
RUN mkdir -p /workspace
WORKDIR /workspace

# Set PATH
ENV PATH="/usr/local/bin:${PATH}"

# Default command
CMD ["/bin/bash"]
