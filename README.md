# Sysrepo general plugin

## Introduction

This Sysrepo plugin is responsible for bridging a Linux system and Sysrepo/YANG datastore system configuration.

## Dependencies
Other than libyang 1 and Sysrepo, no build time dependencies are required.
However, to use the NTP part of the plugin, `ntpd` is required, as the plugin
uses `/etc/ntp.conf`

## Build

This section describes how to build the plugin on hosts that have Sysrepo installed. This 
includes standard Linux machines and docker images with Sysrepo, Netopeer and other required dependencies.
Additionally, a Dockerfile that builds all the dependencies and the plugin is available in `docker/`

First, clone the repo:
```
$ git clone git@lab.sartura.hr:sysrepo/sysrepo-plugin-general.git
```

Next, make a build directory and prepare the build scripts:

```
$ mkdir build && cd build
$ cmake ..
```

This will build the plugin as a standalone executable which can be run without `sysrepo-plugind`

To build the project in plugin mode, run the following instead:

```
$ cmake -DPLUGIN=ON ..
```

After that, run `make` and `make install`

Finally to run the plugin, the `iana-crypt-hash` and `ietf-system` YANG modules need to be installed.

```
$ sysrepoctl -i ../yang/ietf-system@2014-08-06.yang
$ sysrepoctl -i ../yang/iana-crypt-hash@2014-08-06.yang
```

## Development Setup with setup-dev-sysrepo scripts

Setup the development environment using the provided [`setup-dev-sysrepo`](https://github.com/sartura/setup-dev-sysrepo) scripts. This will build all the necessary components.

Subsequent rebuilds of the plugin may be done by navigating to the plugin source directory and executing:

```
$ export SYSREPO_DIR=${HOME}/code/sysrepofs
$ cd ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-general

$ rm -rf ./build && mkdir ./build && cd ./build
$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_PREFIX_PATH=${SYSREPO_DIR} \
		-DCMAKE_INSTALL_PREFIX=${SYSREPO_DIR} \
		-DCMAKE_BUILD_TYPE=Debug \
		..
-- Configuring done
-- Generating done
-- Build files have been written to: ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-general/build
Scanning dependencies of target sysrepo-plugin-general
[ 33%] Building C object CMakeFiles/sysrepo-plugin-general.dir/src/general.c.o
[ 66%] Linking C executable sysrepo-plugin-general
[100%] Built target sysrepo-plugin-general

$ make && make install
[...]
[100%] Built target sysrepo-plugin-general
[100%] Built target sysrepo-plugin-general
Install the project...
-- Install configuration: "Debug"
-- Installing: ${SYSREPO_DIR}/bin/sysrepo-plugin-general
-- Set runtime path of "${SYSREPO_DIR}/bin/sysrepo-plugin-general" to ""

-$ cd ..
```

Before using the plugin it is necessary to install relevant YANG modules. For this particular plugin, the following commands need to be invoked:

```
$ cd ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-general
$ export LD_LIBRARY_PATH="${SYSREPO_DIR}/lib64;${SYSREPO_DIR}/lib"
$ export PATH="${SYSREPO_DIR}/bin:${PATH}"

$ sysrepoctl -i ./yang/ietf-system@2014-08-06.yang
```

## YANG Overview

The `ietf-system` YANG module with the `sys` prefix consists of the following `container` paths:

* `/ietf-system:system` — configuration state data for the system

The following items are not configurational i.e. they are `operational` state data:

* `/ietf-system:system-state` — operational data with general information about the system

The following items are `RPC`:

* `/ietf-system:rpcs` — RPC (set-current-datetime, system-restart and system-shutdown)

## Running and Examples

This plugin is installed as the `sysrepo-plugin-general` binary to `${SYSREPO_DIR}/bin/` directory path. Before executing the plugin binary it is necessary to initialize the datastore with appropriate example data, but before that we have to enable some features:

```
$ sysrepoctl --change ietf-system --enable-feature timezone-name
$ sysrepoctl --change ietf-system --enable-feature ntp
```

After the timezone-name and ntp features are enabled we can initialize the datastore with appropriate example data:

```
$ sysrepocfg -f xml --copy-from=examples/system.xml -d startup -m 'ietf-system'
$ sysrepocfg -f xml --copy-from=examples/system.xml -d running -m 'ietf-system'
```

After loading the example simply invoke this binary, making sure that the environment variables are set correctly. 
Specifically, the `GEN_PLUGIN_DATA_DIR` has to be set, to a directory where the plugin's files can be stored:

```
mkdir -p /usr/local/lib/sysrepo-general-plugin
export GEN_PLUGIN_DATA_DIR=/usr/local/lib/sysrepo-general-plugin

$ sysrepo-plugin-general
[...]
[INF]: Applying scheduled changes.
[INF]: No scheduled changes.
[INF]: Connection "..." created.
[INF]: Session "..." (user "...", CID "...") created.
[INF]: plugin: start session to startup datastore
[INF]: Session "..." (user "...", CID "...") created.
[INF]: plugin: subscribing to module change
[INF]: plugin: subscribing to get oper items
[INF]: plugin: subscribing to rpc
[INF]: plugin: plugin init done
[...]
```

Output from the plugin is expected; the plugin has been initialized with `startup` and `running` datastore contents at `${SYSREPO_DIR}/etc/sysrepo`. We can confirm the contents present in Sysrepo by invoking the following command:

```
$ sysrepocfg -X -d startup -f json -m 'ietf-system'
{
  "ietf-system:system": {
    "clock": {
      "timezone-name": "Europe/Stockholm"
    },
    "location": "basement",
    "contact": "Mr. Admin",
    "hostname": "test.it",
    "ntp": {
      "enabled": true,
      "server": [
        {
          "name": "hr.pool.ntp.org",
          "udp": {
            "address": "162.159.200.123"
          }
        }
      ]
    }
  }
}
```

Operational state data, as defined by the `ietf-system` module can be accessed with:

```
$ sysrepocfg -X -d operational -f json -x '/ietf-system:system-state'
{
  "ietf-system:system-state": {
    "platform": {
      "os-name": "Linux",
      "os-release": "5.10.16-arch1-1",
      "os-version": "#1 SMP PREEMPT Sat, 13 Feb 2021 20:50:18 +0000",
      "machine": "x86_64"
    },
    "clock": {
      "current-datetime": "2021-02-15T18:53:47Z",
      "boot-datetime": "2021-02-15T14:31:33Z"
    }
  }
}
```

Additionally, this plugin handles various RPC paths. For instance, invoking the following example action will set the provided datetime on the system:

```
$ sysrepocfg --rpc=examples/set_datetime.xml -m "ietf-system"
```

This action is followed by output on the plugin standard output:

```
[...]
[INF]: Processing "/ietf-system:set-current-datetime" "rpc" event with ID 1 priority 0 (remaining 1 subscribers).
[INF]: plugin: system_rpc_cb: CURR_DATETIME_YANG_PATH and system time successfully set!
[INF]: Successful processing of "rpc" event with ID 1 priority 0 (remaining 0 subscribers).
[...]
```

## Testing

### Manual tests
The `tests/manual_tests.md` file contains a list of sysrepo commands that can be used to test
the currently implemented features of the plugin.
It can also serve as a list of currently implemented YANG nodes in the model.

### Unit tests
Unit tests for the plugin are available in `tests/`. To build the unit tests, use the `-DENABLE_BUILD_TESTS=ON` option when running cmake. To then run the tests, run `ctest` in the build directory.

The tests can be built with sanitizers, by setting appropriate compile flags through cmake (`-DCMAKE_C_FLAGS='-fsanitize=address,undefined` for example).


## Nodes that are currently implemented
- DONE - nodes are implemented and a value is provided if such information can be retrieved
- NA - node is not implemented
- IN PROGRESS - implementation in progress

```
module: ietf-system
  +--rw system
  |  +--rw contact?          string                                         DONE
  |  +--rw hostname?         inet:domain-name                               DONE
  |  +--rw location?         string                                         DONE
  |  +--rw clock
  |  |  +--rw (timezone)?
  |  |     +--:(timezone-name) {timezone-name}?
  |  |     |  +--rw timezone-name?         timezone-name                    DONE
  |  |     +--:(timezone-utc-offset)
  |  |        +--rw timezone-utc-offset?   int16                            NA
  |  +--rw ntp! {ntp}?
  |  |  +--rw enabled?   boolean                                            DONE
  |  |  +--rw server* [name]
  |  |     +--rw name                string                                 DONE
  |  |     +--rw (transport)
  |  |     |  +--:(udp)
  |  |     |     +--rw udp
  |  |     |        +--rw address    inet:host                              DONE
  |  |     |        +--rw port?      inet:port-number {ntp-udp-port}?       DONE
  |  |     +--rw association-type?   enumeration                            DONE
  |  |     +--rw iburst?             boolean                                DONE
  |  |     +--rw prefer?             boolean                                DONE
  |  +--rw dns-resolver
  |  |  +--rw search*    inet:domain-name                                   IN PROGRESS (implemented with lwres but needs to be implemented using systemd)
  |  |  +--rw server* [name]
  |  |  |  +--rw name                 string                                IN PROGRESS (implemented with lwres but needs to be implemented using systemd)
  |  |  |  +--rw (transport)
  |  |  |     +--:(udp-and-tcp)
  |  |  |        +--rw udp-and-tcp
  |  |  |           +--rw address    inet:ip-address                        IN PROGRESS (implemented with lwres but needs to be implemented using systemd)
  |  |  |           +--rw port?      inet:port-number {dns-udp-tcp-port}?   NA
  |  |  +--rw options
  |  |     +--rw timeout?    uint8                                          IN PROGRESS (implemented with lwres but needs to be implemented using systemd)
  |  |     +--rw attempts?   uint8                                          IN PROGRESS (implemented with lwres but needs to be implemented using systemd)
  |  +--rw radius {radius}?
  |  |  +--rw server* [name]
  |  |  |  +--rw name                   string                              NA
  |  |  |  +--rw (transport)
  |  |  |  |  +--:(udp)
  |  |  |  |     +--rw udp
  |  |  |  |        +--rw address                inet:host                  NA
  |  |  |  |        +--rw authentication-port?   inet:port-number           NA
  |  |  |  |        +--rw shared-secret          string                     NA
  |  |  |  +--rw authentication-type?   identityref                         NA
  |  |  +--rw options
  |  |     +--rw timeout?    uint8                                          NA
  |  |     +--rw attempts?   uint8                                          NA
  |  +--rw authentication {authentication}?
  |     +--rw user-authentication-order*   identityref                      NA
  |     +--rw user* [name] {local-users}?
  |        +--rw name              string                                   IN PROGRESS
  |        +--rw password?         ianach:crypt-hash                        IN PROGRESS
  |        +--rw authorized-key* [name]
  |           +--rw name         string                                     IN PROGRESS
  |           +--rw algorithm    string                                     IN PROGRESS
  |           +--rw key-data     binary                                     IN PROGRESS
  +--ro system-state
     +--ro platform
     |  +--ro os-name?      string                                          DONE
     |  +--ro os-release?   string                                          DONE
     |  +--ro os-version?   string                                          DONE
     |  +--ro machine?      string                                          DONE
     +--ro clock
        +--ro current-datetime?   yang:date-and-time                        DONE
        +--ro boot-datetime?      yang:date-and-time                        DONE

  rpcs:
    +---x set-current-datetime
    |  +---w input
    |     +---w current-datetime    yang:date-and-time                      DONE
    +---x system-restart                                                    DONE
    +---x system-shutdown                                                   DONE

```

