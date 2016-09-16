# KLT_gpgpu_android
Android app which runs the gpgpu implementation of the KL tracker 

16/9/2016
========== USAGE ================
App opens up and waits for user to click start. Beyond this the tracking begins

========== DATA =================
App shows FPS + number of corners that are being tracked at the bottom left

========== CPU / GPU ============
Click options menu and click swap to toggle between cpu / gpu operation

========== OBSERVATIONS ============
a) GPU code seems to run slower than the cpu (opencv) version. Main time consuming part is the #iterations
for KL tracker which remains fixed in the gpu version(klt_gpu.cpp->num_iterations_kl_tracker) with no termination check.
This is because reading the gpu output textures to check for termination at every iteration seems to cost more time
than just running it through all iterations...
b) GL_LINEAR sampling doesnt seem to be supported on the MotoG2 phone. Hence BLT interpolation implementation that
is done manually in the shaders. Performance is hugely dependent on this.