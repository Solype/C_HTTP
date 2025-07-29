#include "response.h"
#include "response_brut_value.h"
#include <string.h>
#include <stddef.h>

int response_set_content(struct response_s *res, size_t code)
{
    res->status_code = code;
    res->body = (char *)status_codes_messages[code];
    res->body_size = strlen(res->body);
    res->status_message = (char *)status_codes_messages[code];
    res->content_type = text_plain;
    return 0;
}
