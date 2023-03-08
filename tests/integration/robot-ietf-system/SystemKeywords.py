from robot.api.deco import keyword

from pwd import getpwuid
import sys
import os


ROBOT_AUTO_KEYWORDS = False


@keyword('Format String')
def format_str(string: str, *args, **kwargs) -> str:
    return string.format(*args, **kwargs)

@keyword('Remove All Whitespace')
def remove_all_whitespace(string: str) -> str:
    return "".join(string.split())

@keyword('Get User Login Name By UID')
def get_pwd_pw_name(uid: int) -> str:
    return getpwuid(uid).pw_name

@keyword('Get User User Name By UID')
def get_pwd_pw_gecos(uid: int) -> str:
    return getpwuid(uid).pw_gecos

@keyword('Get Hostname')
def get_os_uname_hostname() -> str:
    # the 2nd returned attribute [1] of the object is the nodename (machine network name)
    return os.uname()[1] 

@keyword('Get Resolved Symbolic Link')
def get_os_readlink(path: str, *, dir_fd=None) -> str:
    return os.readlink(path, dir_fd=dir_fd)

@keyword('Get NTP Status')
def get_ntp_status() -> str:
    cmd = ['systemctl', 'show', 'ntp']
    proc = subprocess.run(cmd, capture_output=True, encoding="ascii")
    status = list(
        filter(
            lambda x: x.split('=')[0] == 'ActiveState',
            proc.stdout.split()))

    if len(status) != 1:
        raise ValueError(f"invalid {cmd} output")

    return status[0].split("=")[1]

@keyword('Get NTP Server IPs')
def get_ntp_server_ips() -> [str]:
    ntp_conf_path = "/etc/ntp.conf"
    with open(ntp_conf_path, 'r') as f:
        return [l.split()[1] for l in f if 'server' in l and l[0] != '#']

