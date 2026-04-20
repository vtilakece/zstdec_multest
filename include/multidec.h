#ifndef __GST_MULTIDEC_H__
#define __GST_MULTIDEC_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_ZSTDDEC (gst_zstddec_get_type())

typedef struct _GstMultiDec GstMultiDec;
typedef struct _GstMultiDecClass GstMultiDecClass;

struct _GstMultiDec {
  GstBaseTransform parent;
};

struct _GstMultiDecClass {
  GstBaseTransformClass parent_class;
};

GType gst_multidec_get_type(void);
G_END_DECLS

#endif