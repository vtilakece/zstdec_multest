#ifndef __GST_MULTIDEC_H__
#define __GST_MULTIDEC_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_MULTIDEC (gst_multidec_get_type())
#define GST_TYPE_MULTIDEC_FORMAT (gst_multidec_format_get_type())

typedef enum {
  GST_MULTIDEC_FORMAT_AUTO = 0,
  GST_MULTIDEC_FORMAT_ZSTD,
  GST_MULTIDEC_FORMAT_GZIP,
  GST_MULTIDEC_FORMAT_BZIP2
} GstMultiDecFormat;

typedef struct _GstMultiDec GstMultiDec;
typedef struct _GstMultiDecClass GstMultiDecClass;

struct _GstMultiDec {
  GstBaseTransform parent;
  GstMultiDecFormat format;
};

struct _GstMultiDecClass {
  GstBaseTransformClass parent_class;
};

GType gst_multidec_get_type(void);
GType gst_multidec_format_get_type(void);

G_END_DECLS

#endif /* __GST_MULTIDEC_H__ */