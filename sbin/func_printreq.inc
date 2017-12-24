function print_requests() { 
	export CN
	CN_count=0; 
	# echo -en "--- printing certificate requests from certificate requests (func_printreq.inc)...\n" 
	cat $HOSTDIR/hosts \
	| grep -v "^[ ]*#" \
	| grep -v "^[ ]*$" \
	| grep -v "^[ ]*!" \
	| while read cert; do 
		echo -en "\n--> HOST: $cert " 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
		# SALTN is DNS and IP (subjectAltAttributes) 
		# extension is "keyword like tlsserver, tlsboth etc 
		extension=$(echo "$cert" | cut -f1 -d:) 
		#echo "---> extension= $extension" 
		# $CERTSDIR/$extension #
			mkdir $CERTSDIR/$extension 
		if [ ! -e $KEYSDIR/$extension/"$CN".key ]; then 
			echo "no key pair found" 
		fi 
		if [ ! -e $KEYSDIR/$extension/"$CN".pub ]; then 
			echo "no public key found"
		fi 
		
		if [ ! -e $CERTSDIR/$extension/"$CN".req ]; then 
			echo "no request found"
		fi 
		
		done 
		echo -en "\n\t--> Press ENTER to continue" 
		read 
}
