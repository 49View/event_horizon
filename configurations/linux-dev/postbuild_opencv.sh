if [ "$OSNAME" == "linux" ]; then	
	# cp -rf /3rdpartylibs/opencv/LIB_INTERNALBUILD_DIR/3rdparty/lib/* $REPO_ROOT/libs/opencv/lib/$OSNAME/
	# cp -rf /3rdpartylibs/opencv/LIB_INTERNALBUILD_DIR/3rdparty/ippicv/ippicv_lnx/lib/intel64/* $REPO_ROOT/libs/opencv/lib/$OSNAME/
	mv /usr/lib/x86_64-linux-gnu/libIlmImf.so /usr/lib/x86_64-linux-gnu/libIlmImf_original.so
	mv /usr/lib/x86_64-linux-gnu/libIlmImf.a /usr/lib/x86_64-linux-gnu/libIlmImf_original.a
fi
