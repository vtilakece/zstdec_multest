#ifndef __GST_ZSTDDEC_H__
#define __GST_ZSTDDEC_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_ZSTDDEC (gst_zstddec_get_type())

typedef struct _GstZstdDec GstZstdDec;
typedef struct _GstZstdDecClass GstZstdDecClass;

struct _GstZstdDec {
  GstBaseTransform parent;
};

struct _GstZstdDecClass {
  GstBaseTransformClass parent_class;
};

GType gst_zstddec_get_type(void);

G_END_DECLS

#endif