import unittest
import sysrepo
import os
import subprocess
import pwd
import signal
import time
import json
import platform

class SystemTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_GENERAL_PLUGIN_PATH')
        if plugin_path is None:
            self.fail("SYSREPO_GENRAL_PLUGIN_PATH has to point to general plugin executable")

        self.data_dir = os.environ.get('GEN_PLUGIN_DATA_DIR')
        if self.data_dir is None:
            self.fail("GEN_PLUGIN_DATA_DIR has to point to general plugin executable")
        self.plugin = subprocess.Popen([plugin_path], env={"GEN_PLUGIN_DATA_DIR": self.data_dir}, \
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")
        time.sleep(2)

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

class SystemStateTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_GENERAL_PLUGIN_PATH')
        if plugin_path is None:
            self.fail("SYSREPO_GENRAL_PLUGIN_PATH has to point to general plugin executable")

        self.data_dir = os.environ.get('GEN_PLUGIN_DATA_DIR')
        if self.data_dir is None:
            self.fail("GEN_PLUGIN_DATA_DIR has to point to general plugin executable")
        self.plugin = subprocess.Popen([plugin_path], env={"GEN_PLUGIN_DATA_DIR": self.data_dir}, \
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
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

        self.load_initial_data("data/system_contact.xml")

        data = self.session.get_data_ly('/ietf-system:system/contact')
        contact = data.print_mem("xml")
        self.assertEqual(contact, expected_contact, "contact data is wrong")

        passwd = pwd.getpwuid(0)
        self.assertEqual(passwd.pw_name, "root", "uid 0 is not named root")
        self.assertEqual(passwd.pw_gecos, "test_contact", "unexpected contact info in /etc/passwd")
        data.free()

class HostnameTestCase(SystemTestCase):
    def test_hostname(self):
        expected_hostname = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><hostname>test_hostname</hostname></system>'

        self.load_initial_data("data/system_hostname.xml")

        data = self.session.get_data_ly('/ietf-system:system/hostname')
        hostname = data.print_mem("xml")
        self.assertEqual(hostname, expected_hostname, "hostname data is wrong")

        real_hostname = os.uname()[1]
        self.assertEqual(real_hostname, "test_hostname", "hostname on system doesn't match set hostname")
        data.free()

class LocationTestCase(SystemTestCase):
    def test_location(self):
        expected_location= '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><location>test_location</location></system>'

        self.load_initial_data("data/system_location.xml")

        data = self.session.get_data_ly('/ietf-system:system/location')
        location = data.print_mem("xml")
        self.assertEqual(location, expected_location, "location data is wrong")

        real_location = ""
        with open(self.data_dir + "location_info", "r") as f:
            real_location = f.read()
        self.assertEqual(real_location, "test_location", "location on system doesn't match set location")
        data.free()

class NTPTestCase(SystemTestCase):
    def get_ntp_status(self):
        p = subprocess.run(['systemctl', 'show', 'ntp'], capture_output=True, encoding="ascii")
        status = list(filter(lambda x : x.split('=')[0] == 'ActiveState', p.stdout.split()))
        self.assertEqual(len(status), 1, "invalid systemctl show output, got more than one ActiveStatus")

        return status[0].split("=")[1]

    def get_ntp_server_ips(self):
        with open('/etc/ntp.conf', 'r') as f:
            return [l.split()[1] for l in f if 'server' in l and l[0] != '#']


    def test_ntp_enabled(self):
        expected_ntp_enabled = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled></ntp></system>'
        expected_ntp_disabled = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>false</enabled></ntp></system>'

        self.load_initial_data("data/system_ntp_disabled.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_disabled, "ntp data is wrong")

        self.assertEqual(self.get_ntp_status(), "inactive", "ntp service is running")

        self.load_initial_data("data/system_ntp_enabled.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_enabled, "ntp data is wrong")

        self.assertEqual(self.get_ntp_status(), "active", "ntp service is not running")

        # disable NTP so that we are back in an initial state
        self.load_initial_data("data/system_ntp_disabled.xml")
        self.assertEqual(self.get_ntp_status(), "inactive", "ntp service is running")

        data.free()
    def test_ntp_server(self):
        expected_ntp_server_initial = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr.pool.ntp.org</name><udp><address>162.159.200.123</address></udp></server></ntp></system>'

        self.load_initial_data("data/system_ntp_server_initial.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_initial, "ntp data is wrong")

        self.assertEqual(self.get_ntp_status(), "active", "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(len(ips), 1, "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.123", "unexpected server ip")

        expected_ntp_server_add = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr.pool.ntp.org</name><udp><address>162.159.200.123</address></udp></server><server><name>hr2.pool.ntp.org</name><udp><address>162.159.200.124</address></udp></server></ntp></system>'

        self.load_initial_data("data/system_ntp_server_add.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_add, "ntp data is wrong")

        self.assertEqual(self.get_ntp_status(), "active", "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(len(ips), 2, "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.123", "unexpected server ip")
        self.assertEqual(ips[1], "162.159.200.124", "unexpected server ip")

        expected_ntp_server_remove = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled><server><name>hr2.pool.ntp.org</name><udp><address>162.159.200.124</address></udp></server></ntp></system>'

        self.load_initial_data("data/system_ntp_server_remove.xml")

        data = self.session.get_data_ly('/ietf-system:system/ntp')
        ntp = data.print_mem("xml")
        self.assertEqual(ntp, expected_ntp_server_remove, "ntp data is wrong")

        self.assertEqual(self.get_ntp_status(), "active", "ntp service is not running")
        ips = self.get_ntp_server_ips()
        self.assertEqual(len(ips), 1, "unexpected number of servers in /etc/ntp.conf")
        self.assertEqual(ips[0], "162.159.200.124", "unexpected server ip")

class PlatformTestCase(SystemStateTestCase):
    def test_platform(self):
        data = self.session.get_data_ly('/ietf-system:system-state/platform')
        plugin_platform = json.loads(data.print_mem("json"))
        plugin_platform = plugin_platform['ietf-system:system-state']['platform']

        self.assertEqual(plugin_platform['os-name'], platform.system(), "unexpected os-name")
        self.assertEqual(plugin_platform['os-release'], platform.release(), "unexpected os-release")
        self.assertEqual(plugin_platform['os-version'], platform.version(), "unexpected os-version")
        self.assertEqual(plugin_platform['machine'], platform.machine(), "unexpected machine")

        data.free()

if __name__ == '__main__':
    unittest.main()
