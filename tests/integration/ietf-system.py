import unittest
import sysrepo
import os
import subprocess
import pwd
import signal
import time

class SystemTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_GENERAL_PLUGIN_PATH')
        if plugin_path is None:
            self.fail("SYSREPO_GENRAL_PLUGIN_PATH has to point to general plugin executable")

        data_dir = os.environ.get('GEN_PLUGIN_DATA_DIR')
        if data_dir is None:
            self.fail("GEN_PLUGIN_DATA_DIR has to point to general plugin executable")
        self.plugin = subprocess.Popen([plugin_path], env={"GEN_PLUGIN_DATA_DIR": data_dir}, \
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
        f = open(path, "r")
        data = f.read()
        data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
        self.session.replace_config_ly(data, "ietf-system")
        f.close()

class ContactTestCase(SystemTestCase):
    def test_contact(self):
        expected_contact = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><contact>test_contact</contact></system>'

        self.load_initial_data("system_contact.xml")

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

        self.load_initial_data("system_hostname.xml")

        data = self.session.get_data_ly('/ietf-system:system/hostname')
        hostname = data.print_mem("xml")
        self.assertEqual(hostname, expected_hostname, "hostname data is wrong")

        real_hostname = os.uname()[1]
        self.assertEqual(real_hostname, "test_hostname", "hostname on system doesn't match set hostname")
        data.free()

if __name__ == '__main__':
    unittest.main()
