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
bin:x:1:1::/:/usr/bin/nologin
daemon:x:2:2::/:/usr/bin/nologin
mail:x:8:12::/var/spool/mail:/usr/bin/nologin
ftp:x:14:11::/srv/ftp:/usr/bin/nologin
http:x:33:33::/srv/http:/usr/bin/nologin
nobody:x:65534:65534:Nobody:/:/usr/bin/nologin
dbus:x:81:81:System Message Bus:/:/usr/bin/nologin
systemd-journal-remote:x:982:982:systemd Journal Remote:/:/usr/bin/nologin
systemd-network:x:981:981:systemd Network Management:/:/usr/bin/nologin
systemd-resolve:x:980:980:systemd Resolver:/:/usr/bin/nologin
systemd-timesync:x:979:979:systemd Time Synchronization:/:/usr/bin/nologin
systemd-coredump:x:978:978:systemd Core Dumper:/:/usr/bin/nologin
```

We can change the contact field, which will store new contact information in the `/etc/passwd` file.
