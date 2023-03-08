# Robot IETF System
Robot framework tests via the Sysrepo robot library.

## `__init__.robot`
Initializes a default Sysrepo connection as well as a running  datastore session at the start of each test.
At the end of a test, the initial datastore data is restored. At the end of a test suite all connections and sessions are cleaned up.

## `__init__.py`
Sets `sys.path` so the tests don't have to go up to the parent directory when including libraries, resource files, etc.

## `SystemInit.robot`
Additional initialization keywords needed for test setup and teardown.

## `*Tests.robot`
Located in the `tests` directory, these files contain integration test cases

## `SystemKeywords.resource`
Contains robot specific composite keywords.

## `SystemVariables.py`
Creates robot variables through the `getVariable` function.
Key, value pairs of the returned dictionary are mapped to robot Variable names and values respectively.

## `SystemKeywords.py`
Extended robot custom keywords written in Python 3.

