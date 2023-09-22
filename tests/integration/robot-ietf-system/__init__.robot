*** Settings ***
Library             SysrepoLibrary
Library             OperatingSystem
Library             RPA.JSON
Library             Process
Library             __init__.py
Resource            SystemInit.resource

Suite Setup         Setup IETF System
Suite Teardown      Cleanup IETF System

Test Teardown       Restore Initial Running Datastore

*** Variables ***
${Xpath System}     /ietf-system:system
${Running Datastore}    running


*** Keywords ***
Setup IETF System
    [Documentation]    Create a default connection and running session
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session
    Set Sysrepo System Plugin Dir Path
    Import Root Robot Path

Set Sysrepo System Plugin Dir Path
    ${Sysrepo System Plugin Dir Path}    ${_}=    Split Path    %{SYSREPO_SYSTEM_PLUGIN_PATH} 
    Set Global Variable    ${Sysrepo System Plugin Dir Path}

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_SYSTEM_PLUGIN_PATH}
    Set Suite Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=2s    on_timeout=continue

Init Running Session
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    Set Global Variable    ${Session Running}
    ${If Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath System}
    ...    json
    Set Global Variable    ${If Init Str}
    &{If Init JSON}=    Convert String To JSON    ${If Init Str}
    Set Global Variable    ${If Init JSON}

Cleanup IETF System
    # Restore initial data
    Terminate Process    ${Plugin}
    Close All Sysrepo Connections And Sessions

