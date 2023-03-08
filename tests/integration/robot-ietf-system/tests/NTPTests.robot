*** Settings ***
Library         BuiltIn
Library         OperatingSystem
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


Test Teardown    Restore Initial Running Datastore


*** Test Cases ***
Test NTP Enabled
    [Documentation]    Attempt to enable the NTP service
    Fail    msg=NTP tests cause SysrepoUnsupportedError, as well as fail to restore the datastore
     
Test NTP Server
    [Documentation]    Check system and plugin NTP server parity
    Fail    msg=NTP tests cause SysrepoUnsupportedError, as well as fail to restore the datastore
