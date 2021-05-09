import unittest
import sysrepo

class SystemTestCase(unittest.TestCase):
    def setUp(self):
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")

    def tearDown(self):
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

    def test_contact_get(self):
        expected_contact = '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><contact>test_contac</contact></system>'

        self.load_initial_data("system_contact.xml")

        data = self.session.get_data_ly('/ietf-system:system/contact')
        contact = data.print_mem("xml")
        self.assertEqual(contact, expected_contact, "contact data is wrong")
        data.free()

if __name__ == '__main__':
    unittest.main()
