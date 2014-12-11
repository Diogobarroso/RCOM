ifconfig eth0 172.16.60.254/24
ifconfig eth1 172.16.61.253/24
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
