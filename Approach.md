Approach to the problem:
Task 1: zstddec plugin
STEP1:

First create a simple plugin named zstdec, that passes the compressed zst file data as it is to the 
output stream. Test this with a simple filesink or a fakesink. 
This ensures that the plugin is correctly registered in the gstreamer cache and passes the buffers as expected.. 

STEP2:
if STEP1 successful then use zstd linux API in the chain function where the buffers are processed..
A simple transform_ip (in place) does passthrough the basetransform buffers as it is, and that created a compressed bitstream .. 
To overcome this, a prepare_output_buffer call is needed to know how much buffer is to be created for the output after decomopression from zstd

Once STEP2 is implemented, a passthrough is created with the right output buffer size needed for the decompressed stream.. 


Once, both the plugins worked as expected writing to the filesink, I added CI yaml file in the to the github workflow to run a CI workflow and test it probably on a cloud Ubuntu VM that runs in the backend. 
I made some changes to add some test data in the compressed fata, and checked if the output matches to the original uncompressed file. 
Once the CI runs without failues, and push or pull request triggers an activity on the branch, and tests if the build passed.. 