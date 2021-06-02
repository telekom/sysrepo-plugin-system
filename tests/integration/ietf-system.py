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
        self.plugin = subprocess.Popen([plugin_path], env={"GEN_PLUGIN_DATA_DIR": data_dir})
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")
        time.sleep(2)

    def tearDown(self):
        self.plugin.send_signal(signal.SIGINT)
        self.session.stop()
        self.conn.disconnect()

    def load_initial_data(self, path):
        ctx = self.conn.get_ly_ctx()

        self.session.replace_config_ly(None, "ietf-system")
        f = open(path, "r")
        data = f.read()
        data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
        self.session.replace_config_ly(data, "ietf-system")
        f.close()

class ContactTestCase(SystemTestCase):
    def setUp(self):
        super().setUp()

    def tearDown(self):
        super().tearDown()

    def test_contact_get(self):
        expected_contact = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><contact>test_contact</contact></system>'

        self.load_initial_data("system_contact.xml")

        data = self.session.get_data_ly('/ietf-system:system/contact')
        contact = data.print_mem("xml")
        self.assertEqual(contact, expected_contact, "contact data is wrong")

        passwd = pwd.getpwuid(0)
        self.assertEqual(passwd.pw_name, "root", "uid 0 is not named root")
        self.assertEqual(passwd.pw_gecos, "test_contact", "unexpected contact info in /etc/passwd")
        data.free()

if __name__ == '__main__':
    unittest.main()
