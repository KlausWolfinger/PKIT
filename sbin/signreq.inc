function issue_CandCerts(){ 
	export CN CN_count=0; 
	echo -en "--- creating certificates and keys from hosts file (signreq.inc)...\n"
	#CANDKEYSDIR=$CANDdir/CandKeys 
	#CANDCERTSDIR=$CANDdir/CandCerts 
	if [ ! -d $CANDKEYSDIR ]; then 
		mkdir $CANDKEYSDIR 
	fi 
	if [ ! -d $CANDCERTSDIR ]; then 
		mkdir $CANDCERTSDIR 
	fi 
	cat $HOSTDIR/CandReq \ 
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \ 
		| grep -v "^[ ]*!" \ 
		| while read cert; do 
			if [ ! -d $CANDKEYSDIR/$extension ]; then 
				mkdir $CANDKEYSDIR/$extension 
			fi 
			if [ ! -d $CANDCERTSDIR/$extension ]; then 
				mkdir $CANDCERTSDIR/$extension 
			fi
			echo -en "\n--> HOST: $cert\n\n" 
			CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
			SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
			# SALTN is DNS and IP (subjectAltAttributes) 
			# extension is "keyword like tlsserver, tlsboth etc 
			extension=$(echo "$cert" | cut -f1 -d:) 
			if [ -e $CANDCERTSDIR/$extension/"$CN".req -a ! -e $CANDCERTSDIR/$extension/"$CN".crt ]; then 
				mv $CANDCERTSDIR/$extension/"$CN".req $CANDCERTSDIR/$extension/tmp-"$CN".req 
				rm -f $CANDCERTSDIR/$extension/"$CN".* 
				mv $$CANDCERTSDIR/$extension/tmp-"$CN".req $CANDCERTSDIR/$extension/"$CN".req 
				ca $extension "$SALTN" sign \
					| openssl ca \
					-batch \
					-days $CERTDAYS \
					-out $CANDCERTSDIR/$extension/"$CN".crt \
					-in $CANDCERTSDIR/$extension/"$CN".req \
					-extensions exts \
					-preserveDN \
					-config /dev/stdin 
					else 
					echo -en "\t---> $CN crt exists, nothing todo\n" 
			fi 
		done
}
