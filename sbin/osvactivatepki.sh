#!/bin/bash
# su -srx
pkiIP='10.20.200.30'
pkiUser=sysad
pkiAccount=$pkiUser@$pkiIP
scp $pkiAccount:MultiLevelPKI/rootca/rootCA.pem /tmp/rootCA.pem &&
scp $pkiAccount:MultiLevelPKI/certs/osvserver/osv_cluster_s.pem /tmp/server.pem &&
scp $pkiAccount:MultiLevelPKI/certs/osvclient/osv_cluster_c.pem /tmp/client.pem &&
scp $pkiAccount:MultiLevelPKI/dh/dh2048.pem /tmp/dh2048.pem;
read "ENTER"

CertsDir='/usr/local/ssl/certs'
cp -p $CertsDir/root.pem $CertsDir/root.pem.save
cat /tmp/rootCA.pem >$CertsDir/root.pem
PrivateDir='/usr/local/ssl/private'
cp -p $PrivateDir/server.pem $PrivateDir/server.pem.save
cat /tmp/server.pem >$PrivateDir/server.pem

cp -p $PrivateDir/client.pem $PrivateDir/client.pem.save
cat /tmp/client.pem >$PrivateDir/client.pem

dhDir='/usr/local/ssl/dh_keys'
cp -p $dhDir/dh1024.pem $dhDir/dh2048.pem
cat /tmp/dh2048.pem >$dhDir/dh2048.pem

# repeat this on node 2
# change srx params for dh key file

