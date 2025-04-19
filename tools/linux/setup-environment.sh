#!/bin/bash

# Function to install dependencies using apt
install_dependencies_apt() {
    echo "Detected apt package manager. Installing dependencies using apt..."

    # Install pkg-config
    sudo apt install pkg-config -y

    # Install xcb libraries and development headers
    sudo apt install xcb \
        libxcb-keysyms1 libxcb-keysyms1-dev \
        libxcb-xfixes0 libxcb-xfixes0-dev \
        -y
}

# Function to install dependencies using pacman
install_dependencies_pacman() {
    echo "Detected pacman package manager. Installing dependencies using pacman..."

    # Update the system and install pkg-config
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm pkgconf

    # Install xcb libraries and development headers
    sudo pacman -S --noconfirm libxcb libxkbcommon xcb-util xcb-util-keysyms xcb-util-renderutil
}

# Detect the package manager
if command -v apt &>/dev/null; then
    install_dependencies_apt
elif command -v pacman &>/dev/null; then
    install_dependencies_pacman
else
    echo "Error: Neither apt nor pacman package manager found. Unable to install dependencies."
    exit 1
fi
