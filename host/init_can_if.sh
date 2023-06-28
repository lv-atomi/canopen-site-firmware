IF=$1
ip link set ${IF} txqueuelen 1000
ip link set up ${IF} type can bitrate 500000
ip link set up ${IF}
