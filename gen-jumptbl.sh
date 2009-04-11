#!/bin/bash

#1>$2 

cat <<EOF
#ifndef COMPSL_JUMPTBL_H_
#define COMPSL_JUMPTBL_H_

static const ptrdiff_t jmptbl[] =
{
EOF

cut -d '#' -f1 $1 | cut -d: -f1 |
while read line; do
	if [ -n "$line" ]; then
		echo -ne "\tCSL_JTE(${line}),\n"
	fi
done

echo -ne "\tCSL_JTE(UNIMP)\n"

cat <<EOF
};

#endif
EOF
