#! /bin/bash

mask_qemu_check() {
	sed -i '/^\"\$RK_SCRIPTS_DIR\/check-package\.sh\" qemu-\$QEMU_ARCH-static qemu-user-static/,+9 s/^/#/' "${RKROS_SDK_DIR}/device/rockchip/common/scripts/check-debian.sh"
}

ros2_humble_handle() {
	local tmp_file="$(mktemp)"

	cat << EOF > ${tmp_file}
#---------RKROS---------
echo -e "\033[36m Install RKROS.................... \033[0m"
sed -i 's/^# *\(en_US.UTF-8\)/\1/' /etc/locale.gen
locale-gen

\\\${APT_INSTALL} software-properties-common
\\\${APT_INSTALL} /packages/humble/ros2-apt-source.deb
apt-get update -y

\\\${APT_INSTALL} \
python3-flake8-docstrings \
python3-pip \
python3-pytest-cov \
ros-dev-tools

python3 -m pip install -U \
flake8-blind-except \
flake8-builtins \
flake8-class-newline \
flake8-comprehensions \
flake8-deprecated \
flake8-import-order \
flake8-quotes \
"pytest>=5.3" \
pytest-repeat \
pytest-rerunfailures \
empy==3.3.4

cat /packages/humble/rosdep_install_humble | xargs \\\${APT_INSTALL}

\\\${APT_INSTALL} \
libopencv-dev

rosdep init
su linaro
rosdep update
exit

\\\${APT_INSTALL} \
/packages/humble/ros2-humble-bin-1.0.0-1-arm64.deb \
/packages/humble/vision-opencv-1.0.0-1-arm64.deb \
/packages/output/*.deb

history -c

EOF

	if ! grep -qn -- "---------RKROS---------" "${debian_src_dir}/mk-rootfs-bullseye.sh" &>/dev/null; then
		sed -i "$(awk '/rm -rf \/var\/lib\/apt\/lists\/*/{print NR-1; exit}' ${debian_src_dir}/mk-rootfs-bullseye.sh)r ${tmp_file}" "${debian_src_dir}/mk-rootfs-bullseye.sh"
	fi
	rm -rf ${tmp_file}
}

ros2_jazzy_handle() {
	local tmp_file="$(mktemp)"

	cat << EOF > ${tmp_file}

#---------RKROS---------
echo -e "\033[36m Install RKROS.................... \033[0m"
sed -i 's/^# *\(en_US.UTF-8\)/\1/' /etc/locale.gen
locale-gen

\\\${APT_INSTALL} software-properties-common
\\\${APT_INSTALL} /packages/jazzy/ros2-apt-source.deb
apt-get update -y

\\\${APT_INSTALL} \
python3-flake8-blind-except \
python3-flake8-class-newline \
python3-flake8-deprecated \
python3-mypy \
python3-pip \
python3-pytest \
python3-pytest-cov \
python3-pytest-mock \
python3-pytest-repeat \
python3-pytest-rerunfailures \
python3-pytest-runner \
python3-pytest-timeout \
ros-dev-tools

cat /packages/jazzy/rosdep_install_jazzy | xargs \\\${APT_INSTALL}

rosdep init
su linaro
rosdep update
exit

\\\${APT_INSTALL} \
/packages/jazzy/ros2-jazzy-bin-1.0.0-1-arm64.deb \
/packages/output/*.deb

history -c

EOF

	if ! grep -qn -- "---------RKROS---------" "${debian_src_dir}/mk-rootfs-bookworm.sh" &>/dev/null; then
		sed -i "$(awk '/cd -/{print NR-1; exit}' ${debian_src_dir}/mk-rootfs-bookworm.sh)r ${tmp_file}" "${debian_src_dir}/mk-rootfs-bookworm.sh"
	fi
	rm -rf ${tmp_file}
}

debian_ver="${DEBIAN_VERSION}"
debian_src_dir="${RKROS_SDK_DIR}/debian"
debian_pkg_dir="${RKROS_SDK_DIR}/debian/packages/arm64"

if [ "${debian_ver}" = "bullseye" ]; then
	ros2_ver="humble"
elif [ "${debian_ver}" = "bookworm" ]; then
	ros2_ver="jazzy"
fi

cp -rf ${RKROS_SDK_DIR}/robot/prebuilts/ros2/${ros2_ver} ${debian_pkg_dir}
cp -rf ${RKROS_SDK_DIR}/robot/rkros/output ${debian_pkg_dir}

ros2_${ros2_ver}_handle

if [ -n "${SDK_IN_DOCKER}" ]; then
	# sdk in docker
	mask_qemu_check
fi
