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
