Approach to the problem:

STEP1:

First create a simple plugin named zstdec, that passes the compressed zst file data as it is to the 
output stream. Test this with a simple filesink or a fakesink. 
This ensures that the plugin is correctly registered in the gstreamer cache and passes the buffers as expected.. 