

#ifndef HEADER_H_
    #define HEADER_H_

struct header_s {
    int coucou;
};


/**
 * @brief Initialize the header structure
 *
 * @param header The header structure to initialize
 * @return 0 on success, -1 on failure
 */
int header_init(struct header_s *header);

#endif /* !HEADER_H_ */
