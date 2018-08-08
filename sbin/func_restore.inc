# restore from old pki to PKIT
function pki_restore () {
# vars 


# create import dirs
# check files (pkitool.conf x509.inf)
# describe doing
# 
clear
echo "--- RESTORE Utility ---"
echo "--- RESTORE MAY DESTROY YOUR PROJECT ---"
echo "--- ARE YOU SURE ? [Yy or Nn]"
if (yes_no) 
	then
		if [ ! -e $PKIHOME_START/pkiconf/restoreconf.inf ]
			then
			cp -p $PKIHOME_START/pkiconf/restoreconftemplate.inf  $PKIHOME_START/pkiconf/restoreconf.inf
			
			read -p "test1"
		fi
		. $PKIHOME_START/pkiconf/restoreconf.inf
		ImportX509File=$PKIHOME_START/$RestoreSourceDir/pkiconf/x509.inf
		ImportPkiConfFile=$PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf
		# copy oldpki to restore folder
		cp -R $OldPKIDir $PKIHOME_START/$RestoreSourceDir
		read -p "break"
	if [ -e $ImportPkiConfFile ] && [ -e $ImportX509File ]
			then
				cp -p $PKIHOME/pkiconf/x509.inf $PKIHOME/pkiconf/x509.inf.save
				cp -p $PKIHOME/pkiconf/pkitool.conf $PKIHOME/pkiconf/pkitool.conf.save
				
				awk -F= '!a[$1]++' $ImportX509File $PKIHOME/pkiconf/x509.inf  >$PKIHOME_START/$RestoreSourceDir/pkiconf/x509.inf.imported
				cat pkiconf/x509.inf.imported  >$PKIHOME/pkiconf/x509.inf

				awk -F= '!a[$1]++' $ImportPkiConfFile $PKIHOME/pkiconf/pkitool.conf >$PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf.imported
				cat $PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf.imported  >$PKIHOME/pkiconf/pkitool.conf
			else
				echo "Missing files to restore"
				echo "$ImportPkiConfFile and $ImportX509File"
				read -p "Enter to exit ..."
	fi
	
fi	
	
	
# file tests


# cleanup project
# copy rootca
# copy certs
# copy keys
# test root ca
# create crl
# write log
# delete import dir

# mit alten tool testen



}

function pki_restore_conf () {
if [ ! -e $PKIHOME_START/pkiconf/restoreconf.inf ]
			then
			cp -p $PKIHOME_START/pkiconf/restoreconftemplate.inf  $PKIHOME_START/pkiconf/restoreconf.inf
					
		fi
vi $PKIHOME_START/pkiconf/restoreconf.inf
. $PKIHOME_START/pkiconf/restoreconf.inf

}