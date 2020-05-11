#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_request(char *type, char *host, char *url, char* content_type, 
							char **body_data, int body_data_fields_count,
							char *authorization, char **cookies, int cookies_count);

#endif
