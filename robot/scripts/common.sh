#! /bin/bash

# $1: src_dir
# $2: last_build
# $3: timestamp
check_timestamp() {
	for file in $1/*; do
		if [ -d "${file}" ]; then
			check_timestamp "${file}" "$2" "$3"
		elif [ "${file}" -nt "$2" ]; then
			# something update
			touch "$3"
			return
		fi
	done
}

# $1: element
# $2: array[@]
element_in_array() {
	local element="$1"
	shift
	local found=0
	for seek; do
		if [[ "${seek}" == "${element}" ]]; then
			found=1
			break
		fi
	done

	return ${found}
}
