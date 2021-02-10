# Sysrepo DHCP plugin (DT)

## Introduction

This Sysrepo plugin is responsible for bridging a Linux system and Sysrepo/YANG datastore system configuration.

## Development Setup

Setup the development environment using the provided [`setup-dev-sysrepo`](https://github.com/sartura/setup-dev-sysrepo) scripts. This will build all the necessary components.

Subsequent rebuilds of the plugin may be done by navigating to the plugin source directory and executing:

```
$ export SYSREPO_DIR=${HOME}/code/sysrepofs
$ cd ${SYSREPO_DIR}/repositories/plugins/dhcp

$ rm -rf ./build && mkdir ./build && cd ./build
$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_PREFIX_PATH=${SYSREPO_DIR} \
		-DCMAKE_INSTALL_PREFIX=${SYSREPO_DIR} \
		-DCMAKE_BUILD_TYPE=Debug \
		..
-- The C compiler identification is GNU 9.3.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
[...]
-- Configuring done
-- Generating done
-- Build files have been written to: ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-general/build

$ make && make install
[...]
[ 75%] Building C object CMakeFiles/.dir/src/utils/memory.c.o
[100%] Linking C executable sysrepo-plugin-general
[100%] Built target sysrepo-plugin-general
[100%] Built target sysrepo-plugin-general
Install the project...
-- Install configuration: "Debug"
-- Installing: ${SYSREPO_DIR}/bin/sysrepo-plugin-general
-- Set runtime path of "${SYSREPO_DIR}/bin/sysrepo-plugin-general" to ""

$ cd ..
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

This plugin is installed as the `sysrepo-plugin-general` binary to `${SYSREPO_DIR}/bin/` directory path. Simply invoke this binary, making sure that the environment variables are set correctly:

```
$ sysrepo-plugin-general
[INF]: Applying scheduled changes.
[INF]: No scheduled changes.
[INF]: Connection 2 created.
[INF]: Session 3 (user "...", CID 2) created.
[INF]: plugin: start session to startup datastore
[INF]: Session 4 (user "...", CID 2) created.
[INF]: plugin: subscribing to module change
[INF]: plugin: subscribing to get oper items
[INF]: plugin: subscribing to rpc
[INF]: plugin: plugin init done
[...]
```


## TODO: Update everything below


Output from the plugin is expected; the plugin has loaded UCI configuration at `${SYSREPO_DIR}/etc/config/network` into the `startup` datastore. We can confirm this by invoking the following commands:

```
$ cat ${SYSREPO_DIR}/etc/config/network
config interface 'loopback'
        option is_lan '1'
        option ifname 'lo'
        option proto 'static'
        option ipaddr '127.0.0.1'
        option netmask '255.0.0.0'

config interface 'lan'
        option is_lan '1'
        option type 'bridge'
        option proto 'static'
        option ipaddr '192.168.1.1'
        option netmask '255.255.255.0'
        option ip6assign '64'
        option ifname 'eth1 eth2 eth3 wl0 wl1'
        list ip6class 'local'
        list ip6class '5f414e59'

config interface 'wan'
        option proto 'dhcpv6'
        option ifname 'eth0.1'
        option accept_ra '1'
        option request_pd '3'
        option aftr_v4_local '192.0.0.2'
        option aftr_v4_remote '192.0.0.1'
        option request_na '0'
        option reqopts '21 23 31 56 64 67 88 96 99 123 198 199'

config interface 'lan_iptv'
        option proto 'static'
        option ifname 'eth4'
        option ipaddr '192.168.2.1'
        option netmask '255.255.255.0'
        option ip6assign '64'
        list ip6class 'local'
        list ip6class '5f414e59'
        list ip6class '49505456'
        list ip6class '564f4950'

$ sysrepocfg -X -d startup -f json -m 'terastream-dhcp'
{
  "terastream-dhcp:dhcp-clients": {
    "dhcp-client": [
      {
        "name": "wan",
        "proto": "dhcpv6",
        "accept_ra": true,
        "request_pd": "3",
        "aftr_v4_local": "192.0.0.2",
        "aftr_v4_remote": "192.0.0.1",
        "request_na": "0",
        "reqopts": "21 23 31 56 64 67 88 96 99 123 198 199"
      }
    ]
  }
}
```

Provided output suggests that the plugin has correctly initialized Sysrepo `startup` datastore with appropriate data transformations. It can be seen that only the `dhcp-clients` container has been populated as other types are not available.

Changes to the `running` datastore can be done manually by invoking the following command:

```
$ sysrepocfg -E -d running -f json -m 'terastream-dhcp'
[...interactive...]
{
  "terastream-dhcp:dhcp-clients": {
    "dhcp-client": [
      {
        "name": "wan",
        "proto": "dhcpv6",
        "accept_ra": true, // => false
        "request_pd": "3",
        "aftr_v4_local": "192.0.0.2",
        "aftr_v4_remote": "192.0.0.1",
        "request_na": "0",
        "reqopts": "21 23 31 56 64 67 88 96 99 123 198 199"
      }
    ]
  }
}
```

Alternatively, instead of changing the entire module data with `-m 'terastream-dhcp'` we can change data on a certain XPath with e.g. `-x '/terastream-dhcp:dhcp-clients'`.

After executing previous command, the following should appear at plugin binary standard output:

```
[INF]: Processing "terastream-dhcp" "change" event with ID 1 priority 0 (remaining 1 subscribers).
[INF]: plugin: module_name: terastream-dhcp, xpath: /terastream-dhcp:*//*, event: 1, request_id: 1
[DBG]: plugin: uci_path: network.wan.accept_ra; prev_val: true; node_val: false; operation: 1
[INF]: Successful processing of "change" event with ID 1 priority 0 (remaining 0 subscribers).
[INF]: Processing "terastream-dhcp" "done" event with ID 1 priority 0 (remaining 1 subscribers).
[INF]: plugin: module_name: terastream-dhcp, xpath: /terastream-dhcp:*//*, event: 2, request_id: 1
[INF]: Successful processing of "done" event with ID 1 priority 0 (remaining 0 subscribers).
```

The datastore change operation should be reflected in the `/etc/config/network` UCI file:

```
$ cat /etc/config/network | grep accept_ra
        option accept_ra '1'
```

In constrast to the configuration state data, using `sysrepocfg` we can access `operational` state data. For example:

```
$ sysrepocfg -X -d operational -f json -x '/terastream-dhcp:dhcp-v4-leases'
{
  "terastream-dhcp:dhcp-v4-leases": {
    "dhcp-v4-lease": [
      {
        "name": "lease-1",
        "leasetime": 1513286887,
        "hostname": "archy",
        "ipaddr": "192.168.1.231",
        "macaddr": "54:ee:75:94:0b:13",
        "device": "br-lan",
        "connected": true
      }
    ]
  }
}
```

This data is usually provided by certain `ubus` methods which can be acessed via the `ubus` command line utility:

```
$ ubus call router.network leases
{
        "lease-1": {
                "leasetime": "1513286887",
                "hostname": "archy",
                "ipaddr": "192.168.1.231",
                "macaddr": "54:ee:75:94:0b:13",
                "device": "br-lan",
                "connected": true
        }
}
```