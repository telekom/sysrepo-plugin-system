*** Settings ***
Library         BuiltIn
Library         OperatingSystem
Library         DateTime
Library         SystemKeywords.py
Resource        SystemInit.resource
Resource        SystemKeywords.resource
Variables       SystemVariables.py


*** Test Cases ***
Test Date Time
    [Documentation]    Attempt to set current datetime
    ${Date Time RPC Out XML}=    Send RPC By File    
    ...    ${Connection Default}    
    ...    data/system_set_current_date_time.xml
    ...    xml
    ${Date Time RPC Out XML}=    Remove All Whitespace    ${Date Time Rpc Out XML}
    Should Be Equal As Strings    ${Date Time RPC Out XML}    ${Expected Date Time RPC Out}
    ${System Date Time}=    Get Current Date    time_zone=local    result_format=%d/%m/%Y %H:%M
    Should Be Equal As Strings    ${System Date Time}   ${Expected Date Time} 
    
     
    
