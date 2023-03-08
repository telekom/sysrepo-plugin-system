*** Settings ***
Library     BuiltIn
Library     OperatingSystem


*** Keywords ***
List Elements Should Be Empty
    [Arguments]    ${List}
    FOR    ${Item}    IN    @{List}
        Should Be Equal As Strings    ${Item}    ${EMPTY}
    END

Sysrepo Get Json Fields
    [Arguments]    ${Path}    ${Field}
    ${Path Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Path}
    ...    json
    &{Path JSON}=    Convert String To JSON    ${Path Str}
    ${Fields}=    Get Values From Json    ${Path Json}    $..${Field}
    [Return]     ${Fields}
    

