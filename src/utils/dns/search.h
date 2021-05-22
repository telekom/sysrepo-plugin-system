#ifndef DNS_SEARCH_H
#define DNS_SEARCH_H

int dns_search_add(char *search_val);
const char *dns_search_error2str(int err);
int dns_search_remove(char *search_val);

#endif // DNS_SEARCH_H
