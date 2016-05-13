#!/bin/bash

# Downloads dependencies and sets up the environment for builds

function dep() {
  ./.mason/mason install $1 $2
  ./.mason/mason link $1 $2
}

function all_deps() {
  dep boost 1.59.0 &
  dep boost_liball 1.59.0 &
  dep libosmium 2.6.1 &
  dep rapidjson 1.0.2 &
  dep protozero 1.3.0 &
  dep utfcpp 2.3.4 &
  dep zlib system &
  wait
}

MASON_VERSION=061cbd85bc4fbf4cd1ea204ddd4ddfd6d05328ee

function setup_mason() {
  if [[ ! -d ./.mason ]]; then
    git clone https://github.com/mapbox/mason.git ./.mason
    (cd ./.mason && git checkout ${MASON_VERSION})
  else
    (cd ./.mason && git fetch > /dev/null 2>&1 && git checkout ${MASON_VERSION} > /dev/null 2>&1)
  fi

  export MASON_DIR=$(pwd)/.mason
}

function main() {
  setup_mason

  if [[ ! -d ${MASON_HOME} ]]; then
    all_deps
  fi
}

main
