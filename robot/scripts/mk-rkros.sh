#! /bin/bash

deb_pack() {
	local pkg_name="$1"
	local debian_pkg="${pkg_name//_/-}"
	local debian_version="1.0.0-1"
	local debian_arch="arm64"
	local tmp_dir="$(mktemp -d)"
	local debian_dir="${tmp_dir}/${debian_pkg}-${debian_version}-${debian_arch}"
	local debian_deb="${debian_dir}.deb"
	local debian_control="${debian_dir}/DEBIAN/control"
	local debian_postinst="${debian_dir}/DEBIAN/postinst"

	mkdir -p "$(dirname "${debian_control}")"
	cat << EOF > "${debian_control}"
Package: ${debian_pkg}
Version: ${debian_version}
Architecture: ${debian_arch}
Section: utils
Priority: optional
Multi-Arch: foreign
Maintainer: Jinalong Wang <jianlong.wang@rock-chips.com>
Homepage: https://www.ros.org/
Description: ${pkg_name} binary
EOF

	mkdir -p "$(dirname "${debian_postinst}")"
	cat << EOF > "${debian_postinst}"
#! /bin/bash

SETUP_SCRIPT="/etc/profile.d/${pkg_name}.sh"

if [ -f "\$SETUP_SCRIPT" ]; then
    source "\${SETUP_SCRIPT}"
else
    echo "Warning: Setup script \${SETUP_SCRIPT} not found." >&2
fi

exit 0
EOF
	chmod a+x ${debian_postinst}

	mkdir -p "${debian_dir}/etc/profile.d"
	cat << EOF > "${debian_dir}/etc/profile.d/${pkg_name}.sh"
. /opt/rkros/${pkg_name}/share/${pkg_name}/package.sh
EOF

	mkdir -p "${debian_dir}/opt/rkros/"
	cp -rf "/opt/rkros/${pkg_name}" "${debian_dir}/opt/rkros/"

	echo -e "\e[36m"
	echo "Debian control file:"
	echo -e "\e[0m"
	cat "${debian_control}"

	echo -e "\e[36m"
	echo "Packing $(basename "${debian_deb}")..."
	echo -e "\e[0m"
	dpkg-deb -b "${debian_dir}"

	mkdir -p "${DOCKER_SDK_DIR}/robot/rkros/output"
	mv "${debian_deb}" "${DOCKER_SDK_DIR}/robot/rkros/output"

	rm -rf "${tmp_dir}"
}

rkros_pack() {
	rkros_list=$(find /opt/rkros -maxdepth 1 -type d -name "rkros_*" | awk -F '/' '{print$NF}')
	for pkg in ${rkros_list[@]}; do
		deb_pack ${pkg}
	done

	touch   ${DOCKER_SDK_DIR}/robot/rkros/output/.last_build

	echo -e "\e[35m"
	echo "Notice: rkros build succeeded."
	echo "Output: ${RKROS_SDK_DIR}/robot/rkros/output"
	echo -e "\e[0m"
}

rkros_pkg_name=$1
rkros_pkg_action=$2

echo -e "\e[36m"
echo "=========================================="
echo "           Start building rkros           "
echo "=========================================="
echo ""
echo "rkros_pkg_name: ${rkros_pkg_name}"
echo "rkros_pkg_action: ${rkros_pkg_action}"
echo -e "\e[0m"

case "${rkros_pkg_action}" in
	debug)
		cd "${DOCKER_SDK_DIR}/robot/rkros"

		if [ "${rkros_pkg_name}" = "rkros" ]; then
			colcon build --merge-install
		else
			colcon build --merge-install --packages-up-to ${rkros_pkg_name}
		fi
		;;
	release)
		cd "${DOCKER_SDK_DIR}/robot/rkros"

		if [ "${rkros_pkg_name}" = "rkros" ]; then
			colcon --log-base /opt/rkros/log build --build-base /opt/rkros/build --install-base /opt/rkros
		else
			colcon --log-base /opt/rkros/log build --build-base /opt/rkros/build --install-base /opt/rkros --packages-up-to ${rkros_pkg_name}
		fi
		rkros_pack
		;;
	*)
		:
		;;
esac
