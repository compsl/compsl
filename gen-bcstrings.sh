#!/bin/bash

#1>$2 

cat <<EOF
#ifndef COMPSL_BCSTRINGS_H_
#define COMPSL_BCSTRINGS_H_

static const char *tractbl[] = {
EOF

cut -d '#' -f1 $1 | cut -d: -f1 |
while read line; do
	if [ -n "$line" ]; then
		echo -ne "\t\"${line}\",\n"
	fi
done
echo NULL

cat <<EOF
};

#endif
EOF
