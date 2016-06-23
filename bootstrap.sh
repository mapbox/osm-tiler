#!/bin/bash

# Downloads dependencies and sets up the environment for builds

function dep() {
  ./.mason/mason install $1 $2
  ./.mason/mason link $1 $2
}

function all_deps() {
  dep boost 1.61.0 &
  dep boost_libfilesystem 1.61.0 &
  dep boost_libsystem 1.61.0 &
  dep boost_libprogram_options 1.61.0 &
  dep libosmium 2.7.2 &
  dep rapidjson 1.0.2 &
  dep protozero 1.3.0 &
  dep utfcpp 2.3.4 &
  dep zlib system &
  wait
}

MASON_VERSION=master

function setup_mason() {
  if [[ ! -d ./.mason ]]; then
    git clone https://github.com/mapbox/mason.git ./.mason
    (cd ./.mason && git checkout ${MASON_VERSION})
  else
    (cd ./.mason && git fetch > /dev/null 2>&1 && git checkout ${MASON_VERSION} > /dev/null 2>&1 && git pull)
  fi
}

function main() {
  setup_mason

  if [[ ! -d ${MASON_HOME} ]]; then
    all_deps
  fi
}

main
