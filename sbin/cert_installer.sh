# use with care
# not supported by UNIFY
# use only for training
# 
# -------------------------------------------
# here we start
# Installs the certificates to OpenScape directories
# do a check_cert
# check srx params
# dh key change:
# name             : SSL/EndPoint/Server/SupportedDH
# value            : dh1024.pem
# change to        : dh2048.pem


#name: SSL/MutualAuth/Server/SupportedDH
#value: dh1024.pem
#name: SSL/EndPoint/Server/CertificatesPath
#value: /usr/local/ssl/certs
#name: SSL/EndPoint/Server/DHPath
#value: /usr/local/ssl/dh_keys
#name: SSL/EndPoint/Server/Keys
#value: server.pem
#name: SSL/EndPoint/Server/KeysPath
#value: /usr/local/ssl/private
#name: SSL/EndPoint/Server/SupportedCertificates
#value: root.pem
#name: SSL/EndPoint/Server/SupportedCiphers
#value: AES128-SHA:AES256-SHA:DES-CBC3-SHA
#name: SSL/EndPoint/Server/SupportedDH
#value: dh1024.pem
#name: SSL/IdentityVerificationEnabled
#value: RtpFalse
#name: SSL/MutualAuth/Client/CertificatesPath
#value: /usr/local/ssl/certs
#name: SSL/MutualAuth/Client/DHPath
#value: /usr/local/ssl/dh_keys
#name: SSL/MutualAuth/Client/Keys
#value: client.pem
#name: SSL/MutualAuth/Client/KeysPath
#value: /usr/local/ssl/private
#name: SSL/MutualAuth/Client/SupportedCertificates
#value: root.pem
#name: SSL/MutualAuth/Client/SupportedCiphers
#value: AES128-SHA:AES256-SHA:DES-CBC3-SHA
#name: SSL/MutualAuth/Client/SupportedDH
#value: dh1024.pem
#name: SSL/MutualAuth/Server/CertificatesPath
#value: /usr/local/ssl/certs
#name: SSL/MutualAuth/Server/DHPath
#value: /usr/local/ssl/dh_keys
#name: SSL/MutualAuth/Server/Keys
#value: server.pem
#name: SSL/MutualAuth/Server/KeysPath
#value: /usr/local/ssl/private
#name: SSL/MutualAuth/Server/SupportedCertificates
#value: root.pem
#name: SSL/MutualAuth/Server/SupportedCiphers
#value: AES128-SHA:AES256-SHA:DES-CBC3-SHA
#name: SSL/MutualAuth/Server/SupportedDH
#value: dh1024.pem

        
# name             : SSL/MutualAuth/Client/SupportedDH
# value            : dh1024.pem
# change to        : dh2048.pem
       
# name             : SSL/MutualAuth/Server/SupportedDH
# value            : dh1024.pem
# change to        : dh2048.pem

	if [ ! -e $PKIHOME/x509.inf ]; then
		DH_LENGTH=1024
 	fi

	# IMPORT X509 Attributes File if exists
	if [ -e $PKIHOME/x509.inf ]; then
	echo "READ X509 Attributes from $PKIHOME/x509.inf"
		. $PKIHOME/x509.inf
	fi

 
PKIHOME="$(echo $0 | sed 's,/[^/]\+$,,')"
TLSHOME="/usr/local/ssl/"
TLSCERTHOME="/usr/local/ssl/certs"
TLSPRIVHOME="/usr/local/ssl/private"
SERVERCRT="server.pem"
CLIENTCRT="client.pem"
if [ ! -d $PKIHOME/tmp ]; then
mkdir $PKIHOME/tmp
fi
if [ ! -d $PKIHOME/params ]; then
mkdir $PKIHOME/params
fi
CHECKFILE=$PKIHOME/tmp/tls_check_file.$$
# -------- prepare soap command ----------
echo "# Defines the list of TLS srx parameters. " >> $PKIHOME/params/osv_rtptls_parameters.txt
echo "confModifyParameter \"SSL/EndPoint/Server/SupportedDH\" \"dh"$DH_LENGTH".pem\"" >> $PKIHOME/params/osv_rtptls_parameters.txt
echo "confModifyParameter \"SSL/MutualAuth/Client/SupportedDH\" \"dh"$DH_LENGTH".pem\"" >> $PKIHOME/params/osv_rtptls_parameters.txt
echo "confModifyParameter \"SSL/MutualAuth/Server/SupportedDH\" \"dh"$DH_LENGTH".pem\"" >> $PKIHOME/params/osv_rtptls_parameters.txt
# ------------------------------------------

if [ -e '/opt/SMAW/bin/RtpAdmCli' ]; then
# save the current files from CA store
cp -p $TLSCERTHOME/root.pem $TLSCERTHOME/root.pem.save
cp -p $TLSPRIVHOME/server.pem $TLSPRIVHOME/server.pem.save
cp -p $TLSPRIVHOME/client.pem $TLSPRIVHOME/client.pem.save

##
# copy the new certificates to system files
cp $PKIHOME/rootca/root.crt $TLSCERTHOME/root.pem
cp $PKIHOME/certs/$SERVERCRT $TLSPRIVHOME/server.pem
cp $PKIHOME/certs/$CLIENTCRT $TLSPRIVHOME/client.pem
cp $PKIHOME/dh$DH_LENGTH.pem $TLSHOME/dh_keys/dh$DH_LENGTH.pem





su - srx --command "/opt/SMAW/bin/RtpAdmCli -p 40 -l sysad -x" >> $CHECKFILE 2>&1 <<-!
 batch "$PKIHOME/params/osv_rtptls_parameters.txt"
exit 
!
else
echo "THIS IS NOT A OPENSCAPE SYSTEM !!!"
echo "COULD NOT PERFORM THE PKI CHANGE.."
echo
fi

