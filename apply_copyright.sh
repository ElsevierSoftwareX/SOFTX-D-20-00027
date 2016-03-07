#! /bin/bash

SRC_DIR="$(dirname $(realpath $0))"

PROGNAME="Celltracker – A curation tool for object tracks."
YEAR=$(date +%Y)
AUTHORS="TU Dresden"

declare -A CLEAN_AUTHOR_MAP
CLEAN_AUTHOR_MAP["Enrico"]="Enrico Uhlig"
CLEAN_AUTHOR_MAP["Konstantin"]="Konstantin Thierbach"
CLEAN_AUTHOR_MAP["Konstantin Thierbach"]="Konstantin Thierbach"
CLEAN_AUTHOR_MAP["Sebastian Wagner"]="Sebastian Wagner"

CR_HEADER="${PROGNAME}
Copyright (C) ${YEAR} ${AUTHORS}

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA"

LICENSE_URL="https://www.gnu.org/licenses/lgpl-3.0.txt"
LICENSE_FILE="COPYING"

declare -A EXT_COMMENT_MAP_BEGIN
EXT_COMMENT_MAP_BEGIN["cpp"]="/*"
EXT_COMMENT_MAP_BEGIN["h"  ]="/*"
EXT_COMMENT_MAP_BEGIN["svg"]="<!--"
EXT_COMMENT_MAP_BEGIN["qml"]="/*"

declare -A EXT_COMMENT_MAP_LINE
EXT_COMMENT_MAP_LINE["cpp"]=" *"
EXT_COMMENT_MAP_LINE["h"  ]=" *"
EXT_COMMENT_MAP_LINE["qml"]=" *"

declare -A EXT_COMMENT_MAP_END
EXT_COMMENT_MAP_END["cpp"]=" */"
EXT_COMMENT_MAP_END["h"  ]=" */"
EXT_COMMENT_MAP_END["svg"]="-->"
EXT_COMMENT_MAP_END["qml"]=" */"

function commentForExt() {
	EXTENSION="$1"
	TEXT="$2"
	printf "${EXT_COMMENT_MAP_BEGIN[$EXTENSION]}\n"
	while read -r line
	do
		if [ -n ${EXT_COMMENT_MAP_LINE[$EXTENSION]+x} ]; then
		  printf "${EXT_COMMENT_MAP_LINE[$EXTENSION]} "
		fi
		printf "$line\n"
	done < <(printf "%s\n" "$TEXT")
	printf "${EXT_COMMENT_MAP_END[$EXTENSION]}\n"
}

function commentFile() {
	FILE="$1"
	FILENAME="$(basename $1)"
	EXTENSION="${FILENAME##*.}"

	AUTHORS="$(git log --pretty=format:'%an' "$FILE" | sort -u)"
	echo "$AUTHORS"

}

commentFile "Celltracker.pro"
commentForExt "cpp" "$CR_HEADER"
