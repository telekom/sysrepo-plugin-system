*** Settings ***
Library         BuiltIn
Library         OperatingSystem
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Location
    [Documentation]    Attempt to set location location_info value to a test value
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/system_location.xml   xml
    ${Location XML Data}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-system:system/location
    ...    xml
    Should Be Equal As Strings    ${Location XML Data}    ${Expected Location} 
    ${Location Info Path}=    Join Path    ${Sysrepo System Plugin Dir Path}    location_info
    ${Location Info Contents}=    Get File    ${Location Info Path}
    Should Be Equal As Strings    ${Location Info Contents}    test_location
    
