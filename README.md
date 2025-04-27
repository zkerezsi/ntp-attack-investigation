# NTP attack investigation

I found a humorous latin-like qoute on one of the sites of the university where I studied, and I think it might have some connection with the issue I am trying to describe here with more or less success:

> Sir Mérész Dánielus Lepus Curator Septem Jahodus Choteborus Localus Areaus Networkus Routerus Sideeffectus Majordomus Americano Magister Maximus - https://sssl.sch.bme.hu/

I think I discovered some kind of design flaw in the way Network Time Protocol is working which is currently actively expoited on my computer and it might enable third parties to read all TLS encrypted traffic that my computer is sending so I created this public repository to document these phenomena.

I think I am good at remembering specific things, and I remember that on one of the classes where we solved practical tasks the guy who teached us told at some point, that the next task which was about setting up NTP on a linux machine should be solved in a way they described it in __the email__.

I also remember, that I said it loudly that "I did not receive any email about this" and of course I was unable to solve that problem.

I purchased an OpenWRT One router, which has a really good feature under Status > Realtime Graphs > Connections
It shows the currently running source ip:port to destination ip:port traffic with the transferred bytes and packets. I have some images about the UI but I can make new screenshots of course. I noticed that one of the interfaces of the brand new router sends frequent udp packets towards many NTP servers.

![Router Port 53](images/router_port_53.png "Router Port 53")

![Router With DNS](images/router_with_dns.png "Router With DNS")

Then I used tcpdump on my laptop which showed lots of traffic towards even more different servers and the _transmitted timestamp is always wrong by decades_.

```sh
sudo tcpdump -i wlp3s0 udp dst port 123 -vvv
```

```console
14:27:39.633013 IP (tos 0xb8, ttl 64, id 25543, offset 0, flags [DF], proto UDP (17), length 76)
    ubuntu.lan.ntp > etharin.fidesz.hu.ntp: [udp sum ok] NTPv4, Client, length 48
	Leap indicator:  (0), Stratum 0 (unspecified), poll 0 (1s), precision 32
	Root Delay: 0.000000, Root dispersion: 0.000000, Reference-ID: (unspec)
	  Reference Timestamp:  0.000000000
	  Originator Timestamp: 0.000000000
	  Receive Timestamp:    0.000000000
	  Transmit Timestamp:   215924456.708249765 (1906-11-05T03:00:56Z)
	    Originator - Receive Timestamp:  0.000000000
	    Originator - Transmit Timestamp: 215924456.708249765 (1906-11-05T03:00:56Z)
14:27:39.791891 IP (tos 0xb8, ttl 52, id 36727, offset 0, flags [DF], proto UDP (17), length 76)
    etharin.fidesz.hu.ntp > ubuntu.lan.ntp: [udp sum ok] NTPv4, Server, length 48
	Leap indicator:  (0), Stratum 3 (secondary reference), poll 3 (8s), precision -23
	Root Delay: 0.026046, Root dispersion: 0.044662, Reference-ID: 0xc1e3c502
	  Reference Timestamp:  3952583754.000853736 (2025-04-02T11:55:54Z)
	  Originator Timestamp: 215924456.708249765 (1906-11-05T03:00:56Z)
	  Receive Timestamp:    3952585659.764912403 (2025-04-02T12:27:39Z)
	  Transmit Timestamp:   3952585659.765026022 (2025-04-02T12:27:39Z)
	    Originator - Receive Timestamp:  +3736661203.056662637
	    Originator - Transmit Timestamp: +3736661203.056776256
```

The weird dates seem more than suspicious to me although it seems that my computer is sending the bad timestamps first and it is always something completely different datetime.

A lot more of these packets can be found in the tcpdump-output folder, which was copied by me from the terminal.

Based on the available resources online I have the fear that I am one of the few who did not know about this phenomenon of NTP although it is basically common knowledge but still noone really talks about it.
As of my current understanding this problem can be mitigated on your local area network by using a custom NTP server that gets its timesource from GPS or an atomic clock.

I found the following paper about NTP attacks from Boston Unversity:

## Attacking the Network Time Protocol

Aanchal Malhotra, Isaac E. Cohen, Erik Brakke, and Sharon Goldberg
aanchal4@bu.edu, icohen93@bu.edu, ebrakke@bu.edu, goldbe@cs.bu.edu
Boston University, Boston, MA.
<https://www.cs.bu.edu/~goldbe/papers/NTPattack.pdf>

> Abstract — We explore the risk that network attackers can exploit unauthenticated Network Time Protocol (NTP) traffic to alter the time on client systems. We first discuss how an onpath attacker, that hijacks traffic to an NTP server, can quickly shift time on the server’s clients. Then, we present a extremely low-rate (single packet) denial-of-service attack that an off-path attacker, located anywhere on the network, can use to disable NTP clock synchronization on a client. Next, we show how an off-path attacker can exploit IPv4 packet fragmentation to dramatically shift time on a client. We discuss the implications on these attacks on other core Internet protocols, quantify their attack surface using Internet measurements, and suggest a few simple countermeasures that can improve the security of NTP.

I do not understand this paper completely but others might be able to prove or disprove what happens on my network is related to this.

If I understand this right and it really is an attack then I am under attacked by the servers of political parties, institutions and companies like FIDESZ, Telekom, NIIF and and a company that can be connected to a hacker group at BME and all of my network traffic could be compromised. 
I made a list of some the resolved addresses my computer is talking to:

- rave.fidesz.hu.ntp
- bakacsin1.ki.iif.hu.ntp
- ntp1.niif.hu.ntp
- zearla.netinform.hu.ntp
- yikes.bl2.tolna.net.ntp
- www.kapos-net.hu.ntp
- webmail.edupress.hu.ntp
- wdcproxy1.backup.einfra.hu.ntp
- cebece.pomaz.hu.ntp
- mail.deployis.eu.ntp
- etharin.fidesz.hu.ntp
- BC9C7207.catv.pool.telekom.hu.ntp
- trabant.uid0.hu.ntp
- zearla.netinform.hu.ntp
- ntp2.niif.hu.ntp
- blackmage.ki.iif.hu.ntp
- quaen.fradi.hu.ntp
- mailgateway.zsurob.hu.ntp
- server3.fidesz.hu.ntp
- mail.vttnonprofit.hu.ntp
- cns0.telekom.hu.ntp
- zaonce.fidesz.hu.ntp

```console
ghj@ubuntu ~> ntpq -p
     remote                                   refid      st t when poll reach   delay   offset   jitter
=======================================================================================================
 0.ubuntu.pool.ntp.org                   .POOL.          16 p    -  256    0   0.0000   0.0000   0.0001
 1.ubuntu.pool.ntp.org                   .POOL.          16 p    -  256    0   0.0000   0.0000   0.0001
 2.ubuntu.pool.ntp.org                   .POOL.          16 p    -  256    0   0.0000   0.0000   0.0001
 3.ubuntu.pool.ntp.org                   .POOL.          16 p    -  256    0   0.0000   0.0000   0.0001
 prod-ntp-4.ntp1.ps5.canonical.com       .DNS6.          16 u    -   64    0   0.0000   0.0000   0.0001
+mail.deployis.eu                        188.6.106.58     3 u   25   64  377 101.6577 -20.2640  30.8516
-bc9c7207.catv.pool.telekom.hu           131.188.3.223    2 u   27   64  377  57.0894 -46.0249  56.4250
#zaonce.fidesz.hu                        195.111.92.55    3 u   30   64  377   3.2202 -67.7638  47.6786
+mail.vttnonprofit.hu                    193.171.23.163   2 u   25   64  377   5.4753 -67.4238  68.3018
#etharin.fidesz.hu                       195.111.92.55    3 u   19   64  377   3.3290 -65.1794  35.8966
-ntp1.niif.hu                            195.111.107.114  2 u   19   64  377   2.4703 -65.3409  64.7214
-server3.fidesz.hu                       193.227.197.2    3 u   23   64  377  28.9448 -47.0169  42.9327
+webmail.edupress.hu                     193.171.23.163   2 u   13   64  377 100.2723 -20.3356  39.9664
+ntp2.niif.hu                            193.6.222.95     2 u   18   64  377   2.6479 -65.2861  56.0292
-yikes.bl2.tolna.net                     123.19.30.227    2 u   16   64  377   9.2619 -55.8392  62.3079
*eternal.datacontact.hu                  131.188.3.221    2 u   14   64  377   3.7845 -61.2249  78.6109
 ntp.bakacsin1.ki.iif.hu                 .INIT.          16 u    -   64    0   0.0000   0.0000   0.0001
 2001:ac8:26:62:0:1:0:14                 .INIT.          16 u    -   64    0   0.0000   0.0000   0.0001
 cns1.telekom.hu                         .INIT.          16 u    -   64    0   0.0000   0.0000   0.0001
 cns0.telekom.hu                         .INIT.          16 u    -   64    0   0.0000   0.0000   0.0001
-time.oneweb.hu                          150.214.94.5     2 u   18   64  377  59.5280 -40.0905  49.9811
-wdcproxy1.backup.einfra.hu              88.15.187.150    2 u   19   64  377   3.1331 -65.5704  63.6093
```

With bpftrace, I managed to get the process id of the ntp service:

```sh
sudo bpftrace -e 'kprobe:udp_recvmsg /comm == "ntpd"/ { printf("NTP packet received: pid=%d, comm=%s\n", pid, comm); }'
```

Then after plugging that id into strace, we can see the system calls of the ntpd process:

```sh
sudo strace -p 1830
```

```
strace: Process 1830 attached
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = ? ERESTARTNOHAND (To be restarted if no handler)
--- SIGALRM {si_signo=SIGALRM, si_code=SI_TIMER, si_timerid=0, si_overrun=0, si_int=0, si_ptr=NULL} ---
rt_sigreturn({mask=[HUP INT QUIT USR1 USR2 ALRM TERM]}) = -1 EINTR (Interrupted system call)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
clock_adjtime(CLOCK_REALTIME, {modes=ADJ_OFFSET_SINGLESHOT, offset=0, freq=202763, maxerror=16000000, esterror=16000000, status=STA_PLL|STA_UNSYNC, constant=4, precision=1, tolerance=32768000, time={tv_sec=1745006647, tv_usec=907079}, tick=10000, ppsfreq=0, jitter=0, shift=0, stabil=0, jitcnt=0, calcnt=0, errcnt=0, stbcnt=0, tai=0}) = 5 (TIME_ERROR)
getpid()                                = 1830
sendto(21, "#\0\0 \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 48, 0, {sa_family=AF_INET, sin_port=htons(123), sin_addr=inet_addr("62.112.195.56")}, 16) = 48
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = 1 (in [21])
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
recvmsg(21, {msg_name={sa_family=AF_INET, sin_port=htons(123), sin_addr=inet_addr("62.112.195.56")}, msg_namelen=28 => 16, msg_iov=[{iov_base="$\3\0\351\0\0\0p\0\0\7\fQ\0t\r\353\255+\340\23\302\310\356\364\227\3208C\178\271"..., iov_len=4168}], msg_iovlen=1, msg_control=[{cmsg_len=32, cmsg_level=SOL_SOCKET, cmsg_type=SO_TIMESTAMPNS_OLD, cmsg_data={tv_sec=1745006648, tv_nsec=87420846}}], msg_controllen=32, msg_flags=0}, 0) = 48
write(4, "60783 72248.089 62.112.195.56 30"..., 83) = 83
recvmsg(21, {msg_namelen=28}, 0)        = -1 EAGAIN (Resource temporarily unavailable)
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = ? ERESTARTNOHAND (To be restarted if no handler)
--- SIGALRM {si_signo=SIGALRM, si_code=SI_TIMER, si_timerid=0, si_overrun=0, si_int=0, si_ptr=NULL} ---
rt_sigreturn({mask=[HUP INT QUIT USR1 USR2 ALRM TERM]}) = -1 EINTR (Interrupted system call)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
clock_adjtime(CLOCK_REALTIME, {modes=ADJ_OFFSET_SINGLESHOT, offset=0, freq=202763, maxerror=16000000, esterror=16000000, status=STA_PLL|STA_UNSYNC, constant=4, precision=1, tolerance=32768000, time={tv_sec=1745006648, tv_usec=907089}, tick=10000, ppsfreq=0, jitter=0, shift=0, stabil=0, jitcnt=0, calcnt=0, errcnt=0, stbcnt=0, tai=0}) = 5 (TIME_ERROR)
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = 1 (in [20])
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
read(20, "@\0\0\0\20\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\3\0\0\0C\20\1\0\0\0\0\0"..., 5120) = 64
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = ? ERESTARTNOHAND (To be restarted if no handler)
--- SIGALRM {si_signo=SIGALRM, si_code=SI_TIMER, si_timerid=0, si_overrun=0, si_int=0, si_ptr=NULL} ---
rt_sigreturn({mask=[HUP INT QUIT USR1 USR2 ALRM TERM]}) = -1 EINTR (Interrupted system call)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
clock_adjtime(CLOCK_REALTIME, {modes=ADJ_OFFSET_SINGLESHOT, offset=0, freq=202763, maxerror=16000000, esterror=16000000, status=STA_PLL|STA_UNSYNC, constant=4, precision=1, tolerance=32768000, time={tv_sec=1745006649, tv_usec=907052}, tick=10000, ppsfreq=0, jitter=0, shift=0, stabil=0, jitcnt=0, calcnt=0, errcnt=0, stbcnt=0, tai=0}) = 5 (TIME_ERROR)
getpid()                                = 1830
sendto(21, "#\0\0 \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 48, 0, {sa_family=AF_INET, sin_port=htons(123), sin_addr=inet_addr("62.112.195.56")}, 16) = 48
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = 1 (in [21])
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
recvmsg(21, {msg_name={sa_family=AF_INET, sin_port=htons(123), sin_addr=inet_addr("62.112.195.56")}, msg_namelen=28 => 16, msg_iov=[{iov_base="$\3\0\351\0\0\0p\0\0\7\16Q\0t\r\353\255+\340\23\302\310\356}\233T\34\271\213\204]"..., iov_len=4168}], msg_iovlen=1, msg_control=[{cmsg_len=32, cmsg_level=SOL_SOCKET, cmsg_type=SO_TIMESTAMPNS_OLD, cmsg_data={tv_sec=1745006650, tv_nsec=10810941}}], msg_controllen=32, msg_flags=0}, 0) = 48
write(4, "60783 72250.011 62.112.195.56 30"..., 83) = 83
recvmsg(21, {msg_namelen=28}, 0)        = -1 EAGAIN (Resource temporarily unavailable)
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}) = ? ERESTARTNOHAND (To be restarted if no handler)
--- SIGALRM {si_signo=SIGALRM, si_code=SI_TIMER, si_timerid=0, si_overrun=0, si_int=0, si_ptr=NULL} ---
rt_sigreturn({mask=[HUP INT QUIT USR1 USR2 ALRM TERM]}) = -1 EINTR (Interrupted system call)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
clock_adjtime(CLOCK_REALTIME, {modes=ADJ_OFFSET_SINGLESHOT, offset=0, freq=202763, maxerror=16000000, esterror=16000000, status=STA_PLL|STA_UNSYNC, constant=4, precision=1, tolerance=32768000, time={tv_sec=1745006650, tv_usec=907213}, tick=10000, ppsfreq=0, jitter=0, shift=0, stabil=0, jitcnt=0, calcnt=0, errcnt=0, stbcnt=0, tai=0}) = 5 (TIME_ERROR)
socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_ROUTE) = 6
bind(6, {sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, 12) = 0
getsockname(6, {sa_family=AF_NETLINK, nl_pid=-304762728, nl_groups=00000000}, [12]) = 0
sendto(6, [{nlmsg_len=20, nlmsg_type=RTM_GETLINK, nlmsg_flags=NLM_F_REQUEST|NLM_F_DUMP, nlmsg_seq=1745006650, nlmsg_pid=0}, {ifi_family=AF_UNSPEC, ...}], 20, 0, {sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, 12) = 20
recvmsg(6, {msg_name={sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, msg_namelen=12, msg_iov=[{iov_base=[[{nlmsg_len=1444, nlmsg_type=RTM_NEWLINK, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006650, nlmsg_pid=-304762728}, {ifi_family=AF_UNSPEC, ifi_type=ARPHRD_LOOPBACK, ifi_index=if_nametoindex("lo"), ifi_flags=IFF_UP|IFF_LOOPBACK|IFF_RUNNING|IFF_LOWER_UP, ifi_change=0}, [[{nla_len=7, nla_type=IFLA_IFNAME}, "lo"], [{nla_len=8, nla_type=IFLA_TXQLEN}, 1000], [{nla_len=5, nla_type=IFLA_OPERSTATE}, 0], [{nla_len=5, nla_type=IFLA_LINKMODE}, 0], [{nla_len=8, nla_type=IFLA_MTU}, 65536], [{nla_len=8, nla_type=IFLA_MIN_MTU}, 0], [{nla_len=8, nla_type=IFLA_MAX_MTU}, 0], [{nla_len=8, nla_type=IFLA_GROUP}, 0], [{nla_len=8, nla_type=IFLA_PROMISCUITY}, 0], [{nla_len=8, nla_type=IFLA_ALLMULTI}, 0], [{nla_len=8, nla_type=IFLA_NUM_TX_QUEUES}, 1], [{nla_len=8, nla_type=IFLA_GSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_GSO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GSO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SIZE}, 524280], [{nla_len=8, nla_type=IFLA_TSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_NUM_RX_QUEUES}, 1], [{nla_len=5, nla_type=IFLA_CARRIER}, 1], [{nla_len=8, nla_type=IFLA_CARRIER_CHANGES}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_UP_COUNT}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_DOWN_COUNT}, 0], [{nla_len=5, nla_type=IFLA_PROTO_DOWN}, 0], [{nla_len=10, nla_type=IFLA_ADDRESS}, 00:00:00:00:00:00], [{nla_len=10, nla_type=IFLA_BROADCAST}, 00:00:00:00:00:00], [{nla_len=204, nla_type=IFLA_STATS64}, {rx_packets=492, tx_packets=492, rx_bytes=46351, tx_bytes=46351, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=0, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0, rx_otherhost_dropped=0}], [{nla_len=100, nla_type=IFLA_STATS}, {rx_packets=492, tx_packets=492, rx_bytes=46351, tx_bytes=46351, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=0, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0}], [{nla_len=12, nla_type=IFLA_XDP}, [{nla_len=5, nla_type=IFLA_XDP_ATTACHED}, XDP_ATTACHED_NONE]], [{nla_len=12, nla_type=IFLA_QDISC}, "noqueue"], [{nla_len=824, nla_type=IFLA_AF_SPEC}, [[{nla_len=12, nla_type=AF_MCTP}, [{nla_len=8, nla_type=IFLA_MCTP_NET}, 1]], [{nla_len=140, nla_type=AF_INET}, [{nla_len=136, nla_type=IFLA_INET_CONF}, [[IPV4_DEVCONF_FORWARDING-1]=1, [IPV4_DEVCONF_MC_FORWARDING-1]=0, [IPV4_DEVCONF_PROXY_ARP-1]=0, [IPV4_DEVCONF_ACCEPT_REDIRECTS-1]=1, [IPV4_DEVCONF_SECURE_REDIRECTS-1]=1, [IPV4_DEVCONF_SEND_REDIRECTS-1]=1, [IPV4_DEVCONF_SHARED_MEDIA-1]=1, [IPV4_DEVCONF_RP_FILTER-1]=0, [IPV4_DEVCONF_ACCEPT_SOURCE_ROUTE-1]=1, [IPV4_DEVCONF_BOOTP_RELAY-1]=0, [IPV4_DEVCONF_LOG_MARTIANS-1]=0, [IPV4_DEVCONF_TAG-1]=0, [IPV4_DEVCONF_ARPFILTER-1]=0, [IPV4_DEVCONF_MEDIUM_ID-1]=0, [IPV4_DEVCONF_NOXFRM-1]=1, [IPV4_DEVCONF_NOPOLICY-1]=1, [IPV4_DEVCONF_FORCE_IGMP_VERSION-1]=0, [IPV4_DEVCONF_ARP_ANNOUNCE-1]=0, [IPV4_DEVCONF_ARP_IGNORE-1]=0, [IPV4_DEVCONF_PROMOTE_SECONDARIES-1]=0, [IPV4_DEVCONF_ARP_ACCEPT-1]=0, [IPV4_DEVCONF_ARP_NOTIFY-1]=0, [IPV4_DEVCONF_ACCEPT_LOCAL-1]=0, [IPV4_DEVCONF_SRC_VMARK-1]=0, [IPV4_DEVCONF_PROXY_ARP_PVLAN-1]=0, [IPV4_DEVCONF_ROUTE_LOCALNET-1]=0, [IPV4_DEVCONF_IGMPV2_UNSOLICITED_REPORT_INTERVAL-1]=10000, [IPV4_DEVCONF_IGMPV3_UNSOLICITED_REPORT_INTERVAL-1]=1000, [IPV4_DEVCONF_IGNORE_ROUTES_WITH_LINKDOWN-1]=0, [IPV4_DEVCONF_DROP_UNICAST_IN_L2_MULTICAST-1]=0, [IPV4_DEVCONF_DROP_GRATUITOUS_ARP-1]=0, [IPV4_DEVCONF_BC_FORWARDING-1]=0, ...]]], [{nla_len=668, nla_type=AF_INET6}, [[{nla_len=8, nla_type=IFLA_INET6_FLAGS}, IF_READY], [{nla_len=20, nla_type=IFLA_INET6_CACHEINFO}, {max_reasm_len=65535, tstamp=241, reachable_time=30874, retrans_time=1000}], [{nla_len=240, nla_type=IFLA_INET6_CONF}, [[DEVCONF_FORWARDING]=0, [DEVCONF_HOPLIMIT]=64, [DEVCONF_MTU6]=65536, [DEVCONF_ACCEPT_RA]=1, [DEVCONF_ACCEPT_REDIRECTS]=1, [DEVCONF_AUTOCONF]=1, [DEVCONF_DAD_TRANSMITS]=1, [DEVCONF_RTR_SOLICITS]=-1, [DEVCONF_RTR_SOLICIT_INTERVAL]=4000, [DEVCONF_RTR_SOLICIT_DELAY]=1000, [DEVCONF_USE_TEMPADDR]=-1, [DEVCONF_TEMP_VALID_LFT]=604800, [DEVCONF_TEMP_PREFERED_LFT]=86400, [DEVCONF_REGEN_MAX_RETRY]=3, [DEVCONF_MAX_DESYNC_FACTOR]=600, [DEVCONF_MAX_ADDRESSES]=16, [DEVCONF_FORCE_MLD_VERSION]=0, [DEVCONF_ACCEPT_RA_DEFRTR]=1, [DEVCONF_ACCEPT_RA_PINFO]=1, [DEVCONF_ACCEPT_RA_RTR_PREF]=1, [DEVCONF_RTR_PROBE_INTERVAL]=60000, [DEVCONF_ACCEPT_RA_RT_INFO_MAX_PLEN]=0, [DEVCONF_PROXY_NDP]=0, [DEVCONF_OPTIMISTIC_DAD]=0, [DEVCONF_ACCEPT_SOURCE_ROUTE]=0, [DEVCONF_MC_FORWARDING]=0, [DEVCONF_DISABLE_IPV6]=0, [DEVCONF_ACCEPT_DAD]=-1, [DEVCONF_FORCE_TLLAO]=0, [DEVCONF_NDISC_NOTIFY]=0, [DEVCONF_MLDV1_UNSOLICITED_REPORT_INTERVAL]=10000, [DEVCONF_MLDV2_UNSOLICITED_REPORT_INTERVAL]=1000, ...]], [{nla_len=308, nla_type=IFLA_INET6_STATS}, [[IPSTATS_MIB_NUM]=38, [IPSTATS_MIB_INPKTS]=4, [IPSTATS_MIB_INOCTETS]=292, [IPSTATS_MIB_INDELIVERS]=4, [IPSTATS_MIB_OUTFORWDATAGRAMS]=0, [IPSTATS_MIB_OUTPKTS]=4, [IPSTATS_MIB_OUTOCTETS]=292, [IPSTATS_MIB_INHDRERRORS]=0, [IPSTATS_MIB_INTOOBIGERRORS]=0, [IPSTATS_MIB_INNOROUTES]=0, [IPSTATS_MIB_INADDRERRORS]=0, [IPSTATS_MIB_INUNKNOWNPROTOS]=0, [IPSTATS_MIB_INTRUNCATEDPKTS]=0, [IPSTATS_MIB_INDISCARDS]=0, [IPSTATS_MIB_OUTDISCARDS]=0, [IPSTATS_MIB_OUTNOROUTES]=0, [IPSTATS_MIB_REASMTIMEOUT]=0, [IPSTATS_MIB_REASMREQDS]=0, [IPSTATS_MIB_REASMOKS]=0, [IPSTATS_MIB_REASMFAILS]=0, [IPSTATS_MIB_FRAGOKS]=0, [IPSTATS_MIB_FRAGFAILS]=0, [IPSTATS_MIB_FRAGCREATES]=0, [IPSTATS_MIB_INMCASTPKTS]=0, [IPSTATS_MIB_OUTMCASTPKTS]=2, [IPSTATS_MIB_INBCASTPKTS]=0, [IPSTATS_MIB_OUTBCASTPKTS]=0, [IPSTATS_MIB_INMCASTOCTETS]=0, [IPSTATS_MIB_OUTMCASTOCTETS]=152, [IPSTATS_MIB_INBCASTOCTETS]=0, [IPSTATS_MIB_OUTBCASTOCTETS]=0, [IPSTATS_MIB_CSUMERRORS]=0, ...]], [{nla_len=60, nla_type=IFLA_INET6_ICMP6STATS}, [[ICMP6_MIB_NUM]=7, [ICMP6_MIB_INMSGS]=2, [ICMP6_MIB_INERRORS]=0, [ICMP6_MIB_OUTMSGS]=2, [ICMP6_MIB_OUTERRORS]=0, [ICMP6_MIB_CSUMERRORS]=0, [6 /* ICMP6_MIB_??? */]=0]], [{nla_len=20, nla_type=IFLA_INET6_TOKEN}, inet_pton(AF_INET6, "::")], [{nla_len=5, nla_type=IFLA_INET6_ADDR_GEN_MODE}, IN6_ADDR_GEN_MODE_EUI64]]]]], [{nla_len=36, nla_type=IFLA_MAP}, {mem_start=0, mem_end=0, base_addr=0, irq=0, dma=0, port=0}], ...]], [{nlmsg_len=1480, nlmsg_type=RTM_NEWLINK, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006650, nlmsg_pid=-304762728}, {ifi_family=AF_UNSPEC, ifi_type=ARPHRD_ETHER, ifi_index=if_nametoindex("enp2s0"), ifi_flags=IFF_UP|IFF_BROADCAST|IFF_MULTICAST, ifi_change=0}, [[{nla_len=11, nla_type=IFLA_IFNAME}, "enp2s0"], [{nla_len=8, nla_type=IFLA_TXQLEN}, 1000], [{nla_len=5, nla_type=IFLA_OPERSTATE}, 2], [{nla_len=5, nla_type=IFLA_LINKMODE}, 0], [{nla_len=8, nla_type=IFLA_MTU}, 1500], [{nla_len=8, nla_type=IFLA_MIN_MTU}, 68], [{nla_len=8, nla_type=IFLA_MAX_MTU}, 9194], [{nla_len=8, nla_type=IFLA_GROUP}, 0], [{nla_len=8, nla_type=IFLA_PROMISCUITY}, 0], [{nla_len=8, nla_type=IFLA_ALLMULTI}, 0], [{nla_len=8, nla_type=IFLA_NUM_TX_QUEUES}, 1], [{nla_len=8, nla_type=IFLA_GSO_MAX_SEGS}, 64], [{nla_len=8, nla_type=IFLA_GSO_MAX_SIZE}, 64000], [{nla_len=8, nla_type=IFLA_GRO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GSO_IPV4_MAX_SIZE}, 64000], [{nla_len=8, nla_type=IFLA_GRO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SIZE}, 64000], [{nla_len=8, nla_type=IFLA_TSO_MAX_SEGS}, 64], [{nla_len=8, nla_type=IFLA_NUM_RX_QUEUES}, 1], [{nla_len=5, nla_type=IFLA_CARRIER}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_CHANGES}, 1], [{nla_len=8, nla_type=IFLA_CARRIER_UP_COUNT}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_DOWN_COUNT}, 1], [{nla_len=5, nla_type=IFLA_PROTO_DOWN}, 0], [{nla_len=10, nla_type=IFLA_ADDRESS}, fc:5c:ee:d2:7a:fb], [{nla_len=10, nla_type=IFLA_BROADCAST}, ff:ff:ff:ff:ff:ff], [{nla_len=204, nla_type=IFLA_STATS64}, {rx_packets=0, tx_packets=0, rx_bytes=0, tx_bytes=0, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=0, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0, rx_otherhost_dropped=0}], [{nla_len=100, nla_type=IFLA_STATS}, {rx_packets=0, tx_packets=0, rx_bytes=0, tx_bytes=0, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=0, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0}], [{nla_len=12, nla_type=IFLA_XDP}, [{nla_len=5, nla_type=IFLA_XDP_ATTACHED}, XDP_ATTACHED_NONE]], [{nla_len=10, nla_type=IFLA_PERM_ADDRESS}, fc:5c:ee:d2:7a:fb], [{nla_len=13, nla_type=IFLA_QDISC}, "fq_codel"], [{nla_len=812, nla_type=IFLA_AF_SPEC}, [[{nla_len=140, nla_type=AF_INET}, [{nla_len=136, nla_type=IFLA_INET_CONF}, [[IPV4_DEVCONF_FORWARDING-1]=1, [IPV4_DEVCONF_MC_FORWARDING-1]=0, [IPV4_DEVCONF_PROXY_ARP-1]=0, [IPV4_DEVCONF_ACCEPT_REDIRECTS-1]=1, [IPV4_DEVCONF_SECURE_REDIRECTS-1]=1, [IPV4_DEVCONF_SEND_REDIRECTS-1]=1, [IPV4_DEVCONF_SHARED_MEDIA-1]=1, [IPV4_DEVCONF_RP_FILTER-1]=2, [IPV4_DEVCONF_ACCEPT_SOURCE_ROUTE-1]=1, [IPV4_DEVCONF_BOOTP_RELAY-1]=0, [IPV4_DEVCONF_LOG_MARTIANS-1]=0, [IPV4_DEVCONF_TAG-1]=0, [IPV4_DEVCONF_ARPFILTER-1]=0, [IPV4_DEVCONF_MEDIUM_ID-1]=0, [IPV4_DEVCONF_NOXFRM-1]=0, [IPV4_DEVCONF_NOPOLICY-1]=0, [IPV4_DEVCONF_FORCE_IGMP_VERSION-1]=0, [IPV4_DEVCONF_ARP_ANNOUNCE-1]=0, [IPV4_DEVCONF_ARP_IGNORE-1]=0, [IPV4_DEVCONF_PROMOTE_SECONDARIES-1]=0, [IPV4_DEVCONF_ARP_ACCEPT-1]=0, [IPV4_DEVCONF_ARP_NOTIFY-1]=0, [IPV4_DEVCONF_ACCEPT_LOCAL-1]=0, [IPV4_DEVCONF_SRC_VMARK-1]=0, [IPV4_DEVCONF_PROXY_ARP_PVLAN-1]=0, [IPV4_DEVCONF_ROUTE_LOCALNET-1]=0, [IPV4_DEVCONF_IGMPV2_UNSOLICITED_REPORT_INTERVAL-1]=10000, [IPV4_DEVCONF_IGMPV3_UNSOLICITED_REPORT_INTERVAL-1]=1000, [IPV4_DEVCONF_IGNORE_ROUTES_WITH_LINKDOWN-1]=0, [IPV4_DEVCONF_DROP_UNICAST_IN_L2_MULTICAST-1]=0, [IPV4_DEVCONF_DROP_GRATUITOUS_ARP-1]=0, [IPV4_DEVCONF_BC_FORWARDING-1]=0, ...]]], [{nla_len=668, nla_type=AF_INET6}, [[{nla_len=8, nla_type=IFLA_INET6_FLAGS}, 0], [{nla_len=20, nla_type=IFLA_INET6_CACHEINFO}, {max_reasm_len=65535, tstamp=671, reachable_time=43003, retrans_time=1000}], [{nla_len=240, nla_type=IFLA_INET6_CONF}, [[DEVCONF_FORWARDING]=0, [DEVCONF_HOPLIMIT]=64, [DEVCONF_MTU6]=1500, [DEVCONF_ACCEPT_RA]=0, [DEVCONF_ACCEPT_REDIRECTS]=1, [DEVCONF_AUTOCONF]=1, [DEVCONF_DAD_TRANSMITS]=1, [DEVCONF_RTR_SOLICITS]=-1, [DEVCONF_RTR_SOLICIT_INTERVAL]=4000, [DEVCONF_RTR_SOLICIT_DELAY]=1000, [DEVCONF_USE_TEMPADDR]=0, [DEVCONF_TEMP_VALID_LFT]=604800, [DEVCONF_TEMP_PREFERED_LFT]=86400, [DEVCONF_REGEN_MAX_RETRY]=3, [DEVCONF_MAX_DESYNC_FACTOR]=600, [DEVCONF_MAX_ADDRESSES]=16, [DEVCONF_FORCE_MLD_VERSION]=0, [DEVCONF_ACCEPT_RA_DEFRTR]=1, [DEVCONF_ACCEPT_RA_PINFO]=1, [DEVCONF_ACCEPT_RA_RTR_PREF]=1, [DEVCONF_RTR_PROBE_INTERVAL]=60000, [DEVCONF_ACCEPT_RA_RT_INFO_MAX_PLEN]=0, [DEVCONF_PROXY_NDP]=0, [DEVCONF_OPTIMISTIC_DAD]=0, [DEVCONF_ACCEPT_SOURCE_ROUTE]=0, [DEVCONF_MC_FORWARDING]=0, [DEVCONF_DISABLE_IPV6]=0, [DEVCONF_ACCEPT_DAD]=1, [DEVCONF_FORCE_TLLAO]=0, [DEVCONF_NDISC_NOTIFY]=0, [DEVCONF_MLDV1_UNSOLICITED_REPORT_INTERVAL]=10000, [DEVCONF_MLDV2_UNSOLICITED_REPORT_INTERVAL]=1000, ...]], [{nla_len=308, nla_type=IFLA_INET6_STATS}, [[IPSTATS_MIB_NUM]=38, [IPSTATS_MIB_INPKTS]=0, [IPSTATS_MIB_INOCTETS]=0, [IPSTATS_MIB_INDELIVERS]=0, [IPSTATS_MIB_OUTFORWDATAGRAMS]=0, [IPSTATS_MIB_OUTPKTS]=0, [IPSTATS_MIB_OUTOCTETS]=0, [IPSTATS_MIB_INHDRERRORS]=0, [IPSTATS_MIB_INTOOBIGERRORS]=0, [IPSTATS_MIB_INNOROUTES]=0, [IPSTATS_MIB_INADDRERRORS]=0, [IPSTATS_MIB_INUNKNOWNPROTOS]=0, [IPSTATS_MIB_INTRUNCATEDPKTS]=0, [IPSTATS_MIB_INDISCARDS]=0, [IPSTATS_MIB_OUTDISCARDS]=0, [IPSTATS_MIB_OUTNOROUTES]=0, [IPSTATS_MIB_REASMTIMEOUT]=0, [IPSTATS_MIB_REASMREQDS]=0, [IPSTATS_MIB_REASMOKS]=0, [IPSTATS_MIB_REASMFAILS]=0, [IPSTATS_MIB_FRAGOKS]=0, [IPSTATS_MIB_FRAGFAILS]=0, [IPSTATS_MIB_FRAGCREATES]=0, [IPSTATS_MIB_INMCASTPKTS]=0, [IPSTATS_MIB_OUTMCASTPKTS]=0, [IPSTATS_MIB_INBCASTPKTS]=0, [IPSTATS_MIB_OUTBCASTPKTS]=0, [IPSTATS_MIB_INMCASTOCTETS]=0, [IPSTATS_MIB_OUTMCASTOCTETS]=0, [IPSTATS_MIB_INBCASTOCTETS]=0, [IPSTATS_MIB_OUTBCASTOCTETS]=0, [IPSTATS_MIB_CSUMERRORS]=0, ...]], [{nla_len=60, nla_type=IFLA_INET6_ICMP6STATS}, [[ICMP6_MIB_NUM]=7, [ICMP6_MIB_INMSGS]=0, [ICMP6_MIB_INERRORS]=0, [ICMP6_MIB_OUTMSGS]=0, [ICMP6_MIB_OUTERRORS]=0, [ICMP6_MIB_CSUMERRORS]=0, [6 /* ICMP6_MIB_??? */]=0]], [{nla_len=20, nla_type=IFLA_INET6_TOKEN}, inet_pton(AF_INET6, "::")], [{nla_len=5, nla_type=IFLA_INET6_ADDR_GEN_MODE}, IN6_ADDR_GEN_MODE_NONE]]]]], ...]]], iov_len=8192}], msg_iovlen=1, msg_controllen=0, msg_flags=0}, 0) = 2924
recvmsg(6, {msg_name={sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, msg_namelen=12, msg_iov=[{iov_base=[[{nlmsg_len=1476, nlmsg_type=RTM_NEWLINK, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006650, nlmsg_pid=-304762728}, {ifi_family=AF_UNSPEC, ifi_type=ARPHRD_ETHER, ifi_index=if_nametoindex("wlp3s0"), ifi_flags=IFF_UP|IFF_BROADCAST|IFF_RUNNING|IFF_MULTICAST|IFF_LOWER_UP, ifi_change=0}, [[{nla_len=11, nla_type=IFLA_IFNAME}, "wlp3s0"], [{nla_len=8, nla_type=IFLA_TXQLEN}, 1000], [{nla_len=5, nla_type=IFLA_OPERSTATE}, 6], [{nla_len=5, nla_type=IFLA_LINKMODE}, 1], [{nla_len=8, nla_type=IFLA_MTU}, 1500], [{nla_len=8, nla_type=IFLA_MIN_MTU}, 256], [{nla_len=8, nla_type=IFLA_MAX_MTU}, 2304], [{nla_len=8, nla_type=IFLA_GROUP}, 0], [{nla_len=8, nla_type=IFLA_PROMISCUITY}, 0], [{nla_len=8, nla_type=IFLA_ALLMULTI}, 0], [{nla_len=8, nla_type=IFLA_NUM_TX_QUEUES}, 1], [{nla_len=8, nla_type=IFLA_GSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_GSO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GSO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_NUM_RX_QUEUES}, 1], [{nla_len=5, nla_type=IFLA_CARRIER}, 1], [{nla_len=8, nla_type=IFLA_CARRIER_CHANGES}, 6], [{nla_len=8, nla_type=IFLA_CARRIER_UP_COUNT}, 3], [{nla_len=8, nla_type=IFLA_CARRIER_DOWN_COUNT}, 3], [{nla_len=5, nla_type=IFLA_PROTO_DOWN}, 0], [{nla_len=10, nla_type=IFLA_ADDRESS}, b8:1e:a4:c7:38:9d], [{nla_len=10, nla_type=IFLA_BROADCAST}, ff:ff:ff:ff:ff:ff], [{nla_len=204, nla_type=IFLA_STATS64}, {rx_packets=23655, tx_packets=3227, rx_bytes=34219601, tx_bytes=396077, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=2, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0, rx_otherhost_dropped=0}], [{nla_len=100, nla_type=IFLA_STATS}, {rx_packets=23655, tx_packets=3227, rx_bytes=34219601, tx_bytes=396077, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=2, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0}], [{nla_len=12, nla_type=IFLA_XDP}, [{nla_len=5, nla_type=IFLA_XDP_ATTACHED}, XDP_ATTACHED_NONE]], [{nla_len=10, nla_type=IFLA_PERM_ADDRESS}, b8:1e:a4:c7:38:9d], [{nla_len=12, nla_type=IFLA_QDISC}, "noqueue"], [{nla_len=812, nla_type=IFLA_AF_SPEC}, [[{nla_len=140, nla_type=AF_INET}, [{nla_len=136, nla_type=IFLA_INET_CONF}, [[IPV4_DEVCONF_FORWARDING-1]=1, [IPV4_DEVCONF_MC_FORWARDING-1]=0, [IPV4_DEVCONF_PROXY_ARP-1]=0, [IPV4_DEVCONF_ACCEPT_REDIRECTS-1]=1, [IPV4_DEVCONF_SECURE_REDIRECTS-1]=1, [IPV4_DEVCONF_SEND_REDIRECTS-1]=1, [IPV4_DEVCONF_SHARED_MEDIA-1]=1, [IPV4_DEVCONF_RP_FILTER-1]=2, [IPV4_DEVCONF_ACCEPT_SOURCE_ROUTE-1]=1, [IPV4_DEVCONF_BOOTP_RELAY-1]=0, [IPV4_DEVCONF_LOG_MARTIANS-1]=0, [IPV4_DEVCONF_TAG-1]=0, [IPV4_DEVCONF_ARPFILTER-1]=0, [IPV4_DEVCONF_MEDIUM_ID-1]=0, [IPV4_DEVCONF_NOXFRM-1]=0, [IPV4_DEVCONF_NOPOLICY-1]=0, [IPV4_DEVCONF_FORCE_IGMP_VERSION-1]=0, [IPV4_DEVCONF_ARP_ANNOUNCE-1]=0, [IPV4_DEVCONF_ARP_IGNORE-1]=0, [IPV4_DEVCONF_PROMOTE_SECONDARIES-1]=0, [IPV4_DEVCONF_ARP_ACCEPT-1]=0, [IPV4_DEVCONF_ARP_NOTIFY-1]=0, [IPV4_DEVCONF_ACCEPT_LOCAL-1]=0, [IPV4_DEVCONF_SRC_VMARK-1]=0, [IPV4_DEVCONF_PROXY_ARP_PVLAN-1]=0, [IPV4_DEVCONF_ROUTE_LOCALNET-1]=0, [IPV4_DEVCONF_IGMPV2_UNSOLICITED_REPORT_INTERVAL-1]=10000, [IPV4_DEVCONF_IGMPV3_UNSOLICITED_REPORT_INTERVAL-1]=1000, [IPV4_DEVCONF_IGNORE_ROUTES_WITH_LINKDOWN-1]=0, [IPV4_DEVCONF_DROP_UNICAST_IN_L2_MULTICAST-1]=0, [IPV4_DEVCONF_DROP_GRATUITOUS_ARP-1]=0, [IPV4_DEVCONF_BC_FORWARDING-1]=0, ...]]], [{nla_len=668, nla_type=AF_INET6}, [[{nla_len=8, nla_type=IFLA_INET6_FLAGS}, IF_READY], [{nla_len=20, nla_type=IFLA_INET6_CACHEINFO}, {max_reasm_len=65535, tstamp=6336, reachable_time=23348, retrans_time=1000}], [{nla_len=240, nla_type=IFLA_INET6_CONF}, [[DEVCONF_FORWARDING]=0, [DEVCONF_HOPLIMIT]=64, [DEVCONF_MTU6]=1500, [DEVCONF_ACCEPT_RA]=0, [DEVCONF_ACCEPT_REDIRECTS]=1, [DEVCONF_AUTOCONF]=1, [DEVCONF_DAD_TRANSMITS]=1, [DEVCONF_RTR_SOLICITS]=-1, [DEVCONF_RTR_SOLICIT_INTERVAL]=4000, [DEVCONF_RTR_SOLICIT_DELAY]=1000, [DEVCONF_USE_TEMPADDR]=2, [DEVCONF_TEMP_VALID_LFT]=604800, [DEVCONF_TEMP_PREFERED_LFT]=86400, [DEVCONF_REGEN_MAX_RETRY]=3, [DEVCONF_MAX_DESYNC_FACTOR]=600, [DEVCONF_MAX_ADDRESSES]=16, [DEVCONF_FORCE_MLD_VERSION]=0, [DEVCONF_ACCEPT_RA_DEFRTR]=1, [DEVCONF_ACCEPT_RA_PINFO]=1, [DEVCONF_ACCEPT_RA_RTR_PREF]=1, [DEVCONF_RTR_PROBE_INTERVAL]=60000, [DEVCONF_ACCEPT_RA_RT_INFO_MAX_PLEN]=0, [DEVCONF_PROXY_NDP]=0, [DEVCONF_OPTIMISTIC_DAD]=0, [DEVCONF_ACCEPT_SOURCE_ROUTE]=0, [DEVCONF_MC_FORWARDING]=0, [DEVCONF_DISABLE_IPV6]=0, [DEVCONF_ACCEPT_DAD]=1, [DEVCONF_FORCE_TLLAO]=0, [DEVCONF_NDISC_NOTIFY]=0, [DEVCONF_MLDV1_UNSOLICITED_REPORT_INTERVAL]=10000, [DEVCONF_MLDV2_UNSOLICITED_REPORT_INTERVAL]=1000, ...]], [{nla_len=308, nla_type=IFLA_INET6_STATS}, [[IPSTATS_MIB_NUM]=38, [IPSTATS_MIB_INPKTS]=10, [IPSTATS_MIB_INOCTETS]=2274, [IPSTATS_MIB_INDELIVERS]=7, [IPSTATS_MIB_OUTFORWDATAGRAMS]=0, [IPSTATS_MIB_OUTPKTS]=17, [IPSTATS_MIB_OUTOCTETS]=1550, [IPSTATS_MIB_INHDRERRORS]=0, [IPSTATS_MIB_INTOOBIGERRORS]=0, [IPSTATS_MIB_INNOROUTES]=0, [IPSTATS_MIB_INADDRERRORS]=0, [IPSTATS_MIB_INUNKNOWNPROTOS]=0, [IPSTATS_MIB_INTRUNCATEDPKTS]=0, [IPSTATS_MIB_INDISCARDS]=3, [IPSTATS_MIB_OUTDISCARDS]=0, [IPSTATS_MIB_OUTNOROUTES]=0, [IPSTATS_MIB_REASMTIMEOUT]=0, [IPSTATS_MIB_REASMREQDS]=0, [IPSTATS_MIB_REASMOKS]=0, [IPSTATS_MIB_REASMFAILS]=0, [IPSTATS_MIB_FRAGOKS]=0, [IPSTATS_MIB_FRAGFAILS]=0, [IPSTATS_MIB_FRAGCREATES]=0, [IPSTATS_MIB_INMCASTPKTS]=7, [IPSTATS_MIB_OUTMCASTPKTS]=17, [IPSTATS_MIB_INBCASTPKTS]=0, [IPSTATS_MIB_OUTBCASTPKTS]=0, [IPSTATS_MIB_INMCASTOCTETS]=854, [IPSTATS_MIB_OUTMCASTOCTETS]=1550, [IPSTATS_MIB_INBCASTOCTETS]=0, [IPSTATS_MIB_OUTBCASTOCTETS]=0, [IPSTATS_MIB_CSUMERRORS]=0, ...]], [{nla_len=60, nla_type=IFLA_INET6_ICMP6STATS}, [[ICMP6_MIB_NUM]=7, [ICMP6_MIB_INMSGS]=0, [ICMP6_MIB_INERRORS]=0, [ICMP6_MIB_OUTMSGS]=10, [ICMP6_MIB_OUTERRORS]=0, [ICMP6_MIB_CSUMERRORS]=0, [6 /* ICMP6_MIB_??? */]=0]], [{nla_len=20, nla_type=IFLA_INET6_TOKEN}, inet_pton(AF_INET6, "::")], [{nla_len=5, nla_type=IFLA_INET6_ADDR_GEN_MODE}, IN6_ADDR_GEN_MODE_NONE]]]]], ...]], [{nlmsg_len=1880, nlmsg_type=RTM_NEWLINK, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006650, nlmsg_pid=-304762728}, {ifi_family=AF_UNSPEC, ifi_type=ARPHRD_ETHER, ifi_index=if_nametoindex("docker0"), ifi_flags=IFF_UP|IFF_BROADCAST|IFF_MULTICAST, ifi_change=0}, [[{nla_len=12, nla_type=IFLA_IFNAME}, "docker0"], [{nla_len=8, nla_type=IFLA_TXQLEN}, 0], [{nla_len=5, nla_type=IFLA_OPERSTATE}, 2], [{nla_len=5, nla_type=IFLA_LINKMODE}, 0], [{nla_len=8, nla_type=IFLA_MTU}, 1500], [{nla_len=8, nla_type=IFLA_MIN_MTU}, 68], [{nla_len=8, nla_type=IFLA_MAX_MTU}, 65535], [{nla_len=8, nla_type=IFLA_GROUP}, 0], [{nla_len=8, nla_type=IFLA_PROMISCUITY}, 0], [{nla_len=8, nla_type=IFLA_ALLMULTI}, 0], [{nla_len=8, nla_type=IFLA_NUM_TX_QUEUES}, 1], [{nla_len=8, nla_type=IFLA_GSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_GSO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GSO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_GRO_IPV4_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SIZE}, 65536], [{nla_len=8, nla_type=IFLA_TSO_MAX_SEGS}, 65535], [{nla_len=8, nla_type=IFLA_NUM_RX_QUEUES}, 1], [{nla_len=5, nla_type=IFLA_CARRIER}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_CHANGES}, 1], [{nla_len=8, nla_type=IFLA_CARRIER_UP_COUNT}, 0], [{nla_len=8, nla_type=IFLA_CARRIER_DOWN_COUNT}, 1], [{nla_len=5, nla_type=IFLA_PROTO_DOWN}, 0], [{nla_len=10, nla_type=IFLA_ADDRESS}, 02:42:cf:5e:ca:e4], [{nla_len=10, nla_type=IFLA_BROADCAST}, ff:ff:ff:ff:ff:ff], [{nla_len=204, nla_type=IFLA_STATS64}, {rx_packets=0, tx_packets=0, rx_bytes=0, tx_bytes=0, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=15, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0, rx_otherhost_dropped=0}], [{nla_len=100, nla_type=IFLA_STATS}, {rx_packets=0, tx_packets=0, rx_bytes=0, tx_bytes=0, rx_errors=0, tx_errors=0, rx_dropped=0, tx_dropped=15, multicast=0, collisions=0, rx_length_errors=0, rx_over_errors=0, rx_crc_errors=0, rx_frame_errors=0, rx_fifo_errors=0, rx_missed_errors=0, tx_aborted_errors=0, tx_carrier_errors=0, tx_fifo_errors=0, tx_heartbeat_errors=0, tx_window_errors=0, rx_compressed=0, tx_compressed=0, rx_nohandler=0}], [{nla_len=12, nla_type=IFLA_XDP}, [{nla_len=5, nla_type=IFLA_XDP_ATTACHED}, XDP_ATTACHED_NONE]], [{nla_len=444, nla_type=IFLA_LINKINFO}, [[{nla_len=11, nla_type=IFLA_INFO_KIND}, "bridge"], [{nla_len=428, nla_type=IFLA_INFO_DATA}, [[{nla_len=12, nla_type=IFLA_BR_HELLO_TIMER}, 0], [{nla_len=12, nla_type=IFLA_BR_TCN_TIMER}, 0], [{nla_len=12, nla_type=IFLA_BR_TOPOLOGY_CHANGE_TIMER}, 0], [{nla_len=12, nla_type=IFLA_BR_GC_TIMER}, 17725 /* 177.25 s */], [{nla_len=8, nla_type=IFLA_BR_FORWARD_DELAY}, 1500 /* 15.00 s */], [{nla_len=8, nla_type=IFLA_BR_HELLO_TIME}, 200 /* 2.00 s */], [{nla_len=8, nla_type=IFLA_BR_MAX_AGE}, 2000 /* 20.00 s */], [{nla_len=8, nla_type=IFLA_BR_AGEING_TIME}, 30000 /* 300.00 s */], [{nla_len=8, nla_type=IFLA_BR_STP_STATE}, 0], [{nla_len=6, nla_type=IFLA_BR_PRIORITY}, 32768], [{nla_len=5, nla_type=IFLA_BR_VLAN_FILTERING}, 0], [{nla_len=6, nla_type=IFLA_BR_GROUP_FWD_MASK}, 0], [{nla_len=12, nla_type=IFLA_BR_BRIDGE_ID}, {prio=[128, 0], addr=02:42:cf:5e:ca:e4}], [{nla_len=12, nla_type=IFLA_BR_ROOT_ID}, {prio=[128, 0], addr=02:42:cf:5e:ca:e4}], [{nla_len=6, nla_type=IFLA_BR_ROOT_PORT}, 0], [{nla_len=8, nla_type=IFLA_BR_ROOT_PATH_COST}, 0], [{nla_len=5, nla_type=IFLA_BR_TOPOLOGY_CHANGE}, 0], [{nla_len=5, nla_type=IFLA_BR_TOPOLOGY_CHANGE_DETECTED}, 0], [{nla_len=10, nla_type=IFLA_BR_GROUP_ADDR}, 01:80:c2:00:00:00], [{nla_len=12, nla_type=IFLA_BR_MULTI_BOOLOPT}, {optval=0, optmask=1<<BR_BOOLOPT_NO_LL_LEARN|1<<BR_BOOLOPT_MCAST_VLAN_SNOOPING|1<<BR_BOOLOPT_MST_ENABLE}], [{nla_len=8, nla_type=0x30 /* IFLA_BR_??? */}, "\x00\x00\x00\x00"], [{nla_len=8, nla_type=0x31 /* IFLA_BR_??? */}, "\x00\x00\x00\x00"], [{nla_len=6, nla_type=IFLA_BR_VLAN_PROTOCOL}, htons(ETH_P_8021Q)], [{nla_len=6, nla_type=IFLA_BR_VLAN_DEFAULT_PVID}, 1], [{nla_len=5, nla_type=IFLA_BR_VLAN_STATS_ENABLED}, 0], [{nla_len=5, nla_type=IFLA_BR_VLAN_STATS_PER_PORT}, 0], [{nla_len=5, nla_type=IFLA_BR_MCAST_ROUTER}, 1], [{nla_len=5, nla_type=IFLA_BR_MCAST_SNOOPING}, 1], [{nla_len=5, nla_type=IFLA_BR_MCAST_QUERY_USE_IFADDR}, 0], [{nla_len=5, nla_type=IFLA_BR_MCAST_QUERIER}, 0], [{nla_len=5, nla_type=IFLA_BR_MCAST_STATS_ENABLED}, 0], [{nla_len=8, nla_type=IFLA_BR_MCAST_HASH_ELASTICITY}, 16], ...]]]], [{nla_len=12, nla_type=IFLA_QDISC}, "noqueue"], [{nla_len=812, nla_type=IFLA_AF_SPEC}, [[{nla_len=140, nla_type=AF_INET}, [{nla_len=136, nla_type=IFLA_INET_CONF}, [[IPV4_DEVCONF_FORWARDING-1]=1, [IPV4_DEVCONF_MC_FORWARDING-1]=0, [IPV4_DEVCONF_PROXY_ARP-1]=0, [IPV4_DEVCONF_ACCEPT_REDIRECTS-1]=1, [IPV4_DEVCONF_SECURE_REDIRECTS-1]=1, [IPV4_DEVCONF_SEND_REDIRECTS-1]=1, [IPV4_DEVCONF_SHARED_MEDIA-1]=1, [IPV4_DEVCONF_RP_FILTER-1]=2, [IPV4_DEVCONF_ACCEPT_SOURCE_ROUTE-1]=1, [IPV4_DEVCONF_BOOTP_RELAY-1]=0, [IPV4_DEVCONF_LOG_MARTIANS-1]=0, [IPV4_DEVCONF_TAG-1]=0, [IPV4_DEVCONF_ARPFILTER-1]=0, [IPV4_DEVCONF_MEDIUM_ID-1]=0, [IPV4_DEVCONF_NOXFRM-1]=0, [IPV4_DEVCONF_NOPOLICY-1]=0, [IPV4_DEVCONF_FORCE_IGMP_VERSION-1]=0, [IPV4_DEVCONF_ARP_ANNOUNCE-1]=0, [IPV4_DEVCONF_ARP_IGNORE-1]=0, [IPV4_DEVCONF_PROMOTE_SECONDARIES-1]=0, [IPV4_DEVCONF_ARP_ACCEPT-1]=0, [IPV4_DEVCONF_ARP_NOTIFY-1]=0, [IPV4_DEVCONF_ACCEPT_LOCAL-1]=0, [IPV4_DEVCONF_SRC_VMARK-1]=0, [IPV4_DEVCONF_PROXY_ARP_PVLAN-1]=0, [IPV4_DEVCONF_ROUTE_LOCALNET-1]=0, [IPV4_DEVCONF_IGMPV2_UNSOLICITED_REPORT_INTERVAL-1]=10000, [IPV4_DEVCONF_IGMPV3_UNSOLICITED_REPORT_INTERVAL-1]=1000, [IPV4_DEVCONF_IGNORE_ROUTES_WITH_LINKDOWN-1]=0, [IPV4_DEVCONF_DROP_UNICAST_IN_L2_MULTICAST-1]=0, [IPV4_DEVCONF_DROP_GRATUITOUS_ARP-1]=0, [IPV4_DEVCONF_BC_FORWARDING-1]=0, ...]]], [{nla_len=668, nla_type=AF_INET6}, [[{nla_len=8, nla_type=IFLA_INET6_FLAGS}, 0], [{nla_len=20, nla_type=IFLA_INET6_CACHEINFO}, {max_reasm_len=65535, tstamp=1414, reachable_time=18750, retrans_time=1000}], [{nla_len=240, nla_type=IFLA_INET6_CONF}, [[DEVCONF_FORWARDING]=0, [DEVCONF_HOPLIMIT]=64, [DEVCONF_MTU6]=1500, [DEVCONF_ACCEPT_RA]=0, [DEVCONF_ACCEPT_REDIRECTS]=1, [DEVCONF_AUTOCONF]=1, [DEVCONF_DAD_TRANSMITS]=1, [DEVCONF_RTR_SOLICITS]=-1, [DEVCONF_RTR_SOLICIT_INTERVAL]=4000, [DEVCONF_RTR_SOLICIT_DELAY]=1000, [DEVCONF_USE_TEMPADDR]=2, [DEVCONF_TEMP_VALID_LFT]=604800, [DEVCONF_TEMP_PREFERED_LFT]=86400, [DEVCONF_REGEN_MAX_RETRY]=3, [DEVCONF_MAX_DESYNC_FACTOR]=600, [DEVCONF_MAX_ADDRESSES]=16, [DEVCONF_FORCE_MLD_VERSION]=0, [DEVCONF_ACCEPT_RA_DEFRTR]=1, [DEVCONF_ACCEPT_RA_PINFO]=1, [DEVCONF_ACCEPT_RA_RTR_PREF]=1, [DEVCONF_RTR_PROBE_INTERVAL]=60000, [DEVCONF_ACCEPT_RA_RT_INFO_MAX_PLEN]=0, [DEVCONF_PROXY_NDP]=0, [DEVCONF_OPTIMISTIC_DAD]=0, [DEVCONF_ACCEPT_SOURCE_ROUTE]=0, [DEVCONF_MC_FORWARDING]=0, [DEVCONF_DISABLE_IPV6]=0, [DEVCONF_ACCEPT_DAD]=1, [DEVCONF_FORCE_TLLAO]=0, [DEVCONF_NDISC_NOTIFY]=0, [DEVCONF_MLDV1_UNSOLICITED_REPORT_INTERVAL]=10000, [DEVCONF_MLDV2_UNSOLICITED_REPORT_INTERVAL]=1000, ...]], [{nla_len=308, nla_type=IFLA_INET6_STATS}, [[IPSTATS_MIB_NUM]=38, [IPSTATS_MIB_INPKTS]=0, [IPSTATS_MIB_INOCTETS]=0, [IPSTATS_MIB_INDELIVERS]=0, [IPSTATS_MIB_OUTFORWDATAGRAMS]=0, [IPSTATS_MIB_OUTPKTS]=0, [IPSTATS_MIB_OUTOCTETS]=0, [IPSTATS_MIB_INHDRERRORS]=0, [IPSTATS_MIB_INTOOBIGERRORS]=0, [IPSTATS_MIB_INNOROUTES]=0, [IPSTATS_MIB_INADDRERRORS]=0, [IPSTATS_MIB_INUNKNOWNPROTOS]=0, [IPSTATS_MIB_INTRUNCATEDPKTS]=0, [IPSTATS_MIB_INDISCARDS]=0, [IPSTATS_MIB_OUTDISCARDS]=0, [IPSTATS_MIB_OUTNOROUTES]=0, [IPSTATS_MIB_REASMTIMEOUT]=0, [IPSTATS_MIB_REASMREQDS]=0, [IPSTATS_MIB_REASMOKS]=0, [IPSTATS_MIB_REASMFAILS]=0, [IPSTATS_MIB_FRAGOKS]=0, [IPSTATS_MIB_FRAGFAILS]=0, [IPSTATS_MIB_FRAGCREATES]=0, [IPSTATS_MIB_INMCASTPKTS]=0, [IPSTATS_MIB_OUTMCASTPKTS]=0, [IPSTATS_MIB_INBCASTPKTS]=0, [IPSTATS_MIB_OUTBCASTPKTS]=0, [IPSTATS_MIB_INMCASTOCTETS]=0, [IPSTATS_MIB_OUTMCASTOCTETS]=0, [IPSTATS_MIB_INBCASTOCTETS]=0, [IPSTATS_MIB_OUTBCASTOCTETS]=0, [IPSTATS_MIB_CSUMERRORS]=0, ...]], [{nla_len=60, nla_type=IFLA_INET6_ICMP6STATS}, [[ICMP6_MIB_NUM]=7, [ICMP6_MIB_INMSGS]=0, [ICMP6_MIB_INERRORS]=0, [ICMP6_MIB_OUTMSGS]=0, [ICMP6_MIB_OUTERRORS]=0, [ICMP6_MIB_CSUMERRORS]=0, [6 /* ICMP6_MIB_??? */]=0]], [{nla_len=20, nla_type=IFLA_INET6_TOKEN}, inet_pton(AF_INET6, "::")], [{nla_len=5, nla_type=IFLA_INET6_ADDR_GEN_MODE}, IN6_ADDR_GEN_MODE_EUI64]]]]], ...]]], iov_len=8192}], msg_iovlen=1, msg_controllen=0, msg_flags=0}, 0) = 3356
recvmsg(6, {msg_name={sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, msg_namelen=12, msg_iov=[{iov_base=[{nlmsg_len=20, nlmsg_type=NLMSG_DONE, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006650, nlmsg_pid=-304762728}, 0], iov_len=8192}], msg_iovlen=1, msg_controllen=0, msg_flags=0}, 0) = 20
sendto(6, [{nlmsg_len=20, nlmsg_type=RTM_GETADDR, nlmsg_flags=NLM_F_REQUEST|NLM_F_DUMP, nlmsg_seq=1745006651, nlmsg_pid=0}, {ifa_family=AF_UNSPEC, ...}], 20, 0, {sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, 12) = 20
recvmsg(6, {msg_name={sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, msg_namelen=12, msg_iov=[{iov_base=[[{nlmsg_len=76, nlmsg_type=RTM_NEWADDR, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, {ifa_family=AF_INET, ifa_prefixlen=8, ifa_flags=IFA_F_PERMANENT, ifa_scope=RT_SCOPE_HOST, ifa_index=if_nametoindex("lo")}, [[{nla_len=8, nla_type=IFA_ADDRESS}, inet_addr("127.0.0.1")], [{nla_len=8, nla_type=IFA_LOCAL}, inet_addr("127.0.0.1")], [{nla_len=7, nla_type=IFA_LABEL}, "lo"], [{nla_len=8, nla_type=IFA_FLAGS}, IFA_F_PERMANENT], [{nla_len=20, nla_type=IFA_CACHEINFO}, {ifa_prefered=4294967295, ifa_valid=4294967295, cstamp=241, tstamp=241}]]], [{nlmsg_len=88, nlmsg_type=RTM_NEWADDR, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, {ifa_family=AF_INET, ifa_prefixlen=24, ifa_flags=0, ifa_scope=RT_SCOPE_UNIVERSE, ifa_index=if_nametoindex("wlp3s0")}, [[{nla_len=8, nla_type=IFA_ADDRESS}, inet_addr("10.230.35.24")], [{nla_len=8, nla_type=IFA_LOCAL}, inet_addr("10.230.35.24")], [{nla_len=8, nla_type=IFA_BROADCAST}, inet_addr("10.230.35.255")], [{nla_len=11, nla_type=IFA_LABEL}, "wlp3s0"], [{nla_len=8, nla_type=IFA_FLAGS}, IFA_F_NOPREFIXROUTE], [{nla_len=20, nla_type=IFA_CACHEINFO}, {ifa_prefered=3526, ifa_valid=3526, cstamp=6354, tstamp=6354}]]], [{nlmsg_len=88, nlmsg_type=RTM_NEWADDR, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, {ifa_family=AF_INET, ifa_prefixlen=16, ifa_flags=IFA_F_PERMANENT, ifa_scope=RT_SCOPE_UNIVERSE, ifa_index=if_nametoindex("docker0")}, [[{nla_len=8, nla_type=IFA_ADDRESS}, inet_addr("172.17.0.1")], [{nla_len=8, nla_type=IFA_LOCAL}, inet_addr("172.17.0.1")], [{nla_len=8, nla_type=IFA_BROADCAST}, inet_addr("172.17.255.255")], [{nla_len=12, nla_type=IFA_LABEL}, "docker0"], [{nla_len=8, nla_type=IFA_FLAGS}, IFA_F_PERMANENT], [{nla_len=20, nla_type=IFA_CACHEINFO}, {ifa_prefered=4294967295, ifa_valid=4294967295, cstamp=1414, tstamp=1414}]]], [{nlmsg_len=72, nlmsg_type=RTM_NEWADDR, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, {ifa_family=AF_INET6, ifa_prefixlen=128, ifa_flags=IFA_F_PERMANENT, ifa_scope=RT_SCOPE_HOST, ifa_index=if_nametoindex("lo")}, [[{nla_len=20, nla_type=IFA_ADDRESS}, inet_pton(AF_INET6, "::1")], [{nla_len=20, nla_type=IFA_CACHEINFO}, {ifa_prefered=4294967295, ifa_valid=4294967295, cstamp=241, tstamp=241}], [{nla_len=8, nla_type=IFA_FLAGS}, IFA_F_PERMANENT|IFA_F_NOPREFIXROUTE]]], [{nlmsg_len=72, nlmsg_type=RTM_NEWADDR, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, {ifa_family=AF_INET6, ifa_prefixlen=64, ifa_flags=IFA_F_PERMANENT, ifa_scope=RT_SCOPE_LINK, ifa_index=if_nametoindex("wlp3s0")}, [[{nla_len=20, nla_type=IFA_ADDRESS}, inet_pton(AF_INET6, "fe80::bca8:b5a4:3ea:7962")], [{nla_len=20, nla_type=IFA_CACHEINFO}, {ifa_prefered=4294967295, ifa_valid=4294967295, cstamp=6336, tstamp=6336}], [{nla_len=8, nla_type=IFA_FLAGS}, IFA_F_PERMANENT|IFA_F_NOPREFIXROUTE]]]], iov_len=8192}], msg_iovlen=1, msg_controllen=0, msg_flags=0}, 0) = 396
recvmsg(6, {msg_name={sa_family=AF_NETLINK, nl_pid=0, nl_groups=00000000}, msg_namelen=12, msg_iov=[{iov_base=[{nlmsg_len=20, nlmsg_type=NLMSG_DONE, nlmsg_flags=NLM_F_MULTI, nlmsg_seq=1745006651, nlmsg_pid=-304762728}, 0], iov_len=8192}], msg_iovlen=1, msg_controllen=0, msg_flags=0}, 0) = 20
close(6)                                = 0
socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 6
ioctl(6, SIOCGIFINDEX, {ifr_name="lo", ifr_ifindex=1}) = 0
close(6)                                = 0
socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 6
ioctl(6, SIOCGIFINDEX, {ifr_name="wlp3s0", ifr_ifindex=3}) = 0
close(6)                                = 0
socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 6
ioctl(6, SIOCGIFINDEX, {ifr_name="lo", ifr_ifindex=1}) = 0
close(6)                                = 0
socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 6
ioctl(6, SIOCGIFINDEX, {ifr_name="wlp3s0", ifr_ifindex=3}) = 0
close(6)                                = 0
socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP) = 6
connect(6, {sa_family=AF_INET6, sin6_port=htons(123), sin6_flowinfo=htonl(0), inet_pton(AF_INET6, "2001:4c48:1::1", &sin6_addr), sin6_scope_id=0}, 28) = -1 ENETUNREACH (Network is unreachable)
close(6)                                = 0
socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP) = 6
connect(6, {sa_family=AF_INET6, sin6_port=htons(123), sin6_flowinfo=htonl(0), inet_pton(AF_INET6, "2001:ac8:26:62:0:1:0:14", &sin6_addr), sin6_scope_id=0}, 28) = -1 ENETUNREACH (Network is unreachable)
close(6)                                = 0
socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP) = 6
connect(6, {sa_family=AF_INET6, sin6_port=htons(123), sin6_flowinfo=htonl(0), inet_pton(AF_INET6, "2001:738:0:401:80:c106:de2f:123", &sin6_addr), sin6_scope_id=0}, 28) = -1 ENETUNREACH (Network is unreachable)
close(6)                                = 0
socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP) = 6
connect(6, {sa_family=AF_INET6, sin6_port=htons(123), sin6_flowinfo=htonl(0), inet_pton(AF_INET6, "2001:ac8:26:62:0:1:0:21", &sin6_addr), sin6_scope_id=0}, 28) = -1 ENETUNREACH (Network is unreachable)
close(6)                                = 0
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT USR1 USR2 ALRM TERM], [], 8) = 0
pselect6(23, [16 17 18 19 20 21 22], NULL, NULL, NULL, {sigmask=[], sigsetsize=8}^Cstrace: Process 1830 detached
```

I think that some kind of ancillary data is of culprit, but I am not entirely sure: <https://man.archlinux.org/man/core/man-pages/CMSG_DATA.3.en>

## SSL Certificates

```sh
sudo cat /etc/ssl/certs/ssl-cert-snakeoil.pem
-----BEGIN CERTIFICATE-----
MIIC5jCCAc6gAwIBAgIUA4tm9V9+mMwqYXmku5CCpIxDGOUwDQYJKoZIhvcNAQEL
BQAwDjEMMAoGA1UEAwwDZ2hqMB4XDTI1MDMwODE0NTMyMVoXDTM1MDMwNjE0NTMy
MVowDjEMMAoGA1UEAwwDZ2hqMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC
AQEAqC+18U11qJ28ArjfpIuoC2+HASPYRXJcf0Z1dEGLbzzaZ14YzQopTTPxt1dk
SYbOuhHVrYZrhTBXJIN9hN1yediLwLagX2S3wkm6fv+ZbvcN7q+KlmdW2nPANiqJ
IjSFWqbhemx+liR89CM/tTq8KykBMDYR6G1pMM4MeFsfUA6dbe2Fpn0/5dMhCCEs
cHJuBxgtnK5l6WwzBramSiwU8cu5UDqTOOM7PCo3Oj7I+16O6WD6SgLOUcRG3De7
9H02trSHuinUt8GkPVbB40Aq+1KPAedC2RVSLj5+AD03p0vmAdjAvx2pcMqYUBcw
nCjwQ2IpfkTinX4yyf4MLo5XRQIDAQABozwwOjAJBgNVHRMEAjAAMA4GA1UdEQQH
MAWCA2doajAdBgNVHQ4EFgQUh/GdQOH2x89OUGtwu2Wz13oN3a4wDQYJKoZIhvcN
AQELBQADggEBADziLZAp8J9k5Avwm2fh2G9E3Gwx09QZJDE+Bh58ymWGk3elCBor
JVnqt3EMJB5YvjNr9Rnh77OwFCv0DJzyUVhfZhfc+XvjwRzaRK686nUwOdGZY0ks
xwzka7XUq+2L1yM6F0qVGhnrZPdthCT5o3yaHef8k19OzfEt2pLjfEjfguZvyVW0
mg28qdcJOjYt3AEa5rvts4XlfGpjpoDwiaevUAKgRciCOdnWXp6iL77fNeDTxz5w
doy8jWRwME7LfpUz+S+3CdIWNc8BbkxIoVu5Tz9eK9Vu8ZaAjagRV7hyvbLhPcBz
IIoNUFo2WT1Uq7afku4smsg9/w/QoppbUH0=
-----END CERTIFICATE-----
```

```sh
$ sudo cat /etc/ssl/private/ssl-cert-snakeoil.key
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCoL7XxTXWonbwC
uN+ki6gLb4cBI9hFclx/RnV0QYtvPNpnXhjNCilNM/G3V2RJhs66EdWthmuFMFck
g32E3XJ52IvAtqBfZLfCSbp+/5lu9w3ur4qWZ1bac8A2KokiNIVapuF6bH6WJHz0
Iz+1OrwrKQEwNhHobWkwzgx4Wx9QDp1t7YWmfT/l0yEIISxwcm4HGC2crmXpbDMG
tqZKLBTxy7lQOpM44zs8Kjc6Psj7Xo7pYPpKAs5RxEbcN7v0fTa2tIe6KdS3waQ9
VsHjQCr7Uo8B50LZFVIuPn4APTenS+YB2MC/HalwyphQFzCcKPBDYil+ROKdfjLJ
/gwujldFAgMBAAECggEABAOQSZAdZabod+SQPddkoGukfE2UZnyz7wh89CELIisz
7f440RsgrWpQc51r1AKaIdm1txyVECQwgdTh8YJUrmP39EnHhEKDzWjAdGVGqBED
lAJvgc05YIeDrlp2ITFbGQGp/vR8Hu3QmtKeih8HotYP/hkRDovrh0UocG6B54F8
obkAnI/g2Vb2uKhFhK2gCSL+wWvd7DXHsg92r00LxmckKr229Dn0qzMh75oN1hwp
SLB0ld5eSDGPeUXePPJiPOdk+YsDKSv6B87UKZ4H5mN5vHjiOeb41jkxkJbeSIQK
SXKPgtNBewi9n83p9CARotdOdo0ivzWvpmDOKAZQrwKBgQDl4JGEYJOWih/bWkv0
lU27Sl1RLcSoMlz0AJr/1pFPzPPhqu846zNQ5QsWVED+WjnwSz8KnqAm3kJFEUhL
i3ZDymynJQztETQCeZZYSjhfCdlskV6zRCZdN27nqfALKojYCBFWBcB0dtYy3AHe
vJjyDXAiquGF0d3wzohioNp6twKBgQC7THlcuQJCfLX+2sMaiL5aABN5fZc+M6FS
qG25eeJyOC/BeJDQ7S8wFqJrsOn/ThzH2tk8sJTT6S5sw3cKNfUxZhSaKhsi99Oq
vIYiMkL6TveEry8ZCUF1L5FHcybH3ToC4XdBxouO2b2Odl5TGCxOdSpDiepxDPVo
H7AJ4t2x4wKBgFp3xs0SNQNJ2L+7Cal4GN9cpN0N5hdLuslMc7O7/fQp9NEbNIpy
BgAQJAzlm65+U879zhGf2+dwESSdaJZSjHihcaFWM4AZBR70OvcEPqsodSkFDQAA
Ehkdd8g0WQQL+U8L3ca8/MDZA/OAIjxGfOrF3xvFpHvF3FKo9YPbSiCBAoGBAK7c
cb2YWDy8c6kwkc/29EfQJF5bW1VOHIGwD2iHG1tpi2RD7Gex2Ek8CH0kGQIq6gHI
FuJrNTc2Twpp3nwg7f4lgHyBbP5Tg0C2a0AHzrIkAT/V0dG862i9dRPQtNcJefxw
hMda1rYSIL+dUOig+9Rv3Hpbiw3qfD6uvsosDK/jAoGAS+zaReMoQU7PWq59ZpZJ
SC7pl7A8QY8Xo9tByiu/hP6x1IWRuqrWH1TgPmbNevmsa/eoIUE04g37xKSjzbGW
AmP5PW1SgpmLfvh3dk99lmWGHDf47xzVGJxQvR4aPrKzx1lnHXVbIf7/76R7vQj3
GUDxx9y4o83a1keLVYLOQSo=
-----END PRIVATE KEY-----
```

These two file have a very suspicious names. According to wikipedia, snakeoil is a type of fake product advertised as a cure for all illnesses.
The /etc/ssl/private/ folder on my machine contains only a single private key: ssl-cert-snakeoil.key.

The certificate files stored on my machine in /usr/share/ca-certificates/mozilla are dated to Feb 4 2024 which is one year older than I would expect. I provided these files in the certs folder.

## Potential solutions

WIP

## Help is needed!

I would like to ask the IT community of the world to help me find out what this is exactly and what I should do, since my not have enough knowledge to solve it alone.
I belive this issue might have legal consequences, since I was not accused of anything in spite of being under attack and I might not be the only one who suffers from this.
