# Plugin Configuration: systemd vs augyang

This document provides guidelines for configuring sysrepo plugins based on the use of systemd and augyang. It outlines the scenarios in which each configuration method should be employed, ensuring clarity and consistency in plugin development and usage.

## 1. Use systemd for configuring all possible nodes

Systemd is the preferred method for configuring nodes whenever it is feasible. It provides a reliable and standardized approach to managing services and system resources. The following considerations should be taken into account when deciding to use systemd:

- **Availability of systemd**: Ensure that the target system supports systemd. Most modern Linux distributions include systemd by default, making it widely accessible.

- **Node configurability**: If the node can be effectively configured through systemd unit files or associated mechanisms such as sd-bus, opt for systemd as the configuration method. This choice enables seamless integration with existing system management tools and practices.

- **Uniformity**: Utilizing systemd consistently across plugins promotes uniformity and simplifies overall system administration.

## 2. Use augyang to store startup information

Augyang, an abstraction layer for accessing the YANG data models, should be used for storing startup information. This information may include initial configurations, startup state, and other data necessary for plugin operation. Consider the following points when employing augyang:

- **Persistent storage**: Augyang provides a means to store data persistently, ensuring that the startup information is preserved across system reboots or plugin reloads.

- **Modularity**: By separating the startup information from systemd-specific configurations, plugins become more modular and flexible, enabling easier migration and integration in diverse environments.

- **Consistency**: Using augyang for startup information facilitates a consistent approach across plugins, ensuring predictable behavior and simplifying troubleshooting.

## 3. Use augyang for configuring nodes without systemd configurability

In cases where a node lacks the ability to be configured using systemd, augyang should be utilized to provide the necessary configuration options. This scenario may arise when dealing with devices or services that do not have direct support for systemd-based configuration. However, those changes still need to be enabled somehow which should be taken into consideration. Consider the following factors:

- **Limited configurability**: If a node cannot be adequately configured through systemd, augyang becomes the alternative method for managing its configuration.

- **Enhanced functionality**: Augyang allows for the inclusion of additional configuration parameters and options that are not possible through systemd alone. This flexibility can be crucial when working with complex or specialized nodes.

- **Consistency**: Utilizing augyang consistently for nodes without systemd configurability ensures a unified approach to plugin development and maintains a cohesive user experience.

## Conclusion

By following these guidelines, sysrepo plugin developers can establish a clear framework for deciding when to use systemd and augyang for configuration purposes. Utilizing systemd for all possible nodes, using augyang to store startup information, and employing augyang for nodes without systemd configurability will enable consistency, modularity, and ease of maintenance across the sysrepo ecosystem.