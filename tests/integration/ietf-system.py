import unittest
import sysrepo
import os
import subprocess
import pwd
import signal
import time
import json
import platform
import datetime
import pwd
import spwd

class SystemTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_GENERAL_PLUGIN_PATH')
        if plugin_path is None:
            self.fail(
                "SYSREPO_GENRAL_PLUGIN_PATH has to point to general plugin executable")

        self.data_dir = os.environ.get('GEN_PLUGIN_DATA_DIR')
        if self.data_dir is None:
            self.fail(
                "GEN_PLUGIN_DATA_DIR has to point to general plugin executable")
        self.plugin = subprocess.Popen(
            [plugin_path],
            env={
                "GEN_PLUGIN_DATA_DIR": self.data_dir},
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")
        time.sleep(2)

        self.initial_data = self.session.get_data_ly('/ietf-system:system')

    def tearDown(self):
        self.session.stop()
        self.conn.disconnect()
        self.plugin.send_signal(signal.SIGINT)
        self.plugin.wait()

    def load_initial_data(self, path):
        ctx = self.conn.get_ly_ctx()

        self.session.replace_config_ly(None, "ietf-system")
        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.replace_config_ly(data, "ietf-system")
            data.free()

    def edit_config(self, path):
        ctx = self.conn.get_ly_ctx()

        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.edit_batch_ly(data)
            data.free()

        self.session.apply_changes()


class SystemStateTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_GENERAL_PLUGIN_PATH')
        if plugin_path is None:
            self.fail(
                "SYSREPO_GENRAL_PLUGIN_PATH has to point to general plugin executable")

        self.data_dir = os.environ.get('GEN_PLUGIN_DATA_DIR')
        if self.data_dir is None:
            self.fail(
                "GEN_PLUGIN_DATA_DIR has to point to general plugin executable")
        self.plugin = subprocess.Popen(
            [plugin_path],
            env={
                "GEN_PLUGIN_DATA_DIR": self.data_dir},
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("operational")
        time.sleep(2)

    def tearDown(self):
        self.session.stop()
        self.conn.disconnect()
        self.plugin.send_signal(signal.SIGINT)
        self.plugin.wait()


class ContactTestCase(SystemTestCase):
    def test_contact(self):
        expected_contact = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><contact>test_contact</contact></system>'

        self.edit_config("data/system_contact.xml")

        data = self.session.get_data_ly('/ietf-system:system/contact')
        contact = data.print_mem("xml")
        self.assertEqual(contact, expected_contact, "contact data is wrong")

        passwd = pwd.getpwuid(0)
        self.assertEqual(passwd.pw_name, "root", "uid 0 is not named root")
        self.assertEqual(
            passwd.pw_gecos,
            "test_contact",
            "unexpected contact info in /etc/passwd")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        data.free()


class HostnameTestCase(SystemTestCase):
    def test_hostname(self):
        expected_hostname = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><hostname>test_hostname</hostname></system>'

        self.edit_config("data/system_hostname.xml")

        data = self.session.get_data_ly('/ietf-system:system/hostname')
        hostname = data.print_mem("xml")
        self.assertEqual(hostname, expected_hostname, "hostname data is wrong")

        real_hostname = os.uname()[1]
        self.assertEqual(
            real_hostname,
            "test_hostname",
            "hostname on system doesn't match set hostname")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        data.free()


class LocationTestCase(SystemTestCase):
    def test_location(self):
        expected_location = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><location>test_location</location></system>'

        self.edit_config("data/system_location.xml")

        data = self.session.get_data_ly('/ietf-system:system/location')
        location = data.print_mem("xml")
        self.assertEqual(location, expected_location, "location data is wrong")

        real_location = ""
        with open(self.data_dir + "location_info", "r") as f:
            real_location = f.read()
        self.assertEqual(
            real_location,
            "test_location",
            "location on system doesn't match set location")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        data.free()


class NTPTestCase(SystemTestCase):
    def get_ntp_status(self):
        p = subprocess.run(['systemctl', 'show', 'ntp'],
                           capture_output=True, encoding="ascii")
        status = list(
            filter(
                lambda x: x.split('=')[0] == 'ActiveState',
                p.stdout.split()))
        self.assertEqual(
            len(status),
            1,
            "invalid systemctl show output, got more than one ActiveStatus")

        return status[0].split("=")[1]

    def get_ntp_server_ips(self):
        with open('/etc/ntp.conf', 'r') as f:
            return [l.split()[1] for l in f if 'server' in l and l[0] != '#']

    def test_ntp_enabled(self):
        expected_ntp_enabled = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled></ntp></system>'
        expected_ntp_disabled = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>false</enabled></ntp></system>'

        self.edit_config("data/system_ntp_disabled.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_disabled, "ntp data is wrong")

        self.assertEqual(
            self.get_ntp_status(),
            "inactive",
            "ntp service is running")

        self.edit_config("data/system_ntp_enabled.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_enabled, "ntp data is wrong")

        self.assertEqual(
            self.get_ntp_status(),
            "active",
            "ntp service is not running")

        # disable NTP so that we are back in an initial state
        self.edit_config("data/system_ntp_disabled.xml")
        self.assertEqual(
            self.get_ntp_status(),
            "inactive",
            "ntp service is running")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        data.free()

    def test_ntp_server(self):
        expected_ntp_server_initial = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr.pool.ntp.org</name><udp><address>162.159.200.123</address></udp></server></ntp></system>'

        self.edit_config("data/system_ntp_server_initial.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_initial, "ntp data is wrong")

        self.assertEqual(
            self.get_ntp_status(),
            "active",
            "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(
            len(ips),
            1,
            "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.123", "unexpected server ip")

        expected_ntp_server_add = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr.pool.ntp.org</name><udp><address>162.159.200.123</address></udp></server><server><name>hr2.pool.ntp.org</name><udp><address>162.159.200.124</address></udp></server></ntp></system>'

        self.edit_config("data/system_ntp_server_add.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_add, "ntp data is wrong")

        self.assertEqual(
            self.get_ntp_status(),
            "active",
            "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(
            len(ips),
            2,
            "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.123", "unexpected server ip")
        self.assertEqual(ips[1], "162.159.200.124", "unexpected server ip")

        expected_ntp_server_remove = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr2.pool.ntp.org</name><udp><address>162.159.200.124</address></udp></server></ntp></system>'

        self.session.delete_item("/ietf-system:system/ntp/server[name='hr.pool.ntp.org']")
        self.session.apply_changes()

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_remove, "ntp data is wrong")

        self.assertEqual(
            self.get_ntp_status(),
            "active",
            "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(
            len(ips),
            1,
            "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.124", "unexpected server ip")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        data.free()

class AuthenticationTestCase(SystemTestCase):
    def test_authentication_import(self):
        expected_authentication = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><authentication><user><name>test_user</name><password>$6$S05zV2Np5LQzaOpM$qqUxvFsEVg7iwaqnEHhF4ZJv8dwXdtgFpLTHyr78Rr8cz/ml2riPyBlPol.3V8qVXFohR0XSTJXMHO4XLjrXd1</password><authorized-key><name>test_rsa.pub</name><algorithm>ssh-rsa</algorithm><key-data>AAAAB3NzaC1yc2EAAAADAQABAAABAQCiIf32L0B77f//ldk1QpUyfaJQUgI4mXSPtkmaokxUUlj8j9pxlwpFDSmsrZn2H0DJhZZ3ktAGsbFJabZJhV73l7HhQggC/6uzrNPSe+R3lOMGYIAhHaWbGSnT/uvpPMBVA/nWulDkBphiXv606WQHDxqGkngF1kzvvpd5FPpc/jy2vv+66HaP6XA9MgzHLYTOTb3ct3dVoz7HDAQ8tC5l3/3YYLyMhc3LxOBQLZ9PklWvQeSyO6neKi3Au0T13SpUGjtuqKpiCvE/X0ZuFtZSZzPo5UDASD65Er8jOqqYDcfHR1hsfJJjJA/nP+VKoGeBzUBxhxNetqswnEcPDEBv</key-data></authorized-key></user></authentication></system>'

        self.edit_config("data/system_local_user_initial.xml")

        data = self.session.get_data_ly("/ietf-system:system/authentication/user[name='test_user']")
        auth = data.print_mem("xml")
        self.assertEqual(auth, expected_authentication, "authentication data is wrong")

        user = pwd.getpwnam("test_user")
        self.assertEqual(user.pw_name, "test_user", "username in /etc/passwd is wrong")
        self.assertEqual(user.pw_dir, "/home/test_user", "homedir in /etc/passwd is wrong")
        self.assertEqual(user.pw_uid, 1001, "uid in /etc/passwd is wrong")
        self.assertEqual(user.pw_gid, 1001, "gid in /etc/passwd is wrong")
        self.assertEqual(user.pw_shell, '/bin/bash', "shell in /etc/passwd is wrong")

        shadow = spwd.getspnam("test_user")
        self.assertEqual(shadow.sp_namp, "test_user", "username in /etc/shadow is wrong")
        self.assertEqual(shadow.sp_pwdp, "$6$S05zV2Np5LQzaOpM$qqUxvFsEVg7iwaqnEHhF4ZJv8dwXdtgFpLTHyr78Rr8cz/ml2riPyBlPol.3V8qVXFohR0XSTJXMHO4XLjrXd1", "password in /etc/shadow is wrong")

        self.assertEqual(os.path.isdir('/home/test_user'), True, "/home/test_user doesn't exist")

        self.assertEqual(os.path.isdir('/home/test_user/.ssh'), True, "/home/test_user/.ssh doesn't exist")
        self.assertEqual(os.path.exists('/home/test_user/.ssh/test_rsa.pub'), True, "/home/test_user/.ssh/test_rsa.pub doesn't exist")

        with open('/home/test_user/.ssh/test_rsa.pub') as f:
            self.assertEqual(f.readline().strip(), "ssh-rsa", "key algorithm differs")
            self.assertEqual(f.readline().strip(), "AAAAB3NzaC1yc2EAAAADAQABAAABAQCiIf32L0B77f//ldk1QpUyfaJQUgI4mXSPtkmaokxUUlj8j9pxlwpFDSmsrZn2H0DJhZZ3ktAGsbFJabZJhV73l7HhQggC/6uzrNPSe+R3lOMGYIAhHaWbGSnT/uvpPMBVA/nWulDkBphiXv606WQHDxqGkngF1kzvvpd5FPpc/jy2vv+66HaP6XA9MgzHLYTOTb3ct3dVoz7HDAQ8tC5l3/3YYLyMhc3LxOBQLZ9PklWvQeSyO6neKi3Au0T13SpUGjtuqKpiCvE/X0ZuFtZSZzPo5UDASD65Er8jOqqYDcfHR1hsfJJjJA/nP+VKoGeBzUBxhxNetqswnEcPDEBv", "key data differs")

        self.session.replace_config_ly(self.initial_data, "ietf-system")

        with self.assertRaises(KeyError):
            pwd.getpwnam("test_user")

        with self.assertRaises(KeyError):
            spwd.getspnam("test_user")

        self.assertEqual(os.path.exists('/home/test_user'), False, "/home/test_user exists after deletion")

        data.free()

class PlatformTestCase(SystemStateTestCase):
    def test_platform(self):
        data = self.session.get_data_ly('/ietf-system:system-state/platform')
        plugin_platform = json.loads(data.print_mem("json"))
        plugin_platform = plugin_platform['ietf-system:system-state']['platform']

        self.assertEqual(
            plugin_platform['os-name'],
            platform.system(),
            "unexpected os-name")
        self.assertEqual(
            plugin_platform['os-release'],
            platform.release(),
            "unexpected os-release")
        self.assertEqual(
            plugin_platform['os-version'],
            platform.version(),
            "unexpected os-version")
        self.assertEqual(
            plugin_platform['machine'],
            platform.machine(),
            "unexpected machine")

        data.free()


class SetCurrentDateTimeTestCase(SystemStateTestCase):
    def test_set_current_date_time(self):
        ctx = self.conn.get_ly_ctx()

        expected_rpc_output = '<set-current-datetime xmlns="urn:ietf:params:xml:ns:yang:ietf-system"/>'
        expected_date_string = '09/02/2021 09:02'
        old = datetime.datetime.now()
        with open('data/system_set_current_date_time.xml') as rpc:
            rpc_input = ctx.parse_data_mem(
                rpc.read(),
                "xml",
                rpc=True,
                strict=False
            )
            rpc_output = self.session.rpc_send_ly(rpc_input)
            rpc_input.free()
            self.assertEqual(
                rpc_output.print_mem("xml"),
                expected_rpc_output,
                "Unexpected rpc response when calling set-current-date-time")
            rpc_output.free()
            now = datetime.datetime.now()
            date_string = now.strftime("%d/%m/%Y %H:%M")
            self.assertEqual(date_string, expected_date_string, "lol")


if __name__ == '__main__':
    unittest.main()
