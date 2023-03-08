*** Settings ***
Library         BuiltIn
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Hostname
    [Documentation]    Attempt to set "test_hostname" as system hostname
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/system_hostname.xml   xml
    ${Hostname XML Data}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-system:system/hostname
    ...    xml
    Should Be Equal As Strings    ${Hostname XML Data}    ${Expected Hostname}
    ${System Hostname}=    Get Hostname
    Should Be Equal As Strings    ${System Hostname}    test_hostname
    
