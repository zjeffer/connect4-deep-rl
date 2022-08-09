FROM ghcr.io/joren-vangoethem/cuda-libtorch:latest

WORKDIR /app

RUN pacman -Syyu --noconfirm cargo websocketpp boost git base-devel 
# opencv vtk hdf5 glew openmpi fmt qt5-base

RUN chown -R $USERNAME:$USERNAME /app

RUN echo 'Include = /etc/pacman.d/mirrorlist' >> /etc/pacman.conf && \
    useradd -m -r -s /bin/bash aur && \
    passwd -d aur && \
    echo 'aur ALL=(ALL) ALL' > /etc/sudoers.d/aur && \
    mkdir -p /home/aur/.gnupg && \
    echo 'standard-resolver' > /home/aur/.gnupg/dirmngr.conf && \
    chown -R aur:aur /home/aur && \
    mkdir /build && \
    chown -R aur:aur /build && \
    cd /build && \
    sudo -u aur git clone --depth 1 https://aur.archlinux.org/paru.git && \
    cd paru && \
    sudo -u aur makepkg --noconfirm -si && \
    sed -i 's/#RemoveMake/RemoveMake/g' /etc/paru.conf && \
    pacman -Qtdq | xargs -r pacman --noconfirm -Rcns && \
    rm -rf /home/aur/.cache && \
    rm -rf /build

# add user
################# USER & GROUP CREATION #################
ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME
################# SUDO #################
RUN echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME
USER $USERNAME
ENV HOME /home/$USERNAME


RUN paru -S ta-lib ninja --noconfirm

COPY . .