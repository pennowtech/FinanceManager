# FROM python:3.10
# FROM mcr.microsoft.com/devcontainers/base:jammy
FROM carlonluca/qt-dev:6.7.1

ARG DEBIAN_FRONTEND=noninteractive

# Add non-root user
ARG USERNAME=vscode
ARG HOME="/home/$USERNAME"
ENV HOME="${HOME}"

# Check if the group already exists and create it if not
RUN getent group $USERNAME || groupadd --gid 1000 $USERNAME
# Similarly, check if the user already exists and create if not
RUN id -u $USERNAME || useradd --uid 1000 --gid 1000 -m $USERNAME

# Make sure to reflect new user in PATH
ENV PATH="${HOME}/.local/bin:${PATH}"

RUN DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install -y build-essential --no-install-recommends make \
        ca-certificates \
        git \
        zsh \
        libssl-dev \
        zlib1g-dev \
        libbz2-dev \
        libreadline-dev \
        libsqlite3-dev \
        wget \
        curl \
        llvm \
        libncurses5-dev \
        xz-utils \
        tk-dev \
        libxml2-dev \
        libxmlsec1-dev \
        libffi-dev \
        liblzma-dev \

# Install necessary packages for Qt and PySide6
RUN apt-get update && apt-get install -y \
    libgl1-mesa-dev \
    libgl1-mesa-glx \
    libxkbcommon-x11-0 \
    libdbus-1-3 \
    libfontconfig1 \
    libxcb-xinerama0 \
    libxrender1 \
    libxi6 \
    libxrandr2 \
    libxcursor1 \
    libxcomposite1 \
    libxdamage1 \
    libxfixes3 \
    libxtst6 \
    libatk1.0-0 \
    libatk-bridge2.0-0 \
    libepoxy0 \
    libgtk-3-0 \
    libgdk-pixbuf2.0-0 \
    libxcb-cursor0 \
    libxcb-icccm4 \
    x11-apps \
    libxcb-render0-dev \
    libxcb-render-util0-dev \
    libxcb-xkb-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-shape0-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-dri3-dev \
    libxcb-cursor-dev \
    libxcb-util0-dev \
    libxcb-util-dev \
    exa fd-find tree ripgrep neovim bat dos2unix\
    iputils-ping \
    tcpdump \
    wireshark \
    net-tools \
    build-essential \
    cmake make gdb clang clangd \
    python3 python3-pip python3-venv \
    postgresql-client \
    redis-tools \
    clang-format \
    && rm -rf /var/lib/apt/lists/*

COPY ZscalerRootCA.crt /usr/local/share/ca-certificates/ZscalerRootCA.crt
RUN update-ca-certificates

# Python and poetry installation
# RUN DEBIAN_FRONTEND=noninteractive \
#     && apt-get update && apt-get install -y \
#     python3 python3-pip python3-venv \
#     && rm -rf /var/lib/apt/lists/*

RUN pip install --upgrade pip

ENV POETRY_ENV_ROOT="/opt/venvs"
RUN mkdir -p $POETRY_ENV_ROOT && chown -R $USERNAME:$USERNAME $POETRY_ENV_ROOT

# Change shell to ZSH before switching user
RUN chsh -s $(which zsh) $USERNAME

USER $USERNAME
RUN mkdir -p $HOME && chown -R $USERNAME:$USERNAME $HOME

WORKDIR $HOME

RUN curl -sSL https://install.python-poetry.org | python3 -

RUN poetry config virtualenvs.path "${POETRY_ENV_ROOT}"
ENV PATH="${POETRY_ENV_ROOT}/bin:${HOME}/.local/bin:$PATH"

WORKDIR /app
COPY . /app

# Install the dependencies specified in pyproject.toml through devcontainer.json. Comment here
# RUN poetry install


RUN pip install -r requirements.txt


# Install Zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
RUN git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions
RUN git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
RUN git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/themes/powerlevel10k

# Copy Zshell configs to container
COPY linuxconfig/.p10k.zsh ${HOME}/.p10k.zsh
COPY linuxconfig/.zshrc ${HOME}/.zshrc


# Modify zshrc file:
RUN sed -i 's/ZSH_THEME="devcontainers"/ZSH_THEME="powerlevel10k\/powerlevel10k"/' ~/.zshrc

USER root
RUN cat linuxconfig/ZscalerRootCertificate-2048-SHA256.crt >> /etc/ssl/certs/ca-certificates.crt
RUN echo "root:123456" | chpasswd

COPY linuxconfig/conan-debug-profile ${HOME}/.conan2/profiles/conan-debug-profile
# COPY linuxconfig/remotes.json ${HOME}/.conan2/remotes.json
RUN chown -R $USERNAME:$USERNAME ${HOME}/.conan2

# Switch back to the user
USER $USERNAME
RUN conan remote add --force conan-center https://center.conan.io

# Install Cassandra C++ driver dependencies and driver itself
# RUN mkdir -p ${HOME}/tmp/cassandra-cpp-driver
# COPY linuxconfig/cassandra-cpp-driver-2.17.1.tar.gz ${HOME}/tmp/cassandra-cpp-driver/cassandra-cpp-driver-2.17.1.tar.gz
# RUN cd ${HOME}/tmp/cassandra-cpp-driver && \
#     curl -L https://github.com/datastax/cpp-driver/archive/refs/tags/2.17.1.tar.gz && \
#     tar zxf cassandra-cpp-driver-2.17.1.tar.gz --strip-components=1 && cmake . && make

# installaing fzf
RUN git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf && cd ~/.fzf && dos2unix install && ./install --all
RUN echo "[ -f ~/.fzf.zsh ] && source ~/.fzf.zsh" >> ~/.zshrc
