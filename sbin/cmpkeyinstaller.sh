#!/bin/bash
## not tested ...
# shells script to replace the cmp tomcat certificates in one step.

tomcatkeystore="/opt/siemens/common/conf/tomcat-keystore.p12"
newp12file="/root/cmp2000.aptg.info.12345678.p12"
serverdirfile="/opt/siemens/share/tomcat/conf/server.xml"
serverdir="/opt/siemens/share/tomcat/conf/"
serverfile="server.xml"
spmlkeystore="/opt/siemens/common/conf/spmlresponder-keystore.p12"
# 
#cp $tomcatkeystore $tomcatkeystore.save
#cp $spmlkeystore $spmlkeystore.save
#cat $newp12file >$tomcatkeystore
#cat $newp12file >$spmlkeystore
#cp $serverdir/$serverfile /root/$serverfile.tmp
servertmpfile="/root/server.xml.tmp"
keyfile="/root/p12key.txt"
echo " los gehts ..."

keystring=$(cat $keyfile)
# cp $newp12file $keystore
/opt/siemens/servicetools/cmp/encodePassphrase.sh  >/root/tmp.txt
echo

# crl $ca_extension "" | openssl ca  -revoke $CERTSDIR/$extension/"$CN".crt -config /dev/stdin
# remove empty lines
cp /root/server.xml.tmp $serverfile


## sed -i "/keystorePass=/c\keystorePass=xxx" your_file_here
sed '/^$/d' /root/tmp.txt > p12key.txt
rm /root/tmp.txt
# sed to replace the key block
# !!! works only if SPML responder uses the same passphrase
sedcmd="-i \"/keystorePass=/c\keystorePass=\"$keystring\"\" $serverfile"

echo $sedcmd

eval sed $sedcmd
### "the keystorepass have no quotes!!

cp $serverfile server.test
# check server.test

 

