/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DNS_SEARCH_H
#define DNS_SEARCH_H

int dns_search_add(char *search_val);
const char *dns_search_error2str(int err);
int dns_search_remove(char *search_val);

#endif // DNS_SEARCH_H
