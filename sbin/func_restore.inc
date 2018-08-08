# restore from old pki to PKIT
function pki_restore () {

# create import dirs
# check files (pkitool.conf x509.inf)
# describe doing
# 
awk -F= '!a[$1]++' restore/x509.inf pkiconf/x509.inf >pkiconf/x509.inf.imported
cp -p pkiconf/x509.inf pkiconf/x509.inf.save
cat pkiconf/x509.inf.imported  >pkiconf/x509.inf

awk -F= '!a[$1]++' restore/pkitool.conf pkiconf/pkitool.conf >pkiconf/pkitool.conf.imported
cp -p pkiconf/pkitool.conf pkiconf/pkitool.conf.save
cat pkiconf/pkitool.conf.imported  >pkiconf/pkitool.conf

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