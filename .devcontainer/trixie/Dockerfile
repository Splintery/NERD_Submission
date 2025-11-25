FROM docker.io/library/debian:trixie-slim

RUN yes | \
    apt update && apt install -y --no-install-recommends \
        cmake cmake-curses-gui ninja-build make gcc g++ clang clang-tools git libsdl2-dev vim tig \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*