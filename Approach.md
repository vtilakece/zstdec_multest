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

Task 2: multidec Plugin

For the multidec plugin, I addedd the format property to the multidec plugin, so that it could be used to either auto detect the compressed format or give exclusively the format such as auto, gzip, bz2, zstd 

Once the options are added to the property, I used the enums to first define the respective decompress functions to decode their respective compressed formats. 
When the three formats gzip, bz2 and zstd decompression worked, I added a kinf of parser to autodetect the formats based on the first few bytes or magic bytes. 
Once the parser or detct format is completed, it returns the respective format and goes into the switch case to decompress the particular format and writes the decompresed stream to the output buffer. 

I had to restrict the output buffer to 100MB for now, as the  API  other than zstd lib do not necessarily give the expected output size. This could be handled better. 


Once, both the plugins worked as expected writing to the filesink, I added CI yaml file in the to the github workflow to run a CI workflow and test it probably on a cloud Ubuntu VM that runs in the backend. 
I made some changes to add some test data in the compressed fata, and checked if the output matches to the original uncompressed file. 
Once the CI runs without failues, and push or pull request triggers an activity on the branch, and tests if the build passed.. 