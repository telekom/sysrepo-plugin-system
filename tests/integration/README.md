# ietf-system plugin integration tests

This directory contains the integration tests and required data for the ietf-system plugin.

The tests are located in `ietf-system.py` or in the directory `robot-ietf-system` in case of Robot framework tests, while the test data is in XML files in `data`.

# Dependencies

## Python 3
As the integration tests are writen in python, several dependencies are required
in order to run the tests.

The easiest way to set up the dependencies and use the tests is by creating a python
virtual environment and installing the dependencies from the `requirements.txt` file.

```
python3 -m venv test-venv

source test-venv

pip3 install -r requirements.txt
```

## Robot framework

Make sure to have Sysrepo and the accompanying plugins installed under `devel`.

Create a virtual environment, activate it and make sure `pip`, `setuptools` and `wheel` are up to date.
Finally install the packages.

```
$ python3 -m venv sysrepolibrary-venv
$ source sysrepolibrary-venv/bin/activate
$ python3 -m pip install --upgrade pip setuptools wheel
$ python3 -m pip install rpaframework robotframework-sysrepolibrary robotframework-tidy
```

To autoformat the robot code:
```
$ robotidy robot-ietf-interfaces/
```


# Running the tests

## Python 3
After setting up the dependencies, to run the tests either execute the `.py` file or
run it in a python interpreter.

The tests will require an environment variables to be set.
The variable is `SYSREPO_GENERAL_PLUGIN_PATH`, the path to the plugin built in standalone mode.

As the tests are written by using the python unit-test library, the output looks
something like this:

```
#  GEN_PLUGIN_DATA_DIR=/path/to/sysrepo/plugin/system/build/dir SYSREPO_GENERAL_PLUGIN_PATH=/path/to/sysrepo/plugin/system/executable python3 ietf-system.py
```

## Robot framework
Note the root privileges when invoking the command (datastore permission issues otherwise, item not found):

The runner script will check if the plugin executable path is set, as well as if the required python modules are installed

To test the system plugin (set env var path):
```
# SYSREPO_SYSTEM_PLUGIN_PATH=/path/to/interfaces/plugin/executable ./run_robot_ietf_system_tests.sh 
```

