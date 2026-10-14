#! /bin/bash

export RKROS_SCRIPTS_DIR="$(dirname $(realpath ${BASH_SOURCE}))"
export RKROS_SDK_DIR="$(realpath ${RKROS_SCRIPTS_DIR}/../../)"
export RKROS_DEVICE_DIR="${RKROS_SDK_DIR}/device/rockchip"
export RKROS_CHIP_DIR="${RKROS_DEVICE_DIR}/.chip"
export RKROS_CHIP="$(echo $(realpath ${RKROS_CHIP_DIR}) | awk -F '/' '{print$NF}')"

export COMMON_BUILD_SCRIPT="${RKROS_SDK_DIR}/device/rockchip/common/scripts/build.sh"
export DEBIAN_VERSION="$(ls ${RKROS_SDK_DIR}/debian/mk-rootfs-*.sh | awk -F [-.] '{print $(NF-1)}')"

docker_hostname="$(env | grep HOSTNAME | cut -d= -f2)"
if [ -n "${docker_hostname}" ]; then
	export DOCKER_HOSTNAME="${docker_hostname}"
	export SDK_IN_DOCKER=1
fi

# source some common functions
source ${RKROS_SCRIPTS_DIR}/common.sh

supported_rkros_pkgs=$(find "${RKROS_SDK_DIR}/robot/rkros/src" -name package.xml | xargs grep '<name>' | grep -oP '<name>\K[^<]+' | sort -u | sort -n)

usage_oneline() {
        printf "%-40s%s\n" "$1" "${*:2}"
}

rkros_usage() {
	echo -en "\e[36m"
	echo "Available options for RKROS:"
	echo -e "\e[0m"
	usage_oneline "rkros[:debug|:release]" "build all rkros packages"
	usage_oneline "rkros[:cleanall]" "cleanup all rkros packages"
	usage_oneline "rkros[:<pkg>-[debug|release]]" "build the specified rkros package"
	echo "  supported rkros packages:"
	echo ${supported_rkros_pkgs} | xargs -n1 echo | sed "s/^/    /"
}

rkros_handle() {
	local input="$1"
	local prefix=""
	local suffix=""
	local rkros_pkg_name=""
	local rkros_pkg_action=""

	if [ "${input}" = "rkros" ]; then
		rkros_pkg_name="rkros"
		rkros_pkg_action="debug"
		source ${RKROS_SCRIPTS_DIR}/check-docker.sh
		${DOCKER_RUN_CMD} "${DOCKER_SDK_DIR}/robot/scripts/mk-rkros.sh ${rkros_pkg_name} ${rkros_pkg_action}"
		return $?
	fi

	if [[ "${input}" == *:* ]]; then
		prefix="${input%%:*}"
		suffix="${input#*:}"

		if [ "${suffix}" = "cleanall" ]; then
			cd ${RKROS_SDK_DIR}/robot/rkros
			rm -rf build install log output > /dev/null 2>&1
			echo -e "\e[35m"
			echo "Notice: rkros cleanall succeeded."
			echo -e "\e[0m"
			return 0
		fi

		if [ "${suffix}" = "debug" ] || [ "${suffix}" = "release" ]; then
			rkros_pkg_name="${prefix}"
			rkros_pkg_action="${suffix}"
		elif [[ "${suffix}" == *-* ]]; then
			rkros_pkg_name="${suffix%-*}"
			rkros_pkg_action="${suffix#*-}"

			if [ "${rkros_pkg_action}" != "debug" ] && [ "${rkros_pkg_action}" != "release" ]; then
				echo -e "\e[91m"
				echo "Error: unsupported package action: '${rkros_pkg_action}', only supports 'debug' or 'release'."
				echo -e "\e[0m"
				return 1
			fi

			if element_in_array ${rkros_pkg_name} ${supported_rkros_pkgs[@]}; then
				echo -e "\e[91m"
				echo "Error: unsupported package name: '${rkros_pkg_name}'."
				echo -e "\e[0m"
				return 1
			fi
		else
			echo -e "\e[91m"
			echo "Error: invalid rkros argument format: '${suffix}'."
			echo -e "\e[0m"
			return 1
		fi

		if [ -z ${rkros_pkg_name} ]; then
			echo -e "\e[91m"
			echo "Error: invalid rkros package format: '${rkros_pkg_name}'."
			echo -e "\e[0m"
			return 1
		fi

		source ${RKROS_SCRIPTS_DIR}/check-docker.sh
		${DOCKER_RUN_CMD} "${DOCKER_SDK_DIR}/robot/scripts/mk-rkros.sh ${rkros_pkg_name} ${rkros_pkg_action}"
	else
		echo -e "\e[91m"
		echo "Error: unsupported argument: '${input}'."
		echo -e "\e[0m"
		return 1
	fi
}

case "$1" in
	rkros*)
		rkros_handle $1
		;;
	*)
		if [ "$1" = "debian" ] || [ "$1" = "all" ] || [ "$1" = "release" ] || [ "$1" = "all-release" ] || [ -z $1 ]; then
			if [ ! -f "${RKROS_SDK_DIR}/output/.config" ]; then
				$COMMON_BUILD_SCRIPT chip
			fi

			rkros_src="${RKROS_SDK_DIR}/robot/rkros/src"
			rkros_last_build="${RKROS_SDK_DIR}/robot/rkros/output/.last_build"
			rkros_timestamp="${RKROS_SDK_DIR}/robot/rkros/output/.timestamp"
			mkdir -p "${RKROS_SDK_DIR}/robot/rkros/output"
			check_timestamp ${rkros_src} ${rkros_last_build} ${rkros_timestamp}

			if [ "${rkros_timestamp}" -nt "${rkros_last_build}" ]; then
				source ${RKROS_SCRIPTS_DIR}/check-docker.sh
				${DOCKER_RUN_CMD} "${DOCKER_SDK_DIR}/robot/scripts/mk-rkros.sh rkros release"
			fi

			${RKROS_SCRIPTS_DIR}/mk-debian.sh
		fi

		if [ "$1" = "cleanall" ]; then
			pushd ${RKROS_SDK_DIR}/robot/rkros > /dev/null 2>&1
			rm -rf build install log output > /dev/null 2>&1
			popd > /dev/null 2>&1
		fi

		if [ "$1" = "-h" ] || [ "$1" = "--help" ] || [ "$1" = "help" ]; then
			$COMMON_BUILD_SCRIPT --help
			echo ""
			rkros_usage
			exit 0
		fi

		$COMMON_BUILD_SCRIPT $1
		;;
esac
