
def getVariables():
    # Variables get imported into robot from the dict as Key = Value pairs
    variables = {
        'Expected Contact': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><contact>test_contact</contact></system>',
        'Expected Hostname': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><hostname>test_hostname</hostname></system>',
        'Expected Location': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><location>test_location</location></system>',
        'Expected NTP Enabled': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>true</enabled></ntp></system>',
        'Expected NTP Disabled': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><ntp><enabled>false</enabled></ntp></system>',
        'Expected Timezone': '<system xmlns="urn:ietf:params:xml:ns:yang:ietf-system"><clock><timezone-name>Europe/Stockholm</timezone-name></clock></system>',
        'Expected Date Time RPC Out': '<set-current-datetimexmlns="urn:ietf:params:xml:ns:yang:ietf-system"><current-datetime>2021-02-09T10:02:39+01:00</current-datetime></set-current-datetime>',
        'Expected Date Time': '09/02/2021 09:02',
    }

    return variables

