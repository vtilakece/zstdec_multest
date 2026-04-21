#include <gst/gst.h>
#include <inttypes.h>
#include <unistd.h>
#include <gst/base/gstbasetransform.h>

#include "../include/multidec.h"

#ifndef PACKAGE
#define PACKAGE "multidec"
#endif


#define PACKAGE_NAME "multidec"
#define GST_PACKAGE_ORIGIN "https://sometest-website.com"

#ifdef HAVE_ZSTD
#include <zstd.h>
// https://facebook.github.io/zstd/zstd_manual.html for zstd decompression APIs
#endif

#ifdef HAVE_ZLIB
#include <zlib.h>
//See https://www.zlib.net/manual.html for zlib decompression APIs 

static gboolean gst_multidec_gzip_decompress(const guint8 *src,
                             gsize src_size,
                             guint8 *dst,
                             gsize dst_capacity,
                             gsize *dst_size)
{
  z_stream strm;
  int zret;

  memset(&strm, 0, sizeof(strm));
  strm.next_in = (Bytef *)src;
  strm.avail_in = (uInt)src_size;
  strm.next_out = dst;
  strm.avail_out = (uInt)dst_capacity;

  /* 16 + MAX_WBITS => gzip decoding */
  zret = inflateInit2(&strm, 16 + MAX_WBITS);
  if (zret != Z_OK) {
    g_printerr("multidec: inflateInit2 failed: %d\n", zret);
    return FALSE;
  }

  zret = inflate(&strm, Z_FINISH);

  if (zret != Z_STREAM_END) {
    g_printerr("multidec: inflate failed: %d\n", zret);
    inflateEnd(&strm);
    return FALSE;
  }

  *dst_size = (gsize)strm.total_out;

  inflateEnd(&strm);
  return TRUE;
}

#endif

#ifdef HAVE_BZIP2
#include <bzlib.h>
#endif


/*Guard to check max decomporessed size */
#define MAX_DECOMPRESSED_SIZE (100 * 1024 * 1024) // 100 MB


enum
{
  PROP_0,
  PROP_FORMAT,
};

G_DEFINE_TYPE(GstMultiDec, gst_multidec, GST_TYPE_BASE_TRANSFORM)

/*Definition of setter and getters for the properties */

static void gst_multidec_set_property(GObject *object,
                         guint prop_id,
                         const GValue *value,
                         GParamSpec *pspec)
{
  GstMultiDec *self = (GstMultiDec *)object;

  switch (prop_id) {
    case PROP_FORMAT:
      self->format = g_value_get_enum(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void gst_multidec_get_property(GObject *object,
                         guint prop_id,
                         GValue *value,
                         GParamSpec *pspec)
{
  GstMultiDec *self = (GstMultiDec *)object;

  switch (prop_id) {
    case PROP_FORMAT:
      g_value_set_enum(value, self->format);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/* Accept any bytes or bufferin, and same buffer out */
static GstStaticPadTemplate sink_template =
GST_STATIC_PAD_TEMPLATE(
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate src_template =
GST_STATIC_PAD_TEMPLATE(
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);


static GstFlowReturn
gst_multidec_prepare_output_buffer(GstBaseTransform *base,
                                   GstBuffer *inbuf,
                                   GstBuffer **outbuf)
{
  GstMapInfo inmap;
  unsigned long long frame_size = 0;
  GstMultiDec *self = (GstMultiDec *)base;

  if (!gst_buffer_map(inbuf, &inmap, GST_MAP_READ)) {
    g_printerr("multidec: failed to map input buffer in prepare_output_buffer\n");
    return GST_FLOW_ERROR;
  }

  switch (self->format) {
    case GST_MULTIDEC_FORMAT_AUTO:
      g_print("multidec: format=auto\n");
      /* later: detect from header */
      gst_buffer_unmap(inbuf, &inmap);
      g_printerr("multidec: auto detect not implemented yet\n");
      return GST_FLOW_ERROR;

    case GST_MULTIDEC_FORMAT_ZSTD:
      g_print("multidec: format=zstd\n");
#ifdef HAVE_ZSTD
      frame_size = ZSTD_getFrameContentSize(inmap.data, inmap.size);

      if (frame_size == ZSTD_CONTENTSIZE_ERROR) {
        g_printerr("multidec: not a valid zstd frame, or buffer too small\n");
        gst_buffer_unmap(inbuf, &inmap);
        return GST_FLOW_ERROR;
      }

      if (frame_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        g_printerr("multidec: valid zstd frame, but decompressed size is unknown\n");
        gst_buffer_unmap(inbuf, &inmap);
        return GST_FLOW_ERROR;
      }

      if (frame_size == 0 || frame_size > MAX_DECOMPRESSED_SIZE) {
        g_printerr("multidec: suspicious zstd frame size: %llu\n", frame_size);
        gst_buffer_unmap(inbuf, &inmap);
        return GST_FLOW_ERROR;
      }

      self->expected_outbuf_size = (gsize)frame_size;
#else
      gst_buffer_unmap(inbuf, &inmap);
      g_printerr("multidec: built without zstd support\n");
      return GST_FLOW_ERROR;
#endif
      break;

    case GST_MULTIDEC_FORMAT_GZIP:
      g_print("multidec: format=gzip\n");
#ifdef HAVE_ZLIB
      self->expected_outbuf_size = MAX_DECOMPRESSED_SIZE;
#else
      gst_buffer_unmap(inbuf, &inmap);
      g_printerr("multidec: built without zlib support\n");
      return GST_FLOW_ERROR;
#endif
      break;

    case GST_MULTIDEC_FORMAT_BZIP2:
      g_print("multidec: format=bzip2\n");
#ifdef HAVE_BZIP2
      self->expected_outbuf_size = MAX_DECOMPRESSED_SIZE;
#else
      gst_buffer_unmap(inbuf, &inmap);
      g_printerr("multidec: built without bzip2 support\n");
      return GST_FLOW_ERROR;
#endif
      break;

    default:
      gst_buffer_unmap(inbuf, &inmap);
      return GST_FLOW_ERROR;
  }

  *outbuf = gst_buffer_new_allocate(NULL, self->expected_outbuf_size, NULL);
  if (*outbuf == NULL) {
    g_printerr("multidec: failed to allocate output buffer of %zu bytes\n",
               self->expected_outbuf_size);
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }

  g_print("multidec: allocated output buffer of %zu bytes\n",
          self->expected_outbuf_size);

  gst_buffer_unmap(inbuf, &inmap);
  return GST_FLOW_OK;
}

/* For now: just pass buffers through unchanged */
static GstFlowReturn
gst_multidec_transform(GstBaseTransform *base, GstBuffer *inbuf, GstBuffer *outbuf)
{
  GstMultiDec *self = (GstMultiDec *)base;
  GstMapInfo inmap;
  GstMapInfo outmap;
  size_t ret = 0;

  static gsize once = 0;
  if (g_once_init_enter(&once)) {
#ifdef HAVE_ZSTD
    g_print("multidec: libzstd header version: %u\n", ZSTD_versionNumber());
#else
    g_print("multidec: built without zstd support\n");
#endif

#ifdef HAVE_ZLIB
    g_print("multidec: zlib version: %s\n", zlibVersion());
#else
    g_print("multidec: built without zlib support\n");
#endif

#ifdef HAVE_BZIP2
    g_print("multidec: bzip2 support found\n");
#else
    g_print("multidec: built without bzip2 support\n");
#endif

    g_once_init_leave(&once, 1);
  }

  if (!gst_buffer_map(inbuf, &inmap, GST_MAP_READ)) {
    g_printerr("multidec: failed to map input buffer\n");
    return GST_FLOW_ERROR;
  }

  if (!gst_buffer_map(outbuf, &outmap, GST_MAP_WRITE)) {
    g_printerr("multidec: failed to map output buffer\n");
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }

  g_print("multidec: outbuf size=%zu inbuf size=%zu\n",
          gst_buffer_get_size(outbuf),
          gst_buffer_get_size(inbuf));

  switch (self->format) {
    case GST_MULTIDEC_FORMAT_ZSTD:
#ifdef HAVE_ZSTD
      ret = ZSTD_decompress(outmap.data, outmap.size, inmap.data, inmap.size);

      if (ZSTD_isError(ret)) {
        g_printerr("multidec: zstd decompress failed: %s\n",
                   ZSTD_getErrorName(ret));
        gst_buffer_unmap(outbuf, &outmap);
        gst_buffer_unmap(inbuf, &inmap);
        return GST_FLOW_ERROR;
      }

      gst_buffer_set_size(outbuf, ret);
      g_print("multidec: zstd decompressed successfully: %zu bytes\n", ret);
      break;
#else
      g_printerr("multidec: zstd support not built\n");
      gst_buffer_unmap(outbuf, &outmap);
      gst_buffer_unmap(inbuf, &inmap);
      return GST_FLOW_ERROR;
#endif

    case GST_MULTIDEC_FORMAT_GZIP:
#ifdef HAVE_ZLIB
    {
      gsize actual_size = 0;

      if (!gst_multidec_gzip_decompress(inmap.data,
                                        inmap.size,
                                        outmap.data,
                                        outmap.size,
                                        &actual_size)) {
        gst_buffer_unmap(outbuf, &outmap);
        gst_buffer_unmap(inbuf, &inmap);
        return GST_FLOW_ERROR;
      }

      gst_buffer_set_size(outbuf, actual_size);
      g_print("multidec: gzip decompressed successfully: %zu bytes\n", actual_size);
      break;
    }
#else
      g_printerr("multidec: gzip support not built\n");
      gst_buffer_unmap(outbuf, &outmap);
      gst_buffer_unmap(inbuf, &inmap);
      return GST_FLOW_ERROR;
#endif

    case GST_MULTIDEC_FORMAT_BZIP2:
      g_printerr("multidec: bzip2 path not implemented yet\n");
      gst_buffer_unmap(outbuf, &outmap);
      gst_buffer_unmap(inbuf, &inmap);
      return GST_FLOW_ERROR;

    case GST_MULTIDEC_FORMAT_AUTO:
    default:
      g_printerr("multidec: auto/unknown format handling not implemented in transform yet\n");
      gst_buffer_unmap(outbuf, &outmap);
      gst_buffer_unmap(inbuf, &inmap);
      return GST_FLOW_ERROR;
  }

  gst_buffer_unmap(outbuf, &outmap);
  gst_buffer_unmap(inbuf, &inmap);

  return GST_FLOW_OK;
}

static void gst_multidec_class_init(GstMultiDecClass *klass)
{
   g_print("CLASS INIT pid=%d\n", getpid());
//Instantiate a pointer to the GObjectClass structure, which is used to set the class methods for the GstMultiDec class, such as property setters and getters, and other virtual methods that the class may override.
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

 //Define the setters and getters for the element properties:
  gobject_class->set_property = gst_multidec_set_property;
  gobject_class->get_property = gst_multidec_get_property;

 // g_print("CLASS INIT\n");
  GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
  GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS(klass);

  gst_element_class_set_static_metadata(
      element_class,
      "Multidec passthrough decoder",
      "Filter/Decoder",
      "zstddec element test: passes buffers through unchanged",
      "Tilak");

  gst_element_class_add_pad_template(
      element_class,
      gst_static_pad_template_get(&sink_template));

  gst_element_class_add_pad_template(
      element_class,
      gst_static_pad_template_get(&src_template));

  g_object_class_install_property(
    gobject_class,
    PROP_FORMAT,
    g_param_spec_enum(
        "format",
        "Format",
        "Compression format: auto, zstd, gzip, bzip2",
        GST_TYPE_MULTIDEC_FORMAT,
        GST_MULTIDEC_FORMAT_AUTO,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));    

  /* Replace in-place transform to allow modifications to out buffer*/
  //trans_class->transform = gst_zstddec_transform;
  // Prepare output buffer needed to be implemented to allocate the output buffer of the right size for the decompressed data. This is required since we are not doing in-place transformation and need to create a new buffer for the output.
  trans_class->prepare_output_buffer = gst_multidec_prepare_output_buffer;
  trans_class->transform = gst_multidec_transform;
}

static void gst_multidec_init(GstMultiDec *self)
{
   //g_print("INSTANCE INIT\n");
   g_print("INSTANCE INIT pid=%d\n", getpid());

   self->format = GST_MULTIDEC_FORMAT_AUTO;
   //This ensures no new buf create and or copy needed..  set_in_place as passthrough... 

   gst_base_transform_set_in_place(GST_BASE_TRANSFORM(self), FALSE);
   gst_base_transform_set_passthrough(GST_BASE_TRANSFORM(self), FALSE);
}

GType gst_multidec_format_get_type(void)
{
  static GType type = 0;
  
  // Map the table of values with the enum values to register the enum type with GLib's type system, so it can be used in properties and other places where GTypes are needed.
  static const GEnumValue values[] = {
    { GST_MULTIDEC_FORMAT_AUTO,  "Auto detect", "auto"  },
    { GST_MULTIDEC_FORMAT_ZSTD,  "Zstandard",   "zstd"  },
    { GST_MULTIDEC_FORMAT_GZIP,  "Gzip",        "gzip"  },
    { GST_MULTIDEC_FORMAT_BZIP2, "Bzip2",       "bzip2" },
    { 0, NULL, NULL }
  };
  
  //Register once the struct of enums GstMultiDecFormat with GLib's type system with the above filled values. 
  if (type == 0) {
    type = g_enum_register_static("GstMultiDecFormat", values);
  }

  return type;
}


static gboolean plugin_init(GstPlugin *plugin)
{
  return gst_element_register(plugin, "multidec", GST_RANK_NONE, gst_multidec_get_type());
}


GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    multidec,   // the plugin name for multidec 
    "Simple multidec plugin that parses the comporessed data from zstd or  zlib or bzip file and does nothing else with it",
    plugin_init,
    "0.1.0",
    "LGPL",
    PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)
