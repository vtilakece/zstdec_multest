#include <gst/gst.h>
#include <inttypes.h>
#include <unistd.h>
#include <gst/base/gstbasetransform.h>

#include "../include/zstddec.h"

#ifndef PACKAGE
#define PACKAGE "zstddec"
#endif


#define PACKAGE_NAME "zstddec"
#define GST_PACKAGE_ORIGIN "https://sometest-website.com"

#ifdef HAVE_ZSTD
#include <zstd.h>
#endif

/*Guard to check max decomporessed size */
#define MAX_DECOMPRESSED_SIZE (100 * 1024 * 1024) // 100 MB

G_DEFINE_TYPE(GstZstdDec, gst_zstddec, GST_TYPE_BASE_TRANSFORM)

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
gst_zstddec_prepare_output_buffer(GstBaseTransform *base,
                                  GstBuffer *inbuf,
                                  GstBuffer **outbuf)
{
  GstMapInfo inmap;
  unsigned long long frame_size = 0;

  (void)base;

  if (!gst_buffer_map(inbuf, &inmap, GST_MAP_READ)) {
    g_printerr("zstddec: failed to map input buffer in prepare_output_buffer\n");
    return GST_FLOW_ERROR;
  }

  frame_size = ZSTD_getFrameContentSize(inmap.data, inmap.size);

  if (frame_size == ZSTD_CONTENTSIZE_ERROR) {
    g_printerr("zstddec: not a valid zstd frame, or buffer too small\n");
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }

  if (frame_size == ZSTD_CONTENTSIZE_UNKNOWN) {
    g_printerr("zstddec: valid zstd frame, but decompressed size is unknown\n");
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }
   
  if (frame_size == 0 || frame_size > MAX_DECOMPRESSED_SIZE ) {
    g_printerr("zstddec: suspicious frame size: %llu\n", frame_size);
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }
 //TODO: Check if the frame_size/outbuf  needs padding offsets... 
  *outbuf = gst_buffer_new_allocate(NULL, (gsize)frame_size, NULL);
  if (*outbuf == NULL) 
  g_printerr("zstddec: failed to allocate output buffer of %llu bytes\n", frame_size);

  g_print("zstddec-preparing outbuf: Allocated output buffer: frame size is %llu bytes\n", frame_size);

  if (*outbuf == NULL) {
    g_printerr("zstddec: failed to allocate output buffer of %llu bytes\n",
               frame_size);
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }

  gst_buffer_unmap(inbuf, &inmap);
  return GST_FLOW_OK;
}

/* For now: just pass buffers through unchanged */
static GstFlowReturn
gst_zstddec_transform(GstBaseTransform *base, GstBuffer *inbuf, GstBuffer *outbuf)
{
  (void)(base);
  //(void)(buf);
#ifdef HAVE_ZSTD
  static gsize once = 0;
  if (g_once_init_enter(&once)) {
    g_print("zstddec loaded, libzstd header version: %u\n",
            ZSTD_versionNumber());
    g_once_init_leave(&once, 1);
  }
#else
  static gsize once = 0;
  if (g_once_init_enter(&once)) {
    g_print("zstddec loaded, built without zstd support\n");
    g_once_init_leave(&once, 1);
  }
#endif

  GstMapInfo inmap;
  GstMapInfo outmap;
  size_t ret = 0;

  /* no-op passthrough */
  if (!gst_buffer_map(inbuf, &inmap, GST_MAP_READ)) {
    g_printerr("zstddec: failed to map input buffer\n");
    return GST_FLOW_ERROR;
  }

  if (!gst_buffer_map(outbuf, &outmap, GST_MAP_WRITE)) {
    g_printerr("zstddec: failed to map output buffer\n");
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }

  ret = ZSTD_decompress(outmap.data, outmap.size, inmap.data, inmap.size);

  if (ZSTD_isError(ret)) {
    g_printerr("zstddec: decompress failed: %s\n", ZSTD_getErrorName(ret));
    gst_buffer_unmap(outbuf, &outmap);
    gst_buffer_unmap(inbuf, &inmap);
    return GST_FLOW_ERROR;
  }
  
  g_print("zstddec: decompressed successfully: %zu bytes\n", ret);

  gst_buffer_unmap(outbuf, &outmap);
  gst_buffer_unmap(inbuf, &inmap);

  return GST_FLOW_OK;
  }


  

static void
gst_zstddec_class_init(GstZstdDecClass *klass)
{
   g_print("CLASS INIT pid=%d\n", getpid());

 // g_print("CLASS INIT\n");
  GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
  GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS(klass);

  gst_element_class_set_static_metadata(
      element_class,
      "Zstd passthrough decoder",
      "Filter/Decoder",
      "zstddec element test: passes buffers through unchanged",
      "Tilak");

  gst_element_class_add_pad_template(
      element_class,
      gst_static_pad_template_get(&sink_template));

  gst_element_class_add_pad_template(
      element_class,
      gst_static_pad_template_get(&src_template));

  /* Replace in-place transform to allow modifications to out buffer*/
  //trans_class->transform = gst_zstddec_transform;
  // Prepare output buffer needed to be implemented to allocate the output buffer of the right size for the decompressed data. This is required since we are not doing in-place transformation and need to create a new buffer for the output.
  trans_class->prepare_output_buffer = gst_zstddec_prepare_output_buffer;
  trans_class->transform = gst_zstddec_transform;
}

static void
gst_zstddec_init(GstZstdDec *self)
{
   //g_print("INSTANCE INIT\n");
   g_print("INSTANCE INIT pid=%d\n", getpid());

    //This ensures no new buf create and or copy needed..  set_in_place ad passthrough... 

 gst_base_transform_set_in_place(GST_BASE_TRANSFORM(self), FALSE);
 gst_base_transform_set_passthrough(GST_BASE_TRANSFORM(self), FALSE);
}


static gboolean
plugin_init(GstPlugin *plugin)
{
  return gst_element_register(plugin, "zstddec", GST_RANK_NONE, gst_zstddec_get_type());
}


GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    zstddec,   // the plugin name GStreamer uses
    "Simple zstddec plugin that decodes a zstd compressed file and passes through the decompressed data. This is a test plugin for zstd decompression.",
    plugin_init,
    "0.1.0",
    "LGPL",
    PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)
