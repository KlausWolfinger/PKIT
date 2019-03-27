# restore from old pki to PKIT
function createRestoreProject() {
	clear 
	echo -en "\nEnter an new projectname, please: ";read NewProjectName
	if [ -d "$PKIHOME_START/PKIclone-Restored-$NewProjectName" ]; then
		echo "This project name exists already, delete manually or create a new one"
		echo "ENTER to continue ..."
		read
		# Back to mainmenu
		menu0
	else
	mkdir $PKIHOME_START/PKIclone-Restored-$NewProjectName
		if [ ! -d $LOGDIR ]; then 
			mkdir $LOGDIR 
			touch $LASTLOG
		else
		echo "## logging by restore tool ..." >>$LASTLOG
		echo "Dir $PKIHOME_START/PKIclone-Restored-$NewProjectName created for restore ..." >>$LASTLOG
		fi
	fi
}

function pki_restore() {
# 
clear
echo "--- RESTORE Utility ---"
echo "--- RESTORES TO PROJECT $NewProjectName ---"
echo "--- ARE YOU SURE ? [Yy or Nn]"
if (yes_no) 
	then
		if [ ! -e $PKIHOME_START/pkiconf/restoreconf.inf ]; then
			cp -p $PKIHOME_START/pkiconf/restoreconftemplate.inf  $PKIHOME_START/pkiconf/restoreconf.inf
		else
			. $PKIHOME_START/pkiconf/restoreconf.inf
		fi
				
		
		if [ ! -d "$PKIHOME_START/$RestoreSourceDir" ]; then
			mkdir $PKIHOME_START/$RestoreSourceDir
		fi	
		
		#echo "test 1"
		#read
		createRestoreProject
		#echo "test2"
		#read
		
		# now check if zip exists
		
		if [ -e $PKIHOME_START/$RestoreSourceDir/*.zip ]; then
			echo "ok, zip file found in $PKIHOME_START/$RestoreSourceDir ...."
			dir $PKIHOME_START/$RestoreSourceDir
			rm -R -f $PKIHOME_START/$RestoreSourceDir/tmp/
			unzip $PKIHOME_START/$RestoreSourceDir/*.zip -d $PKIHOME_START/$RestoreSourceDir/tmp/
		else
			echo ".... no zip found in $PKIHOME_START/$RestoreSourceDir ..."
			echo -e $red ".... unzip to restore failed, no zip file found ..." >>$LASTLOG
			
			echo "ENTER to continue ..."
			read
			return
			
		fi
		
		if [ ! -e $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf ]; then
			echo "There is no valid import filetree available ..."
			
				if [ ! -d $LOGDIR ]; then 
					mkdir $LOGDIR 
				fi
				if [ ! -e $LASTLOG ];then
				touch $LASTLOG
					echo "## File created initially by restore tool ..." >>$LASTLOG
				fi
				
			echo -e $red ".... no valid import filetree ..." >>$LASTLOG
			echo "ENTER to continue ..."
			read
			return			
		fi
		
		ImportX509File=$PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf
		ImportPkiConfFile=$PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/pkitool.conf
		# copy oldpki to restore folder
		# $PKIHOME_START/PKIclone-Restored-$NewProjectName
		
		cp -p -R $PKIHOME_START/$RestoreSourceDir/tmp/rootca $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/certs $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/keys $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/host $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/dh $PKIHOME_START/PKIclone-Restored-$NewProjectName
		cp -R $PKIHOME_START/$RestoreSourceDir/tmp/crl $PKIHOME_START/PKIclone-Restored-$NewProjectName
		read -p "--- copied all files to new Project $NewProjectName, ENTER to continue ..."
	if [ -e $ImportPkiConfFile ] && [ -e $ImportX509File ];	then
				cp -p $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf.save
				cp -p $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/pkitool.conf $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/pkitool.conf.save
				
				awk -F= '!a[$1]++'  $ImportX509File $PKIHOME_START/pkiconf/x509template.inf   >$PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf.imported
				# mkdir -p $PKIHOME_START/PKIclone-$NewProjectName/pkiconf/
				awk -F= '!a[$1]++'  $ImportPkiConfFile $PKIHOME_START/pkiconf/pkitooltemplate.conf  >$PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/pkitool.conf.imported
				# cp -R $PKIHOME_START/$RestoreSourceDir/* $PKIHOME_START/PKIclone-Restored-$NewProjectName
				echo "PKITprojectName=$NewProjectName" >$PKIHOME_START/PKIclone-Restored-$NewProjectName/pkiconf/projectname.inf
				cat $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/pkitool.conf.imported  >$PKIHOME_START/PKIclone-Restored-$NewProjectName/pkiconf/pkitool.conf
				cat $PKIHOME_START/$RestoreSourceDir/tmp/pkiconf/x509.inf.imported  >$PKIHOME_START/PKIclone-Restored-$NewProjectName/pkiconf/x509.inf
				echo "... sucessfully imported to $NewProjectName"
				read -p "Enter to continue ...."
				
		else
				echo "Missing files to restore"
				echo "$ImportPkiConfFile and $ImportX509File"
				read -p "Enter to exit ..."
	fi
	
fi	

}

function pki_restore_conf () {
if [ ! -e $PKIHOME_START/pkiconf/restoreconf.inf ]
			then
			cp -p $PKIHOME_START/pkiconf/restoreconftemplate.inf  $PKIHOME_START/pkiconf/restoreconf.inf
					
		fi
vi $PKIHOME_START/pkiconf/restoreconf.inf
. $PKIHOME_START/pkiconf/restoreconf.inf

}