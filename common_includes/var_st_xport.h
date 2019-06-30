#ifndef VAR_ST_XPORT_H
#define VAR_ST_XPORT_H

using namespace std;
#ifndef MIN
#define  MIN(x,y)  ((x < y) ? x : y)
#endif

/* This templated class will reassemble variable length structures from a byte stream */
template <typename mv_st_hdr, unsigned int valid_type, unsigned int valid_version,
          size_t body_biggest, typename mv_st_body> class var_st_xport {

    enum accum_state_e { BUF_EMPTY, WAITING_FOR_HDR_COMPLETE, WAITING_FOR_BUFFER_COMPLETE };
    accum_state_e in_accum;

    typedef void (*struct_handler)(const mv_st_hdr *hdr, const void *var_part, void *closure);
    struct_handler sh;
    void *closure;

    unsigned int bytes_in_header;
    mv_st_hdr whole_header;
    unsigned int bytes_in_var_part;
    mv_st_body *var_part;               // Working output area.
    unsigned char *record;              // byte based pointer into working output area.

public:

    var_st_xport(char *out_var_part_record = nullptr) {
        in_accum = BUF_EMPTY;
        bytes_in_header = 0;
        bytes_in_var_part = 0;
        sh = (struct_handler)0;
        var_part = (mv_st_body *)out_var_part_record;
        record = (unsigned char *)out_var_part_record;
    }

    void set_var_st_handler(struct_handler in_sh, void *in_closure) {
        closure = in_closure;
        sh = in_sh;
    }

    bool buffer_in_progress() {
        return in_accum != BUF_EMPTY;
    }

    void add_byte(char b) {
        char bytes[1];
        bytes[0] = b;
        add_bytes(bytes, 1);
    }

    void add_bytes(char bytes[], int bytes_length) {
        unsigned int bytes_available = bytes_length;
        unsigned int bytes_to_copy;
        unsigned int bytes_used = 0;

        while (bytes_available > 0) {
            switch (in_accum) {
            case BUF_EMPTY:
                if (bytes[bytes_used] != valid_type) {
                    ++bytes_used;
                    --bytes_available;
                    continue;
                } else {
                    in_accum = WAITING_FOR_HDR_COMPLETE;
                } /* endif */
            case WAITING_FOR_HDR_COMPLETE:
                /* Copy as many bytes as are available or will fit into header */
                bytes_to_copy = MIN((unsigned int)bytes_available, sizeof(whole_header)-bytes_in_header);
                memcpy(&((char *)&whole_header)[bytes_in_header], &bytes[bytes_used], bytes_to_copy);
                bytes_available -= bytes_to_copy;
                bytes_in_header += bytes_to_copy;
                bytes_used += bytes_to_copy;
                if (bytes_in_header < sizeof(whole_header)) {
                    in_accum = WAITING_FOR_HDR_COMPLETE;
                } else {
                    if ((whole_header.type == valid_type) &&
                        (whole_header.version == valid_version) &&
                        (whole_header.len == sizeof(whole_header) &&
                        (whole_header.b_len <= body_biggest))) {
                        bytes_in_var_part = 0;
                        if (var_part == nullptr) {
                            var_part = new mv_st_body(body_biggest+1);
                            record = (unsigned char *)var_part;
                        }
                        in_accum = WAITING_FOR_BUFFER_COMPLETE;
                    } else {
                        /* Something is off, start over. */
                        bytes_in_header = 0;
                        in_accum = BUF_EMPTY;
                    } /* endif */
                } /* endif */
                break;
            case WAITING_FOR_BUFFER_COMPLETE:
                /* Copy bytes into record area */
                bytes_to_copy = MIN(bytes_available, (unsigned int)whole_header.b_len);
                bytes_to_copy = MIN(bytes_to_copy, whole_header.b_len-bytes_in_var_part);
                memcpy(&(record[bytes_in_var_part]), &bytes[bytes_used], bytes_to_copy);
                bytes_in_var_part += bytes_to_copy;
                bytes_used += bytes_to_copy;
                bytes_available -= bytes_to_copy;
                if (bytes_in_var_part >= whole_header.b_len) {
                    if (sh) {
                        sh(&whole_header, var_part, closure);
                    } /* endif */
                    bytes_in_header = 0;
                    bytes_in_var_part = 0;
                    in_accum = BUF_EMPTY;
                } /* endif */
                break;
            default:
                in_accum = BUF_EMPTY;
                break;
            } /* endswitch */
        } /* endwhile */
    } /* add_bytes */
};

#endif // VAR_ST_XPORT_H
