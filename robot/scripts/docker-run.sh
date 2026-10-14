#! /bin/bash

CURRENT_USER=$(whoami)
RKROS_SCRIPTS_DIR="$(dirname $(realpath ${BASH_SOURCE}))"
RKROS_SDK_DIR="$(realpath ${RKROS_SCRIPTS_DIR}/../../)"

VOLUME="-v ${RKROS_SDK_DIR}:/home/rk/sdk"

DooD="-v /usr/bin/docker:/usr/bin/docker \
      -v /var/run/docker.sock:/var/run/docker.sock"

IMAGE="ubuntu-for-sdk-x86:22.04"

/usr/bin/docker run --privileged -it \
	-e HOST_UID=$(id -u) \
	-e HOST_GID=$(id -g) \
	-e DOCKER_GID=$(getent group docker | cut -d: -f3) \
	$VOLUME \
	$DooD \
	$IMAGE
