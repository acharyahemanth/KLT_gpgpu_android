if [ ! -d "./assets" ]; then
	mkdir assets
fi
	
#Copy ALL shaders
if [ ! -d "./assets/shaders" ]; then
	mkdir assets/shaders
fi
find ./jni -name "*.?sh" -exec cp "{}" ./assets/shaders/ \;

