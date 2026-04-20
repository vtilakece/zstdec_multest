Approach to the problem:

STEP1:

First create a simple plugin named zstdec, that passes the compressed zst file data as it is to the 
output stream. Test this with a simple filesink or a fakesink. 
This ensures that the plugin is correctly registered in the gstreamer cache and passes the buffers as expected.. 

STEP2:
if STEP1 successful then use zstd linux API in the chain function where the buffers are processed..
A simple transform_ip (in place) does passthrough the basetransform buffers as it is, and that created a compressed bitstream .. 
To overcome this, a prepare_buffer call is needed to know how much buffer is to be created for the output after decomopression from zstd

Once STEP2 is implemented, a passthrough is created with the right output buffer size needed for the decompressed stream.. 

