#ifndef RESPONSE_H_
    #define RESPONSE_H_

enum constent_type_e {
    application_octet_stream = 0,
    text_plain  = 1,
    text_html   = 2,
    text_css    = 3,
    text_js     = 4,
    text_json   = 5,
    text_xml    = 6,
    image_jpeg  = 7,
    image_png   = 8,
    image_gif   = 9,
    image_svg   = 10,
    image_bmp   = 11,
    image_tiff  = 12,
    image_webp  = 13,
    audio_mpeg  = 14,
    audio_ogg   = 15,
    audio_wav   = 16,
    video_mp4   = 17,
    video_ogg   = 18,
    video_webm  = 19,
    video_mpeg  = 20,
    video_flv   = 21,
    video_mov   = 22,
    application_json    = 23,
    application_xml     = 24,
    application_css     = 25,
    application_js      = 26,
    application_html    = 27,
    application_jpeg    = 28,
    application_png     = 29,
    application_gif     = 30,
    application_svg     = 31,
    application_bmp     = 32,
    application_tiff    = 33,
    application_webp    = 34,
    application_mpeg    = 35,
    application_ogg     = 36,
    application_wav     = 37,
    application_mp4     = 38,
    application_webm    = 39,
    application_flv     = 40,
    application_mov     = 41,
    application_pdf     = 42,
    application_msword  = 43,
    application_msexcel = 44,
    NB_CONTENT_TYPE     = 45
};

struct response_s {
    char *body;
    char *status_message;
    size_t status_code;
    enum constent_type_e content_type;
};


#endif /* !RESPONSE_H_ */
