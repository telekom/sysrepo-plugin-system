*** Settings ***
Library         BuiltIn
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Contact
    [Documentation]    Attempt to add "test_contact" contact info
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/system_contact.xml   xml
    ${Contact XML Data}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-system:system/contact
    ...    xml
    Should Be Equal As Strings    ${Contact XML Data}    ${Expected Contact}
    ${Login Name}=    Get User Login Name By UID    0
    Should Be Equal As Strings    ${Login Name}    root
    ${User Name}=    Get User User Name By UID    0
    Should Be Equal As Strings    ${User Name}    test_contact

