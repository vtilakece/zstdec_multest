#include <gst/gst.h>
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

/* For now: just pass buffers through unchanged */
static GstFlowReturn
gst_zstddec_transform_ip(GstBaseTransform *base, GstBuffer *buf)
{
  (void)(base);
  (void)(buf);
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

  /* no-op passthrough */
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

  /* in-place transform: input buffer is forwarded unchanged */
  trans_class->transform_ip = gst_zstddec_transform_ip;
}

static void
gst_zstddec_init(GstZstdDec *self)
{
   //g_print("INSTANCE INIT\n");
   g_print("INSTANCE INIT pid=%d\n", getpid());

    //This ensures no new buf create and or copy needed..  set_in_place ad passthrough... 

  gst_base_transform_set_in_place(GST_BASE_TRANSFORM(self), TRUE);
  gst_base_transform_set_passthrough(GST_BASE_TRANSFORM(self), TRUE);
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
    "Simple zstddec plugin that passes the data from the compressed zstd file and does nothing else with it",
    plugin_init,
    "0.1.0",
    "LGPL",
    PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)
