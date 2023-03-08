*** Settings ***
Library         BuiltIn
Library         OperatingSystem
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Timezone
    [Documentation]    Attempt to set /etc/localtime timezone
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/system_set_timezone_name.xml   xml
    ${Timezone XML Data}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-system:system/clock/timezone-name
    ...    xml
    Should Be Equal As Strings    ${Timezone XML Data}    ${Expected Timezone} 
    ${System Timezone}=    Get Resolved Symbolic Link    /etc/localtime
    Should Be Equal As Strings    ${System Timezone}    /usr/share/zoneinfo/Europe/Stockholm
     
    
