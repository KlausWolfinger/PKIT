# restore from old pki to PKIT
function createRestoreProject() {
	clear 
	echo -en "\nEnter an new prjectname, please: ";read NewProjectName
	if [ -d "$PKIHOME/PKIclone-$NewProjectName" ]; then
		echo "This project name exists already, delete manually or creata a new one"
		echo "ENTER to continue ..."
		read
		return
	fi 
	
	# do not change order !!
	mkdir $PKIHOME/PKIclone-$NewProjectName
	
	
}

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
			
			fi
		createRestoreProject
		
		. $PKIHOME_START/pkiconf/restoreconf.inf
		ImportX509File=$PKIHOME_START/$RestoreSourceDir/pkiconf/x509.inf
		ImportPkiConfFile=$PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf
		# copy oldpki to restore folder
		mkdir $PKIHOME_START/$RestoreSourceDir/rootca
		cp -p -R $OldPKIDir/rootca $PKIHOME_START/$RestoreSourceDir
		cp -R $OldPKIDir/certs $PKIHOME_START/$RestoreSourceDir
		cp -R $OldPKIDir/keys $PKIHOME_START/$RestoreSourceDir
		cp -R $OldPKIDir/pkiconf $PKIHOME_START/$RestoreSourceDir
		read -p "--- old project copied, ENTER to continue ..."
	if [ -e $ImportPkiConfFile ] && [ -e $ImportX509File ]
			then
				cp -p $PKIHOME/pkiconf/x509.inf $PKIHOME/pkiconf/x509.inf.save
				cp -p $PKIHOME/pkiconf/pkitool.conf $PKIHOME/pkiconf/pkitool.conf.save
				
				awk -F= '!a[$1]++'  $PKIHOME/pkiconf/x509.inf $ImportX509File  >$PKIHOME_START/$RestoreSourceDir/pkiconf/x509.inf.imported
				
				cat $PKIHOME_START/$RestoreSourceDir/pkiconf/x509.inf.imported  >$PKIHOME_START/PKIclone-$NewProjectName/pkiconf/x509.inf

				awk -F= '!a[$1]++'  $PKIHOME/pkiconf/pkitool.conf $ImportPkiConfFile >$PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf.imported
				cat $PKIHOME_START/$RestoreSourceDir/pkiconf/pkitool.conf.imported  >$PKIHOME/PKIclone-$NewProjectName/pkiconf/pkitool.conf
				cp -R $PKIHOME_START/$RestoreSourceDir/* $PKIHOME_START/PKIclone-$NewProjectName
				echo "PKITprojectName=$NewProjectName" >$PKIHOME_START/PKIclone-$NewProjectName/pkiconf/projectname.inf
				echo "... sucessfully imported to $NewProjectName"
				read -p "Enter to continue ...."
				rm -f -R $PKIHOME_START/$RestoreSourceDir/*
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