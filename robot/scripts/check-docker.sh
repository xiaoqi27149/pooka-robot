#! /bin/bash

docker_handle() {
	echo -e "\e[36m"
	echo "=========================================="
	echo "        Start prepare rkros docker        "
	echo "=========================================="
	echo -en "\e[0m"

	local debian_version="$DEBIAN_VERSION"
	local docker_image="debian-for-ros-arm64:${debian_version}"

	if docker image ls |  grep "debian-for-ros-arm64" | grep ${debian_version} > /dev/null 2>&1; then
		echo -e "\e[36m"
		echo "Found docker image: ${docker_image}"
		echo -en "\e[0m"
	else
		echo -e "\e[91m"
		echo "Error: not found docker image: ${docker_image}"
		echo "Please creat it refer to quick start documents"
		echo -en "\e[0m"
		return 1
	fi

	if [ -n "${SDK_IN_DOCKER}" ]; then
		# sdk in docker
		export DOCKER_SDK_DIR="${RKROS_SDK_DIR}"
		local docker_volume="--volumes-from ${DOCKER_HOSTNAME}"
	else
		export DOCKER_SDK_DIR="/home/rk/sdk"
		local docker_volume="-v ${RKROS_SDK_DIR}/robot:${DOCKER_SDK_DIR}/robot"
	fi

	local docker_run_cmd="/usr/bin/docker run --privileged -it \
		-e HOST_UID=$(id -u) \
		-e HOST_GID=$(id -g) \
		-e RKROS_SDK_DIR=${RKROS_SDK_DIR} \
		-e DOCKER_SDK_DIR=${DOCKER_SDK_DIR} \
		${docker_volume} \
		${docker_image}"

	export DOCKER_RUN_CMD="${docker_run_cmd}"
}

docker_handle
