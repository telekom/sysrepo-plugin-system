# Manual testing

## Intro
This document contains a list of sysrepocfg commands that can be used to testand expected outputs
currently implemented plugin functionality.

It can also serve as a reference containing a list of currently implemented leafs in the model.

It is assumed that the datastore has already been initialized, and the data in examples imported.
To import the example data run the following command:
```
sysrepocfg --edit=examples/system.xml -m "ietf-system"
```

### ietf-system

First we can confirm that the datastore contains any data at all.

```
$ sysrepocfg -X -m ietf-system
sysrepocfg -X -m 'ietf-system'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <contact>Mr. Admin</contact>
  <hostname>test.it</hostname>
  <clock>
    <timezone-name>Europe/Stockholm</timezone-name>
  </clock>
  <ntp>
    <enabled>true</enabled>
    <server>
      <name>hr.pool.ntp.org</name>
      <udp>
        <address>162.159.200.123</address>
      </udp>
    </server>
  </ntp>
</system>
```

#### Contact
``` 
$ sysrepocfg -X -x '/ietf-system:system/contact
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <contact>Mr. Admin</contact>
</system>
```

The contact information is retrieved from the `/etc/passwd` file, which we can confirm.
```
$ cat /etc/passwd
root:x:0:0:Mr. Admin:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
sys:x:3:3:sys:/dev:/usr/sbin/nologin
sync:x:4:65534:sync:/bin:/bin/sync
```

We can change the contact field, which will store new contact information in the `/etc/passwd` file.

```
$ sysrepocfg -X -x '/ietf-system:system/contact'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <contact>test contact</contact>
</system>

$ cat /etc/passwd
root:x:0:0:test contact:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
sys:x:3:3:sys:/dev:/usr/sbin/nologin
sync:x:4:65534:sync:/bin:/bin/sync
```

#### Hostname
We can retrieve the hostname:

```
$ sysrepocfg -X -x '/ietf-system:system/hostname'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <hostname>test.it</hostname>
</system>
```

The hostname is set and retrieved using the `sethostname()` and `gethostname()` system calls.

We can verify that the hostname matches the system hostname:
```
hostnamectl
   Static hostname: c70d08e518bd
Transient hostname: test.it
         Icon name: computer-laptop
           Chassis: laptop
        Machine ID: d95430cef4c5dce339469c8c6011e470
           Boot ID: ac5d9f56dbde476aaec818a8016df0d7
  Operating System: Ubuntu 18.04.5 LTS
            Kernel: Linux 5.10.8
      Architecture: x86-64
```

The plugin also supports hostname editing. After setting it to `test2` with `sysrepocfg -Evim -fjson -m ietf-system`,
we can verify that the changes have been applied.

```
$ sysrepocfg -X -x '/ietf-system:system/hostname'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <hostname>test2</hostname>
</system>
$ hostnamectl
   Static hostname: c70d08e518bd
Transient hostname: test2
         Icon name: computer-laptop
           Chassis: laptop
        Machine ID: d95430cef4c5dce339469c8c6011e470
           Boot ID: ac5d9f56dbde476aaec818a8016df0d7
  Operating System: Ubuntu 18.04.5 LTS
            Kernel: Linux 5.10.8
      Architecture: x86-64
```

#### Location
We can retrieve the location:

```
$ sysrepocfg -X -x '/ietf-system:system/location'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <location>basement</location>
</system>
```

The location is set and retrieved from a file (`location_info`) located in a directory whos path is set via the `GEN_PLUGIN_DATA_DIR` environment variable.

After setting it to `3rd floor` with `sysrepocfg -Evim -fjson -m ietf-system`, we can verify the changes.
```
$ sysrepocfg -X -x '/ietf-system:system/location'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <location>3rd floor</location>
</system
```

#### Timezone-name
If the timezone-name feature is enabled, we can retrieve and change the timezone-name.
The timezone data is retrieved and set by using the `/etc/localtime` symlink.

```
$ sysrepocfg -X -x '/ietf-system:system/clock/timezone-name'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <clock>
    <timezone-name>Europe/Stockholm</timezone-name>
  </clock>
</system>

$ ls -al /etc/localtime
lrwxrwxrwx 1 root root 33 Mar 22 16:28 /etc/localtime -> /usr/share/zoneinfo/Europe/Stockholm
```

After setting it to `Europe/Zagreb` with `sysrepocfg -Evim -fjson -m ietf-system`, we can verify the changes.

```
$ sysrepocfg -X -x '/ietf-system:system/clock/timezone-name'
<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system">
  <clock>
    <timezone-name>Europe/Zagreb</timezone-name>
  </clock>
</system>
$ ls -al /etc/localtime
lrwxrwxrwx 1 root root 33 Mar 22 16:28 /etc/localtime -> /usr/share/zoneinfo/Europe/Zagreb
```

