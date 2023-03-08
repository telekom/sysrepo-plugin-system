#!/bin/env bash

log() { printf ">>> %s\n" "$*" >&2; }

if [ -z $SYSREPO_SYSTEM_PLUGIN_PATH ]; then
    log "SYSREPO_SYSTEM_PLUGIN_PATH not set to the plugin executable path"
    exit 1
fi

log "check if required modules are installed"

fail_module_check() { log "modules check failed - missing a module" && exit 1; }

python3 -m pip show rpaframework &> /dev/null || fail_module_check
python3 -m pip show SysrepoLibrary &> /dev/null || fail_module_check

log "modules check passed"

OUTPUTDIR=./out

robot                      \
    --outputdir $OUTPUTDIR \
    --output output.xml    \
    robot-ietf-system

robot                         \
    --outputdir $OUTPUTDIR    \
    --rerunfailed output.xml  \
    --output output_rerun.xml \
    robot-ietf-system

rebot                                                         \
    --outputdir $OUTPUTDIR                                    \
    --output output.xml                                       \
    --merge $OUTPUTDIR/output.xml $OUTPUTDIR/output_rerun.xml \

exit 0
