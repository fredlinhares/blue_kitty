# SPDX-License-Identifier: MIT
require "mkmf"

$CXXFLAGS += " -std=gnu++17 "

libs = %w{SDL2 vulkan}
libs.each {|lib| have_library(lib)}

create_makefile("blue_kitty/blue_kitty")
