*** Settings ***
Library         BuiltIn
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Dummy
    [Documentation]    Check if the initialization part works as intended
    Log To Console    "in dummy test case"
    Log To Console    "check if test and setup setup/teardown pass"
