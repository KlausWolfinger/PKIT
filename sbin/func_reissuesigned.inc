function reissue_signed() {
export CN
	# reissue 
	echo -en "--- reissuing certificates and revoke old...\n" 
	echo -en "\t ------- choose one from the list --------\n"
	certArray=($(cat $HOSTDIR/hosts \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		)) 
	tLen=${#certArray[@]}
	for (( i=1;i<=${tLen}; i++ )); do
		
		echo -en "$i --> \t${certArray[$(( i-1 ))]}\n" 
		done 
		echo
		echo -en "\t--> For reissuing enter number from list: " 
		read certnum;
		cert=${certArray[((certnum--))]} 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,)
		extension=$(echo "$cert" | cut -f1 -d:)
		# echo -e "\n ... choosen extension = $extension and CN = $CN";read		
		if [ ! -r $CERTSDIR/$extension/"$CN".crt ]; then
			echo  -e "\n ------------------------------
Can't find the $CN certificate, it should signed first ... 
--> [ENTER]";read
			return;
		fi
		echo -en "\n\t--> Revoking $CN from Cert store"
		echo -en "\n\t    Your choice for cert: $certnum: with CN = $CN : [Yy]es or [Nn]o ?\n" 
		if (yes_no); then 
			 
			crl $ca_extension "" | openssl ca -revoke $CERTSDIR/$extension/"$CN".crt -config /dev/stdin 
		fi
 
	# delete files
	# echo -en "--- creating certificate requests from hosts file (func_requests.inc)...\n" 
		# echo "---> reissueing: CN= $CN";read 
		
		cp -r -p $CERTSDIR/$extension/"$CN".* $TMPDIR/
		rm -f $CERTSDIR/$extension/"$CN".* 
		ca $extension "$SALTN" req | openssl req \
			-new -days $CERTDAYS \
			-nodes -key $KEYSDIR/$extension/"$CN".key \
			-out $CERTSDIR/$extension/"$CN".req \
			-config /dev/stdin \
			-reqexts exts \
			-subj /"$SUBDN"/CN="$CN""$CERTMAIL" 
		echo -en "\n\t Optionally edit the Candidate $CA ...";
		echo -en "\n\t Then you need to Re-Sign the $CA request ...";
		echo -en "\n\t--> Press ENTER to continue";read 
	

	# select from array
	# revocation list CA
	# mv all CA certs to tmp
	# cp rep from tmp back
	# test if ca is in candidates
	# mv -p $CERTSDIR/$extension/"$CN".req $TMPDIR/"$CN".req.tmp
	# rm "$CN".*
	# shopt -s extglob
	# rm -- !(*.req)
	# rm $CERTSDIR/$extension/"$CN".*
	
}
