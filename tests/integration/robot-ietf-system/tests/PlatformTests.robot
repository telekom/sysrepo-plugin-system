*** Settings ***
Library         BuiltIn
Library         OperatingSystem
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Platform
    [Documentation]    Check platform plugin and system data parity
    Log To Console    Platform not implemented
    
