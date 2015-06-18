#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: parser.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v bison >/dev/null 2>&1 || { echo "bison is not installed, aborting" >&2; exit 1; }

BASE_DIRECTORY=$(dirname $0)
SOURCE_DIRECTORY=${BASE_DIRECTORY}

SOURCE_FILE=${SOURCE_DIRECTORY}/IRCparser.y
[ ! -f ${SOURCE_FILE} ] && echo "ERROR: file ${SOURCE_FILE} not found, aborting" && exit 1
#bison --verbose --graph=parser_graph.txt --xml=parser_graph.xml ${SOURCE_FILE} --report=all --report-file=parser_report.txt --warnings=all --warnings=no-precedence
bison --verbose --graph=parser_graph.txt --xml=parser_graph.xml ${SOURCE_FILE} --report=all --report-file=parser_report.txt --warnings=all
[ $? -ne 0 ] && echo "ERROR: \"${SOURCE_FILE}\" failed (status was: $?), aborting" && exit 1

# move generated file(s) into the project directory
#mv -f position.hh ./..
#mv -f stack.hh ./..
#mv -f location.hh ./..
# --> these files are static (*CHECK*) and included by default
#mv -f IRC_client_IRCparser.h/.cpp ./..
# *NOTE*: need to add a specific method to the parser class
# --> copy a pre-patched version (back) into the project directory instead
# *TODO*: needs to be updated after every change
TARGET_DIRECTORY=${BASE_DIRECTORY}/..
TARGET_FILE=${TARGET_DIRECTORY}/IRC_client_IRCparser.h
SOURCE_FILE=${SOURCE_DIRECTORY}/IRC_client_IRCparser_patched.h
[ ! -f ${SOURCE_FILE} ] && echo "ERROR: file ${SOURCE_FILE} not found, aborting" && exit 1

cp -f ${SOURCE_FILE} ${TARGET_FILE}
[ $? -ne 0 ] && echo "ERROR: failed to cp \"${SOURCE_FILE}\", aborting" && exit 1
# clean up
SOURCE_FILE=./IRC_client_IRCparser.h
rm -f ${SOURCE_FILE}

#FILES="IRC_client_IRCparser.cpp
#location.hh
#position.hh
#stack.hh"
FILES="IRC_client_IRCparser.cpp"
for FILE in $FILES
do
 mv -f ./${FILE} ${TARGET_DIRECTORY}
 [ $? -ne 0 ] && echo "ERROR: failed to mv \"${FILE}\", aborting" && exit 1
done