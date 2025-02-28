#ifndef RESPONSE_BRUT_VALUE_H_
    #define RESPONSE_BRUT_VALUE_H_

static const char *basic_response_message =
    "HTTP/1.1 %.3d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %u\r\n"
    "Connection: close\r\n\r\n%s";

static const char *basic_response_message_no_body =
    "HTTP/1.1 %.3d %s\r\n"
    "Connection: close\r\n\r\n";

static const char *status_codes_messages[] = {
    [100] = "Continue",
    [101] = "Switching Protocols",
    [102] = "Processing",
    [103] = "Early Hints",

    [200] = "OK",
    [201] = "Created",
    [202] = "Accepted",
    [203] = "Non-Authoritative Information",
    [204] = "No Content",
    [205] = "Reset Content",
    [206] = "Partial Content",
    [207] = "Multi-Status",
    [208] = "Already Reported",
    [226] = "IM Used",

    [300] = "Multiple Choices",
    [301] = "Moved Permanently",
    [302] = "Found",
    [303] = "See Other",
    [304] = "Not Modified",
    [305] = "Use Proxy",
    [307] = "Temporary Redirect",
    [308] = "Permanent Redirect",

    [400] = "Bad Request",
    [401] = "Unauthorized",
    [402] = "Payment Required",
    [403] = "Forbidden",
    [404] = "Not Found",
    [405] = "Method Not Allowed",
    [406] = "Not Acceptable",
    [407] = "Proxy Authentication Required",
    [408] = "Request Timeout",
    [409] = "Conflict",
    [410] = "Gone",
    [411] = "Length Required",
    [412] = "Precondition Failed",
    [413] = "Payload Too Large",
    [414] = "URI Too Long",
    [415] = "Unsupported Media Type",
    [416] = "Range Not Satisfiable",
    [417] = "Expectation Failed",
    [418] = "I'm a teapot",
    [421] = "Misdirected Request",
    [422] = "Unprocessable Entity",
    [423] = "Locked",
    [424] = "Failed Dependency",
    [425] = "Too Early",
    [426] = "Upgrade Required",
    [428] = "Precondition Required",
    [429] = "Too Many Requests",
    [431] = "Request Header Fields Too Large",
    [451] = "Unavailable For Legal Reasons",

    [500] = "Internal Server Error",
    [501] = "Not Implemented",
    [502] = "Bad Gateway",
    [503] = "Service Unavailable",
    [504] = "Gateway Timeout",
    [505] = "HTTP Version Not Supported",
    [506] = "Variant Also Negotiates",
    [507] = "Insufficient Storage",
    [508] = "Loop Detected",
    [510] = "Not Extended",
    [511] = "Network Authentication Required"
};

    #define NB_STATUS_CODES (sizeof(status_codes_messages) / sizeof(const char *))

static const char *content_types_str[] = {
    "no_body",
    "text/plain",
    "text/html",
    "text/css",
    "text/js",
    "text/json",
    "text/xml",
    "image/jpeg",
    "image/png",
    "image/gif",
    "image/svg",
    "image/bmp",
    "image/tiff",
    "image/webp",
    "audio/mpeg",
    "audio/ogg",
    "audio/wav",
    "video/mp4",
    "video/ogg",
    "video/webm",
    "video/mpeg",
    "video/flv",
    "video/mov",
    "application/json",
    "application/xml",
    "application/css",
    "application/js",
    "application/html",
    "application/jpeg",
    "application/png",
    "application/gif",
    "application/svg",
    "application/bmp",
    "application/tiff",
    "application/webp",
    "application/mpeg",
    "application/ogg",
    "application/wav",
    "application/mp4",
    "application/ogg",
    "application/webm",
    "application/flv",
    "application/mov",
    "application/pdf",
    "application/msword",
    "application/msexcel",
    "application/msword",
    "application/octet-stream"
};

#endif /* !RESPONSE_BRUT_VALUE_H_ */
