#!/bin/env bash

EXIT_SUCCESS=0
EXIT_NO_BINARY=1
EXIT_NO_MODULES=2

log() { printf ">>> %s\n" "$*" >&2; }

# check if path to the system plugin binary is set
if [ -z $SYSREPO_SYSTEM_PLUGIN_PATH ]; then
    log "SYSREPO_SYSTEM_PLUGIN_PATH not set to the plugin executable path"
    exit $EXIT_NO_BINARY
fi

log "check if required modules are installed"

# check dependencies
DEPS=("rpaframework" \
      "robotframework-sysrepolibrary")
for MODULE in "${DEPS[@]}"; do
    python3 -m pip show $MODULE &> /dev/null
    if [ $? -ne 0 ]; then
        log "module $MODULE not found" 
        exit $EXIT_NO_MODULES
    fi
done

log "modules check passed"

# run the tests
robot                      \
    --output output.xml    \
    robot-ietf-system

# rerun based on exit code
if [ $? -ne 0 ]; then
    log "some tests failed, rerunning tests"
    robot                         \
        --rerunfailed output.xml  \
        --output output_rerun.xml \
        robot-ietf-system

    log "merging output file"
    rebot                                   \
        --output output.xml                 \
        --merge output.xml output_rerun.xml 
fi

exit $EXIT_SUCCESS
