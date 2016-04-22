#!/bin/bash

# Downloads dependencies and sets up the environment for builds

function dep() {
  ./.mason/mason install $1 $2
  ./.mason/mason link $1 $2
}

function all_deps() {
  dep boost 1.59.0 &
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

  wd=$(pwd)
  export MASON_DIR=${wd}/.mason
  export MASON_HOME=${wd}/mason_packages/.link
  export PATH=${wd}/.mason:$PATH
  export CXX=${CXX:-clang++}
  export CC=${CC:-clang}
}

function main() {
  setup_mason

  if [[ ! -d ${MASON_HOME} ]]; then
    all_deps
  fi

  export C_INCLUDE_PATH="${MASON_HOME}/include"
  export CPLUS_INCLUDE_PATH="${MASON_HOME}/include"
  export CXXFLAGS="-I${MASON_HOME}/include"
  export LIBRARY_PATH="${MASON_HOME}/lib"
  export LDFLAGS="-L${MASON_HOME}/lib"
}

main
