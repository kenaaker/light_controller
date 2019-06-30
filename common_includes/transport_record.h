#ifndef TRANSPORT_RECORD_H
#define TRANSPORT_RECORD_H
#include "fixedendian.h"
#include "var_st_xport.h"
#include <stdint.h>
#include <QObject>
#include <QString>
#include <QDebug>

static const uint8_t report_transport_type = 'S';
static const uint8_t report_transport_version = 2;
static const uint16_t report_transport_biggest_body = 4096;

#pragma pack(push,1)
typedef struct rep_hdr {
    uint8_t type;
    uint8_t len;
    uint8_t version;
    BigEndian<uint16_t> b_len;

    rep_hdr(unsigned int in_len=0) {
        type = 'S';
        len = sizeof(struct rep_hdr);
        version = 2;
        b_len = in_len;
    }
} rep_hdr_t;

typedef struct report_transport_body {
    char record[report_transport_biggest_body];
    report_transport_body() {
        memset(record, 0, sizeof(record));
    }
    report_transport_body(unsigned int) {
        memset(record, 0, sizeof(record));
    }
} report_transport_body_t;

typedef struct report_transport_struct {
    rep_hdr_t rt_hdr;
    report_transport_body_t rt_body;
    void init(void) {
        rt_hdr.b_len = sizeof(rt_body);
        memset(rt_body.record, 0, sizeof(rt_body.record));
    }
    report_transport_struct(unsigned int) {
        init();
    }

    report_transport_struct(QString const &in_str) {
        init();
        if (in_str.size() != 0) {
            QByteArray in_bytes(in_str.toUtf8());
            rt_hdr.b_len = in_str.size()+1;
            memset(rt_body.record, 0, sizeof(rt_body.record));
            memcpy(rt_body.record, in_bytes, in_bytes.size());
            rt_body.record[rt_hdr.b_len] = 0;
        }
    }

    report_transport_struct(unsigned int size, const QByteArray report_data) {
        init();
        rt_hdr.b_len = size;
        memset(rt_body.record, 0, sizeof(rt_body.record));
        memcpy(rt_body.record, report_data, rt_hdr.b_len);
        rt_body.record[rt_hdr.b_len] = 0;
    }
} report_transport_struct_t;
#pragma pack(pop)

static const uint8_t report_transport_hdr_len = sizeof(rep_hdr_t);
static const uint16_t report_transport_body_len_empty = sizeof(report_transport_body_t);

#endif // TRANSPORT_RECORD_H
