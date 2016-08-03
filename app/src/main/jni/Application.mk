APP_PLATFORM := android-19
APP_ABI := armeabi-v7a
APP_CPPFLAGS := -frtti -fexceptions -DTARGET_IS_ANDROID -DBASE_TEST_DIR="" -std=c++11
APP_CPPFLAGS += -w -Waddress -Wc++11-compat -Wchar-subscripts -Wformat -Winit-self
-Wmaybe-uninitialized -Wnarrowing -Wnonnull -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wsign-compare -Wsizeof-pointer-memaccess -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var
APP_STL := gnustl_static #stlport_static
